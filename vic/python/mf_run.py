#%%
import numpy as np
import netCDF4 as nc
import os
os.chdir('/lustre/nobackup/WUR/ESG/liu297/gitrepo/VIC-WUR-GWM-1910/vic_online/python')
from osgeo import gdal
import flopy
import calendar
from datetime import datetime,timedelta
from dateutil.relativedelta import relativedelta
from netCDF4 import Dataset, date2num
import vic_runner as vr
import support_function as sf
from config_module import config_indus_ubuntu 
from osgeo import gdal
from netCDF4 import Dataset, date2num
from matplotlib import pyplot as plt
import shutil


#%%
class mfrun:
    def __init__(self, current_date,stress_period,config_instance,layer1_head,layer2_head, ts_gwrecharge, ts_discharge, ts_gwabstract):
        self.current_date = current_date
        self.stress_period = stress_period
        self.config = config_indus_ubuntu
        self.name = '2Ltrans_off'
        self.rclose = 10
        self.maxinner = 500
        self.maxouter = 200
        self.outer_dvclose = 5
        self.inner_dvclose = 5
        self.top_layer1 = self.config.cal_toplayer_elevation()
        self.layer1_head = layer1_head
        self.layer2_head = layer2_head
        self.ts_gwrecharge = ts_gwrecharge.data
        self.ts_gwrecharge = np.flip(self.ts_gwrecharge,axis = 0)
        self.ts_discharge = ts_discharge.data
        self.ts_discharge = np.flip(self.ts_discharge,axis = 0)
        self.ts_gwabstract = ts_gwabstract.data
        self.startinghead = self.get_startinghead()


    def set_convergence_criteron(self,rclose,maxinner,maxouter,outer_dvclose,inner_dvclose):
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
    def get_startinghead(self):
        startingheadl1 = self.layer1_head
        startingheadl2 = self.layer2_head
        self.startinghead = [startingheadl1,startingheadl2]            
        return self.startinghead
    def run_modflow(self):
        print('it is running')
        perioddata = self.online_cp_stress_period_data()
        botm = self.config.cal_botlayer_elevation()
        khor,kver,stor = self.config.get_npf_param()
        CHDstress_period_data = self.config.get_chd_input()
        
        config_indus_ubuntu.set_ts_gwrecharge(self.ts_gwrecharge[0])
        config_indus_ubuntu.set_ts_discharge(self.ts_discharge[0])
        #config_indus_ubuntu.paths.set_ts_gwabstract(self.ts_gwabstract[0]) #TODO
        RCHstress_period_data = self.config.get_rch_param(self.current_date)
        RIVstress_period_data = self.config.get_riv_param()
        CPRstress_period_data = self.config.get_cpr_param()
        sim = flopy.mf6.MFSimulation(sim_name= self.name, 
                                     version='mf6', 
                                     sim_ws=self.config.paths.mfoutput_dir, 
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
        cpr = flopy.mf6.ModflowGwfevt(gwf,
                                      fixed_cell = True,
                                      print_input = True,
                                      print_flows = True,
                                      save_flows = True,
                                      surf_rate_specified = False,
                                      nseg = 1,
                                      stress_period_data = CPRstress_period_data
                                      )
        saverecord = [("HEAD", "ALL"), ("BUDGET", "ALL")]
        printrecord = [("HEAD", "ALL"), ("BUDGET", "ALL")]
        headfile = "{}_{}.hds".format(self.name,self.current_date.strftime("%Y%m%d"))
        head_filerecord = [headfile]
        budgetfile = "{}_{}.cbb".format(self.name,self.current_date.strftime("%Y%m%d"))
        budget_filerecord = [budgetfile]
        ov = flopy.mf6.mfgwfoc.ModflowGwfoc(gwf,
                                            saverecord = saverecord,
                                            head_filerecord = head_filerecord,
                                            budget_filerecord = budget_filerecord
                                            )
        print('finish writing the flopy variables \n')
        print(f"writing the simulation for stress period {self.stress_period}...\n")
        sim.write_simulation()
        print('finish writing the simulation \n')
        print(f"running the simulation for stress period {self.stress_period}...\n")
        success, buff = sim.run_simulation()

        if not success:
            raise Exception(f"MODFLOW 6 did not terminate normally for stressperiod: {self.stress_period}\n")
        else:
            print(f"MODFLOW 6 terminated normally for stressperiod: {self.stress_period}\n")
            print(f"extracting the head and budget files for stress period {self.stress_period}...\n")
            head = flopy.utils.binaryfile.HeadFile(self.config.paths.mfoutput_dir+'/'+headfile)
            self.layer1_head = head.get_data()[0] # top
            self.layer2_head = head.get_data()[1] # bot
        return self.layer1_head,self.layer2_head 

class PostProcessMF:
    def __init__(self, config_instance, current_date):
        self.mfoutput_dir = config_instance.paths.mfoutput_dir
        self.current_date = current_date
        self.name = '2Ltrans_off'
        self.cbbfile = "{}_{}.cbb".format(self.name, self.current_date.strftime("%Y%m%d"))
        self.headfile = "{}_{}.hds".format(self.name, self.current_date.strftime("%Y%m%d"))
        self.config = config_instance
        self.baseflow_array = self.get_baseflow_array()
        self.cpr_mm = self.get_cpr_array()

    def get_baseflow_array(self):
        cbb = flopy.utils.CellBudgetFile(self.mfoutput_dir+'/'+self.cbbfile)
        riv_raw = cbb.get_data(text='RIV')
        riv = [(item['node'],item['q'],item['q']) for item in riv_raw]
        ncol, nrow = self.config.Ncol, self.config.Nrow
        baseflow = []
        print('start extracting the baseflow from the cbb file \n')
        for i in range(len(riv[0][0])):
            idx = riv[0][0][i]
            lay = 0
            row = idx//ncol
            col = idx%ncol-1
            flow = riv[0][2][i]
            rec = [lay,row,col,flow]
            baseflow.append(rec)
        totalbaseflow = 0
        totalriverleakage = 0
        
        baseflow_array = np.zeros((1,nrow,ncol))
        for item in baseflow:
            lay, row, col, flow = item
            flow /= 86400
            if flow<0:
                totalbaseflow += flow
            else:
                totalriverleakage += flow
            baseflow_array[lay,row,col] = flow
        print('the total baseflow is :',totalbaseflow,'m3/day')
        print('the total river leakage is :',totalriverleakage,'m3/day')
        #baseflow_array[baseflow_array == -8888] = np.nan
        return baseflow_array

    def savebf2nc(self):
        indus_file_path = '/lustre/nobackup/WUR/ESG/yuan018/99Unuse/domain_Indus.nc'
        forcinginputdir = self.config.paths.vicextrainput_dir
      
        
        if self.current_date.month == 12:
            output_file = os.path.join(forcinginputdir, f"forcing_discharge_{self.current_date.year+1}.nc")
            shutil.copy(indus_file_path, output_file)
            with nc.Dataset(output_file,'a') as nc_file:
                nc_file.createDimension('time', None)
                time_var = nc_file.createVariable('time', 'f8', ('time',))
                time_var.units = 'days since 1968-01-01 00:00:00'
                time_var.calendar = 'standard'
                time_var.standard_name = 'time'
                time_var.long_name = 'time'
                discharge_var = nc_file.createVariable('discharge_mf', 'f4', ('time', 'lat', 'lon'))
                monthly_dates = [datetime(self.current_date.year+1, month, 1) for month in range(1, 13)]
                monthly_times = date2num(monthly_dates, units=time_var.units, calendar=time_var.calendar)
                time_var[:] = monthly_times
                baseflow2forcing = -1 * np.flip(self.baseflow_array[0], axis=0)
                discharge_var[0,:,:] = baseflow2forcing
        else:
            output_file = os.path.join(forcinginputdir, f"forcing_discharge_{self.current_date.year}.nc")
            if not os.path.exists(output_file):
                shutil.copy(indus_file_path, output_file)
                with nc.Dataset(output_file, 'a') as nc_file:
                    nc_file.createDimension('time', 12)
                    time_var = nc_file.createVariable('time', 'f8', ('time',))
                    time_var.units = f'days since {self.current_date.year}-01-01 00:00:00'
                    time_var.calendar = 'standard'
                    time_var.standard_name = 'time'
                    time_var.long_name = 'time'
                    discharge_var = nc_file.createVariable('discharge_mf', 'f4', ('time', 'lat', 'lon'))

                    # 为每个月创建一个时间点
                    monthly_dates = [datetime(self.current_date.year, month, 1) for month in range(1, 13)]
                    monthly_times = date2num(monthly_dates, units=time_var.units, calendar=time_var.calendar)
                    time_var[:] = monthly_times
            # 根据月份的不同来管理NetCDF文件和数据更新
            with nc.Dataset(output_file, 'a') as nc_file:
                time_var = nc_file.variables['time']
                discharge_var = nc_file.variables['discharge_mf']

                # 查找当前月份在时间变量中的索引
                new_date = self.current_date + relativedelta(months=1)
                new_time = date2num(new_date, units=time_var.units, calendar=time_var.calendar)
                month_index = np.where(time_var[:] == new_time)[0][0]
                baseflow2forcing = -1 * np.flip(self.baseflow_array[0], axis=0)
                discharge_var[month_index, :, :] = baseflow2forcing
                
                
    def get_cpr_array(self):
        cbb = flopy.utils.CellBudgetFile(self.mfoutput_dir+'/'+self.cbbfile)
        cellarea = self.config.paths.cellarea
        
        evt_raw = cbb.get_data(text = 'EVT')
        evt = [(item['node'],item['q'],item['q']) for item in evt_raw]
        ncol, nrow = 204,180
        cpr = []
        for i in range(len(evt[0][0])):
            idx = evt[0][0][i]
            lay = 0
            row = idx//ncol
            col = idx%ncol-1
            flow = evt[0][2][i]
            rec = [lay,row,col,flow]
            cpr.append(rec)
        totalcpr = 0
        cpr_array = np.full((nrow, ncol), 0)

        totalcpr = 0
        for item in cpr:
            lay, row, col, flow = item
            totalcpr += flow
            cpr_array[row,col] = flow

        cpr_mm = cpr_array/cellarea
      
        print(f'the total capillary rise is {totalcpr} m3/d')
        
        return cpr_mm
                                      
                                      
                                      
                                      
                                      
                                      
                                      
# %%
