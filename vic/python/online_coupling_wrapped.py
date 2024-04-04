#%%
import os 
os.chdir('/lustre/nobackup/WUR/ESG/liu297/gitrepo/VIC-WUR-GWM-1910/vic_online/python')
import sys
sys.path.append('/lustre/nobackup/WUR/ESG/liu297/gitrepo/VIC-WUR-GWM-1910/vic_online/python')
import flopy
import numpy as np
import xarray as xr
import netCDF4 as nc
import subprocess  # for calling shell commands
from datetime import datetime, timedelta
from dateutil.relativedelta import relativedelta
import calendar
import vic_runner as vr
import support_function as sf
from config_module import config_indus_ubuntu
from osgeo import gdal
from netCDF4 import Dataset, date2num
from matplotlib import pyplot as plt
import mf_run as mf
import shutil


#%env LD_LIBRARY_PATH=/shared/legacyapps/netcdf/gcc/64/4.6.1/lib:$LD_LIBRARY_PATH   
#%%
cwd = '/lustre/nobackup/WUR/ESG/liu297/gitrepo/VIC-WUR-GWM-1910/vic_online/'
config_indus_ubuntu.paths.set_template_dir(os.path.join(cwd, 'python', 'VIC_config_file_naturalized_template_pyread_anunna.txt'))
config_indus_ubuntu.paths.set_statefile_dir(os.path.join(cwd, 'python', 'statefile'))
config_indus_ubuntu.paths.set_configfile_dir(os.path.join(cwd, 'python', 'configfile'))
config_indus_ubuntu.paths.set_vic_executable('/lustre/nobackup/WUR/ESG/liu297/vic_indus/11indus_run/99vic_offline_src/drivers/image/vic_image_gwm.exe')
config_indus_ubuntu.set_startstamp(datetime(1968, 1, 1))
config_indus_ubuntu.paths.set_mfinput_dir('/lustre/nobackup/WUR/ESG/yuan018/04Input_Indus/')
config_indus_ubuntu.paths.set_mfoutput_dir('/lustre/nobackup/WUR/ESG/liu297/gitrepo/VIC-WUR-GWM-1910/vic_online/python/mfoutput/workspace/')
config_indus_ubuntu.set_humanimpact(False)



current_date = datetime(1968,1,1)
finishdate = datetime(2000,1,1)

# Loop over the dates
while current_date <= finishdate:
    if current_date == datetime(1968,1,1):
        indus_file_path = '/lustre/nobackup/WUR/ESG/yuan018/99Unuse/domain_Indus.nc'
        forcinginputdir = config_indus_ubuntu.paths.vicextrainput_dir
        output_file = os.path.join(forcinginputdir, f"forcing_discharge_{current_date.year}.nc")
        shutil.copy(indus_file_path, output_file)
        with nc.Dataset(output_file,'a') as nc_file:
            nc_file.createDimension('time', 12)
            time_var = nc_file.createVariable('time', 'f8', ('time',))
            time_var.units = f'days since {current_date.year+1}-01-01 00:00:00'
            time_var.calendar = 'standard'
            time_var.standard_name = 'time'
            time_var.long_name = 'time'
            discharge_var = nc_file.createVariable('discharge_mf', 'f4', ('time', 'lat', 'lon'))
            monthly_dates = [datetime(current_date.year, month, 1) for month in range(1, 13)]
            monthly_times = date2num(monthly_dates, units=time_var.units, calendar=time_var.calendar)
            time_var[:] = monthly_times
            discharge_var[0,:,:] = 0            
    
    print("Running VIC for the time step [{}-{}]".format(current_date.year, current_date.month))
    #generate dmy for VIC run
    startyear, startmonth, startday = sf.startdate(current_date)
    endyear, endmonth, endday = sf.enddate(current_date)
    stateyear, statemonth, stateday = sf.statefiledate(current_date)
    init_date,init_datestr = sf.init_datestr(current_date)
    
         
    # prepare VIC config file
    config_file = vr.prepare_vic(startyear, startmonth, startday, 
               endyear, endmonth, endday, 
               stateyear, statemonth, stateday, 
               init_date, init_datestr, 
               config_indus_ubuntu)    
    vr.run_vic(config_indus_ubuntu, config_file, startyear, startmonth)    
    
    ts_gwrecharge, ts_discharge, ts_gwabstract = vr.PostProcessVIC(config_indus_ubuntu, startyear, startmonth) # read VIC output and prepare for MODFLOW input
         
    stress_period = config_indus_ubuntu.timestep_counter() # let stress period ++1 after each VIC run
    print(f"start assigning the MODFLOW inputs for stress period {stress_period}...\n")

    if stress_period == 0:
        layer1_head = config_indus_ubuntu.get_initial_head()[0]
        layer2_head = config_indus_ubuntu.get_initial_head()[1]
    
    mfrun = mf.mfrun(current_date,stress_period,config_indus_ubuntu,layer1_head,layer2_head,ts_gwrecharge,ts_discharge,ts_gwabstract) # 把日期传递给mfrun这个类
   
    layer1_head, layer2_head = mfrun.run_modflow() # run MODFLOW and get the head for each layer
    pp = mf.PostProcessMF(config_indus_ubuntu, current_date) # get the baseflow from MODFLOW output
    
    baseflow = pp.baseflow_array
    print('generated baseflow')
    pp.savebf2nc()
    print('saved baseflow to nc')
    cpr_mm = pp.cpr_mm 
    cpr_mm_month = cpr_mm * endday*1000*-1
    
    # update cpr_mm 
    vr.update_statefile(current_date, stateyear, statemonth, stateday,cpr_mm_month, config_indus_ubuntu)
    print('updated statefile')    
    #move to next time step
    #config_indus_ubuntu.paths.close_all_nc()
    current_date += relativedelta(months=1)
    
#%%
