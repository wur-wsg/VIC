#!/usr/bin/env python
"""Build a tiny self-contained VIC image-driver case for the monthly
vegetation-history forcing interface.

The case is deliberately small (a few active cells, two years) so that it can be
run repeatedly on a login node or a single Slurm core.  It produces two forcing
sets that are numerically identical by construction:

  A. monthly veg-history files   <prefix>YYYY.nc with time = 12
  B. daily-expanded veg-history  <prefix>YYYY.nc with time = 365 or 366,
                                 each day carrying its month's value

Running VIC with A and with B must give bitwise-identical output.  Because both
files store the same float32 bit patterns and VIC widens them to double on read,
any difference points at a real indexing bug rather than at floating-point noise.

The domain and parameter files are cut out of an existing, known-good VIC
parameter set rather than synthesised field by field, so the case is guaranteed
to be internally consistent.  The source files are only ever read.

Usage:
    python make_synthetic_case.py --outdir <dir>
"""
import argparse
import os

import numpy as np
import xarray as xr

def template_inputs(args):
    """Known-good full-size inputs, used read-only as a structural template.

    These are machine-specific, so they come from the site profile rather than
    from this file.  See sites/anunna.sh.
    """
    domain = args.template_domain or os.environ.get("VIC_TEST_TEMPLATE_DOMAIN")
    params = args.template_params or os.environ.get("VIC_TEST_TEMPLATE_PARAMS")
    missing = [name for name, value in
               (("--template-domain / VIC_TEST_TEMPLATE_DOMAIN", domain),
                ("--template-params / VIC_TEST_TEMPLATE_PARAMS", params))
               if not value]
    if missing:
        raise SystemExit(
            "Missing template input(s): %s\n"
            "Source a site profile first, e.g. `source sites/anunna.sh`.\n"
            "Sites without the template inputs (Snellius) should copy a case "
            "built elsewhere rather than regenerating it." % ", ".join(missing))
    return domain, params

# 2003 is a common year and 2004 a leap year, so a two-year case exercises the
# December-to-January rollover, a 28-day February and a 29-day February.
YEARS = [2003, 2004]
CALENDAR = "proleptic_gregorian"
TIME_UNITS = "days since 1900-01-01 00:00:00"

# Size of the cut-out box.  Kept tiny on purpose.
NLAT = 4
NLON = 4


def days_in_year(year):
    return 366 if (year % 4 == 0 and (year % 100 != 0 or year % 400 == 0)) else 365


def month_lengths(year):
    base = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
    if days_in_year(year) == 366:
        base[1] = 29
    return base


def days_since_epoch(year, month, day):
    """Days since 1900-01-01 in the proleptic Gregorian calendar."""
    a = (14 - month) // 12
    y = year + 4800 - a
    m = month + 12 * a - 3
    jdn = (day + (153 * m + 2) // 5 + 365 * y + y // 4 - y // 100 + y // 400
           - 32045)
    jdn_epoch = 2415021  # 1900-01-01
    return jdn - jdn_epoch


def pick_active_box(domain):
    """Find a NLAT x NLON window that is fully inside the active mask."""
    mask = np.nan_to_num(domain["mask"].values, nan=0).astype(int)
    nlat, nlon = mask.shape
    for i in range(nlat - NLAT):
        for j in range(nlon - NLON):
            if mask[i:i + NLAT, j:j + NLON].all():
                return i, j
    raise RuntimeError("No fully active %dx%d window found in the source domain"
                       % (NLAT, NLON))


def write_domain_and_params(outdir, src_domain, src_params):
    src_dom = xr.open_dataset(src_domain)
    src_par = xr.open_dataset(src_params)

    i0, j0 = pick_active_box(src_dom)
    sel = dict(lat=slice(i0, i0 + NLAT), lon=slice(j0, j0 + NLON))

    dom = src_dom.isel(**sel).load()
    par = src_par.isel(**sel).load()

    # Every cell in the cut-out is active, so run_cell must agree with the mask.
    # VIC requires run_cell to stay an integer variable, so rebuild it rather
    # than using .where(), which would promote it to float.
    if "run_cell" in par:
        par["run_cell"] = xr.DataArray(
            np.ones(par["run_cell"].shape, dtype="i4"),
            dims=par["run_cell"].dims,
            attrs=par["run_cell"].attrs,
        )
        par["run_cell"].encoding["dtype"] = "i4"

    dom_path = os.path.join(outdir, "domain.nc")
    par_path = os.path.join(outdir, "params.nc")
    dom.to_netcdf(dom_path)
    par.to_netcdf(par_path)

    nveg = int(par.sizes["veg_class"])
    lat = par["lat"].values
    lon = par["lon"].values

    src_dom.close()
    src_par.close()
    return dom_path, par_path, nveg, lat, lon


