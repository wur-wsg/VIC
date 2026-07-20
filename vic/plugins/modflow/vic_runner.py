#%%
from os.path import join
import os
import subprocess
from datetime import datetime, timedelta
from dateutil.relativedelta import relativedelta
#from config_module import config_global
import netCDF4 as nc
import numpy as np
import support_function as sf
import calendar
import xarray as xr
import multiprocessing
from pumping import (
    apply_dynamic_capacity,
    prepare_nonrenewable_proxy_abstraction,
    prepare_uncapped_abstraction,
)
#%%
def create_extra_forcing_file(current_date, config):    
    domain_file_path = config.paths.vic_domain_file
    os.makedirs(config.paths.vic_forcing_dir, exist_ok=True)
    output_file = config.paths.get_vic_forcing_file(config.modestr, config.couplingstr, current_date.year)


    # Read dimensions from domain file
    with nc.Dataset(domain_file_path, 'r') as src:
        lat = src.variables['lat'][:]
        lon = src.variables['lon'][:]
        nlat = len(lat)
        nlon = len(lon)

    # Create new netCDF file
    with nc.Dataset(output_file, 'w', format='NETCDF4') as dst:
        # Create dimensions
        dst.createDimension('time', 12)
        dst.createDimension('lat', nlat)
        dst.createDimension('lon', nlon)

        # Copy lat/lon attributes from source first
        with nc.Dataset(domain_file_path, 'r') as src:
            # Create variables with attributes
            time_var = dst.createVariable('time', 'f8', ('time',))
            lat_var = dst.createVariable('lat', 'f8', ('lat',))
            lon_var = dst.createVariable('lon', 'f8', ('lon',))
            discharge = dst.createVariable('discharge_mf', 'f8', ('time', 'lat', 'lon',), fill_value=-9999.0)
            
            # Copy attributes first
            for name in src.variables['lat'].__dict__:
                if name != '_FillValue':
                    lat_var.setncattr(name, src.variables['lat'].__dict__[name])
            for name in src.variables['lon'].__dict__:
                if name != '_FillValue':
                    lon_var.setncattr(name, src.variables['lon'].__dict__[name])
            
            # Then copy data
            lat_var[:] = src.variables['lat'][:]
            lon_var[:] = src.variables['lon'][:]

        # Set time values and attributes
        time_values = [(datetime(current_date.year, month, 1) - datetime(1979, 1, 1)).days 
                      for month in range(1, 13)]
        time_var[:] = time_values
        time_var.units = 'days since 1979-01-01 00:00:00'
        time_var.calendar = 'standard'
        time_var.standard_name = 'time'
        time_var.long_name = 'time'

        # Initialize discharge with zeros
        discharge[:] = np.zeros((12, nlat, nlon))

def create_next_year_extra_forcing_file(current_date, config):    
    domain_file_path = config.paths.vic_domain_file
    os.makedirs(config.paths.vic_forcing_dir, exist_ok=True)
    output_file = config.paths.get_vic_forcing_file(config.modestr, config.couplingstr, current_date.year + 1)
    # Read dimensions from domain file
    with nc.Dataset(domain_file_path, 'r') as src:
        lat = src.variables['lat'][:]
        lon = src.variables['lon'][:]
        nlat = len(lat)
        nlon = len(lon)

    # Create new netCDF file
    with nc.Dataset(output_file, 'w', format='NETCDF4') as dst:
        # Create dimensions
        dst.createDimension('time', 12)
        dst.createDimension('lat', nlat)
        dst.createDimension('lon', nlon)

        # Copy lat/lon attributes from source first
        with nc.Dataset(domain_file_path, 'r') as src:
            # Create variables with attributes
            time_var = dst.createVariable('time', 'f8', ('time',))
            lat_var = dst.createVariable('lat', 'f8', ('lat',))
            lon_var = dst.createVariable('lon', 'f8', ('lon',))
            discharge = dst.createVariable('discharge_mf', 'f8', ('time', 'lat', 'lon',), fill_value=-9999.0)
            
            # Copy attributes first
            for name in src.variables['lat'].__dict__:
                if name != '_FillValue':
                    lat_var.setncattr(name, src.variables['lat'].__dict__[name])
            for name in src.variables['lon'].__dict__:
                if name != '_FillValue':
                    lon_var.setncattr(name, src.variables['lon'].__dict__[name])
            
            # Then copy data
            lat_var[:] = src.variables['lat'][:]
            lon_var[:] = src.variables['lon'][:]

        # Set time values and attributes
        time_values = [(datetime(current_date.year + 1, month, 1) - datetime(1979, 1, 1)).days 
                      for month in range(1, 13)]
        time_var[:] = time_values
        time_var.units = 'days since 1979-01-01 00:00:00'
        time_var.calendar = 'standard'
        time_var.standard_name = 'time'
        time_var.long_name = 'time'

        # Initialize discharge with zeros
        discharge[:] = np.zeros((12, nlat, nlon))

