#%%
from os.path import join
import os
os.chdir('/lustre/nobackup/WUR/ESG/liu297/gitrepo/VIC-WUR-GWM-1910/vic_online/python')
import subprocess
from datetime import datetime, timedelta
import config_module
import netCDF4 as nc
import numpy as np
import support_function as sf
import calendar

#%%
def prepare_vic(startyear, startmonth, startday, endyear, endmonth, endday, 
            stateyear, statemonth, stateday, init_date, init_datestr,
            config):
    print("startyear,month, day is {},{},{}".format(startyear, startmonth, startday))
    print("end year, month, day is {},{},{}".format(endyear, endmonth, endday))
    print("statefile wll be save for {},{},{}".format(stateyear, statemonth, stateday))
    prefixes_firststep = {
    "STARTYEAR": startyear,    "STARTMONTH": startmonth,    "STARTDAY": startday,
    "ENDYEAR": endyear,    "ENDMONTH": endmonth,    "ENDDAY": endday,
    "STATENAME":  os.path.join(config.paths.statefile_dir, "state_file_"),
    "STATEYEAR": stateyear,    "STATEMONTH": statemonth,    "STATEDAY": stateday, 
    # Add other prefixes and their corresponding values here later if necessary
    }
    prefixes = {
    "STARTYEAR": startyear,    "STARTMONTH": startmonth,    "STARTDAY": startday,
    "ENDYEAR": endyear,    "ENDMONTH": endmonth,    "ENDDAY": endday,
    "INIT_STATE": os.path.join(config.paths.statefile_dir, f"state_file_.{init_datestr}_00000.nc"),
    "STATENAME":  os.path.join(config.paths.statefile_dir, "state_file_"),
    "STATEYEAR": stateyear,    "STATEMONTH": statemonth,    "STATEDAY": stateday,
    # Add other prefixes and their corresponding values here later if necessary
    }
    current_date = datetime(startyear, startmonth, startday)
    # Determine which prefixes to use
    if current_date ==  config.startstamp:
        current_prefixes = prefixes_firststep
    else:
        current_prefixes = prefixes
    # Update the prefixes with the current date
    current_prefixes["STARTYEAR"] = startyear
    current_prefixes["STARTMONTH"] = startmonth
    current_prefixes["STARTDAY"] = startday
    current_prefixes["ENDYEAR"] = endyear
    current_prefixes["ENDMONTH"] = endmonth
    current_prefixes["ENDDAY"] = endday
    current_prefixes["STATENAME"] = os.path.join(config.paths.statefile_dir, "state_file_")
    current_prefixes["STATEYEAR"] = stateyear
    current_prefixes["STATEMONTH"] = statemonth
    current_prefixes["STATEDAY"] = stateday
    if "INIT_STATE" in current_prefixes:
        current_prefixes["INIT_STATE"] = os.path.join(config.paths.statefile_dir, f"state_file_.{init_datestr}_00000.nc")
    
    with open(config.paths.template_dir, 'r') as file:
        lines = file.readlines()

    for i, line in enumerate(lines):
        for prefix, value in current_prefixes.items():
            if prefix in line:
                lines[i] = f"{prefix}               {value}\n"
                break    
    # Write the modified lines back to the file
    config_file = os.path.join(config.paths.configfile_dir, f"config_{startyear}_{startmonth}.txt")
    with open(config_file, 'w') as file:
        file.writelines(lines)
        
    return config_file

#%%
def run_vic(config, config_file, startyear, startmonth):
    vic_executable = config.paths.vic_executable
  
    try:
        command = 'bash -c "{} -g {}"'.format(vic_executable, config_file)
        subprocess.run(command, check=True, shell=True)
        print(f'currently using the config file {config_file}')
        print(f'currently using the vic executable {vic_executable}')
        print("VIC-WUR run successfully for time step [{}-{}]".format(startyear, startmonth))
    except subprocess.CalledProcessError:
        raise SystemExit("Stopping the simulation due to failure in VIC execution.")
