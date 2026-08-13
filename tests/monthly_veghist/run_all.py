#!/usr/bin/env python
"""Run the whole monthly veg-history test suite and write one JSON summary.

Stages:
  1. build the synthetic case (tiny domain, 2003 + 2004)
  2. monthly vs daily-expanded equivalence      -- expects bitwise equality
  3. 1-rank vs 4-rank MPI consistency           -- expects bitwise equality
  4. continuous vs restarted run                -- expects bitwise equality
  5. malformed-input error paths                -- expects specific failures
  6. backward compatibility against a baseline binary, if one is supplied

Usage:
    python run_all.py --workdir <dir> --exe <vic_image.exe> \
        [--baseline-exe <vic_image.exe>] [--json <path>]
"""
import argparse
import glob
import json
import os
import subprocess
import sys

import numpy as np
import xarray as xr

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import site_config  # noqa: E402

COMPARE_VARS = [
    "OUT_LAI", "OUT_FCANOPY", "OUT_ALBEDO", "OUT_SWNET", "OUT_LWNET",
    "OUT_LATENT", "OUT_SENSIBLE", "OUT_EVAP", "OUT_RUNOFF", "OUT_BASEFLOW",
    "OUT_WATER_ERROR", "OUT_ENERGY_ERROR",
]


def run(cmd, log_path):
    with open(log_path, "w") as log:
        proc = subprocess.run(cmd, stdout=log, stderr=subprocess.STDOUT)
    return proc.returncode


def retarget_case(case):
    """Point a copied case's global parameter files at their new location.

    The generated global files carry absolute paths, so a case built on one
    site and copied to another would otherwise still refer to the first site's
    directories.  The old case directory is recovered from the DOMAIN line
    rather than assumed, and every occurrence of it is rewritten.

    Returns the old directory if anything was rewritten, else None.
    """
    globals_ = sorted(glob.glob(os.path.join(case, "global_*.txt")))
    if not globals_:
        raise SystemExit("No global_*.txt found in %s" % case)

    old = None
    for line in open(globals_[0]):
        tokens = line.split()
        if tokens and tokens[0] == "DOMAIN":
            old = os.path.dirname(tokens[1])
            break
    if old is None:
        raise SystemExit("No DOMAIN line found in %s" % globals_[0])
    moved = os.path.abspath(old) != os.path.abspath(case)
    if moved:
        for path in globals_:
            text = open(path).read()
            with open(path, "w") as handle:
                handle.write(text.replace(old, case))

    # Unconditionally, not only when the case moved: a case that is already at
    # the right path can still have had its result directories cleaned out
    # between runs.  VIC does not create RESULT_DIR itself, and reports the
    # failure as a misleading "Permission denied" on the output file rather
    # than as a missing directory.
    for path in globals_:
        for line in open(path):
            tokens = line.split()
            if tokens and tokens[0] == "RESULT_DIR":
                os.makedirs(tokens[1], exist_ok=True)
    return old if moved else None


def open_result(result_dir):
    paths = sorted(glob.glob(os.path.join(result_dir, "*.nc")))
    if not paths:
        raise FileNotFoundError("No output in %s" % result_dir)
    if len(paths) == 1:
        return xr.open_dataset(paths[0])
    return xr.open_mfdataset(paths, combine="by_coords")


def bitwise_compare(dir_a, dir_b):
    a = open_result(dir_a)
    b = open_result(dir_b)
    out = {}
    ok = True
    for name in COMPARE_VARS:
        if name not in a or name not in b:
            out[name] = {"status": "MISSING"}
            ok = False
            continue
        av = np.asarray(a[name].values, dtype="f8")
        bv = np.asarray(b[name].values, dtype="f8")
        if av.shape != bv.shape:
            out[name] = {"status": "SHAPE_MISMATCH"}
            ok = False
            continue
        valid = ~(np.isnan(av) | np.isnan(bv))
        identical = bool(np.array_equal(av[valid].view(np.uint64),
                                        bv[valid].view(np.uint64)))
        diff = np.abs(av[valid] - bv[valid])
        out[name] = {
            "status": "BITWISE_IDENTICAL" if identical else "DIFFERENT",
            "bitwise_identical": identical,
            "n_compared": int(valid.sum()),
            "max_abs_diff": float(diff.max()) if diff.size else 0.0,
        }
        ok &= identical
    return ok, out


