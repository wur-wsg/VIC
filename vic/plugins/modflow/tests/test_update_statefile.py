"""update_statefile: the capillary rise written into the VIC state must be the
cell-mean depth MODFLOW removed.

STATE_SOIL_MOISTURE is per tile (mm); the cell mean is sum(Cv * tile). So a
capillary rise of cpr mm must raise every present tile by cpr, leave absent
tiles (Cv == 0, state fill value) untouched, leave other snow bands untouched,
and when layer 3 saturates the excess must move to layer 2 without loss.

Run:  python -m pytest vic/plugins/modflow/tests/test_update_statefile.py
"""
import os
import sys
from datetime import datetime
from types import SimpleNamespace

import netCDF4 as nc
import numpy as np
import xarray as xr

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))
import vic_runner  # noqa: E402

FILL = 9.96920996838687e+36


def _make_case(tmp_path):
    nveg, nband, nlay, nlat, nlon = 2, 2, 3, 2, 2
    cv = np.zeros((nveg, nlat, nlon))
    cv[:, 0, 0] = [0.7, 0.3]     # two tiles
    cv[:, 0, 1] = [1.0, 0.0]     # one tile, one absent
    cv[:, 1, 0] = [0.5, 0.5]     # no capillary rise this month
    cv[:, 1, 1] = [0.6, 0.4]     # will saturate layer 3
    cpr = np.array([[5.0, 3.0], [0.0, 50.0]])
    max_moist = np.zeros((nlay, nlat, nlon))
    max_moist[0] = 130.0
    max_moist[1] = 700.0
    max_moist[2] = 100.0

    state = np.full((nveg, nband, nlay, nlat, nlon), 20.0)
    state[:, :, 1, :, :] = 300.0
    state[:, :, 2, :, :] = 60.0
    state[:, :, 2, 1, 1] = 90.0            # 90 + 50 = 140 > 100 -> excess 40 to layer 2
    state[1, :, :, 0, 1] = FILL             # absent tile

    statefile_dir = str(tmp_path)
    fname = os.path.join(statefile_dir, 'nat_foc_state_file_.19790201_00000.nc')
    with nc.Dataset(fname, 'w') as ds:
        for name, n in (('veg_class', nveg), ('snow_band', nband), ('nlayer', nlay), ('lat', nlat), ('lon', nlon)):
            ds.createDimension(name, n)
        v = ds.createVariable('STATE_SOIL_MOISTURE', 'f8', ('veg_class', 'snow_band', 'nlayer', 'lat', 'lon'), fill_value=FILL)
        v[:] = np.ma.masked_equal(state, FILL)

    config = SimpleNamespace(
        modestr='nat', couplingstr='foc',
        paths=SimpleNamespace(
            vic_parameter=xr.Dataset({'Cv': (('veg_class', 'lat', 'lon'), cv)}),
            capillary=xr.Dataset({'max_moist': (('nlayer', 'lat', 'lon'), max_moist)}),
            statefile_dir=statefile_dir))
    return fname, config, cv, cpr, state, max_moist


def _cell_mean(arr, cv):
    """Cv-weighted mean over tiles for band 0, per layer -> (nlay, nlat, nlon)."""
    a = np.where(np.isnan(arr[:, 0]), 0.0, arr[:, 0])
    return np.einsum('vlyx,vyx->lyx', a, cv)


def test_capillary_rise_is_conserved_per_cell(tmp_path):
    fname, config, cv, cpr, before, max_moist = _make_case(tmp_path)
    vic_runner.update_statefile(datetime(1979, 1, 1), config, cpr)
    with nc.Dataset(fname) as ds:
        after = np.ma.filled(ds.variables['STATE_SOIL_MOISTURE'][:].astype(float), np.nan)
    before_nan = np.where(before == FILL, np.nan, before)

    delta = _cell_mean(after, cv) - _cell_mean(before_nan, cv)
    # what the cell received, summed over layers, equals the capillary rise
    np.testing.assert_allclose(delta.sum(axis=0), cpr, atol=1e-9)
    # unsaturated cells: all of it sits in layer 3
    np.testing.assert_allclose(delta[2, 0, :], cpr[0, :], atol=1e-9)
    # the saturating cell: layer 3 capped at max_moist, the excess moved up
    np.testing.assert_allclose(_cell_mean(after, cv)[2, 1, 1], max_moist[2, 1, 1], atol=1e-9)
    np.testing.assert_allclose(delta[1, 1, 1], 140.0 - 100.0, atol=1e-9)
    # the cell without capillary rise is untouched
    np.testing.assert_array_equal(after[:, :, :, 1, 0], before_nan[:, :, :, 1, 0])


def test_absent_tiles_and_other_bands_untouched(tmp_path):
    fname, config, cv, cpr, before, _ = _make_case(tmp_path)
    vic_runner.update_statefile(datetime(1979, 1, 1), config, cpr)
    with nc.Dataset(fname) as ds:
        raw = ds.variables['STATE_SOIL_MOISTURE']
        raw.set_auto_mask(False)
        after = raw[:]
    assert np.all(after[1, :, :, 0, 1] == FILL)                       # absent tile still fill
    np.testing.assert_array_equal(after[:, 1], before[:, 1])          # band 1 untouched
    assert np.all(after[before != FILL] < 1e30)                       # no fill leaked into present tiles


if __name__ == '__main__':
    import tempfile
    for t in (test_capillary_rise_is_conserved_per_cell, test_absent_tiles_and_other_bands_untouched):
        with tempfile.TemporaryDirectory() as d:
            t(__import__('pathlib').Path(d))
            print(t.__name__, 'ok')
