#%%
import os
import numpy as np
import netCDF4 as nc
import xarray as xr
import flopy
import calendar
from datetime import datetime,timedelta
from dateutil.relativedelta import relativedelta
from netCDF4 import Dataset, date2num
import vic_runner as vr
import support_function as sf
#from config_module import config_global
from netCDF4 import Dataset, date2num
from matplotlib import pyplot as plt
import shutil
import netCDF4 as nc

#%%
class mfrun:
    def __init__(self, config_instance, current_date: datetime, ts_gwrecharge: xr.DataArray, ts_discharge: xr.DataArray, ts_gwabstract: int):  #TODO: tw_gwabstract is int 0 because now it is designed for natrual run 
        self.current_date = current_date
        self.last_end_date = current_date - relativedelta(days = 1)
        self.end_date = current_date + relativedelta(months=1) - relativedelta(days = 1)
        #self.stress_period = stress_period
        self.config = config_instance
        self.name = self.config.mfname
        self.rclose = 10
        self.maxinner = 500
        self.maxouter = 200
        self.outer_dvclose = 5
        self.inner_dvclose = 5
        self.top_layer1 = self.config.cal_toplayer_elevation()
        
        self.ts_gwrecharge = ts_gwrecharge.values
        self.ts_discharge = ts_discharge.values
        self.ts_gwabstract = ts_gwabstract # TODO: this is int 0 because now it is designed for natrual run
        self.startinghead = self.get_startinghead()
    
    
    def get_startinghead(self) -> list:
        self.startinghead_path = os.path.join(
            self.config.paths.get_gwl_dir(self.config.modestr, self.config.couplingstr),
            f'{self.name}_gwl_{self.last_end_date.strftime("%Y%m%d")}.nc'
        )
        if not os.path.exists(self.startinghead_path):
            # fallback to initial head for the first timestep or missing previous output
            return self.config.get_initial_head()
        ds_prev = xr.open_dataset(self.startinghead_path)
        startingheadl1 = ds_prev['gwl'].values[0]
        startingheadl2 = ds_prev['gwl'].values[1]
        self.startinghead = [startingheadl1, startingheadl2]
        return self.startinghead

    def set_convergence_criteron(self,rclose,maxinner,maxouter,outer_dvclose,inner_dvclose) -> tuple[float, int, int]:
        self.rclose = rclose
        self.maxinner = maxinner
        self.maxouter = maxouter
        self.outer_dvclose = 5
        self.inner_dvclose = 5
        return self.rclose,self.maxinner,self.maxouter        
           
    def online_cp_stress_period_data(self):
        #count how many days in this month
        _, days = calendar.monthrange(self.current_date.year, self.current_date.month)
        nstp, tsmult = 1, 1
        perioddata = [days,nstp,tsmult]
        return perioddata

    def run_modflow(self):
        ws = self.config.paths.get_modflow_workspace_dir(self.config.mfname)
        if not os.path.exists(ws):
            os.makedirs(ws)
        perioddata = self.online_cp_stress_period_data()
        botm = self.config.cal_botlayer_elevation()
        khor,kver,stor = self.config.get_npf_param()
        CHDstress_period_data = self.config.get_chd_input()
        
        self.config.set_ts_gwrecharge(self.ts_gwrecharge[0])
        self.config.set_ts_discharge(self.ts_discharge[0])
        #config_indus_ubuntu.paths.set_ts_gwabstract(self.ts_gwabstract[0]) #TODO
        RCHstress_period_data = self.config.get_rch_param(self.current_date)
        RIVstress_period_data = self.config.get_riv_param()
        if self.config.foc:
            CPRstress_period_data = self.config.get_cpr_param()
        else:
            CPRstress_period_data = None
        DRNstress_period_data = self.config.get_drn_param()
        sim = flopy.mf6.MFSimulation(sim_name= self.name, 
                                     version='mf6', 
                                     sim_ws= ws,
                                     exe_name = self.config.paths.mf6exe,
                                     memory_print_option= None,
                                     #print_input=True, 
                                     verbosity_level= 1, 
                                     write_headers= True, 
                                     #use_pandas = True,
                                     continue_=False, 
                                     nocheck = True,
                                     #lazy_io = True #TODO: this is only for development phase to speed up testing. false it later
                                     )  
        tdis = flopy.mf6.ModflowTdis(sim,
                                     time_units= "DAYS",
                                     #start_date_time= self.current_date,
                                     nper = 1, 
                                     perioddata= perioddata)               
        ims = flopy.mf6.ModflowIms(sim,
                                   complexity='COMPLEX',
                                   print_option="SUMMARY",
                                   outer_dvclose= self.outer_dvclose,      
                                   inner_dvclose= self.inner_dvclose, 
                                   under_relaxation="simple",
                                   under_relaxation_gamma=0.99,
                                   relaxation_factor=0,
                                   linear_acceleration="bicgstab", 
                                   outer_maximum= self.maxouter,
                                   inner_maximum= self.maxinner,
                                   rcloserecord="{} strict".format(self.rclose))
        gwf = flopy.mf6.ModflowGwf(sim,
                                   modelname = self.name,
                                   model_nam_file= f"{self.name}.nam",
                                   exe_name = self.config.paths.mf6exe,
                                   model_rel_path = '.',
                                   list= None,
                                   print_input = True, #todo false it later
                                   print_flows= True, #todo false it later
                                   save_flows= True)
        dis = flopy.mf6.ModflowGwfdis(gwf,
                                      length_units='METERS',
                                      nogrb=True,
                                      xorigin=0,
                                      yorigin=0,
                                      angrot=0,
                                      nlay=self.config.Nlay,
                                      ncol=self.config.Ncol,
                                      nrow=self.config.Nrow,
                                      top=self.top_layer1,
                                      idomain=[self.config.idomain,self.config.idomain],
                                      delr=self.config.delrow,
                                      delc=self.config.delcol,
                                      botm=botm)
        npf = flopy.mf6.ModflowGwfnpf(gwf,
                                      save_flows = True,
                                      print_flows = True,
                                      save_specific_discharge = True,
                                      save_saturation = True,
                                      perioddata = None,
                                      dev_no_newton = False,
                                      icelltype = 0,
                                      k = khor,
                                      k33 = kver
                                      )
        sto = flopy.mf6.ModflowGwfsto(gwf,
                                      save_flows = None,
                                      storagecoefficient = True,
                                      ss_confined_only = None,
                                      iconvert = 0,
                                      ss = stor,
                                      transient = True)
        chd = flopy.mf6.ModflowGwfchd(gwf,
                                      print_input = False,
                                      print_flows = True,
                                      save_flows = True,
                                      stress_period_data = CHDstress_period_data
                                      )
        ic = flopy.mf6.ModflowGwfic(gwf,
                                    strt = self.startinghead
                                    )
        rch = flopy.mf6.ModflowGwfrch(gwf,
                                      stress_period_data = RCHstress_period_data,
                                      save_flows = True
                                      )
        riv = flopy.mf6.ModflowGwfriv(gwf,
                                      stress_period_data = RIVstress_period_data,
                                      save_flows = True
                                      )
        if self.config.foc:
            cpr = flopy.mf6.ModflowGwfevt(gwf,
                          fixed_cell = True,
                          print_input = True,
                          print_flows = True,
                          save_flows = True,
                          surf_rate_specified = False,
                          nseg = 1,
                          stress_period_data = CPRstress_period_data
                          )
        drn = flopy.mf6.ModflowGwfdrn(gwf,
                                      print_input = False,
                                      print_flows = False,
                                      stress_period_data = DRNstress_period_data,
                                      save_flows = True
                                      ) 
        
        saverecord = [("HEAD", "ALL"), ("BUDGET", "ALL")]
        printrecord = [("HEAD", "ALL"), ("BUDGET", "ALL")]
        headfile = "{}_{}.hds".format(self.name,self.end_date.strftime("%Y%m%d"))
        head_filerecord = [headfile]
        budgetfile = "{}_{}.cbb".format(self.name,self.end_date.strftime("%Y%m%d"))
        budget_filerecord = [budgetfile]
        oc = flopy.mf6.mfgwfoc.ModflowGwfoc(gwf,
                                            saverecord = saverecord,
                                            head_filerecord = head_filerecord,
                                            budget_filerecord = budget_filerecord
                                            )
        print(f"writing the simulation for date {self.end_date.strftime('%Y-%m')}...\n")
        sim.write_simulation()
        print('finish writing the simulation \n')
        print(f"running the simulation for date {self.end_date.strftime('%Y-%m')}...\n")
        success, buff = sim.run_simulation()

        if not success:
            raise Exception(f"MODFLOW 6 did not terminate normally for {self.end_date.strftime('%Y-%m')}\n")
        else:
            print(f"MODFLOW 6 terminated normally for time step: {self.end_date.strftime('%Y-%m')}\n")
            print(f"extracting the head and budget files for time step {self.end_date.strftime('%Y-%m')}...\n")
            head_path = os.path.join(ws,headfile)
            head = flopy.utils.binaryfile.HeadFile(head_path)
            self.layer1_head = head.get_data()[0] # top
            self.layer2_head = head.get_data()[1] # bot
        


        # save the gwl to netcdf file
        gwl_dir = self.config.paths.get_gwl_dir(self.config.modestr, self.config.couplingstr)
        if not os.path.exists(gwl_dir):
            os.makedirs(gwl_dir)
        output_head = os.path.join(gwl_dir, f'{self.config.mfname}_gwl_{self.end_date.strftime("%Y%m%d")}.nc')
        with nc.Dataset(output_head, 'w', format='NETCDF4') as ds:
            nlay = self.config.Nlay
            nrow = self.config.Nrow
            ncol = self.config.Ncol
            ds.createDimension('layer', nlay)
            ds.createDimension('lat', nrow)
            ds.createDimension('lon', ncol)

            latitudes = ds.createVariable('latitude', np.float32, ('lat',))
            longitudes = ds.createVariable('longitude', np.float32, ('lon',))
            layers = ds.createVariable('layer', np.int32, ('layer',))
            head = ds.createVariable('gwl', np.float32, ('layer', 'lat', 'lon'))
            
            latitudes[:] = np.linspace(-56+1/24, 84-1/24, nrow)
            longitudes[:] = np.linspace(-180+1/24, 180-1/24, ncol)
            layers[:] = np.arange(nlay)
            head[0] = self.layer1_head
            head[0] = np.where(np.isnan(self.config.paths.landmask), np.nan, head[0])
            head[1] = self.layer2_head
            head[1] = np.where(np.isnan(self.config.paths.landmask), np.nan, head[1])

            ds.description = f'Transient groundwater head for {self.end_date.strftime("%Y-%m-%d")}, partially online coupled with VIC-WUR 5min natural run'
            head.units = 'm'
            head.long_name = 'groundwater head'
            ds.author = 'S.Liu'
            ds.date = datetime.now().strftime('%Y-%m-%d')
            ds.history = 'Created ' + datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        return None

