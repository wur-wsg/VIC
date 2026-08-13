#!/usr/bin/env python
"""Compare the monthly and daily-expanded veg-history runs day by day.

Both runs are driven by forcing that carries identical float32 values, so the
expectation is bitwise equality.  Anything else is reported with the largest
absolute and relative differences so that a real indexing bug cannot hide
behind a tolerance.

Writes a machine-readable summary as JSON and exits non-zero on any mismatch.

Usage:
    python compare_outputs.py --case <dir> [--json <path>]
"""
import argparse
import glob
import json
import os
import sys

import numpy as np
import xarray as xr

# OUT_SWNET is what VIC-5 calls net shortwave; the older OUT_NET_SHORT name no
# longer exists.  It is the output most directly sensitive to albedo, so it is
# the sharpest check that the monthly reader picked the right record.
VARIABLES = [
    "OUT_LAI", "OUT_FCANOPY", "OUT_ALBEDO", "OUT_SWNET", "OUT_LWNET",
    "OUT_LATENT", "OUT_SENSIBLE", "OUT_EVAP", "OUT_RUNOFF", "OUT_BASEFLOW",
    "OUT_WATER_ERROR", "OUT_ENERGY_ERROR",
]


def open_run(result_dir):
    paths = sorted(glob.glob(os.path.join(result_dir, "*.nc")))
    if not paths:
        raise FileNotFoundError("No output files in %s" % result_dir)
    if len(paths) == 1:
        return xr.open_dataset(paths[0])
    return xr.open_mfdataset(paths, combine="by_coords")


def compare_variable(a, b):
    """Compare two arrays, ignoring positions where both sides are NaN."""
    av = np.asarray(a.values, dtype="f8")
    bv = np.asarray(b.values, dtype="f8")

    if av.shape != bv.shape:
        return {"status": "SHAPE_MISMATCH",
                "shape_monthly": list(av.shape),
                "shape_daily": list(bv.shape)}

    both_nan = np.isnan(av) & np.isnan(bv)
    # Bitwise identity: equal bit patterns, with matching NaNs treated as equal.
    identical = np.array_equal(
        av.view(np.uint64)[~both_nan], bv.view(np.uint64)[~both_nan])

    valid = ~(np.isnan(av) | np.isnan(bv))
    if not valid.any():
        return {"status": "ALL_NAN", "bitwise_identical": bool(identical),
                "n_compared": 0}

    diff = np.abs(av[valid] - bv[valid])
    scale = np.maximum(np.abs(av[valid]), np.abs(bv[valid]))
    with np.errstate(divide="ignore", invalid="ignore"):
        rel = np.where(scale > 0, diff / scale, 0.0)

    n_diff = int(np.count_nonzero(diff))
    return {
        "status": "BITWISE_IDENTICAL" if identical and n_diff == 0 else (
            "EQUAL_VALUES" if n_diff == 0 else "DIFFERENT"),
        "bitwise_identical": bool(identical),
        "n_compared": int(valid.sum()),
        "n_differing": n_diff,
        "max_abs_diff": float(diff.max()),
        "max_rel_diff": float(np.nanmax(rel)),
        "nan_mismatch": int(np.count_nonzero(np.isnan(av) != np.isnan(bv))),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--case", required=True)
    parser.add_argument("--json", default=None)
    args = parser.parse_args()

    case = os.path.abspath(args.case)
    monthly = open_run(os.path.join(case, "result_monthly"))
    daily = open_run(os.path.join(case, "result_daily"))

    summary = {"case": case, "variables": {}}

    n_month = monthly.sizes.get("time")
    n_day = daily.sizes.get("time")
    summary["n_timesteps"] = {"monthly": n_month, "daily": n_day}
    if n_month != n_day:
        summary["status"] = "FAIL"
        summary["reason"] = "time dimension differs"
        print(json.dumps(summary, indent=2))
        return 1

    failures = []
    for name in VARIABLES:
        if name not in monthly or name not in daily:
            summary["variables"][name] = {"status": "MISSING"}
            failures.append(name)
            continue
        result = compare_variable(monthly[name], daily[name])
        summary["variables"][name] = result
        if result["status"] not in ("BITWISE_IDENTICAL", "EQUAL_VALUES"):
            failures.append(name)

    summary["status"] = "PASS" if not failures else "FAIL"
    summary["failing_variables"] = failures

    if args.json:
        with open(args.json, "w") as handle:
            json.dump(summary, handle, indent=2)

    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    sys.exit(main())
