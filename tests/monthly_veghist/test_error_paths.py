#!/usr/bin/env python
"""Check that malformed monthly veg-history configurations fail loudly.

Every case here is expected to make VIC exit non-zero with a specific message.
A case that runs to completion is a failure of the test, because it means a
broken dataset would have produced silent nonsense.

The one exception is the FillValue case: per the agreed contract, individual
bad values on active tiles are a warning plus a fall back to the parameter
climatology, not a hard stop.  That case therefore expects success plus a
specific warning.

Usage:
    python test_error_paths.py --case <dir> --exe <vic_image.exe> \
        [--json <path>]
"""
import argparse
import json
import os
import shutil
import subprocess
import sys

import numpy as np
import xarray as xr

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import site_config  # noqa: E402

YEAR = 2003


def run_vic(exe, global_file, log_path):
    return site_config.run_vic(exe, global_file, log_path)


def write_global(base_global, dst, replacements, drop_prefixes=()):
    out = []
    for line in open(base_global):
        tokens = line.split()
        key = tokens[0] if tokens and not line.startswith("#") else None
        if key == "FORCE_TYPE" and len(tokens) > 1 and tokens[1] in drop_prefixes:
            continue
        if key in replacements:
            value = replacements.pop(key)
            if value is not None:
                out.append("%-23s %s\n" % (key, value))
            continue
        out.append(line)
    for key, value in replacements.items():
        if value is not None:
            out.append("%-23s %s\n" % (key, value))
    with open(dst, "w") as handle:
        handle.writelines(out)


def make_broken_veghist(src_prefix, dst_prefix, mutate):
    """Copy the good monthly file for YEAR and apply a mutation."""
    src = "%s%d.nc" % (src_prefix, YEAR)
    dst = "%s%d.nc" % (dst_prefix, YEAR)
    # Keep time as raw numbers with units/calendar in attrs: these mutations
    # operate on the on-disk representation, and decoding would make xarray
    # fight us over the calendar attribute on write.
    ds = xr.open_dataset(src, decode_times=False).load()
    ds = mutate(ds)
    if os.path.exists(dst):
        os.remove(dst)
    ds.to_netcdf(dst)
    ds.close()
    # The two-year case needs the second year to exist too; copy it unchanged.
    src2 = "%s%d.nc" % (src_prefix, YEAR + 1)
    dst2 = "%s%d.nc" % (dst_prefix, YEAR + 1)
    if os.path.exists(src2):
        shutil.copyfile(src2, dst2)
    return dst_prefix


def drop_a_month(ds):
    return ds.isel(time=[i for i in range(12) if i != 6])


def duplicate_a_month(ds):
    return ds.isel(time=[0, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11])


def shuffle_months(ds):
    return ds.isel(time=[0, 2, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11])


def wrong_calendar(ds):
    ds["time"].attrs["calendar"] = "noleap"
    return ds


def wrong_veg_class(ds):
    return ds.isel(veg_class=slice(0, 5))


def make_inject_fill(active_tiles):
    """Corrupt LAI on tiles that are genuinely active (Cv > 0).

    Bad values on tiles with no vegetation allocation are skipped by VIC before
    validation, so the corruption has to land on an allocated tile for the
    fallback path to be exercised at all.
    """
    def inject_fill(ds):
        lai = ds["lai"].values.copy()
        (v0, la0, lo0), (v1, la1, lo1) = active_tiles[:2]
        lai[3, v0, la0, lo0] = 9.96920996838687e+36   # unmasked _FillValue
        lai[4, v1, la1, lo1] = np.nan                 # NaN
        ds["lai"] = (ds["lai"].dims, lai)
        return ds
    return inject_fill


def find_active_tiles(params_path, count=2):
    """Return (veg_class, lat, lon) triples where Cv > 0."""
    par = xr.open_dataset(params_path)
    cv = par["Cv"].values
    par.close()
    idx = np.argwhere(np.nan_to_num(cv) > 0)
    if len(idx) < count:
        raise RuntimeError("Need at least %d active tiles, found %d"
                           % (count, len(idx)))
    return [tuple(int(x) for x in row) for row in idx[:count]]