def sensitivity_check(case):
    """Confirm the equivalence test could actually detect a wrong record.

    If every month carried the same value, monthly-vs-daily equality would be
    vacuous.  This asserts that each month is constant, that all twelve months
    differ, and that the same month differs between the two years.
    """
    import pandas as pd

    ds = open_result(os.path.join(case, "result_monthly"))
    lai = ds["OUT_LAI"]
    series = lai.mean(dim=[d for d in lai.dims if d != "time"]).values
    times = pd.DatetimeIndex(ds["time"].values)

    per_month_constant = True
    monthly_values = {}
    for year in sorted(set(times.year)):
        for month in range(1, 13):
            sel = (times.year == year) & (times.month == month)
            if not sel.any():
                continue
            vals = np.unique(series[sel])
            if vals.size != 1:
                per_month_constant = False
            monthly_values[(year, month)] = float(vals[0])

    first_year = min(times.year)
    second_year = max(times.year)
    y1 = [monthly_values[(first_year, m)] for m in range(1, 13)]
    all_months_differ = len(set(y1)) == 12
    years_differ = (monthly_values[(first_year, 1)]
                    != monthly_values[(second_year, 1)])

    return {
        "value_constant_within_each_month": per_month_constant,
        "all_twelve_months_distinct": all_months_differ,
        "january_differs_between_years": bool(years_differ),
        "passed": bool(per_month_constant and all_months_differ and years_differ),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--workdir", required=True)
    parser.add_argument("--exe", required=True)
    parser.add_argument("--baseline-exe", default=None)
    parser.add_argument("--json", default=None)
    parser.add_argument(
        "--skip-case", action="store_true",
        help="reuse the case already in <workdir>/case instead of building it. "
             "Use this on a site that has no template inputs: build the case "
             "once, copy it across, and both sites then run an identical case.")
    args = parser.parse_args()

    for label, path in (("--exe", args.exe),
                        ("--baseline-exe", args.baseline_exe)):
        if path and not os.path.isfile(path):
            # Worth catching here: a missing binary surfaces from the launcher
            # as a rank-start failure, which reads like an MPI problem.
            raise SystemExit("%s does not exist: %s" % (label, path))

    work = os.path.abspath(args.workdir)
    case = os.path.join(work, "case")
    os.makedirs(work, exist_ok=True)
    summary = {"workdir": work, "exe": args.exe,
               "site": site_config.describe(), "stages": {}}

    # 1. build the case
    if args.skip_case:
        if not os.path.isdir(case):
            raise SystemExit("--skip-case given but %s does not exist" % case)
        old = retarget_case(case)
        summary["stages"]["build_case"] = {
            "skipped": True, "passed": True, "retargeted_from": old}
    else:
        rc = run([site_config.python_interpreter(),
                  os.path.join(HERE, "make_synthetic_case.py"),
                  "--outdir", case], os.path.join(work, "log_case.txt"))
        summary["stages"]["build_case"] = {"returncode": rc, "passed": rc == 0}
        if rc != 0:
            return finish(summary, args, failed=True)

    # 2. run monthly and daily
    for tag in ("monthly", "daily"):
        rc = run(site_config.mpi_command(
                     args.exe, os.path.join(case, "global_%s.txt" % tag)),
                 os.path.join(work, "log_%s.txt" % tag))
        summary["stages"]["run_%s" % tag] = {"returncode": rc,
                                             "passed": rc == 0}
        if rc != 0:
            return finish(summary, args, failed=True)

    ok, detail = bitwise_compare(os.path.join(case, "result_monthly"),
                                 os.path.join(case, "result_daily"))
    summary["stages"]["monthly_vs_daily_equivalence"] = {
        "passed": ok, "variables": detail}

    summary["stages"]["sensitivity_of_equivalence_test"] = sensitivity_check(
        case)

    # 3. MPI consistency
    mpi_dir = os.path.join(case, "result_monthly_mpi4")
    os.makedirs(mpi_dir, exist_ok=True)
    gfile = os.path.join(case, "global_monthly_mpi4.txt")
    text = open(os.path.join(case, "global_monthly.txt")).read().replace(
        os.path.join(case, "result_monthly"), mpi_dir)
    with open(gfile, "w") as handle:
        handle.write(text)
    rc = run(site_config.mpi_command(args.exe, gfile, nranks=4),
             os.path.join(work, "log_monthly_mpi4.txt"))
    if rc == 0:
        ok, detail = bitwise_compare(os.path.join(case, "result_monthly"),
                                     mpi_dir)
    else:
        ok, detail = False, {}
    summary["stages"]["mpi_1_vs_4_ranks"] = {
        "returncode": rc, "passed": ok, "variables": detail}

    # 4. restart continuity
    restart_json = os.path.join(work, "restart.json")
    rc = run([site_config.python_interpreter(), os.path.join(HERE, "test_restart_continuity.py"),
              "--case", case, "--exe", args.exe, "--json", restart_json],
             os.path.join(work, "log_restart.txt"))
    detail = json.load(open(restart_json)) if os.path.exists(restart_json) else {}
    summary["stages"]["restart_continuity"] = {
        "returncode": rc, "passed": rc == 0,
        "break_points": detail.get("break_points"),
        "variables": detail.get("variables", {})}

    # 5. error paths
    errors_json = os.path.join(work, "errorpaths.json")
    rc = run([site_config.python_interpreter(), os.path.join(HERE, "test_error_paths.py"),
              "--case", case, "--exe", args.exe, "--json", errors_json],
             os.path.join(work, "log_errorpaths.txt"))
    detail = json.load(open(errors_json)) if os.path.exists(errors_json) else {}
    summary["stages"]["error_paths"] = {
        "returncode": rc, "passed": rc == 0, "cases": detail.get("cases", {})}

    # 6. backward compatibility
    if args.baseline_exe:
        base_dir = os.path.join(case, "result_daily_baseline")
        os.makedirs(base_dir, exist_ok=True)
        gfile = os.path.join(case, "global_daily_baseline.txt")
        text = open(os.path.join(case, "global_daily.txt")).read().replace(
            os.path.join(case, "result_daily"), base_dir)
        with open(gfile, "w") as handle:
            handle.write(text)
        rc = run(site_config.mpi_command(args.baseline_exe, gfile),
                 os.path.join(work, "log_daily_baseline.txt"))
        if rc == 0:
            ok, detail = bitwise_compare(base_dir,
                                         os.path.join(case, "result_daily"))
        else:
            ok, detail = False, {}
        summary["stages"]["backward_compatibility_vs_baseline"] = {
            "baseline_exe": args.baseline_exe, "returncode": rc,
            "passed": ok, "variables": detail}

    failed = any(not stage.get("passed", False)
                 for stage in summary["stages"].values())
    return finish(summary, args, failed)


def finish(summary, args, failed):
    summary["status"] = "FAIL" if failed else "PASS"
    summary["failing_stages"] = [name for name, stage
                                 in summary["stages"].items()
                                 if not stage.get("passed", False)]
    if args.json:
        with open(args.json, "w") as handle:
            json.dump(summary, handle, indent=2)
    print(json.dumps({"status": summary["status"],
                      "stages": {k: v.get("passed")
                                 for k, v in summary["stages"].items()},
                      "failing_stages": summary["failing_stages"]}, indent=2))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
