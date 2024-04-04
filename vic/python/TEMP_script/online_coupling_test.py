#%%
import os 
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

#cwd = '/home/sliu/Documents/gitversion/VIC-WUR-GWM-1910/vic_online/'
#config_indus_ubuntu.set_template_dir(os.path.join(cwd, 'python', 'VIC_config_file_human_impact_template_pyread.txt'))
#config_indus_ubuntu.set_statefile_dir(os.path.join(cwd, 'python', 'statefile'))
#config_indus_ubuntu.set_configfile_dir(os.path.join(cwd, 'python', 'configfile'))
#config_indus_ubuntu.set_vic_executable('/home/sliu/Documents/vic_indus/99SourceCode/VIC-WUR-GWM-1910/vic_offline/drivers/image/vic_image_gwm_offline.exe')
#config_indus_ubuntu.set_startstamp(datetime(1968, 1, 1))
#config_indus_ubuntu.set_mfinput_dir(os.path.join(cwd, 'python', 'mfinput'))
#humanimpact = False

cwd = '/lustre/nobackup/WUR/ESG/liu297/gitrepo/VIC-WUR-GWM-1910/vic_online/'
config_indus_ubuntu.set_template_dir(os.path.join(cwd, 'python', 'VIC_config_file_naturalized_template_pyread_anunna.txt'))
config_indus_ubuntu.set_statefile_dir(os.path.join(cwd, 'python', 'statefile'))
config_indus_ubuntu.set_configfile_dir(os.path.join(cwd, 'python', 'configfile'))
config_indus_ubuntu.set_vic_executable('/lustre/nobackup/WUR/ESG/liu297/vic_indus/11indus_run/99vic_offline_src/drivers/image/vic_image_gwm.exe')
config_indus_ubuntu.set_startstamp(datetime(1968, 1, 1))
config_indus_ubuntu.set_mfinput_dir(os.path.join(cwd, 'python', 'mfinput'))
humanimpact = False




#%%
current_date = datetime(1968,1,1)
finishdate = datetime(1975, 12, 31)

# Loop over the dates
print("Running VIC for the time step [{}-{}]".format(current_date.year, current_date.month))
#generate dmy for VIC run
startyear, startmonth, startday = sf.startdate(current_date)
endyear, endmonth, endday = sf.enddate(current_date)
stateyear, statemonth, stateday = sf.statefiledate(current_date)
init_date,init_datestr = sf.init_datestr(current_date)
#%%
        
# prepare VIC config file
config_file = vr.prepare_vic(startyear, startmonth, startday, 
            endyear, endmonth, endday, 
            stateyear, statemonth, stateday, 
            init_date, init_datestr, 
            config_indus_ubuntu)    

vr.run_vic(config_indus_ubuntu, config_file, startyear, startmonth)    

#%%   
output_dir = config_indus_ubuntu.output_dir
output_file = os.path.join(output_dir, f"fluxes_human_impact_.{startyear}-{startmonth:02d}.nc")

vicout = nc.Dataset(output_file)
vicout.variables.keys()

# check if there is no baseflow reported, aka, only gwrecharge is reported. 
if vicout.variables['OUT_BASEFLOW'][:,:,:].sum() == 0:
    print("baseflow is all 0")
else:
    #stop the program
    print("there is baseflow reported while GWM options == true")
    print("program will be stopped, please check the VIC configuration file")
    exit()
#%%
#try:
 #   from osgeo import gdal
#except ImportError:
#    import gdal
gwrecharge_mf = np.flip(vicout.variables['OUT_GWRECHARGE'][0,:,:],0)  # flip the matrix to match the mf input
surfQ_mf = np.flip(vicout.variables['OUT_DISCHARGE'][0,:,:],0)  # flip the matrix to match the mf input
if humanimpact ==True:
    gwdemand= np.sum(np.flip(vicout.variables['OUT_WI_NREN_SECT'][0,:,:,:],1),axis =0)  # flip the matrix to match the mf input
   # cell_area = gdal.Open(os.path.join(config_indus_ubuntu.mfinput_dir,'indus_Indus_CellArea_m2_05min.nc')).ReadAsArray()


#%%
#plt.imshow(np.ma.getmask(gwrecharge_mf)[:,:])
#plt.colorbar()
#plt.show()


#mf.PrepareMF()  prepare other modlofw inputs   finished. 

#mf.RunMF() basically is already there

#mf.PostProcessMF() 
    #1. prepare GWD data to identfy conditon of GWD and VIC soil layer depth
    
    #2. export Baseflow data from Water Budget output, save it as a netcdf file for VIC to read in the next time step

#mf.Feedback2VIC()
    #1. read the current soil moisture and update it based on the condition. 

#vr.update_statefile()
    #1. update the statefile's soil moisture for the next time step. 

#move to next time step
current_date += relativedelta(months=1)

# %%