class PostProcessMF:
    def __init__(self, config_instance, current_date):
        self.current_date = current_date
        self.last_end_date = current_date - relativedelta(days = 1)
        self.end_date = current_date + relativedelta(months=1) - relativedelta(days = 1)
        self.name = config_instance.mfname
        self.mf_workspace_dir = config_instance.paths.get_modflow_workspace_dir(self.name)
        self.mf_result_dir = config_instance.paths.modflow_result_dir
        self.cbbfile = "{}_{}.cbb".format(self.name, self.end_date.strftime("%Y%m%d"))
        self.headfile = "{}_{}.hds".format(self.name, self.end_date.strftime("%Y%m%d"))
        self.config = config_instance
        self.baseflow_array = None
        # self.cpr_mm = self.get_cpr_array() 

    def get_baseflow_array(self):
        cbb_path = os.path.join(self.mf_workspace_dir, self.cbbfile)
        cbb = flopy.utils.CellBudgetFile(cbb_path)
        riv_raw = cbb.get_data(text='RIV')
        riv = [(item['node'], item['q']) for item in riv_raw]
        ncol, nrow = self.config.Ncol, self.config.Nrow
        # extracting baseflow/river leakage from the RIV package:
        baseflow = []
        print('start extracting the baseflow from the cbb file \n')
        for i in range(len(riv[0][0])):
            idx = riv[0][0][i]
            idx0 = int(idx) - 1
            lay = 0
            row = idx0 // ncol
            col = idx0 % ncol
            flow = riv[0][1][i]
            rec = [lay, row, col, flow]
            baseflow.append(rec)
        
        baseflow_array = np.zeros((nrow, ncol))
        for item in baseflow:
            lay, row, col, flow = item
            flow 

            baseflow_array[row, col] = flow
        
        # extracting baseflow from the DRN package:
        drn_raw = cbb.get_data(text='DRN')
        drn1 = [(item['node'], item['q']) for item in drn_raw]

        drainage = []
        for i in range(len(drn1[0][0])):
            idx = drn1[0][0][i]
            idx0 = int(idx) - 1
            lay = 0
            row = idx0 // ncol
            col = idx0 % ncol
            flow = drn1[0][1][i]
            rec = [lay,row,col,flow]
            drainage.append(rec)

        drainage_array = np.zeros((nrow, ncol))
        for item in drainage:
            lay, row, col, flow = item
            flow
            drainage_array[row,col] = flow

        # adding the two array together:
        total_array = (baseflow_array + drainage_array) / 86400 / self.end_date.day * -1
        total_array = np.where(abs(total_array) > 1e8, 0, total_array)
        total_array = np.where(np.isnan(self.config.paths.landmask), np.nan, total_array)
        return total_array
    
    def update_vic_forcing(self, total_array):
        # save it to netcdf file for vic extra forcing next time step:

        if self.current_date.month == 12:
            vr.create_next_year_extra_forcing_file(self.current_date, self.config)
            output_file = self.config.paths.get_vic_forcing_file(self.config.modestr, self.config.couplingstr, self.current_date.year + 1)
        else:
            output_file = self.config.paths.get_vic_forcing_file(self.config.modestr, self.config.couplingstr, self.current_date.year)
        
        with nc.Dataset(output_file, 'a') as nc_file:
            time_var = nc_file.variables['time']
            discharge_var = nc_file.variables['discharge_mf']
            new_date = self.current_date + relativedelta(months=1)
            new_time = date2num(new_date, units=time_var.units, calendar='standard')
            times = time_var[:]
            matches = np.where(np.isclose(times, new_time, rtol=0, atol=1e-6))[0]
            if matches.size == 0:
                raise IndexError(f"Time {new_date} not found in forcing file time axis")
            month_index = matches[0]
            baseflow2forcing = total_array
            discharge_var[month_index, :, :] = baseflow2forcing            

        return None


    def get_cpr_array(self):
        cbb_path = os.path.join(self.mf_workspace_dir, self.cbbfile)
        cbb = flopy.utils.CellBudgetFile(cbb_path)
        cellarea = self.config.paths.cellarea
        
        evt_raw = cbb.get_data(text = 'EVT')
        evt = [(item['node'],item['q'],item['q']) for item in evt_raw]
        ncol, nrow = self.config.Ncol, self.config.Nrow
        cpr = []
        for i in range(len(evt[0][0])):
            idx = evt[0][0][i]
            # Convert from 1-based linear index (MODFLOW) to 0-based row/col
            idx0 = int(idx) - 1
            lay = 0
            row = idx0 // ncol
            col = idx0 % ncol
            flow = evt[0][2][i]
            rec = [lay,row,col,flow]
            cpr.append(rec)

        cpr_array = np.full((nrow, ncol), 0)

        for item in cpr:
            lay, row, col, flow = item
            cpr_array[row,col] = flow * -1  # this is m3/month, outflow (negative), so we need to multiply -1 to convert it into positive value. 

        cpr_mm_month = cpr_array/cellarea *1000  # convert it into mm/month prepare to update it to the vic statefile. 
   
        return cpr_mm_month
    
    def export_cpr_to_nc(self, cpr_mm_month):
        # Save to a single netCDF file with lat/lon coordinates
        os.makedirs(self.mf_result_dir, exist_ok=True)
        output_file = os.path.join(self.mf_result_dir, f'glob_{self.config.modestr}_{self.config.couplingstr}_cpr.nc')
        
        # Create file if it doesn't exist
        if not os.path.exists(output_file):
            with nc.Dataset(output_file, 'w', format='NETCDF4') as ds:
                # Create dimensions
                nrow = self.config.Nrow
                ncol = self.config.Ncol
                ds.createDimension('time', None)  # unlimited
                ds.createDimension('lat', nrow)
                ds.createDimension('lon', ncol)
                
                # Create variables
                times = ds.createVariable('time', 'f8', ('time',))
                lats = ds.createVariable('latitude', 'f4', ('lat',))
                lons = ds.createVariable('longitude', 'f4', ('lon',))
                cpr = ds.createVariable('capillary_rise', 'f4', ('time', 'lat', 'lon',), 
                                      fill_value=np.nan)
                
                # Set coordinates
                lats[:] = np.linspace(-56+1/24, 84-1/24, nrow)
                lons[:] = np.linspace(-180+1/24, 180-1/24, ncol)
                
                # Set attributes
                times.units = 'days since 1979-01-01'
                times.calendar = 'standard'
                cpr.units = 'mm/month'
                cpr.long_name = 'capillary rise flux'
            
    # Append data
        with nc.Dataset(output_file, 'a') as ds:
            time_var = ds.variables['time']
            current_time = date2num(self.current_date, units=time_var.units, calendar='standard')
            
            # Determine write index: append if new time, else overwrite existing index
            existing = time_var[:]
            matches = np.where(existing == current_time)[0]
            if matches.size > 0:
                time_idx = matches[0]
            else:
                time_idx = len(existing)
                time_var[time_idx:time_idx+1] = current_time
            ds.variables['capillary_rise'][time_idx, :, :] = cpr_mm_month

        return None 
                                      
                                      
                                      
                                      
                                      
                                      
# %%
