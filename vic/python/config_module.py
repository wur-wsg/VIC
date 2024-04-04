#%%
# config_module.py

import os
from osgeo import gdal
import numpy as np
import netCDF4 as nc
from datetime import datetime 
from dataclasses import dataclass
from matplotlib import pyplot as plt
import calendar
@dataclass
class Pathconfig:
    cwd: str = '/lustre/nobackup/WUR/ESG/liu297/gitrepo/VIC-WUR-GWM-1910/vic_online/'
    template_dir: str = os.path.join(cwd, 'python', 'VIC_config_file_naturalized_template_pyread_txt')
    statefile_dir: str = os.path.join(cwd , 'python', 'statefile')
    configfile_dir: str =  os.path.join(cwd , 'python', 'configfile')
    vic_executable: str = '/lustre/nobackup/WUR/ESG/liu297/vic_indus/11indus_run/99vic_offline_src/drivers/image/vic_image_gwm.exe'
    vic_param: str = '/lustre/nobackup/WUR/ESG/liu297/vic_indus/11indus_run/05Parameters/VIC_params_Modis_calibrated_Indus.nc' #TODO: later if you want to change it? 
    vic_derived_param: str = '/lustre/nobackup/WUR/ESG/liu297/vic_indus/11indus_run/05Parameters/VIC_derived_param_for_coupling.nc' #TODO: later if you want to change it?
    mfinput_dir: str = '/lustre/nobackup/WUR/ESG/yuan018/04Input_Indus/'  #TODO: later move everything from here to my own folder
    vicextrainput_dir: str = os.path.join(cwd, 'python', 'vicextrainput')
    output_dir: str = os.path.join(cwd, 'python', 'output')
    mfoutput_dir: str = os.path.join(cwd, 'python', 'mfoutput')
    mf6exe: str = '/lustre/nobackup/WUR/ESG/yuan018/mf6.4.1_linux/bin/mf6'
    clonemap: gdal.Dataset = gdal.Open(mfinput_dir + 'Clone_05min.nc')
    dem_ini: np.ndarray = gdal.Open(mfinput_dir + 'demfrom30s.nc').ReadAsArray()
    aqdepth_ini: np.ndarray = gdal.Open(mfinput_dir + 'damc_ave.nc').ReadAsArray()
    ksat_log: np.ndarray = gdal.Open(mfinput_dir + 'lkmc_ave.nc').ReadAsArray()
    cellarea: np.ndarray = gdal.Open(mfinput_dir + 'Indus_CellArea_m2_05min.nc').ReadAsArray()
    qbank: np.ndarray =gdal.Open(mfinput_dir + 'Qbank_new_average.nc').ReadAsArray()
    riv_slope1: np.ndarray = gdal.Open(mfinput_dir+'slope05min_avgFrom30sec.nc').ReadAsArray()
    Z0_floodplain: np.ndarray = gdal.Open(mfinput_dir+'efplact_new_05min.nc').ReadAsArray()
    #qbank: np.ndarray = gdal.Open(mfinput_dir + 'mean_discharge_edwinInput.nc').ReadAsArray()
    min_dem: np.ndarray = gdal.Open(mfinput_dir+'mindem_05min.nc').ReadAsArray()
    KQ3: np.ndarray = gdal.Open(mfinput_dir + 'Recess_NEW.nc').ReadAsArray()
    conflayers: np.ndarray = gdal.Open(mfinput_dir + 'conflayers4.nc').ReadAsArray()
    ksat_l1_conf_log: np.ndarray = gdal.Open(mfinput_dir + 'kl1B_ave.nc').ReadAsArray()
    ksat_l2_conf_log: np.ndarray = gdal.Open(mfinput_dir + 'kl2B_ave.nc').ReadAsArray()
    spe_yi_inp: np.ndarray = gdal.Open(mfinput_dir+ 'StorCoeff_NEW.nc').ReadAsArray()
    vic_output_file: nc.Dataset = nc.Dataset('/lustre/nobackup/WUR/ESG/yuan018/04Input_Indus/fluxes_Modis_GFDL-ESM4adj_historical_1970_2000_naturalized_offline_.1968-01.nc') 
    nc_boundary: nc.Dataset = nc.Dataset(mfinput_dir + 'boundary.nc')
    bdmask: np.ndarray = nc_boundary.variables['idomain'][:].data
    topl1_gwl: nc.Dataset = nc.Dataset(mfinput_dir + 'topl1_gwl_Indus_monthly_1968to2000.nc')
    gwll1: np.ndarray = topl1_gwl.variables['gwl'][:].data
    gwll1: np.ndarray = np.flip(gwll1, axis=1)
    topl2_gwl: nc.Dataset = nc.Dataset(mfinput_dir + 'topl2_gwl_Indus_monthly_1968to2000.nc')
    gwll2: np.ndarray = topl2_gwl.variables['gwl'][:].data
    gwll2: np.ndarray = np.flip(gwll2, axis=1)
    initialhead: nc.Dataset = nc.Dataset(mfinput_dir + 'Indus_gwl_steady_state_final.nc')
    cpr: nc.Dataset = nc.Dataset('/lustre/nobackup/WUR/ESG/liu297/gitrepo/VIC-WUR-GWM-1910/vic_online/python/mfinput/CPsurface.nc')
    cvdir: str = '/lustre/nobackup/WUR/ESG/liu297/vic_indus/11indus_run/04Forcing/02HumanImpact/coverage_monthly_MIRCA/'
    
    def set_cwd(self, cwd):
        self.cwd = cwd
    def set_template_dir(self, template_dir):
        self.template_dir = template_dir
    def set_statefile_dir(self, statefile_dir):
        self.statefile_dir = statefile_dir    
    def set_configfile_dir(self, configfile_dir): # Directory to store the vic configuration files
        self.configfile_dir = configfile_dir
    def set_vic_executable(self, vic_executable): # VIC executable
        self.vic_executable = vic_executable
    def set_template_dir(self, template_dir): # VIC configuration file template
        self.template_dir = template_dir
    def set_output_dir(self, output_dir): # Directory to store the vic output files
        self.output_dir = output_dir
    def set_mfinput_dir(self, mfinput_dir): # raw input data directory for modflow simulation
        self.mfinput_dir = mfinput_dir
    def set_mfoutput_dir(self, mfoutput_dir): # Directory to store the modflow output files and the modflow generated inputs
        self.mfoutput_dir = mfoutput_dir
    def set_mf6exe(self, mf6exe): # modflow executable
        self.mf6exe = mf6exe
    def set_clonemap(self, clonemap): # a map to determin the extend and resolution of the MODFLOW simulation
        self.clonemap = clonemap
    def set_dem_ini(self, dem_ini):# raw dem data (modflow)
        self.dem_ini = dem_ini
    def set_aqdepth_ini(self, aqdepth_ini): # raw aquifer depth data (modflow)
        self.aqdepth_ini = aqdepth_ini
    def set_ksat_log(self, ksat_log): # raw ksat data (modflow)
        self.ksat_log = ksat_log
    def set_cellarea(self, cellarea): # cell area data (modflow, change the geographical grid in vic into orthogonal grid in modflow)
        self.cellarea = cellarea
    def set_nc_qbank(self, nc_qbank): # raw 30 year river discharge data (modflow input for RIV_bot_elevation )
        self.nc_qbank = nc_qbank
    def set_allqbank(self, allqbank): 
        self.allqbank = allqbank
    def set_riv_slope1(self, riv_slope1): # raw river slope data (modflow)
        self.riv_slope1 = riv_slope1
    def set_Z0_floodplain(self, Z0_floodplain):  # raw floodplain depth data (modflow)
        self.Z0_floodplain = Z0_floodplain
    def set_min_dem(self, min_dem): # raw minimum dem data (modflow)
        self.min_dem = min_dem
    def set_KQ3(self, KQ3): # raw KQ3 data (modflow)
        self.KQ3 = KQ3
    def set_conflayers(self, conflayers): # raw conflayers data (modflow)
        self.conflayers = conflayers
    def set_ksat_l1_conf_log(self, ksat_l1_conf_log): # raw ksat_l1 data (modflow, but it is the bottom layer))
        self.ksat_l1_conf_log = ksat_l1_conf_log 
    def set_ksat_l2_conf_log(self, ksat_l2_conf_log): # raw ksat_l2 data (modflow, but it is the top layer))
        self.ksat_l2_conf_log = ksat_l2_conf_log
    def set_spe_yi_inp(self, spe_yi_inp): # raw specific yield data (modflow)
        self.spe_yi_inp = spe_yi_inp

    def set_vic_output_file(self, vic_output_file): # vic output file for the current time step. it must be specified for each time step, the default it just a random file with vic results
        self.vic_output_file = vic_output_file
    def set_vicextra_input_dir(self, vicextrainput_dir): # Directory to store the vic extra input files
        self.vicextrainput_dir = vicextrainput_dir
        
    def set_nc_boundary(self, nc_boundary): 
        self.nc_boundary = nc_boundary
    def set_bdmask(self, bdmask):
        self.bdmask = bdmask
    def set_topl1_gwl(self, topl1_gwl):
        self.topl1_gwl = topl1_gwl
    def set_gwll1(self, gwll1):
        self.gwll1 = gwll1
    def set_topl2_gwl(self, topl2_gwl):
        self.topl2_gwl = topl2_gwl
    def set_gwll2(self, gwll2):
        self.gwll2 = gwll2
    def set_missingvalue(self, missingvalue):
        self.missingvalue = missingvalue
    def set_idomain(self, idomain):
        self.idomain = idomain
    def set_qbank(self, qbank):
        self.qbank = qbank
    def set_cpr(self, cpr):
        self.cpr = cpr
    def close_all_nc(self):
        #self.vic_output_file.close()
        self.nc_boundary.close()
        self.topl1_gwl.close()
        self.topl2_gwl.close()
           
        
