"""Prepare monthly VIC groundwater demand for the MODFLOW WEL package."""

from pathlib import Path

import numpy as np
import xarray as xr


DEMAND_VARIABLE = 'OUT_DEMAND'
SUPPLY_VARIABLES = (
    'OUT_WI_SURF_SECT',
    'OUT_WI_DAM_SECT',
    'OUT_WI_TREM_SECT',
    'OUT_WI_GW_SECT',
    'OUT_WI_NREN_SECT',
)
NEGATIVE_RESIDUAL_TOLERANCE_MM = 1.0e-4


def _monthly_2d(dataset, variable):
    if variable not in dataset:
        raise KeyError(f'Required VIC variable {variable} is missing')
    data = dataset[variable]
    if data.attrs.get('units') != 'mm':
        raise ValueError(f'{variable} units must be mm, found {data.attrs.get("units")!r}')
    if 'time' in data.dims:
        if data.sizes['time'] != 1:
            raise ValueError(f'{variable} must have one monthly time record')
        data = data.isel(time=0, drop=True)
    extra = [dim for dim in data.dims if dim not in ('lat', 'lon')]
    if extra:
        raise ValueError(f'{variable} has unexpected dimensions {extra}')
    return data.transpose('lat', 'lon')


def _monthly_sector_total(dataset, variable):
    if variable not in dataset:
        raise KeyError(f'Required VIC supply variable {variable} is missing')
    data = dataset[variable]
    if data.attrs.get('units') != 'mm':
        raise ValueError(f'{variable} units must be mm, found {data.attrs.get("units")!r}')
    if 'time' in data.dims:
        if data.sizes['time'] != 1:
            raise ValueError(f'{variable} must have one monthly time record')
        data = data.isel(time=0, drop=True)
    sector_dims = [dim for dim in data.dims if dim not in ('lat', 'lon')]
    if not sector_dims:
        raise ValueError(f'{variable} has no sector dimension')
    return data.sum(dim=sector_dims, skipna=True, keep_attrs=True).transpose('lat', 'lon')


def calculate_unmet_water_demand(dataset, pumping_mask):
    """Return VIC demand not met by its actual supply sources in mm/month."""
    demand = _monthly_2d(dataset, DEMAND_VARIABLE)
    shape = demand.shape
    pumping_mask = np.asarray(pumping_mask, dtype=bool)
    if pumping_mask.shape != shape:
        raise ValueError(f'pumping_mask must match VIC grid {shape}; found {pumping_mask.shape}')
    supply = xr.zeros_like(demand)
    for variable in SUPPLY_VARIABLES:
        supply = supply + _monthly_sector_total(dataset, variable).fillna(0)
    residual = demand.fillna(0) - supply
    minimum = float(residual.min(skipna=True).item())
    if minimum < -NEGATIVE_RESIDUAL_TOLERANCE_MM:
        raise ValueError(f'Demand minus actual supply is negative ({minimum:.6g} mm)')
    unmet_mm = residual.clip(min=0).where(pumping_mask)
    unmet_mm.name = 'unmet_water_demand'
    unmet_mm.attrs = {
        'units': 'mm/month',
        'long_name': 'water demand not met by VIC actual supply sources',
        'formula': 'max(OUT_DEMAND - sum(actual supply sources), 0)',
    }
    return unmet_mm


def prepare_uncapped_abstraction(dataset, cell_area, pumping_mask, days_in_month):
    """Return unmet demand in mm/month and positive pumping demand in m3/day."""
    if days_in_month <= 0:
        raise ValueError('days_in_month must be positive')
    unmet_mm = calculate_unmet_water_demand(dataset, pumping_mask)
    cell_area = np.asarray(cell_area, dtype=float)
    if cell_area.shape != unmet_mm.shape:
        raise ValueError(
            f'cell_area must match VIC grid {unmet_mm.shape}; found {cell_area.shape}'
        )
    abstraction = unmet_mm * 1.0e-3 * cell_area / days_in_month
    abstraction = abstraction.fillna(0).astype(np.float64)
    abstraction.name = 'groundwater_abstraction'
    abstraction.attrs = {
        'units': 'm3/day',
        'long_name': 'monthly mean unmet VIC demand assigned to groundwater pumping',
        'formula': 'max(OUT_DEMAND - sum(actual supply sources), 0) / 1000 * cell_area / days_in_month',
    }
    return unmet_mm, abstraction


def calculate_uncapped_abstraction(dataset, cell_area, pumping_mask, days_in_month):
    """Return positive monthly-mean groundwater demand in m3/day."""
    return prepare_uncapped_abstraction(
        dataset,
        cell_area,
        pumping_mask,
        days_in_month,
    )[1]


def apply_dynamic_capacity(abstraction, capacity_file, current_date):
    """Limit positive abstraction with the matching monthly capacity field."""
    capacity_file = Path(capacity_file)
    if not capacity_file.exists():
        raise FileNotFoundError(f'Pumping capacity file does not exist: {capacity_file}')
    with xr.open_dataset(capacity_file) as dataset:
        if 'pumping_capacity' not in dataset:
            raise KeyError(f'pumping_capacity is missing from {capacity_file}')
        capacity = dataset['pumping_capacity']
        if capacity.attrs.get('units') != 'm3/day':
            raise ValueError(
                f'pumping_capacity units must be m3/day, found {capacity.attrs.get("units")!r}'
            )
        if not np.array_equal(dataset.lat.values, abstraction.lat.values):
            raise ValueError('Pumping capacity latitude does not match the VIC/MODFLOW grid')
        if not np.array_equal(dataset.lon.values, abstraction.lon.values):
            raise ValueError('Pumping capacity longitude does not match the VIC/MODFLOW grid')
        timestamp = np.datetime64(current_date.strftime('%Y-%m-01'))
        available = dataset.time.values.astype('datetime64[D]')
        matches = np.flatnonzero(available == timestamp)
        if matches.size != 1:
            raise KeyError(
                f'Expected one pumping capacity record for {current_date:%Y-%m}, '
                f'found {matches.size}'
            )
        capacity = capacity.isel(time=int(matches[0])).load().transpose('lat', 'lon')
    if bool(capacity.isnull().any().item()):
        raise ValueError(f'Pumping capacity contains missing values for {current_date:%Y-%m}')
    capped = xr.apply_ufunc(np.minimum, abstraction, capacity.clip(min=0))
    capped.name = 'groundwater_abstraction'
    capped.attrs = dict(abstraction.attrs)
    capped.attrs['long_name'] = 'pumping-capacity-capped monthly mean groundwater abstraction'
    capped.attrs['pumping_capacity_source'] = str(capacity_file)
    return capped
