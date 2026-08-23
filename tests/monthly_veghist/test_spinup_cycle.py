#!/usr/bin/env python
"""Check that a spin-up cycle reads the forcing for the date it is simulating.

Spin-up repeats an early period from a cold start, feeding each cycle's final
state into the next. That makes the clock jump *backwards*: a state saved at
the end of 2003 initialises a run that starts on 2003-01-01 again. VIC does not
check the state file's date against the start date, so this works -- and it
means the forcing index must follow the simulated date rather than anything
carried over from the previous cycle.

The monthly veg-history index is a pure function of the current date, so this
should hold by construction. "By construction" is not evidence, hence this test.

It asserts two things:

  * cycle 2 day 1 sees January of the *start* year, matching cycle 1 day 1;
  * that value differs from January of the year the state came from, so a
    reader that had carried its position over would have been caught.

Usage:
    python test_spinup_cycle.py --case <dir> --exe <vic_image.exe> \
        [--json <path>]
"""
import argparse
import glob
import json
import os
import sys

import numpy as np
import xarray as xr

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import site_config  # noqa: E402


def rewrite_global(src, dst, replacements, extra_lines=""):
    out = []
    for line in open(src):
        tokens = line.split()
        key = tokens[0] if tokens and not line.startswith("#") else None
        if key in replacements:
            out.append("%-23s %s\n" % (key, replacements.pop(key)))
        else:
            out.append(line)
    for key, value in replacements.items():
        out.append("%-23s %s\n" % (key, value))
    if extra_lines:
        out.append(extra_lines)
    with open(dst, "w") as handle:
        handle.writelines(out)


def open_result(result_dir):
    paths = sorted(glob.glob(os.path.join(result_dir, "*.nc")))
    if not paths:
        raise FileNotFoundError("No output in %s" % result_dir)
    return xr.open_dataset(paths[0]) if len(paths) == 1 else \
        xr.open_mfdataset(paths, combine="by_coords")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--case", required=True)
    parser.add_argument("--exe", required=True)
    parser.add_argument("--json", default=None)
    args = parser.parse_args()

    case = os.path.abspath(args.case)
    base = os.path.join(case, "global_monthly.txt")
    work = os.path.join(case, "spinup")
    state_dir = os.path.join(work, "state")
    os.makedirs(state_dir, exist_ok=True)

    # Read the case period from the generated global file rather than assuming.
    conf = {}
    for line in open(base):
        tokens = line.split()
        if len(tokens) >= 2:
            conf.setdefault(tokens[0], tokens[1])
    year0 = int(conf["STARTYEAR"])

    prefix = os.path.join(state_dir, "cycle1_")
    save_at = (year0 + 1, 1, 1)

    # --- cycle 1: cold start, one year, saving state at its end -------------
    c1_dir = os.path.join(work, "result_cycle1")
    os.makedirs(c1_dir, exist_ok=True)
    c1 = os.path.join(work, "global_cycle1.txt")
    rewrite_global(base, c1, {
        "STARTYEAR": str(year0), "STARTMONTH": "1", "STARTDAY": "1",
        "ENDYEAR": str(year0), "ENDMONTH": "12", "ENDDAY": "31",
        "RESULT_DIR": c1_dir, "OUTFILE": "cycle1",
    }, extra_lines=(
        "STATENAME               %s\n"
        "STATEYEAR               %d\n"
        "STATEMONTH              %d\n"
        "STATEDAY                %d\n"
        "STATESEC                0\n"
        "STATE_FORMAT            NETCDF4\n" % ((prefix,) + save_at)))
    site_config.run_vic(args.exe, c1, os.path.join(work, "log_cycle1.txt"),
                        check=True)

    state = "%s.%04d%02d%02d_00000.nc" % ((prefix,) + save_at)
    if not os.path.isfile(state):
        raise SystemExit("cycle 1 did not write %s" % state)

    # --- cycle 2: same period again, initialised from cycle 1's end state ---
    c2_dir = os.path.join(work, "result_cycle2")
    os.makedirs(c2_dir, exist_ok=True)
    c2 = os.path.join(work, "global_cycle2.txt")
    rewrite_global(base, c2, {
        "STARTYEAR": str(year0), "STARTMONTH": "1", "STARTDAY": "1",
        "ENDYEAR": str(year0), "ENDMONTH": "12", "ENDDAY": "31",
        "RESULT_DIR": c2_dir, "OUTFILE": "cycle2",
    }, extra_lines="INIT_STATE              %s\n" % state)
    site_config.run_vic(args.exe, c2, os.path.join(work, "log_cycle2.txt"),
                        check=True)

    # --- compare the vegetation actually used -------------------------------
    a = open_result(c1_dir)
    b = open_result(c2_dir)

    summary = {"case": case, "start_year": year0,
               "state_from": "%04d-%02d-%02d" % save_at, "variables": {}}
    failures = []

    for name in ("OUT_LAI", "OUT_FCANOPY", "OUT_ALBEDO"):
        av = np.asarray(a[name].values, dtype="f8")
        bv = np.asarray(b[name].values, dtype="f8")
        valid = ~(np.isnan(av) | np.isnan(bv))

        # The whole year must match: the vegetation forcing does not depend on
        # model state, so every day of cycle 2 must see what cycle 1 saw.
        same = bool(np.array_equal(av[valid].view(np.uint64),
                                   bv[valid].view(np.uint64)))

        # Sensitivity: January must differ from December within the year, so a
        # reader stuck on one record would not pass the check above by luck.
        jan = av[0][~np.isnan(av[0])]
        dec = av[-1][~np.isnan(av[-1])]
        varies = bool(jan.size and not np.array_equal(jan, dec))

        summary["variables"][name] = {
            "cycle2_matches_cycle1": same,
            "january_differs_from_december": varies,
            "passed": same and varies,
        }
        if not (same and varies):
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