class config:
    def __init__(self): #without specifying the input, the default input will be used as below: 
        self.paths = Pathconfig()
        self.startstamp =  datetime(1968, 1, 1)
        self.ts_gwrecharge = None
        self.ts_discharge = None
        #self.ts_gwrecharge = self.paths.vic_output_file.variables['OUT_GWRECHARGE'][0,:,:].data
        #self.ts_gwrecharge = np.flip(self.ts_gwrecharge, axis=1)
        #self.ts_discharge = self.paths.vic_output_file.variables['OUT_DISCHARGE'][0,:,:].data
        #self.ts_discharge = np.flip(self.ts_discharge, axis=1)   #TODO: later on this needs to be updated with vic output file for each time step
        self.bdmask = self.paths.nc_boundary.variables['idomain'][:].data 
        self.humanimpact = False
       
        #from here on are some derived variables based on the variables above:
        self.missingvalue = self.paths.aqdepth_ini[0][0] 
        self.idomain = self.paths.bdmask.astype(int)
        self.Nlay = 2  # number of layers in modflow
        self.Nrow, self.Ncol = self.paths.bdmask.shape  # number of rows and columns in modflow
        self.delrow = self.paths.clonemap.GetGeoTransform()[1]*111*1000 # cell size in y direction in modflow

        self.delcol = abs(self.paths.clonemap.GetGeoTransform()[5]*111*1000) # cell size in x direction in modflow
        self.stress_period = -1
    def set_ts_discharge(self, ts_discharge):
        self.ts_discharge = ts_discharge
        
    def set_ts_gwrecharge(self, ts_gwrecharge): # raw groundwater recharge data from the vic simulation for modflow
        self.ts_gwrecharge = ts_gwrecharge        
    
    def set_startstamp(self, startstamp): # VIC start time
        self.startstamp = startstamp
    
    def timestep_counter(self): #remember to call this function after each time step
        self.stress_period += 1
        return self.stress_period    
    def set_humanimpact(self, humanimpact): # whether to vic simulation options for human impact is turned on
        self.humanimpact = humanimpact
    def cal_aqdepth(self):
        self.aqdepth = self.paths.aqdepth_ini.copy()
        self.aqdepth[self.paths.bdmask==-1] = 200
        self.aqdepth[self.paths.bdmask==0] = 200
        self.aqdepth[self.paths.bdmask==-2] = 200
        self.aqdepth = np.where(self.aqdepth > 0, self.aqdepth, 200)
        self.aqdepth[self.aqdepth==self.missingvalue] = 200
        return self.aqdepth
    
    def cal_toplayer_elevation(self):
        combined_mask = np.logical_or(self.paths.bdmask == 1, self.paths.bdmask == 2)
        self.top_layer1 = np.where(combined_mask, self.paths.dem_ini, 0)
        self.top_layer1 = np.where(self.top_layer1== self.missingvalue, 0, self.top_layer1)
        return self.top_layer1
    
    def cal_botlayer_elevation(self):
        if not hasattr(self, 'aqdepth'): # lazy loading/ on demand loading
            self.cal_aqdepth()        
        bot_layer1 = self.top_layer1-(self.aqdepth*0.1) #second layer is 10% total thickness
        bot_layer1 = top_layer2 = np.where(bot_layer1==self.missingvalue, -20 ,bot_layer1)
        top_layer2 = np.where(top_layer2==self.missingvalue, -20 ,top_layer2)
        bot_layer2 = self.top_layer1-self.aqdepth
        bot_layer2 = np.where(bot_layer2==self.missingvalue, -200 ,bot_layer2)
        self.botm = [bot_layer1, bot_layer2]
        return self.botm
    def get_initial_head(self): #this is only for the first time step. 
        startinghead_layer1 = self.paths.initialhead.variables['tophead'][:].data  
        startinghead_layer1 = np.flip(startinghead_layer1,axis = 0)
        #startinghead_layer1 = np.where(np.isnan(startinghead_layer1), 200, startinghead_layer1)
        
        startinghead_layer2 = self.paths.initialhead.variables['bothead'][:].data    
        
        #startinghead_layer2 = np.where(np.isnan(startinghead_layer2), 200, startinghead_layer2)
        startinghead_layer2 = np.flip(startinghead_layer2,axis = 0)
        self.startinghead = [startinghead_layer1, startinghead_layer2]
        return self.startinghead
    
    def get_npf_param(self): 
        if not hasattr(self, 'aqdepth'): # lazy loading/ on demand loading
            self.cal_aqdepth()        
        rho_water,miu_water,g_gravity = 1000,0.001,9.81
        dikte_l1_ini = self.aqdepth *0.1  #top layer
        dikte_l2_ini = self.aqdepth -dikte_l1_ini  #bottom layer
        dikte_l1= dikte_l1_ini.copy() 
        dikte_l1[self.bdmask == -1] = self.missingvalue
        dikte_l1[self.bdmask == 0] = self.missingvalue
        dikte_l1[self.bdmask == -2] = self.missingvalue
        dikte_l2 = dikte_l2_ini.copy()
        dikte_l2[self.bdmask == -1] = self.missingvalue
        dikte_l2[self.bdmask == 0] = self.missingvalue
        dikte_l2[self.bdmask == -2] = self.missingvalue
        conflayers =  np.where(self.paths.conflayers !=1, 0, self.paths.conflayers)  
        ksat_inp = rho_water * (10 ** self.paths.ksat_log) * (g_gravity / miu_water) * 24.0 * 3600.0
        ksat_l2_conf=rho_water* (10**self.paths.ksat_l2_conf_log)* (g_gravity/ miu_water) *24.0 *3600.0 # coarse grained
        ksat_l1_conf=rho_water* (10**self.paths.ksat_l1_conf_log)* (g_gravity/ miu_water) *24.0 *3600.0 # fine grained
        khoriz_l2_ini = np.where(ksat_l2_conf != 0, ksat_l2_conf, ksat_inp)
        khoriz_l1_ini =	np.where(ksat_l1_conf != 0, ksat_l1_conf, ksat_inp)
        khoriz_l2_ini = np.clip(khoriz_l2_ini,a_min = 0.01, a_max = None)
        khoriz_l1_ini = np.clip(khoriz_l1_ini,a_min = 0.01, a_max = None)  
        kvert_l1_ini = np.where(khoriz_l1_ini >-999,khoriz_l1_ini*self.paths.cellarea/(self.delcol*self.delrow), (10.0*self.paths.cellarea/(self.delcol*self.delrow)))
        kvert_l2 = np.where(khoriz_l2_ini>-999, (10.0*self.paths.cellarea/(self.delcol*self.delrow)),(10.0*self.paths.cellarea/(self.delcol*self.delrow)))
        kvert_l1 = np.maximum(dikte_l1/5000,kvert_l1_ini)
        kd_l1_ini = khoriz_l1_ini.astype(np.float128)*(dikte_l1).astype(np.float128)
        kd_l2_ini = khoriz_l2_ini.astype(np.float128)*(dikte_l2).astype(np.float128)
        kd_l1 = np.maximum(30,kd_l1_ini)
        kd_l2 = np.maximum(30,kd_l2_ini)
        khoriz_l1 = kd_l1/dikte_l1
        khoriz_l2 = kd_l2/dikte_l2
        khoriz_l1[khoriz_l1<0]  = 20
        khoriz_l2[khoriz_l2<0]  = 190
        kvert_l1[kvert_l1<0] = 1e10
        kvert_l2[kvert_l2<0] = 1e10
        k_hor = [khoriz_l1, khoriz_l2]
        k_ver = [khoriz_l1*0.1,khoriz_l2*0.1]    
        spe_yi_inp = self.paths.spe_yi_inp
        spe_yi_inp = np.where(self.bdmask >=1, spe_yi_inp,self.missingvalue)
        spe_yi_inp = np.maximum(0.01,spe_yi_inp)
        spe_yi_inp = np.minimum(1,spe_yi_inp)
        spe_yi_inp = np.where(self.aqdepth >-999.9,np.maximum(spe_yi_inp, 0.11),spe_yi_inp)
        stor_prim = spe_yi_inp
        stor_sec = spe_yi_inp
        stor=[stor_prim,stor_prim]
        return k_hor,k_ver,stor

    def get_rch_param(self,current_date):
        _, numofdays = calendar.monthrange(current_date.year, current_date.month)
        
        rch_nat = self.ts_gwrecharge
        

        rch_nat = np.flip(rch_nat, axis=0)
        rcmissingvalue = rch_nat[179][0]
        rch_nat[self.paths.bdmask == 2 & (rch_nat ==rcmissingvalue)] = 0
        recharge_inp = ((rch_nat/(numofdays)*self.paths.cellarea)/(self.delcol*self.delrow))
        recharge_inp = np.where(recharge_inp < 10000, recharge_inp, 0)
        npzero = np.zeros_like(recharge_inp)
        # recharge on the top layer    
        recharge_inp = np.maximum(recharge_inp,npzero)
        # recharge_inp[idomain == -1]=np.nan
        recharge_inp[self.idomain == -1]=0
        recharge_inp[self.idomain == -2]=0
        recharge_inp[self.idomain == 0]=np.nan
        nrow, ncol = recharge_inp.shape

        cellids = [(0, i, j) for i in range(nrow) for j in range(ncol)]
        # Create stress_period_data as a list of lists (leave it for mf6)
        RCHstress_period_data = []
        for cellid, value, bdmask_ind in zip(cellids, recharge_inp.flatten(), self.paths.bdmask.flatten()):
            if np.isnan(value) or value ==0 or bdmask_ind != 2:
            # if np.isnan(value) or value ==0:
                continue
            cellid_1, cellid_2, cellid_3 = cellid
            RCHstress_period_data.append([cellid_1, cellid_2, cellid_3, value])

        return RCHstress_period_data
    
    def get_riv_param(self):
        if not hasattr(self, 'top_layer1'):
            self.cal_toplayer_elevation()
        qaverage = self.paths.qbank
        monthly_discharge = self.ts_discharge

        monthly_discharge = np.flip(monthly_discharge, axis=0)
        riv_manning, resistance, riv_bedres_inp = 0.045,1.0,1.0000
        min_dem2 = np.where(self.paths.min_dem <0,0,self.paths.min_dem)
        Z0_floodplain1 = np.maximum(min_dem2, self.paths.Z0_floodplain)
        Z0_floodplain2 = np.where(self.paths.Z0_floodplain < 0.0, Z0_floodplain1, self.paths.Z0_floodplain)
        riv_width = 4.8* ((qaverage)**0.5)
        riv_width = np.maximum(riv_width, 0.5)
                
        riv_slope = np.where(riv_width > 0.0, self.paths.riv_slope1,self.missingvalue)
        riv_slope_used = np.where(riv_slope>0.00005, riv_slope,self.missingvalue)
        riv_head_ini = np.where(riv_width > 30.0, Z0_floodplain2, self.top_layer1)
        # riv_head_ini = np.where(riv_width > 30.0, Z0_floodplain2, topl2)
        riv_head_ini[self.bdmask==-1] = 0  
        riv_head_ini[self.bdmask==0] = 0
        riv_head_ini[self.bdmask==-2] = 0   
                
        riv_depth_bkfl = ((riv_manning*(qaverage)**0.5)/(riv_width*riv_slope_used**0.5))**(3.0/5.0)
        riv_depth_bkfl = np.where(np.isnan(riv_depth_bkfl),self.missingvalue,riv_depth_bkfl)
        riv_bot_bkfl = min_dem2 - riv_depth_bkfl
        
        riv_depth_avg1 = ((riv_manning*((monthly_discharge)**0.5))/(riv_width*riv_slope_used**0.5))**(3.0/5.0) #  
        riv_depth_avg = np.where(riv_depth_avg1 < 0.01, 0.0, riv_depth_avg1)
        riv_depth_avg = np.where(np.isnan(riv_depth_avg),0,riv_depth_avg)
        riv_head=riv_bot_bkfl + riv_depth_avg
        riv_head2 = np.where(np.isnan(riv_head), riv_head_ini, riv_head)
        riv_head2 = np.where(riv_head == -1* self.missingvalue, riv_head_ini, riv_head)
        denominator = np.where(riv_width >= 30.0, riv_width * (self.paths.cellarea*2.0) ** 0.5, 0.0)
        riv_cond =  1 / resistance * denominator
        riv_cond[self.bdmask == -1]=0
        riv_cond[self.bdmask == 0]=0
        riv_cond[self.bdmask == -2]=0
        drn_width1 = np.maximum(10,riv_width)
        drn_width = np.where(riv_cond == 0.0, drn_width1, 0.0)
        drn_cond = np.where(riv_cond ==0, (1.0/resistance)*drn_width*(self.paths.cellarea*2)**0.5,0.0)
        drn_cond = np.where(drn_cond<1e-20, 0,drn_cond)
        riv_head_comb = np.where(riv_cond > 0.0, riv_head2, riv_head_ini)
        riv_head_comb = np.where(np.isnan(riv_head_comb),0,riv_head_comb)
        riv_head_comb[self.bdmask==-1]=np.nan
        riv_head_comb[self.bdmask==0]=np.nan
        riv_head_comb[self.bdmask==-2]=np.nan
        riv_bot_comb = np.where(riv_cond >0.0, riv_bot_bkfl,riv_head_ini)#TODO
        riv_bot_comb[np.isinf(riv_bot_comb)] = 0
        riv_bot_comb = np.where(riv_bot_comb == -(self.missingvalue), np.nan, riv_bot_comb) #TODO
        riv_bot_comb[self.bdmask==-1]=np.nan
        riv_bot_comb[self.bdmask==0]=np.nan
        riv_bot_comb[self.bdmask==-2]=np.nan
        riv_cond_comb = np.where(riv_cond>0, riv_cond, drn_cond)
        riv_cond_comb[self.bdmask==-1]=np.nan
        riv_cond_comb[self.bdmask==0]=np.nan
        riv_cond_comb[self.bdmask==-2]=np.nan

        
        nrow, ncol = self.bdmask.shape
        cellids = [(0, i, j) for i in range(nrow) for j in range(ncol)]
        RIVstress_period_data = []
        for cellid, stage, rbot, cond, bdmask_ind in zip(cellids, riv_head_comb.flatten(), riv_bot_comb.flatten(), riv_cond_comb.flatten(), self.bdmask.flatten()): 
            # Skip cellids with NaN values in stage or rbot
            # if np.isnan(stage) or np.isnan(rbot) or np.isnan(cond) or cond == 0 or bdmask_ind == 0 or bdmask_ind == -1:
            if np.isnan(stage) or np.isnan(rbot) or np.isnan(cond) or cond == 0 or bdmask_ind != 2:
                continue
            cellid_1, cellid_2, cellid_3 = cellid
            RIVstress_period_data.append([cellid_1, cellid_2, cellid_3, stage, cond, rbot])
        return RIVstress_period_data
    def get_chd_input(self):
        CHDstress_period_data = []
        # first layer (top layer)
        nrow, ncol = self.bdmask.shape
        cellids = [(0, i, j) for i in range(nrow) for j in range(ncol)] # 创建一个包含元组的列表，每个元组包含三个值：0、i 和 j
        for cellid, id_num in zip(cellids, self.idomain.flatten()):
            cellid_1, cellid_2, cellid_3 = cellid
            # if id_num != 1:# 
            if id_num != 1 and id_num != 4:# 
                continue
            if id_num == 4:# 
                # gwll2[stress_period][cellid_2][cellid_3] = 0
                self.paths.gwll1[self.stress_period][cellid_2][cellid_3] = 0
            # if np.isnan(gwll2[stress_period][cellid_2][cellid_3]):
            if np.isnan(self.paths.gwll1[self.stress_period][cellid_2][cellid_3]):
                # gwll2[stress_period][cellid_2][cellid_3] = 0
                self.paths.gwll1[self.stress_period][cellid_2][cellid_3] = 0
            # CHDstress_period_data.append([cellid_1, cellid_2, cellid_3, gwll2[stress_period][cellid_2][cellid_3]]) # cellid_1 is layer number: 0 for top, 1 for bot
            CHDstress_period_data.append([cellid_1, cellid_2, cellid_3, self.paths.gwll1[self.stress_period][cellid_2][cellid_3]]) # cellid_1 is layer number: 0 for top, 1 for bot

        # second layer (bot layer)
        nrow, ncol = self.bdmask.shape
        cellids = [(1, i, j) for i in range(nrow) for j in range(ncol)]
        for cellid, id_num in zip(cellids, self.idomain.flatten()):
            cellid_1, cellid_2, cellid_3 = cellid
            # if id_num != 1:
            if id_num != 1 and id_num != 4:# 
                continue
            if id_num == 4:# 
                # gwll1[stress_period][cellid_2][cellid_3] = 0
                self.paths.gwll2[self.stress_period][cellid_2][cellid_3] = 0
            # if np.isnan(gwll1[stress_period][cellid_2][cellid_3]):
            if np.isnan(self.paths.gwll2[self.stress_period][cellid_2][cellid_3]):
                # gwll1[stress_period][cellid_2][cellid_3] = 0
                self.paths.gwll2[self.stress_period][cellid_2][cellid_3] = 0
            # CHDstress_period_data.append([cellid_1, cellid_2, cellid_3, gwll1[stress_period][cellid_2][cellid_3]]) # cellid_1 is layer number: 0 for top, 1 for bot
            CHDstress_period_data.append([cellid_1, cellid_2, cellid_3, self.paths.gwll2[self.stress_period][cellid_2][cellid_3]]) # cellid_1 is layer number: 0 for top, 1 for bot
        return CHDstress_period_data  
    
    def get_cpr_param(self):
        CPRstress_period_data = []
        CPsurface = np.flip(self.paths.cpr.variables['cpsurface'][:].data, axis=0)
        CPrate = np.flip(self.paths.cpr.variables['max_capillary_flux'][:].data, axis=0)
        CPdistinct = np.flip(self.paths.cpr.variables['distinction_depth'][:].data, axis=0)
        
        nrow, ncol = self.bdmask.shape
        cellids = [(0, i, j) for i in range(nrow) for j in range(ncol)]        
        for cellid,surface,rate,distinction,bdmask in zip(cellids,CPsurface.flatten(),CPrate.flatten(),CPdistinct.flatten(),self.bdmask.flatten()):
            cellid_1, cellid_2, cellid_3 = cellid
            if np.isnan(surface) or np.isnan(rate) or np.isnan(distinction) or bdmask <=0 or rate==0:
                continue
            CPRstress_period_data.append([(cellid_1, cellid_2, cellid_3), surface, rate, distinction])
        
        return CPRstress_period_data
    
        

        
            
        