def time_coord(values, name="time"):
    return xr.DataArray(
        np.asarray(values, dtype="f8"),
        dims=(name,),
        attrs={"units": TIME_UNITS, "calendar": CALENDAR},
    )


def write_met_forcing(outdir, lat, lon):
    """Daily meteorology.  Values vary smoothly in time so that the run is not
    degenerate, but they are identical between the two experiments."""
    prefix = os.path.join(outdir, "met_")
    for year in YEARS:
        nday = days_in_year(year)
        t = np.array([days_since_epoch(year, 1, 1) + d for d in range(nday)],
                     dtype="f8")
        doy = np.arange(nday, dtype="f4")
        season = np.sin(2 * np.pi * doy / nday).astype("f4")

        shape = (nday, len(lat), len(lon))
        ones = np.ones(shape, dtype="f4")
        seas = season[:, None, None] * ones

        ds = xr.Dataset(
            {
                "tair": (("time", "lat", "lon"), (15.0 + 10.0 * seas).astype("f4")),
                "prec": (("time", "lat", "lon"), (2.0 + 2.0 * np.abs(seas)).astype("f4")),
                "psurf": (("time", "lat", "lon"), (95.0 * ones).astype("f4")),
                "swdown": (("time", "lat", "lon"), (150.0 + 100.0 * seas).astype("f4")),
                "lwdown": (("time", "lat", "lon"), (300.0 + 30.0 * seas).astype("f4")),
                "vp": (("time", "lat", "lon"), (1.0 + 0.5 * np.abs(seas)).astype("f4")),
                "wind": (("time", "lat", "lon"), (2.5 * ones).astype("f4")),
            },
            coords={"time": time_coord(t), "lat": lat, "lon": lon},
        )
        ds.to_netcdf("%s%d.nc" % (prefix, year))
    return prefix


def monthly_veghist_values(year, nveg, nlat, nlon):
    """The single source of truth for both experiments.

    Returns float32 arrays of shape (12, nveg, nlat, nlon).  Everything else is
    derived from these exact bit patterns, which is what makes bitwise
    comparison meaningful.
    """
    rng = np.random.default_rng(seed=year)
    lai = rng.uniform(0.1, 6.0, size=(12, nveg, nlat, nlon)).astype("f4")
    fcan = rng.uniform(0.2, 0.95, size=(12, nveg, nlat, nlon)).astype("f4")
    alb = rng.uniform(0.10, 0.30, size=(12, nveg, nlat, nlon)).astype("f4")
    return lai, fcan, alb


def write_veghist(outdir, nveg, lat, lon):
    """Write experiment A (monthly) and experiment B (daily-expanded)."""
    monthly_prefix = os.path.join(outdir, "veghist_monthly_")
    daily_prefix = os.path.join(outdir, "veghist_daily_")

    for year in YEARS:
        lai, fcan, alb = monthly_veghist_values(year, nveg, len(lat), len(lon))

        # --- A: one record per month, timestamped at the first of the month ---
        t_month = np.array([days_since_epoch(year, m, 1) for m in range(1, 13)],
                           dtype="f8")
        dims = ("time", "veg_class", "lat", "lon")
        xr.Dataset(
            {"lai": (dims, lai), "fcanopy": (dims, fcan), "albedo": (dims, alb)},
            coords={
                "time": time_coord(t_month),
                "veg_class": np.arange(nveg, dtype="i4"),
                "lat": lat,
                "lon": lon,
            },
        ).to_netcdf("%s%d.nc" % (monthly_prefix, year))

        # --- B: the same values copied to every day of their month ---
        lengths = month_lengths(year)
        day_index = np.concatenate(
            [np.full(n, m, dtype=int) for m, n in enumerate(lengths)])
        nday = days_in_year(year)
        assert day_index.size == nday

        t_day = np.array([days_since_epoch(year, 1, 1) + d for d in range(nday)],
                         dtype="f8")
        xr.Dataset(
            {
                "lai": (dims, lai[day_index]),
                "fcanopy": (dims, fcan[day_index]),
                "albedo": (dims, alb[day_index]),
            },
            coords={
                "time": time_coord(t_day),
                "veg_class": np.arange(nveg, dtype="i4"),
                "lat": lat,
                "lon": lon,
            },
        ).to_netcdf("%s%d.nc" % (daily_prefix, year))

    return monthly_prefix, daily_prefix


