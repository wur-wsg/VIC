#!/usr/bin/env python
"""E0 preflight for a VIC-WUR parameter bundle.

Checks a parameter file against what the image driver actually requires, before
anything is transferred to another site or handed to a long run. Every rule here
was read out of the VIC source rather than assumed; the source reference is
given next to each one, because several of them are not what a reasonable person
would guess.

Fields are read a class at a time: the 4-D vegetation fields are around 10 GB
each when stored as doubles.

Usage:
    check_parameter_bundle.py <bundle.nc> <domain.nc> [--nbare N] [--json PATH]
"""
import argparse
import json
import sys

import numpy as np
import netCDF4 as nc

# Read unconditionally by vic_init.c, or under options the natural baseline
# uses. Baseline: no CARBON, no LAKES, no ORGANIC_FRACT, no BLOWING_SNOW,
# no SPATIAL_FROST, no SPATIAL_SNOW, FROZEN_SOIL FALSE, SNOW_BAND TRUE.
REQUIRED_SOIL = [
    "Ds", "Dsmax", "Ws", "c", "infilt", "depth", "expt", "Ksat", "bubble",
    "quartz", "bulk_density", "soil_density", "init_moist", "resid_moist",
    "phi_s", "Wcr_FRACT", "Wfc_FRACT", "Wpwp_FRACT", "rough", "snow_rough",
    "avg_T", "dp", "elev", "fs_active",
]
REQUIRED_BAND = ["AreaFract", "Pfactor", "elevation"]
REQUIRED_VEG = [
    "LAI", "albedo", "fcanopy", "displacement", "veg_rough", "overstory",
    "rarc", "rmin", "RGL", "rad_atten", "wind_atten", "trunk_ratio", "wind_h",
]
REQUIRED_OTHER = ["run_cell", "lat", "lon", "Cv", "Nveg", "root_depth",
                  "root_fract"]

# Read into soil_con but never used in any computation in this version -- the
# only other reference is print_library_shared.c. A gap here will not affect a
# run, but it is still a hole in the product.
READ_BUT_UNUSED = ["annual_prec"]