#%%
config_indus_ubuntu = config()


#cwd = '/lustre/nobackup/WUR/ESG/liu297/gitrepo/VIC-WUR-GWM-1910/vic_online/'
#config_indus_ubuntu.paths.set_template_dir(os.path.join(cwd, 'python', 'VIC_config_file_naturalized_template_pyread_anunna.txt'))
#config_indus_ubuntu.paths.set_statefile_dir(os.path.join(cwd, 'python', 'statefile'))
#config_indus_ubuntu.paths.set_configfile_dir(os.path.join(cwd, 'python', 'configfile'))
#config_indus_ubuntu.paths.set_vic_executable('/lustre/nobackup/WUR/ESG/liu297/vic_indus/11indus_run/99vic_offline_src/drivers/image/vic_image_gwm.exe')
#config_indus_ubuntu.paths.set_startstamp(datetime(1968, 1, 1))
#config_indus_ubuntu.paths.set_mfinput_dir('/lustre/nobackup/WUR/ESG/yuan018/04Input_Indus/')
#config_indus_ubuntu.paths.set_mfoutput_dir('/lustre/nobackup/WUR/ESG/liu297/gitrepo/VIC-WUR-GWM-1910/vic_online/python/mfoutput/workspace/')
#config_indus_ubuntu.set_humanimpact(False)
