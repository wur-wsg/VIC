"""update_statefile: the capillary rise written into the VIC state must be the
cell-mean depth MODFLOW removed.

STATE_SOIL_MOISTURE is per tile (mm); the cell mean is sum(Cv * tile). So a
capillary rise of cpr mm must raise every present tile in every present snow
band by cpr, leave absent tiles and absent bands (state fill value) untouched,
cap only cells that received capillary rise, and when layer 3 saturates move
the excess to layer 2 without loss.

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
    cv[:, 0, 0] = [0.7, 0.3]     # two tiles, two bands
    cv[:, 0, 1] = [1.0, 0.0]     # one tile, one absent; second band absent
    cv[:, 1, 0] = [0.5, 0.5]     # no capillary rise, and layer 3 already above max_moist
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
    state[:, :, 2, 1, 0] = 110.0           # above max_moist on its own (ice); must be left alone
    state[1, :, :, 0, 1] = FILL             # absent tile
    state[:, 1, :, 0, 1] = FILL             # absent band

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


def _band_mean(arr, cv, band):
    """Cv-weighted mean over tiles for one band, per layer -> (nlay, nlat, nlon)."""
    a = np.where(np.isnan(arr[:, band]), 0.0, arr[:, band])
    return np.einsum('vlyx,vyx->lyx', a, cv)


def _run(tmp_path):
    fname, config, cv, cpr, before, max_moist = _make_case(tmp_path)
    vic_runner.update_statefile(datetime(1979, 1, 1), config, cpr)
    with nc.Dataset(fname) as ds:
        raw = ds.variables['STATE_SOIL_MOISTURE']
        raw.set_auto_mask(False)
        after_raw = raw[:]
    after = np.where(after_raw == FILL, np.nan, after_raw)
    before_nan = np.where(before == FILL, np.nan, before)
    return cv, cpr, before, before_nan, after, after_raw, max_moist


def test_capillary_rise_is_conserved_per_cell_and_band(tmp_path):
    cv, cpr, before, before_nan, after, _, max_moist = _run(tmp_path)
    for band in range(2):
        delta = _band_mean(after, cv, band) - _band_mean(before_nan, cv, band)
        band_exists = ~np.isnan(before_nan[0, band, 2])           # cell (0,1) has no band 1
        expected = np.where(band_exists, cpr, 0.0)
        # what each present band received, summed over layers, equals the capillary rise
        np.testing.assert_allclose(delta.sum(axis=0), expected, atol=1e-9)
        # unsaturated cells: all of it sits in layer 3
        np.testing.assert_allclose(delta[2, 0, :], expected[0, :], atol=1e-9)
        # the saturating cell: layer 3 capped at max_moist, the excess moved up
        np.testing.assert_allclose(_band_mean(after, cv, band)[2, 1, 1], max_moist[2, 1, 1], atol=1e-9)
        np.testing.assert_allclose(delta[1, 1, 1], 140.0 - 100.0, atol=1e-9)


def test_cells_without_capillary_rise_are_left_alone(tmp_path):
    cv, cpr, before, before_nan, after, _, _ = _run(tmp_path)
    # cell (1,0): no capillary rise and layer 3 already above max_moist -> untouched, not capped
    np.testing.assert_array_equal(after[:, :, :, 1, 0], before_nan[:, :, :, 1, 0])


def test_absent_tiles_and_bands_keep_fill(tmp_path):
    cv, cpr, before, before_nan, after, after_raw, _ = _run(tmp_path)
    assert np.all(after_raw[1, :, :, 0, 1] == FILL)                    # absent tile still fill
    assert np.all(after_raw[:, 1, :, 0, 1] == FILL)                    # absent band still fill
    assert np.all(after_raw[before != FILL] < 1e30)                    # no fill leaked into present entries
    assert np.array_equal(after_raw == FILL, before == FILL)           # fill pattern unchanged


if __name__ == '__main__':
    import tempfile
    for t in (test_capillary_rise_is_conserved_per_cell_and_band, test_cells_without_capillary_rise_are_left_alone,
              test_absent_tiles_and_bands_keep_fill):
        with tempfile.TemporaryDirectory() as d:
            t(__import__('pathlib').Path(d))
            print(t.__name__, 'ok')