# Read only under options the baseline does not use; absence is expected.
OPTIONAL = {
    "CARBON": ["Ctype", "LUE", "MaxCarboxRate", "NPPfactor_sat", "Nscale",
               "Wnpp_inhib"],
    "LAKES": ["basin_area", "basin_depth", "lake_idx", "mindepth", "numnod",
              "rpercent", "wfrac", "depth_in", "lag_one"],
    "ORGANIC_FRACT": ["organic", "bulk_density_org", "soil_density_org"],
    "BLOWING_SNOW": ["fetch", "sigma_slope"],
    "SPATIAL_SNOW": ["max_snow_distrib_slope"],
    "SPATIAL_FROST": ["frost_slope"],
}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("bundle")
    ap.add_argument("domain")
    ap.add_argument("--nbare", type=int, default=1,
                    help="NBARE from the global parameter file (default 1)")
    ap.add_argument("--json", default=None)
    args = ap.parse_args()

    ds = nc.Dataset(args.bundle)
    dom = nc.Dataset(args.domain)
    problems, notes = [], []
    report = {"bundle": args.bundle, "domain": args.domain, "nbare": args.nbare}

    required = REQUIRED_OTHER + REQUIRED_SOIL + REQUIRED_BAND + REQUIRED_VEG
    have = set(ds.variables)

    # 1. presence
    missing = [v for v in required if v not in have]
    print("1. required variables  : %d/%d present"
          % (len(required) - len(missing), len(required)))
    report["missing_required"] = missing
    if missing:
        problems.append("missing: %s" % ", ".join(missing))
        print("   MISSING: %s" % ", ".join(missing))
    for opt, names in OPTIONAL.items():
        present = [v for v in names if v in have]
        if present:
            notes.append("%s inputs present but unused by this configuration: %s"
                         % (opt, ", ".join(present)))

    # 2. grid
    blat, blon = ds["lat"][:], ds["lon"][:]
    same = (blat.shape == dom["lat"][:].shape
            and blon.shape == dom["lon"][:].shape
            and np.allclose(blat, dom["lat"][:])
            and np.allclose(blon, dom["lon"][:]))
    print("2. grid matches domain : %s  (lat=%d lon=%d)"
          % (same, blat.size, blon.size))
    report["grid_matches_domain"] = bool(same)
    if not same:
        problems.append("lat/lon differ from the domain file")

    # 3. active cells
    run_cell = np.ma.filled(ds["run_cell"][:], 0).astype(int)
    mask = np.ma.filled(dom["mask"][:], 0).astype(int)
    active = run_cell == 1
    n_run, n_mask = int(active.sum()), int((mask == 1).sum())
    print("3. active cells        : run_cell=%d domain=%d agree=%s"
          % (n_run, n_mask, n_run == n_mask))
    report["active_cells"] = n_run
    if n_run != n_mask:
        problems.append("run_cell active %d != domain mask %d" % (n_run, n_mask))

    # 4. Cv closure, and the per-class presence mask reused below
    ncls = ds.dimensions["veg_class"].size
    cv = np.empty((ncls,) + run_cell.shape)
    for k in range(ncls):
        cv[k] = np.ma.filled(ds["Cv"][k], 0.0)
    cv_sum = cv.sum(axis=0)
    present = cv > 0
    n_tiles = present.sum(axis=0)
    bad_cv = int((np.abs(cv_sum[active] - 1.0) > 1e-6).sum())
    print("4. Cv sums to 1        : %d active cells outside 1e-6" % bad_cv)
    report["cv_closure_failures"] = bad_cv
    if bad_cv:
        problems.append("%d active cells where Cv does not sum to 1" % bad_cv)

    # 5. root_fract closure, excluding barren.
    # The barren class carries no roots, so its root_fract sums to 0 by design.
    barren = ncls - 1
    bad_root = 0
    for k in range(ncls):
        if k == barren:
            continue
        rs = np.nansum(np.ma.filled(ds["root_fract"][k], np.nan), axis=0)
        sel = active & present[k]
        if sel.any():
            bad_root += int((np.abs(rs[sel] - 1.0) > 1e-5).sum())
    print("5. root_fract sums to 1: %d vegetated tiles outside 1e-5" % bad_root)
    report["root_closure_failures"] = bad_root
    if bad_root:
        problems.append("%d vegetated tiles where root_fract != 1" % bad_root)

    # 6. Nveg against Cv.
    # vic_alloc.c:128 allocates nveg + Nbare tiles, and vic_init.c:1186-1197
    # then requires  Nveg <= count(Cv>0) <= Nveg + Nbare.  Equality is NOT
    # required and testing for it rejects correct files: the bare-soil tile may
    # or may not appear in Cv.
    nveg = np.ma.filled(ds["Nveg"][:], -1).astype(int)
    over = int((n_tiles[active] > nveg[active] + args.nbare).sum())
    under = int((n_tiles[active] < nveg[active]).sum())
    print("6. Nveg <= tiles <= Nveg+%d : %d over, %d under"
          % (args.nbare, over, under))
    report["nveg_violations"] = {"over": over, "under": under}
    if over or under:
        problems.append("Nveg inconsistent with Cv on %d cells" % (over + under))

    # 7. non-finite values where they would actually be used.
    # A vegetation field is only meaningful where that class exists, so the
    # check is restricted to tiles with Cv > 0; testing every class everywhere
    # would flag three quarters of the file as broken.
    print("7. non-finite values where used:")
    bad = {}
    for name in REQUIRED_SOIL + REQUIRED_BAND + READ_BUT_UNUSED:
        if name not in have:
            continue
        var = ds[name]
        lead = var.shape[:-2]
        n = 0
        if not lead:
            arr = np.ma.filled(var[:].astype("f8"), np.nan)
            n = int((~np.isfinite(arr[active])).sum())
        else:
            for idx in np.ndindex(*lead):
                arr = np.ma.filled(var[idx].astype("f8"), np.nan)
                n += int((~np.isfinite(arr[active])).sum())
        if n:
            bad[name] = n

    for name in REQUIRED_VEG + ["root_depth", "root_fract"]:
        if name not in have:
            continue
        var = ds[name]
        n = 0
        for k in range(ncls):
            sel = active & present[k]
            if not sel.any():
                continue
            arr = np.ma.filled(var[k].astype("f8"), np.nan)
            if arr.ndim == 2:
                n += int((~np.isfinite(arr[sel])).sum())
            else:
                for j in range(arr.shape[0]):
                    n += int((~np.isfinite(arr[j][sel])).sum())
        if n:
            bad[name] = n

    if bad:
        for name, n in sorted(bad.items(), key=lambda kv: -kv[1]):
            tag = " (read but unused)" if name in READ_BUT_UNUSED else ""
            print("   %-20s %d%s" % (name, n, tag))
    else:
        print("   none")
    report["non_finite"] = bad

    used_bad = {k: v for k, v in bad.items() if k not in READ_BUT_UNUSED}
    unused_bad = {k: v for k, v in bad.items() if k in READ_BUT_UNUSED}
    if used_bad:
        problems.append("non-finite in variables VIC uses: %s"
                        % ", ".join(used_bad))
    for name, n in unused_bad.items():
        notes.append("%s has %d non-finite active cells; VIC reads it but never "
                     "uses it, so this cannot affect a run -- it is still a gap "
                     "in the product" % (name, n))

    print()
    for n in notes:
        print("NOTE: %s" % n)
    print()
    report["status"] = "FAIL" if problems else "PASS"
    report["problems"] = problems
    report["notes"] = notes
    if args.json:
        with open(args.json, "w") as fh:
            json.dump(report, fh, indent=2)
    if problems:
        print("PREFLIGHT FAILED")
        for p in problems:
            print("  - %s" % p)
        return 1
    print("PREFLIGHT PASSED")
    return 0


if __name__ == "__main__":
    sys.exit(main())