GLOBAL_TEMPLATE = """\
# Minimal VIC image-driver case for the monthly veg-history interface.
# Generated by make_synthetic_case.py -- do not edit by hand.
MODEL_STEPS_PER_DAY     1
SNOW_STEPS_PER_DAY      1
RUNOFF_STEPS_PER_DAY    1
STARTYEAR               {startyear}
STARTMONTH              {startmonth}
STARTDAY                {startday}
ENDYEAR                 {endyear}
ENDMONTH                {endmonth}
ENDDAY                  {endday}
CALENDAR                PROLEPTIC_GREGORIAN

FULL_ENERGY             FALSE
CLOSE_ENERGY            FALSE
FROZEN_SOIL             FALSE
SNOW_BAND               TRUE
BASEFLOW                ARNO
{state_lines}
DOMAIN                  {domain}
DOMAIN_TYPE             LAT     lat
DOMAIN_TYPE             LON     lon
DOMAIN_TYPE             MASK    mask
DOMAIN_TYPE             AREA    area
DOMAIN_TYPE             FRAC    frac
DOMAIN_TYPE             YDIM    lat
DOMAIN_TYPE             XDIM    lon

FORCE_TYPE              AIR_TEMP    tair      {met}
FORCE_TYPE              PREC        prec      {met}
FORCE_TYPE              PRESSURE    psurf     {met}
FORCE_TYPE              SWDOWN      swdown    {met}
FORCE_TYPE              LWDOWN      lwdown    {met}
FORCE_TYPE              VP          vp        {met}
FORCE_TYPE              WIND        wind      {met}
FORCE_TYPE              LAI         lai       {freq}{veghist}
FORCE_TYPE              FCANOPY     fcanopy   {freq}{veghist}
FORCE_TYPE              ALBEDO      albedo    {freq}{veghist}

PARAMETERS              {params}
LAI_SRC                 FROM_VEGHIST
FCAN_SRC                FROM_VEGHIST
ALB_SRC                 FROM_VEGHIST
WFC_SRC                 FROM_VEGPARAM
NBARE                   1

RESULT_DIR              {resultdir}
OUTFILE                 {outfile}
OUT_FORMAT              NETCDF4
COMPRESS                FALSE
AGGFREQ                 NDAYS 1
OUTVAR                  OUT_LAI
OUTVAR                  OUT_FCANOPY
OUTVAR                  OUT_ALBEDO
OUTVAR                  OUT_SWNET
OUTVAR                  OUT_LWNET
OUTVAR                  OUT_LATENT
OUTVAR                  OUT_SENSIBLE
OUTVAR                  OUT_EVAP
OUTVAR                  OUT_RUNOFF
OUTVAR                  OUT_BASEFLOW
OUTVAR                  OUT_WATER_ERROR
OUTVAR                  OUT_ENERGY_ERROR
"""


def write_global(path, *, domain, params, met, veghist, freq, resultdir,
                 outfile, start, end, state_lines=""):
    text = GLOBAL_TEMPLATE.format(
        domain=domain,
        params=params,
        met=met,
        veghist=veghist,
        # Line up the optional frequency token without disturbing the legacy
        # three-token layout when it is absent.
        freq=("%-9s " % freq) if freq else "",
        resultdir=resultdir,
        outfile=outfile,
        startyear=start[0], startmonth=start[1], startday=start[2],
        endyear=end[0], endmonth=end[1], endday=end[2],
        state_lines=state_lines,
    )
    with open(path, "w") as handle:
        handle.write(text)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--outdir", required=True)
    parser.add_argument("--template-domain", default=None,
                        help="defaults to $VIC_TEST_TEMPLATE_DOMAIN")
    parser.add_argument("--template-params", default=None,
                        help="defaults to $VIC_TEST_TEMPLATE_PARAMS")
    args = parser.parse_args()

    src_domain, src_params = template_inputs(args)

    outdir = os.path.abspath(args.outdir)
    os.makedirs(outdir, exist_ok=True)

    domain, params, nveg, lat, lon = write_domain_and_params(
        outdir, src_domain, src_params)
    met = write_met_forcing(outdir, lat, lon)
    monthly, daily = write_veghist(outdir, nveg, lat, lon)

    start = (YEARS[0], 1, 1)
    end = (YEARS[-1], 12, 31)

    for tag, veghist, freq in (("monthly", monthly, "MONTH"),
                               ("daily", daily, "")):
        resultdir = os.path.join(outdir, "result_%s" % tag)
        os.makedirs(resultdir, exist_ok=True)
        write_global(
            os.path.join(outdir, "global_%s.txt" % tag),
            domain=domain, params=params, met=met, veghist=veghist, freq=freq,
            resultdir=resultdir, outfile="veghist_%s" % tag,
            start=start, end=end,
        )

    print("Synthetic case written to %s" % outdir)
    print("  domain     : %s" % domain)
    print("  params     : %s (veg_class=%d, %dx%d cells)"
          % (params, nveg, len(lat), len(lon)))
    print("  met prefix : %s" % met)
    print("  monthly    : %s" % monthly)
    print("  daily      : %s" % daily)
    print("  years      : %s" % YEARS)


if __name__ == "__main__":
    main()