def calculate_vic_dates(current_date)-> tuple[int, int, int, int, int, int, int, int, int, datetime, str]:
    """
    Calculate various dates needed for VIC model operation.
    
    Args:
        current_date: A datetime object representing the current date
    
    Returns:
        Tuple of date values for VIC configuration
    """
    startyear, startmonth, startday = current_date.year, current_date.month, current_date.day
    endyear, endmonth, endday = current_date.year, current_date.month, calendar.monthrange(current_date.year, current_date.month)[1]
    stateyear, statemonth, stateday = (current_date+relativedelta(months=1)).year, (current_date+relativedelta(months=1)).month, 1
    init_date, init_datestr = current_date, current_date.strftime('%Y%m%d')
    
    return (startyear, startmonth, startday, 
            endyear, endmonth, endday, 
            stateyear, statemonth, stateday, 
            init_date, init_datestr)

def prepare_vic(current_date, config):
    # Get date values from the new function
    (startyear, startmonth, startday, 
        endyear, endmonth, endday, 
        stateyear, statemonth, stateday, 
        init_date, init_datestr) = calculate_vic_dates(current_date)
    print(f"startyear,month, day is {startyear}, {startmonth}, {startday}")
    print(f"end year, month, day is {endyear}, {endmonth}, {endday}")
    print(f"statefile will be saved for {stateyear}, {statemonth}, {stateday}")
        
    # Create a single prefixes dictionary with common values
    prefixes = {
        "STARTYEAR": startyear,
        "STARTMONTH": startmonth,
        "STARTDAY": startday,
        "ENDYEAR": endyear,
        "ENDMONTH": endmonth,
        "ENDDAY": endday,
        "STATENAME": os.path.join(config.paths.statefile_dir, f"{config.modestr}_{config.couplingstr}_state_file_"),
        "STATEYEAR": stateyear,
        "STATEMONTH": statemonth,
        "STATEDAY": stateday,
        "PLUGIN_FORCE_TYPE     DISCHARGE": f'discharge_mf \t MONTH \t {config.paths.get_vic_forcing_prefix(config.modestr, config.couplingstr)}',
        "RESULT_DIR": config.paths.get_vic_result_dir(config.modestr, config.couplingstr),
    "OUTFILE": f'{config.modestr}_{config.couplingstr}_{config.vic_out_suffix}'

    }
    if not os.path.exists(prefixes["RESULT_DIR"]):
        os.makedirs(prefixes["RESULT_DIR"])
    # if config.humanimpact:
    #     if config.foc:
    #         prefixes["STATENAME"] = os.path.join(config.paths.statefile_dir, "state_file_human_foc_")
    #     else:
    #         prefixes["STATENAME"] = os.path.join(config.paths.statefile_dir, "state_file_human_poc_")
    # else:
    #     if config.foc:
    #         prefixes["STATENAME"] = os.path.join(config.paths.statefile_dir, "state_file_nat_foc_")
    #     else:
    #         prefixes["STATENAME"] = os.path.join(config.paths.statefile_dir, "state_file_nat_poc_")
    cold_start = os.environ.get("VIC_MF_COLD_START", "").strip().lower() in {"1", "true", "yes", "y"}
    if not cold_start:
        if current_date == config.startstamp:
            # Allow run-specific initial state injection from the launcher.
            prefixes["INIT_STATE"] = os.environ.get(
                "VIC_MF_INITIAL_STATE_FILE",
                os.path.join(
                    config.paths.statefile_dir,
                    "10years_spinup_g_r_20250317_.19900101_00000_Lisanne.nc",
                ),
            )
        else:
            prefixes["INIT_STATE"] = os.path.join(config.paths.statefile_dir, f"{config.modestr}_{config.couplingstr}_state_file_.{init_datestr}_00000.nc")
        # if config.humanimpact:
        #     if config.foc:
        #         prefixes["INIT_STATE"] = os.path.join(config.paths.statefile_dir, f"state_human_foc_file_.{init_datestr}_00000.nc")
        #     else:
        #         prefixes["INIT_STATE"] = os.path.join(config.paths.statefile_dir, f"state_human_poc_file_.{init_datestr}_00000.nc")
        # else:
        #     if config.foc:
        #         prefixes["INIT_STATE"] = os.path.join(config.paths.statefile_dir, f"state_nat_foc_file_.{init_datestr}_00000.nc")
        #     else:
        #         prefixes["INIT_STATE"] = os.path.join(config.paths.statefile_dir, f"state_nat_poc_file_.{init_datestr}_00000.nc")

                
    with open(config.paths.template_dir, 'r') as file:
        lines = file.readlines()

    for i, line in enumerate(lines):
        for prefix, value in prefixes.items():
            if prefix in line:
                lines[i] = f"{prefix}               {value}\n"
                break
        if cold_start and line.lstrip().startswith("INIT_STATE"):
            lines[i] = "#INIT_STATE\n"
    # if config.humanimpact:
    #     if config.foc:
    #         subdir = "human_foc"
    #     else:
    #         subdir = "human_poc"
    # else:
    #     if config.foc:
    #         subdir = "nat_foc"
    #     else:
    #         subdir = "nat_poc"
    config_subdir = config.paths.get_historical_config_dir(config.modestr, config.couplingstr)
    if not os.path.exists(config_subdir):
        os.makedirs(config_subdir)
        
    config_file = os.path.join(config_subdir, f"config_{startyear}_{startmonth}.txt")
    with open(config_file, 'w') as file:
        file.writelines(lines)
        
    return config_file