#%%    
def PostProcessVIC(config, startyear, startmonth):
    # Read the VIC output
    if config.humanimpact:
        human_or_nat = "human"
    else:
        human_or_nat = "naturalized"
    output_dir = config.paths.output_dir
    output_file = os.path.join(output_dir, f"fluxes_{human_or_nat}_gwm_.{startyear}-{startmonth:02d}.nc")
    
    vicout = nc.Dataset(output_file, 'r')
    print('Do a check if it is a coupling run(check if there is baseflow reported from VIC:)')
    if vicout.variables['OUT_BASEFLOW'][:,:,:].sum() == 0:
        print("baseflow is all 0")
    else:
    #stop the program
        print("there is baseflow reported,probably means it is not a coupling run, GWM.options is set incorrectly.")
        print("program will be stopped, please check the VIC configuration file")
        exit()   
    
    ts_gwrecharge = vicout.variables['OUT_GWRECHARGE'][:,:,:] /1000 # mm/day to m/day
    ts_discharge = vicout.variables['OUT_DISCHARGE'][:,:,:]  # keep it as m3/s
    if config.humanimpact: 
        ts_gwabstract = np.zeros_like(ts_gwrecharge) #TODO add process abstraction
    else:
        ts_gwabstract = np.zeros_like(ts_gwrecharge)
    
    return ts_gwrecharge, ts_discharge, ts_gwabstract
#%%
def update_statefile(current_date,stateyear,statemonth,stateday,cpr_mm_month,config):
    currentyear = current_date.year
    currentmonth = current_date.month

    #find the cv file with the corresponding year
    cv_file = config.paths.cvdir+f'coverage_monthly_MIRCA_{currentyear}.nc'
    
    
    max_moist_file = config.paths.vic_derived_param
    #read the parameter file to get the fraction for each veg class
    with nc.Dataset(cv_file,'r') as param:
        veg_class_fraction = param.variables['coverage'][currentmonth-1,:,:,:]
    with nc.Dataset(max_moist_file,'r') as max_moist:
        max_moisture = max_moist.variables['max_moist'][:]
    veg_class_fraction = np.flip(veg_class_fraction,axis = 1)
    max_moisture = np.flip(max_moisture,axis = 1)
    
    cpr_mm_month_input = np.repeat(cpr_mm_month[np.newaxis,:,:],22,axis = 0)

    
    
    
    # Read the state file
    statefile_dir = config.paths.statefile_dir
    state_file = os.path.join(statefile_dir, f"state_file_.{stateyear:04d}{statemonth:02d}{stateday:02d}_00000.nc")
    with nc.Dataset(state_file,'r') as state:
        state_soil_moisture = state.variables['STATE_SOIL_MOISTURE'][:,0,:,:,:]
        state_soil_moisture = np.flip(state_soil_moisture,axis = 2)

    sum_soil_moisture = np.zeros((3, 180, 204))
    state_soil_moisture_new = state_soil_moisture.copy()
    state_soil_moisture_new[:,2,:,:] = state_soil_moisture [:,2,:,:]+ cpr_mm_month_input
    for layer in range(2, -1, -1):  # loop through the soil layers from bottom to top
        sum_soil_moisture[layer] = np.nansum(state_soil_moisture_new[:, layer, :, :] * veg_class_fraction, axis=0) # sum up the soil moisture for each veg type

        #check if the soil moist is saturated.  
        checksaturation = (sum_soil_moisture[layer]> max_moisture[layer]) #true if it is saturated
        if checksaturation.any():
            print(f'there are cells in layer {layer+1} saturated')
            excess_water = sum_soil_moisture[layer] - max_moisture[layer]
            excess_water[excess_water<0] = 0
            # let the current layer soil moisture be the max moisture
            for i in range(22):
                state_soil_moisture_new[i, layer, :, :][checksaturation] = max_moisture[layer][checksaturation]
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
        state.variables['STATE_SOIL_MOISTURE'][:,0,:,:,:] = np.flip(state_soil_moisture_new,axis = 2)
    
    print("updated the state file for the next time step")
    
# %%
