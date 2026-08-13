#!/usr/bin/env python
"""Check that a segmented (restarted) monthly-veghist run reproduces a
continuous one.

The monthly record index is derived from the current date rather than from an
accumulating counter, so restarting part-way through a month must land on the
same record as a continuous run.  This test breaks the two-year case at two
deliberately awkward points:

  * 2003-07-15  -- mid-month, mid-year
  * 2004-01-01  -- a year boundary, which also forces a new forcing file

and compares the concatenated segment output against the continuous run day by
day.  Bitwise equality is expected.

Usage:
    python test_restart_continuity.py --case <dir> --exe <vic_image.exe> \
        [--json <path>]
"""
import argparse
import glob
import json
import os
import subprocess
import sys

import numpy as np
import xarray as xr

VARIABLES = [
    "OUT_LAI", "OUT_FCANOPY", "OUT_ALBEDO", "OUT_SWNET", "OUT_LATENT",
    "OUT_SENSIBLE", "OUT_EVAP", "OUT_RUNOFF", "OUT_BASEFLOW",
    "OUT_WATER_ERROR", "OUT_ENERGY_ERROR",
]

# (start, end, saves_state_at) -- end is the last simulated day.
SEGMENTS = [
    ((2003, 1, 1), (2003, 7, 14), (2003, 7, 15)),
    ((2003, 7, 15), (2003, 12, 31), (2004, 1, 1)),
    ((2004, 1, 1), (2004, 12, 31), None),
]


def rewrite_global(src, dst, replacements, extra_lines=""):
    """Rewrite a generated global file, replacing whole directives."""
    out = []
    for line in open(src):
        key = line.split()[0] if line.strip() and not line.startswith("#") else None
        if key in replacements:
            out.append("%-23s %s\n" % (key, replacements.pop(key)))
        else:
            out.append(line)
    # Anything not present in the template gets appended.
    for key, value in replacements.items():
        out.append("%-23s %s\n" % (key, value))
    if extra_lines:
        out.append(extra_lines)
    with open(dst, "w") as handle:
        handle.writelines(out)


def run_vic(exe, global_file, log_path, env):
    with open(log_path, "w") as log:
        proc = subprocess.run(["mpirun", "-np", "1", exe, "-g", global_file],
                              stdout=log, stderr=subprocess.STDOUT, env=env)
    if proc.returncode != 0:
        raise RuntimeError("VIC failed for %s; see %s" % (global_file, log_path))


def open_result(result_dir):
    paths = sorted(glob.glob(os.path.join(result_dir, "*.nc")))
    if not paths:
        raise FileNotFoundError("No output in %s" % result_dir)
    if len(paths) == 1:
        return xr.open_dataset(paths[0])
    return xr.open_mfdataset(paths, combine="by_coords")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--case", required=True)
    parser.add_argument("--exe", required=True)
    parser.add_argument("--json", default=None)
    args = parser.parse_args()

    case = os.path.abspath(args.case)
    base_global = os.path.join(case, "global_monthly.txt")
    state_dir = os.path.join(case, "state_restart")
    os.makedirs(state_dir, exist_ok=True)

    env = dict(os.environ)
    env["LD_LIBRARY_PATH"] = (
        "/home/WUR/liu297/miniconda3/envs/nco_env/lib:"
        + env.get("LD_LIBRARY_PATH", ""))

    # --- segmented run -----------------------------------------------------
    previous_state = None
    for idx, (start, end, save_at) in enumerate(SEGMENTS):
        result_dir = os.path.join(case, "result_seg%d" % idx)
        os.makedirs(result_dir, exist_ok=True)

        replacements = {
            "STARTYEAR": str(start[0]), "STARTMONTH": str(start[1]),
            "STARTDAY": str(start[2]),
            "ENDYEAR": str(end[0]), "ENDMONTH": str(end[1]),
            "ENDDAY": str(end[2]),
            "RESULT_DIR": result_dir,
            "OUTFILE": "seg%d" % idx,
        }
        extra = ""
        if save_at is not None:
            prefix = os.path.join(state_dir, "seg%d_state_" % idx)
            extra += ("STATENAME               %s\n"
                      "STATEYEAR               %d\n"
                      "STATEMONTH              %d\n"
                      "STATEDAY                %d\n"
                      "STATESEC                0\n"
                      "STATE_FORMAT            NETCDF4\n"
                      % ((prefix,) + save_at))
            previous_state_next = "%s.%04d%02d%02d_00000.nc" % (
                (prefix,) + save_at)
        else:
            previous_state_next = None

        if previous_state is not None:
            extra += "INIT_STATE              %s\n" % previous_state

        seg_global = os.path.join(case, "global_seg%d.txt" % idx)
        rewrite_global(base_global, seg_global, replacements, extra)
        run_vic(args.exe, seg_global,
                os.path.join(case, "log_seg%d.txt" % idx), env)
        previous_state = previous_state_next

    # --- compare against the continuous run --------------------------------
    continuous = open_result(os.path.join(case, "result_monthly"))
    segments = xr.concat(
        [open_result(os.path.join(case, "result_seg%d" % i))
         for i in range(len(SEGMENTS))],
        dim="time")
    segments = segments.sortby("time")

    summary = {
        "case": case,
        "break_points": ["2003-07-15 (mid-month)", "2004-01-01 (year end)"],
        "n_timesteps": {"continuous": int(continuous.sizes["time"]),
                        "segmented": int(segments.sizes["time"])},
        "variables": {},
    }

    failures = []
    if continuous.sizes["time"] != segments.sizes["time"]:
        summary["status"] = "FAIL"
        summary["reason"] = "time dimension differs"
        print(json.dumps(summary, indent=2))
        return 1

    for name in VARIABLES:
        if name not in continuous or name not in segments:
            summary["variables"][name] = {"status": "MISSING"}
            failures.append(name)
            continue
        av = np.asarray(continuous[name].values, dtype="f8")
        bv = np.asarray(segments[name].values, dtype="f8")
        valid = ~(np.isnan(av) | np.isnan(bv))
        bitwise = bool(np.array_equal(av[valid].view(np.uint64),
                                      bv[valid].view(np.uint64)))
        diff = np.abs(av[valid] - bv[valid])
        entry = {
            "status": "BITWISE_IDENTICAL" if bitwise else "DIFFERENT",
            "bitwise_identical": bitwise,
            "n_compared": int(valid.sum()),
            "max_abs_diff": float(diff.max()) if diff.size else 0.0,
        }
        summary["variables"][name] = entry
        if not bitwise:
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