def run_vic(current_date,config, config_file, num_procs: int = 8) -> None:
    (startyear, startmonth) = calculate_vic_dates(current_date)[0:2]
    vic_executable = config.paths.vic_executable

    try:
        print(f'Running VIC with {num_procs} processesors...')
        # Use shell=False and split the command into a list of arguments
        command = ['mpirun', '-np', str(num_procs), vic_executable, '-g', config_file]
        #command = ['mpirun', vic_executable, '-g', config_file]
        subprocess.run(command, check=True, shell=False)
        print("VIC-WUR run successfully for time step [{}-{}]".format(startyear, startmonth))
    except subprocess.CalledProcessError:
        raise SystemExit("Stopping the simulation due to failure in VIC-WUR execution.")
    


def PostProcessVIC(config, current_date) -> tuple[xr.DataArray, xr.DataArray]:
    result = calculate_vic_dates(current_date)
    startyear, startmonth, endday = result[0], result[1], result[5]

    output_file = os.path.join(
        config.paths.get_vic_result_dir(config.modestr, config.couplingstr),
        f"{config.modestr}_{config.couplingstr}_{config.vic_out_suffix}.{startyear}-{startmonth:02d}.nc"
    )
    # if config.humanimpact:
    #     if config.foc:
    #         output_file = os.path.join(output_dir, f"human_foc_5min_nogl.{startyear}-{startmonth:02d}.nc")
    #     else:
    #         output_file = os.path.join(output_dir, f"human_poc_5min_nogl.{startyear}-{startmonth:02d}.nc")
    # else:
    #     if config.foc:
    #         output_file = os.path.join(output_dir, f"nat_foc_5min_nogl.{startyear}-{startmonth:02d}.nc")
    #     else:
    #         output_file = os.path.join(output_dir, f"nat_poc_5min_nogl.{startyear}-{startmonth:02d}.nc")
    vicout = xr.open_dataset(output_file)
    
    ts_gwrecharge = vicout['OUT_GWRECHARGE']/ endday / 1000  # mm/month -> m/day (keep m/day for MF RCH)
    ts_discharge = vicout['OUT_DISCHARGE']  # keep it as m3/s
    # if config.humanimpact: 
    #     ts_gwabstract = np.zeros_like(ts_gwrecharge) #TODO add process abstraction
    # else:
    #     ts_gwabstract = np.zeros_like(ts_gwrecharge)
    
    return ts_gwrecharge, ts_discharge