def build_cases(params_path):
    """(name, mutation, expect_nonzero_exit, expected_log_text)"""
    return [
        ("missing_month", drop_a_month, True, "exactly 12"),
        ("duplicate_month", duplicate_a_month, True, "in order"),
        ("out_of_order_month", shuffle_months, True, "in order"),
        ("calendar_mismatch", wrong_calendar, True, "calendar"),
        ("wrong_veg_class", wrong_veg_class, True, None),
        ("fillvalue_on_active_tile",
         make_inject_fill(find_active_tiles(params_path)), False,
         "falling back to the parameter climatology"),
    ]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--case", required=True)
    parser.add_argument("--exe", required=True)
    parser.add_argument("--json", default=None)
    args = parser.parse_args()

    case = os.path.abspath(args.case)
    base_global = os.path.join(case, "global_monthly.txt")
    good_prefix = os.path.join(case, "veghist_monthly_")
    work = os.path.join(case, "error_paths")
    os.makedirs(work, exist_ok=True)


    results = {}
    failures = []

    # --- malformed monthly files ------------------------------------------
    for name, mutate, expect_rc, expect_text in build_cases(
            os.path.join(case, "params.nc")):
        prefix = os.path.join(work, "%s_" % name)
        make_broken_veghist(good_prefix, prefix, mutate)

        result_dir = os.path.join(work, "result_%s" % name)
        os.makedirs(result_dir, exist_ok=True)
        gfile = os.path.join(work, "global_%s.txt" % name)
        text = open(base_global).read()
        text = text.replace(good_prefix, prefix)
        text = text.replace(os.path.join(case, "result_monthly"), result_dir)
        with open(gfile, "w") as handle:
            handle.write(text)

        rc, log = run_vic(args.exe, gfile,
                          os.path.join(work, "log_%s.txt" % name))
        ok = (rc != 0) if expect_rc else (rc == 0)
        if expect_text and expect_text.lower() not in log.lower():
            ok = False
        results[name] = {
            "expected_failure": bool(expect_rc),
            "returncode": rc,
            "expected_text": expect_text,
            "text_found": bool(expect_text) and expect_text.lower() in log.lower(),
            "passed": ok,
        }
        if not ok:
            failures.append(name)

    # --- malformed configuration ------------------------------------------
    config_cases = [
        ("month_on_precip",
         lambda t: t.replace(
             "FORCE_TYPE              PREC        prec      ",
             "FORCE_TYPE              PREC        prec      MONTH     "),
         "only supported for LAI"),
        ("month_without_veghist_src",
         lambda t: t.replace("LAI_SRC                 FROM_VEGHIST",
                             "LAI_SRC                 FROM_VEGPARAM"),
         "not FROM_VEGHIST"),
        ("unknown_frequency",
         lambda t: t.replace(
             "FORCE_TYPE              LAI         lai       MONTH    ",
             "FORCE_TYPE              LAI         lai       WEEK     "),
         "Undefined forcing frequency"),
        ("day_frequency_not_implemented",
         lambda t: t.replace(
             "FORCE_TYPE              LAI         lai       MONTH    ",
             "FORCE_TYPE              LAI         lai       DAY      "),
         "not implemented"),
    ]

    # Trailing comments are legal on FORCE_TYPE lines and must not be mistaken
    # for a frequency token now that the layout is chosen by token count.
    # Both of these are expected to SUCCEED.
    comment_cases = [
        ("comment_after_legacy_force_type", "WIND"),
        ("comment_after_monthly_force_type", "LAI"),
    ]

    for name, mutate, expect_text in config_cases:
        result_dir = os.path.join(work, "result_%s" % name)
        os.makedirs(result_dir, exist_ok=True)
        gfile = os.path.join(work, "global_%s.txt" % name)
        text = mutate(open(base_global).read())
        text = text.replace(os.path.join(case, "result_monthly"), result_dir)
        with open(gfile, "w") as handle:
            handle.write(text)

        rc, log = run_vic(args.exe, gfile,
                          os.path.join(work, "log_%s.txt" % name))
        found = expect_text.lower() in log.lower()
        ok = rc != 0 and found
        results[name] = {
            "expected_failure": True,
            "returncode": rc,
            "expected_text": expect_text,
            "text_found": found,
            "passed": ok,
        }
        if not ok:
            failures.append(name)

    for name, variable in comment_cases:
        result_dir = os.path.join(work, "result_%s" % name)
        os.makedirs(result_dir, exist_ok=True)
        gfile = os.path.join(work, "global_%s.txt" % name)

        lines = []
        for line in open(base_global):
            tokens = line.split()
            if (len(tokens) > 1 and tokens[0] == "FORCE_TYPE"
                    and tokens[1] == variable):
                line = line.rstrip("\n") + "   # trailing comment\n"
            lines.append(line)
        text = "".join(lines).replace(
            os.path.join(case, "result_monthly"), result_dir)
        with open(gfile, "w") as handle:
            handle.write(text)

        rc, log = run_vic(args.exe, gfile,
                          os.path.join(work, "log_%s.txt" % name))
        ok = rc == 0
        results[name] = {
            "expected_failure": False,
            "returncode": rc,
            "expected_text": None,
            "text_found": False,
            "passed": ok,
        }
        if not ok:
            failures.append(name)

    summary = {"case": case, "cases": results,
               "status": "PASS" if not failures else "FAIL",
               "failing_cases": failures}

    if args.json:
        with open(args.json, "w") as handle:
            json.dump(summary, handle, indent=2)
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    sys.exit(main())