def PostProcessVICPumping(config, current_date) -> xr.DataArray:
    """Build this month's positive MODFLOW pumping demand in m3/day."""
    output_file = os.path.join(
        config.paths.get_vic_result_dir(config.modestr, config.couplingstr),
        f"{config.modestr}_{config.couplingstr}_{config.vic_out_suffix}."
        f"{current_date.year}-{current_date.month:02d}.nc",
    )
    with xr.open_dataarray(
        config.paths.mf_static_file('ibound'),
        mask_and_scale=False,
    ) as data:
        ibound_da = data.load()
    if not config.humanimpact:
        return xr.zeros_like(ibound_da, dtype=np.float64).rename('groundwater_abstraction')
    with xr.open_dataset(output_file) as dataset:
        prepare = (
            prepare_nonrenewable_proxy_abstraction
            if config.pumping_source == 'nonrenewable-proxy'
            else prepare_uncapped_abstraction
        )
        unmet_demand, abstraction = prepare(
            dataset,
            config.paths.cellarea,
            config.ibound == 1,
            calendar.monthrange(current_date.year, current_date.month)[1],
        )
        unmet_demand = unmet_demand.load()
        abstraction = abstraction.load()
    export_unmet_water_demand(config, current_date, unmet_demand)
    if config.pumping_mode == 'off':
        return xr.zeros_like(abstraction)
    if config.pumping_mode == 'capped':
        abstraction = apply_dynamic_capacity(
            abstraction,
            config.paths.pumping_capacity_file,
            current_date,
        )
    total = float(abstraction.sum().item())
    active_cells = int((abstraction > 0).sum().item())
    print(
        f'Prepared {config.pumping_mode} groundwater pumping from '
        f'{config.pumping_source} for '
        f'{current_date:%Y-%m}: {active_cells} cells, {total:.6g} m3/day',
        flush=True,
    )
    return abstraction


def export_unmet_water_demand(config, current_date, unmet_demand) -> None:
    """Append the human-run unmet demand diagnostic without feeding it back."""
    output_dir = config.paths.modflow_result_dir
    os.makedirs(output_dir, exist_ok=True)
    output_file = os.path.join(output_dir, 'unmet_water_demand.nc')
    if not os.path.exists(output_file):
        with nc.Dataset(output_file, 'w', format='NETCDF4') as dataset:
            dataset.createDimension('time', None)
            dataset.createDimension('lat', unmet_demand.sizes['lat'])
            dataset.createDimension('lon', unmet_demand.sizes['lon'])
            time = dataset.createVariable('time', 'f8', ('time',))
            lat = dataset.createVariable('lat', 'f8', ('lat',))
            lon = dataset.createVariable('lon', 'f8', ('lon',))
            variable = dataset.createVariable(
                'unmet_water_demand',
                'f4',
                ('time', 'lat', 'lon'),
                zlib=True,
                complevel=2,
                fill_value=np.nan,
            )
            time.units = 'days since 1970-01-01 00:00:00'
            time.calendar = 'standard'
            lat[:] = unmet_demand.lat.values
            lon[:] = unmet_demand.lon.values
            lat.units = 'degrees_north'
            lon.units = 'degrees_east'
            variable.units = 'mm/month'
            variable.long_name = unmet_demand.attrs['long_name']
            variable.formula = unmet_demand.attrs['formula']
            dataset.description = (
                'Monthly groundwater pumping-target diagnostic before capacity '
                'capping; this field is not fed back to VIC bookkeeping.'
            )
            dataset.pumping_mode = config.pumping_mode
            dataset.pumping_source = config.pumping_source
    with nc.Dataset(output_file, 'a') as dataset:
        if dataset.getncattr('pumping_mode') != config.pumping_mode:
            raise ValueError(
                f'{output_file} was created for pumping mode '
                f'{dataset.getncattr("pumping_mode")}, not {config.pumping_mode}'
            )
        if dataset.getncattr('pumping_source') != config.pumping_source:
            raise ValueError(
                f'{output_file} was created for pumping source '
                f'{dataset.getncattr("pumping_source")}, not {config.pumping_source}'
            )
        if not np.array_equal(dataset.variables['lat'][:], unmet_demand.lat.values):
            raise ValueError(f'Latitude mismatch in {output_file}')
        if not np.array_equal(dataset.variables['lon'][:], unmet_demand.lon.values):
            raise ValueError(f'Longitude mismatch in {output_file}')
        time = dataset.variables['time']
        timestamp = nc.date2num(current_date, units=time.units, calendar=time.calendar)
        matches = np.flatnonzero(np.isclose(time[:], timestamp, rtol=0, atol=1.0e-6))
        if matches.size > 1:
            raise ValueError(f'Duplicate time records for {current_date:%Y-%m} in {output_file}')
        index = int(matches[0]) if matches.size == 1 else len(time)
        time[index] = timestamp
        dataset.variables['unmet_water_demand'][index] = unmet_demand.values.astype(np.float32)
    print(f'Wrote unmet water demand for {current_date:%Y-%m} to {output_file}', flush=True)


def update_statefile(current_date, config, cpr_mm_month):
    currentyear = current_date.year
    currentmonth = current_date.month

    # FOC state update uses numpy-style boolean indexing below.
    # Convert xarray-backed inputs explicitly to numpy arrays to avoid
    # xarray's unsupported 2D boolean indexing behavior.
    cv = config.paths.vic_parameter['Cv'].values #TODO: later on if cv change with time, this needs to be updated. 
    max_moist = config.paths.capillary['max_moist'].values
    
    
    cpr_mm_month_input = np.expand_dims(cpr_mm_month, axis=0) * cv   # create a 3d array with the shape of number of veg types, lat ,lon
    
    

    stateyear, statemonth, stateday = (current_date+relativedelta(months=1)).year, (current_date+relativedelta(months=1)).month, 1
    
    # Read the state file
    statefile_dir = config.paths.statefile_dir
    state_file = os.path.join(statefile_dir, f"{config.modestr}_{config.couplingstr}_state_file_.{stateyear:04d}{statemonth:02d}{stateday:02d}_00000.nc")
    
    
    
    
    with nc.Dataset(state_file,'r') as state:
        state_soil_moisture = state.variables['STATE_SOIL_MOISTURE'][:,0,:,:,:]

        #state_soil_moisture = np.flip(state_soil_moisture,axis = 2)
    num_layers = state_soil_moisture.shape[1]
    sum_soil_moisture = np.zeros((num_layers, state_soil_moisture.shape[2], state_soil_moisture.shape[3])) # create a 3d array to store the sum of soil moisture for each veg type
    state_soil_moisture_new = state_soil_moisture.copy()
    state_soil_moisture_new[:,2,:,:] = state_soil_moisture [:,2,:,:]+ cpr_mm_month_input
    for layer in range(2, -1, -1):  # loop through the soil layers from bottom to top
        sum_soil_moisture[layer] = np.nansum(state_soil_moisture_new[:, layer, :, :] * cv, axis=0) # sum up the soil moisture for each veg type

        #check if the soil moist is saturated.  
        checksaturation = (sum_soil_moisture[layer]> max_moist[layer]) #true if it is saturated
        if checksaturation.any():
            print(f'there are cells in layer {layer+1} saturated')
            excess_water = sum_soil_moisture[layer] - max_moist[layer]
            excess_water[excess_water<0] = 0
            # let the current layer soil moisture be the max moisture
            for i in range(cv.shape[0]):
                state_soil_moisture_new[i, layer, :, :][checksaturation] = max_moist[layer][checksaturation]
            # add the extra to the upper layer
                if layer > 0:
                    state_soil_moisture_new[i, layer-1, :, :][checksaturation] += excess_water[checksaturation]
                else:
                    print(f'layer {layer+1} is the top layer, no where to add the excess water')
                            
        else:
            print(f'layer {layer+1} is not saturated')
            break     
    #write the soil moisture back to the state file
    with nc.Dataset(state_file,'a') as state:
        state.variables['STATE_SOIL_MOISTURE'][:,0,:,:,:] = state_soil_moisture_new
    
    print("updated the state file for the next time step")
    
# %%
