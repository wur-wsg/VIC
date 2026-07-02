#%%
# config_module.py
import os
from osgeo import gdal
import numpy as np
import netCDF4 as nc
from datetime import datetime 
from dataclasses import dataclass
from matplotlib import pyplot as plt
import xarray as xr
import calendar
import calendar

PROJECT_ROOT = os.environ.get('VIC_COUPLED_PROJECT_ROOT', '/lustre/nobackup/WUR/ESG/liu297/vic_coupled')
VIC_REPO_ROOT = os.environ.get('VIC_REPO_ROOT', os.path.join(PROJECT_ROOT, 'VIC'))
PLUGIN_DIR = os.environ.get('VIC_MODFLOW_PLUGIN_DIR', os.path.join(VIC_REPO_ROOT, 'vic', 'plugins', 'modflow'))
DATA_ROOT = os.environ.get('VIC_MF_DATA_ROOT', os.path.join(PROJECT_ROOT, 'Data'))
SIMULATION_ROOT = os.environ.get('VIC_MF_SIMULATION_ROOT', os.path.join(PROJECT_ROOT, 'Simulation'))
CONFIG_DIR = os.environ.get('VIC_MF_CONFIG_DIR', os.path.join(SIMULATION_ROOT, 'configuration'))
SLURM_DIR = os.environ.get('VIC_MF_SLURM_DIR', os.path.join(SIMULATION_ROOT, 'slurmjob'))
LOG_DIR = os.environ.get('VIC_MF_LOG_DIR', os.path.join(SIMULATION_ROOT, 'log'))
RESULT_DIR = os.environ.get('VIC_MF_RESULT_DIR', os.path.join(SIMULATION_ROOT, 'result'))
STATE_DIR = os.environ.get('VIC_MF_STATE_DIR', os.path.join(SIMULATION_ROOT, 'state'))
TMP_DIR = os.environ.get('VIC_MF_TMP_DIR', os.path.join(SIMULATION_ROOT, 'tmp'))
GLOBAL_STEADY_HEAD_FILE = os.environ.get(
    'VIC_MF_GLOBAL_STEADY_HEAD_FILE',
    '/lustre/nobackup/WUR/ESG/liu297/vic_global/01oc_natural/99input_processing/output_steadystate/mf_ss_gwl_5minDRN.nc',
)


def _env_path(env_name, default):
    return os.environ.get(env_name, default)

@dataclass
class Pathconfig:
    project_root: str = PROJECT_ROOT
    case_name = 'global'
    vic_template_filename = 'vic_global_5min_natural_template.txt'
    vic_domain_filename = 'vic_global_5min_domain_nogl.nc'
    vic_parameter_filename = 'vic_global_5min_params_nogl.nc'
    vic_routing_filename = 'vic_global_5min_routing_param_wbt.nc'
    vic_derived_param_filename = 'capillary_rise_oc_updated.nc'
    mf_static_filenames = {
        'clonemap': 'Clone_05min_oc.nc',
        'dem_ini': 'dem_ini_oc.nc',
        'aqdepth_ini': 'aqdepth_ini_oc.nc',
        'ksat_log': 'ksat_log_oc.nc',
        'cellarea': 'cell_area_oc.nc',
        'qbank': 'bankfull_discharge_1979_2010_vicwur5min.nc',
        'riv_slope1': 'riv_slope_oc.nc',
        'z0_floodplain': 'Z0_floodplain_oc.nc',
        'min_dem': 'min_dem_oc.nc',
        'kq3': 'KQ3_oc.nc',
        'conflayers': 'conflayers_oc.nc',
        'ksat_l1_conf_log': 'ksat_l1_conf_log_oc.nc',
        'ksat_l2_conf_log': 'ksat_l2_conf_log_oc.nc',
        'spe_yi_inp': 'spe_yi_inp_oc.nc',
        'landmask': 'landmask_oc.nc',
        'initialhead': 'daily_bankfull_mf_ss_gwl_5minDRN.nc',
        'ibound': 'ibound_oc.nc',
        'ldd': 'ldd_oc.nc',
        'capillary': 'capillary_rise_oc.nc',
    }
    # paths only — do not load data yet
    def __init__(self):
        self.project_root = self.project_root
        self.vic_repo_root = VIC_REPO_ROOT
        self.plugin_dir = PLUGIN_DIR
        self.data_root = DATA_ROOT
        self.simulation_root = SIMULATION_ROOT
        self.config_dir = CONFIG_DIR
        self.slurm_dir = SLURM_DIR
        self.log_dir = LOG_DIR
        self.result_dir = RESULT_DIR
        self.state_dir = STATE_DIR
        self.tmp_dir = TMP_DIR

        self.vic_domain_dir = _env_path('VIC_MF_VIC_DOMAIN_DIR',
                                        os.path.join(self.data_root, 'VIC', 'domain', self.case_name))
        self.vic_parameter_dir = _env_path('VIC_MF_VIC_PARAMETER_DIR',
                                           os.path.join(self.data_root, 'VIC', 'parameter', self.case_name))
        self.vic_routing_dir = _env_path('VIC_MF_VIC_ROUTING_DIR',
                                         os.path.join(self.data_root, 'VIC', 'routing', self.case_name))
        self.vic_template_dir = _env_path('VIC_MF_VIC_TEMPLATE_DIR',
                                          os.path.join(self.data_root, 'VIC', 'templates', self.case_name))
        self.mf_static_dir = _env_path('VIC_MF_MODFLOW_STATIC_DIR',
                                       os.path.join(self.data_root, 'MODFLOW', 'static', self.case_name))
        self.mf_executable_dir = os.path.join(self.data_root, 'MODFLOW', 'executable')

        self.historical_config_dir = os.path.join(self.config_dir, 'historical')
        self.vic_result_dir = os.path.join(self.result_dir, 'vic')
        self.modflow_result_dir = os.path.join(self.result_dir, 'modflow')
        self.vic_state_dir = os.path.join(self.state_dir, 'vic')
        self.vic_forcing_dir = os.path.join(self.tmp_dir, 'vic_forcing')
        self.modflow_workspace_dir = os.path.join(self.tmp_dir, 'modflow_workspace')

        self.template_dir = _env_path('VIC_MF_TEMPLATE_FILE',
                                      os.path.join(self.vic_template_dir, self.vic_template_filename))
        self.vic_domain_file = _env_path('VIC_MF_VIC_DOMAIN_FILE',
                                         os.path.join(self.vic_domain_dir, self.vic_domain_filename))
        self.vic_parameter_file = _env_path('VIC_MF_VIC_PARAMETER_FILE',
                                            os.path.join(self.vic_parameter_dir, self.vic_parameter_filename))
        self.vic_routing_file = _env_path('VIC_MF_VIC_ROUTING_FILE',
                                          os.path.join(self.vic_routing_dir, self.vic_routing_filename))
        self.vic_derived_param = _env_path('VIC_MF_VIC_DERIVED_PARAM_FILE',
                                           os.path.join(self.mf_static_dir, self.vic_derived_param_filename))

        vic_exe_default = os.path.join(self.vic_repo_root, 'vic', 'drivers', 'image', 'vic_image_coupled.exe')
        self.vic_executable = os.environ.get('VIC_EXE', vic_exe_default)
        self.mf6exe = os.environ.get('MF6_EXE', os.path.join(self.mf_executable_dir, 'mf6'))
        self.global_steady_head_file = GLOBAL_STEADY_HEAD_FILE

        # Backward-compatible aliases for older code paths.
        self.cwd = self.project_root
        self.vic_param = self.vic_parameter_dir
        self.mfinput_dir = self.mf_static_dir
        self.output_dir = self.vic_result_dir
        self.mfoutput_dir = self.modflow_result_dir
        self.statefile_dir = self.vic_state_dir
        self.configfile_dir = self.historical_config_dir
        # Load data
        self.load_data()

    def mf_static_file(self, key):
        return _env_path(f'VIC_MF_{key.upper()}_FILE',
                         os.path.join(self.mf_static_dir, self.mf_static_filenames[key]))

    def get_mode_coupling_name(self, modestr, couplingstr):
        return f'{modestr}_{couplingstr}'

    def get_historical_config_dir(self, modestr, couplingstr):
        return os.path.join(self.historical_config_dir, self.get_mode_coupling_name(modestr, couplingstr))

    def get_vic_result_dir(self, modestr, couplingstr):
        return os.path.join(self.vic_result_dir, self.get_mode_coupling_name(modestr, couplingstr))

    def get_vic_forcing_prefix(self, modestr, couplingstr):
        return os.path.join(self.vic_forcing_dir, f'glob_{modestr}_{couplingstr}_forcing_discharge_')

    def get_vic_forcing_file(self, modestr, couplingstr, year):
        return f'{self.get_vic_forcing_prefix(modestr, couplingstr)}{year}.nc'

    def get_modflow_workspace_dir(self, mfname):
        return os.path.join(self.modflow_workspace_dir, mfname)

    def get_gwl_dir(self, modestr, couplingstr):
        return os.path.join(self.modflow_result_dir, 'gwl', self.get_mode_coupling_name(modestr, couplingstr))

    def load_data(self):
        self.clonemap = gdal.Open(self.mf_static_file('clonemap'))
        self.dem_ini = xr.open_dataarray(self.mf_static_file('dem_ini')).values
        self.aqdepth_ini = xr.open_dataarray(self.mf_static_file('aqdepth_ini')).values
        self.ksat_log = xr.open_dataarray(self.mf_static_file('ksat_log')).values
        self.cellarea = xr.open_dataarray(self.mf_static_file('cellarea')).values
        self.qbank = xr.open_dataarray(self.mf_static_file('qbank')).values
        self.riv_slope1 = xr.open_dataarray(self.mf_static_file('riv_slope1')).values
        self.Z0_floodplain = xr.open_dataarray(self.mf_static_file('z0_floodplain')).values
        self.min_dem = xr.open_dataarray(self.mf_static_file('min_dem')).values
        self.KQ3 = xr.open_dataarray(self.mf_static_file('kq3')).values
        self.conflayers = xr.open_dataarray(self.mf_static_file('conflayers')).values
        self.ksat_l1_conf_log = xr.open_dataarray(self.mf_static_file('ksat_l1_conf_log')).values
        self.ksat_l2_conf_log = xr.open_dataarray(self.mf_static_file('ksat_l2_conf_log')).values
        self.spe_yi_inp = xr.open_dataarray(self.mf_static_file('spe_yi_inp')).values
        self.landmask = xr.open_dataarray(self.mf_static_file('landmask')).values
        self.initialhead = xr.open_dataarray(self.mf_static_file('initialhead')).values
        self.ibound = xr.open_dataarray(self.mf_static_file('ibound')).values
        self.ldd = xr.open_dataarray(self.mf_static_file('ldd')).values
        self.capillary = xr.open_dataset(self.mf_static_file('capillary'))
        self.vic_parameter = xr.open_dataset(self.vic_parameter_file)


class PathconfigIndus(Pathconfig):
    case_name = 'indus'
    vic_template_filename = 'vic_indus_natural_template.txt'
    vic_domain_filename = 'domain_Indus.nc'
    vic_parameter_filename = 'VIC_params_Mirca_calibrated_Indus.nc'
    vic_routing_filename = 'old_rout.nc'
    vic_derived_param_filename = 'capillary_rise_indus.nc'
    mf_static_filenames = {
        'clonemap': 'Clone_05min_indus.nc',
        'dem_ini': 'dem_ini_indus.nc',
        'aqdepth_ini': 'aqdepth_ini_indus.nc',
        'ksat_log': 'ksat_log_indus.nc',
        'cellarea': 'cell_area_indus.nc',
        'qbank': 'bankfull_discharge_indus.nc',
        'riv_slope1': 'riv_slope_indus.nc',
        'z0_floodplain': 'Z0_floodplain_indus_updated.nc',
        'min_dem': 'min_dem_indus.nc',
        'kq3': 'KQ3_indus.nc',
        'conflayers': 'conflayers_indus.nc',
        'ksat_l1_conf_log': 'ksat_l1_conf_log_indus.nc',
        'ksat_l2_conf_log': 'ksat_l2_conf_log_indus.nc',
        'spe_yi_inp': 'spe_yi_inp_indus.nc',
        'landmask': 'landmask_indus.nc',
        'initialhead': 'initial_head_indus.nc',
        'ibound': 'ibound_indus_updated.nc',
        'ldd': 'ldd_indus_updated.nc',
        'capillary': 'capillary_rise_indus.nc',
    }

class config:
    def __init__(self, pathconfig_cls=Pathconfig): #without specifying the input, the default input will be used as below: 
        self.paths = pathconfig_cls()
        self.startstamp =  datetime(1979, 1, 1)
        # Initialize time series arrays to zeros (fast default, correct shapes)
        lm_shape = self.paths.landmask.shape
        self.ts_gwrecharge = np.zeros(lm_shape, dtype=np.float64)  # m/day
        self.ts_discharge = np.zeros(lm_shape, dtype=np.float64)   # m3/s
        self.humanimpact = False # whether to vic simulation options for human impact is turned on
        self.foc = None  # Initialize foc as None - must be explicitly set using set_foc()
        # VIC output file base name suffix (after mode_coupling): e.g., '5min_nogl'
        self.vic_out_suffix = '5min_nogl'
        #self.mfname = self._get_mfname()  # Set initial model name
        #from here on are some derived variables based on the variables above:
        self.dem = self.paths.dem_ini
        self.dem = np.where(self.paths.landmask ==1, self.paths.dem_ini, 200)
        self.dem = np.where(np.isnan(self.paths.landmask),0, self.paths.dem_ini)
        self.missingvalue = self.paths.aqdepth_ini[0][0] 
        self.idomain = self.paths.ibound
        self.ibound = self.paths.ibound
        self.Nlay = 2  # number of layers in modflow
        self.Nrow, self.Ncol = self.paths.landmask.shape  # number of rows and columns in modflow
        self.delrow = self.paths.clonemap.GetGeoTransform()[1]*111*1000 # cell size in y direction in modflow
        self.delcol = abs(self.paths.clonemap.GetGeoTransform()[5]*111*1000) # cell size in x direction in modflow
        self.stress_period = -1  #TODO:  is this still needed? 
        self.dem = self.paths.dem_ini
        self.dem = np.where(self.paths.landmask ==1, self.paths.dem_ini, 200)
        self.dem = np.where(np.isnan(self.paths.landmask),0, self.paths.dem_ini)
        self.missingvalue = self.paths.aqdepth_ini[0][0] 

        
    def set_foc(self, foc):
        """Set coupling type and update model name accordingly."""
        if foc is None:
            raise ValueError("Focus type must be True or False.")
        self.foc = foc
        self.couplingstr = "foc" if self.foc else "poc"
        self.modestr = 'human' if self.humanimpact else 'nat'
        self.mfname = f'mf_{self.modestr}_{self.couplingstr}'


    def set_humanimpact(self, humanimpact): # whether to vic simulation options for human impact is turned on
        self.humanimpact = humanimpact

    def _get_mfname(self):
        """Internal method to determine model name based on coupling type and human impact."""
        if self.humanimpact:
            return 'human_foc' if self.foc else 'human_poc'
        return 'mf_nat_foc' if self.foc else 'mf_nat_poc'
    
    def set_vic_out_suffix(self, suffix: str):
        """Set the VIC output filename suffix used for OUTFILE and path checks."""
        if not isinstance(suffix, str) or not suffix:
            raise ValueError("vic_out_suffix must be a non-empty string")
        # Normalize: strip leading underscores or dots to avoid double separators
        self.vic_out_suffix = suffix.lstrip('._')
        

    def set_ts_discharge(self, ts_discharge):
        arr = np.asarray(ts_discharge)
        if arr.ndim == 3:
            arr = arr[0]
        else:
            arr = np.squeeze(arr)
        if arr.shape != self.paths.landmask.shape:
            raise ValueError(f"ts_discharge shape {arr.shape} does not match grid {self.paths.landmask.shape}")
        # Ensure numeric, replace non-finite with 0 for stability
        arr = np.where(np.isfinite(arr), arr, 0.0).astype(np.float64, copy=False)
        self.ts_discharge = arr
        
    def set_ts_gwrecharge(self, ts_gwrecharge): # raw groundwater recharge data from the vic simulation for modflow
        # Expect m/day (already converted upstream). Normalize dtype/shape for speed.
        arr = np.asarray(ts_gwrecharge)
        if arr.ndim == 3:
            arr = arr[0]
        else:
            arr = np.squeeze(arr)
        if arr.shape != self.paths.landmask.shape:
            raise ValueError(f"ts_gwrecharge shape {arr.shape} does not match grid {self.paths.landmask.shape}")
        # Ensure numeric, replace non-finite with 0 for stability
        arr = np.where(np.isfinite(arr), arr, 0.0).astype(np.float64, copy=False)
        self.ts_gwrecharge = arr
    
    def set_startstamp(self, startstamp): # VIC start time
        self.startstamp = startstamp
    
    def timestep_counter(self): #remember to call this function after each time steo TODO: this one is also not used anymore... check neccissity. 
        self.stress_period += 1
        return self.stress_period    
    
    def cal_aqdepth(self):
        self.aqdepth = np.where(np.isnan(self.paths.landmask),200, self.paths.aqdepth_ini)
        self.aqdepth = np.where(np.isnan(self.aqdepth), 200, self.aqdepth)
        self.aqdepth = np.where(self.aqdepth >= 0, self.aqdepth, 200)
        return self.aqdepth
    
    def cal_toplayer_elevation(self):
        self.top_layer1 = np.where(np.isnan(self.dem), 0, self.dem)
        self.top_layer = self.top_layer1
        return self.top_layer1
    
    def cal_botlayer_elevation(self):
        if not hasattr(self, 'aqdepth'): # lazy loading/ on demand loading
            self.cal_aqdepth()        
        if not hasattr(self, 'top_layer1'): # lazy loading/ on demand loading
            self.cal_toplayer_elevation()
        bot_layer1 = self.top_layer1-(self.aqdepth*0.1) #second layer is 10% total thickness
        bot_layer2 = self.top_layer1-self.aqdepth
        bot_layer1 = np.where(np.isnan(bot_layer1),0, bot_layer1)
        bot_layer2 = np.where(np.isnan(bot_layer2),0, bot_layer2)
        self.botm = [bot_layer1, bot_layer2]
        return self.botm
    
    def _get_non_global_steady_head(self):
        if hasattr(self, '_non_global_steady_head'):
            return self._non_global_steady_head

        ibound_da = xr.open_dataarray(self.paths.mf_static_file('ibound'), mask_and_scale=False)
        gwd = xr.open_dataset(self.paths.global_steady_head_file, mask_and_scale=False)['gwd']
        subset = gwd.sel(
            latitude=ibound_da['lat'].values,
            longitude=ibound_da['lon'].values,
            method='nearest',
        )
        self._non_global_steady_head = subset.values
        return self._non_global_steady_head

    def get_initial_head(self): #this is only for the first time step. 
        if getattr(self.paths, 'case_name', 'global') != 'global':
            steady = self._get_non_global_steady_head()
            startinghead_layer1 = np.where(np.isnan(steady[0]), 0, steady[0])
            startinghead_layer2 = np.where(np.isnan(steady[1]), 0, steady[1])
            self.startinghead = [startinghead_layer1, startinghead_layer2]
            return self.startinghead

        startinghead_layer1 = self.paths.initialhead[0]
        startinghead_layer1 = np.where(np.isnan(startinghead_layer1),0, startinghead_layer1)
        startinghead_layer2 = self.paths.initialhead[1]
        startinghead_layer2 = np.where(np.isnan(startinghead_layer2),0, startinghead_layer2)
        self.startinghead = [startinghead_layer1, startinghead_layer2]
        return self.startinghead
      
    def get_npf_param(self): 
        if not hasattr(self, 'aqdepth'): # lazy loading/ on demand loading
            self.cal_aqdepth()        
        rho_water,miu_water,g_gravity = 1000,0.001,9.81
        dikte_l1_ini = self.aqdepth *0.1  #top layer
        dikte_l2_ini = self.aqdepth - dikte_l1_ini  #bottom layer
        dikte_l1= dikte_l1_ini.copy() 
        dikte_l1 = np.where(np.isnan(self.paths.landmask),np.nan, dikte_l1)
        dikte_l2 = dikte_l2_ini.copy()
        dikte_l2 = np.where(np.isnan(self.paths.landmask),np.nan, dikte_l2)

        ksat_inp = rho_water * (10 ** self.paths.ksat_log) * (g_gravity / miu_water) * 24.0 *3600

        ksat_l2_conf = rho_water * (10 ** self.paths.ksat_l2_conf_log) * (g_gravity / miu_water) * 24.0 * 3600 # coarse grained
        ksat_l1_conf = rho_water * (10 ** self.paths.ksat_l1_conf_log) * (g_gravity / miu_water) * 24.0 * 3600 # fine grained
        khoriz_l2_ini = np.where(~np.isnan(ksat_l2_conf), ksat_l2_conf, ksat_inp)
        khoriz_l1_ini = np.where(~np.isnan(ksat_l1_conf), ksat_l1_conf, ksat_inp)
        khoriz_l2_ini = np.clip(khoriz_l2_ini, a_min=0.01, a_max=None)
        khoriz_l1_ini = np.clip(khoriz_l1_ini, a_min=0.01, a_max=None)

        kvert_l1_ini = np.where(khoriz_l1_ini > -999, khoriz_l1_ini * self.paths.cellarea / (self.delcol * self.delrow), (10.0 * self.paths.cellarea / (self.delcol * self.delrow)))
        kvert_l2 = np.where(khoriz_l2_ini>-999, (10.0*self.paths.cellarea/(self.delcol*self.delrow)),(10.0*self.paths.cellarea/(self.delcol*self.delrow)))
        kvert_l1 = np.maximum(dikte_l1/5000,kvert_l1_ini)
        kd_l1_ini = khoriz_l1_ini * dikte_l1 
        kd_l2_ini = khoriz_l2_ini * dikte_l2
        kd_l1 = np.maximum(30, kd_l1_ini)
        kd_l2 = np.maximum(30, kd_l2_ini)
        khoriz_l1  = kd_l1 / dikte_l1
        khoriz_l2  = kd_l2 / dikte_l2
        khoriz_l1[khoriz_l1 < 0] = 20
        khoriz_l2[khoriz_l2 < 0] = 190
        kvert_l1[kvert_l1 < 0] = 1e10
        kvert_l2[kvert_l2 < 0] = 1e10
        k_hor = [khoriz_l1, khoriz_l2]
        k_ver = [kvert_l1 ,kvert_l2]

        spe_yi_inp = self.paths.spe_yi_inp
        spe_yi_inp = np.where(self.paths.landmask >= 1, spe_yi_inp, self.missingvalue)
        spe_yi_inp = np.maximum(0.01, spe_yi_inp)
        spe_yi_inp = np.minimum(1, spe_yi_inp)
        spe_yi_inp = np.where(self.aqdepth > -999.9, np.maximum(spe_yi_inp, 0.11), spe_yi_inp)
        stor_prim = spe_yi_inp
        stor_sec = spe_yi_inp
        stor = [stor_prim, stor_prim]

        k_hor = [np.where(np.isnan(k), 20, k) for k in k_hor]
        k_ver = [np.where(np.isnan(k), 20, k) for k in k_ver]
        stor = [np.where(np.isnan(k), 0.1, k) for k in stor]
        return k_hor,k_ver,stor

    def get_rch_param(self,current_date):
        # Expect ts_gwrecharge in m/day; convert to cell flux per MF6 convention
        rch_nat = self.ts_gwrecharge  # m/day from VIC post-processing
        rch_nat = np.where(np.isnan(rch_nat), 0, rch_nat)
        recharge_inp = (rch_nat * self.paths.cellarea) / (self.delcol * self.delrow)
        recharge_inp = np.where(recharge_inp <0, 0, recharge_inp)
        nrow, ncol = self.paths.landmask.shape
        cellids = [(0, i, j) for i in range(nrow) for j in range(ncol)]
        # Create stress_period_data as a list of lists (leave it for mf6)
        RCHstress_period_data = []
        for cellid, value, ibound in zip(cellids, recharge_inp.flatten(), self.ibound.flatten()):
            # Temporary tightening for domain consistency: only write RCH on active MODFLOW cells.
            if np.isnan(value) or value == 0 or ibound <= 0:
                continue
            cellid_1, cellid_2, cellid_3 = cellid
            RCHstress_period_data.append([cellid_1, cellid_2, cellid_3, value])

        return RCHstress_period_data

    def get_riv_param(self):
        if not hasattr(self, 'top_layer1'):
            self.cal_toplayer_elevation()
        riv_manning,resistance,riv_bedres_inp = 0.045,1.0,1.0000
        min_dem2 = np.where(self.paths.min_dem < 0, 0, self.paths.min_dem)

        Z0_floodplain1 = np.maximum(min_dem2, self.paths.Z0_floodplain)
        Z0_floodplain2 = np.where(self.paths.Z0_floodplain <0, Z0_floodplain1, self.paths.Z0_floodplain)

        riv_width = 4.8 * (self.paths.qbank ** 0.5)
        riv_slope = np.where(riv_width >0, self.paths.riv_slope1, np.nan)
        riv_slope_used = np.where(riv_slope > 0.00005, riv_slope, np.nan)

        riv_head_ini = np.where(riv_width > 30, Z0_floodplain2, self.top_layer1)
        riv_head_ini = np.where(np.isnan(self.paths.landmask), 0, riv_head_ini)

        riv_depth_bkfl = ((riv_manning*(self.paths.qbank)**0.5)/(4.8*riv_slope_used**0.5)) ** (3.0/5.0)
        riv_depth_bkfl = np.where(np.isnan(riv_depth_bkfl), np.nan, riv_depth_bkfl)

        riv_bot_bkfl = min_dem2 - riv_depth_bkfl

        riv_depth_avg1 = (riv_manning * self.ts_discharge) / (riv_width * riv_slope_used ** 0.5) ** (3.0 / 5.0)
        riv_depth_avg = np.where(riv_depth_avg1 < 0.01, 0, riv_depth_avg1)
        riv_depth_avg = np.where(np.isnan(riv_depth_avg), 0, riv_depth_avg)

        riv_head = riv_bot_bkfl + riv_depth_avg
        riv_head2 = np.where(np.isnan(riv_head), riv_head_ini, riv_head)
        
        denominator = np.where(riv_width >30, riv_width * (self.paths.cellarea * 2) ** 0.5, 0)
        riv_cond = 1/resistance * denominator 
        riv_cond = np.where(np.isnan(self.paths.landmask), 0, riv_cond)

        drn_width1 = np.maximum(10, riv_width)
        drn_width = np.where(riv_cond ==0, drn_width1, 0)
        drn_cond = np.where(riv_cond ==0, (1/resistance) * drn_width * ((self.paths.cellarea * 2) ** 0.5), 0)

        riv_head_comb = np.where(riv_cond > 0, riv_head2, riv_head_ini)
        riv_head_comb = np.where(np.isnan(riv_head_comb), 0, riv_head_comb)
        riv_head_comb = np.where(np.isnan(self.paths.landmask), np.nan, riv_head_comb)

        riv_bot_comb = np.where(riv_cond >0, riv_bot_bkfl, riv_head_ini)
        riv_bot_comb = np.where(np.isinf(riv_bot_comb), 0, riv_bot_comb)
        riv_bot_comb = np.where(np.isnan(self.paths.landmask), np.nan, riv_bot_comb)
        # if negative discharge, use the bottom elevation for river head, generate no flow from surface to groundwater, to avoid next time step more negative discharge. 
        riv_head_comb = np.where(self.ts_discharge < 0, riv_bot_comb, riv_head_comb)
        riv_cond_comb = np.where(riv_cond >0, riv_cond, drn_cond)
        riv_cond_comb = np.where(np.isnan(self.paths.landmask), np.nan, riv_cond_comb)

        nrow, ncol = self.paths.landmask.shape
        cellids = [(0, i, j) for i in range(nrow) for j in range(ncol)]
        RIVstress_period_data = []
        for cellid, stage, rbot, cond, landmask, ibound in zip(cellids, riv_head_comb.flatten(), riv_bot_comb.flatten(), riv_cond_comb.flatten(), self.paths.landmask.flatten(), self.ibound.flatten()): 
            # Skip cellids with NaN values in stage or rbot
            # Temporary tightening for domain consistency: only write RIV on active MODFLOW cells.
            if np.isnan(stage) or np.isnan(rbot) or np.isnan(cond) or cond == 0 or np.isnan(landmask) or ibound <= 0:
                continue
            cellid_1, cellid_2, cellid_3 = cellid
            RIVstress_period_data.append([cellid_1, cellid_2, cellid_3, stage, cond, rbot])
        return RIVstress_period_data
    
    def get_drn_param(self):
        # Temporary solution for Indus: skip DRN until the Indus-specific drain setup is rebuilt.
        if getattr(self.paths, 'case_name', 'global') == 'indus':
            return None
        if not hasattr(self, 'aqdepth'):
            self.cal_aqdepth()
        if not hasattr(self, 'stor'):
            self.stor = self.get_npf_param()[2]
        DZS3INFLUENCED = 5.0
        KQ3min = 1.0e-4
        no_data_value = -9999.9
        invalid_base_s3_value = -900000.0
        subcatchment_id = self.paths.ldd 
        storage_coefficient = self.stor[0]
        unique_ids = np.unique(subcatchment_id)[:-1]
        valid_mask = (self.paths.landmask==1)
        subcatchment_minimum = {uid:self.paths.Z0_floodplain[(subcatchment_id == uid) & valid_mask].min() for uid in unique_ids}
        BASE_S3 = np.full_like(self.paths.Z0_floodplain, np.nan)
        for uid in unique_ids:
            BASE_S3[subcatchment_id == uid] = subcatchment_minimum[uid]
        BASE_SE = np.maximum(self.paths.Z0_floodplain-DZS3INFLUENCED, BASE_S3)
        aqdepth_mask = self.aqdepth > no_data_value 
        BASE_S3 = np.where(aqdepth_mask, np.maximum(self.paths.Z0_floodplain, BASE_S3), BASE_S3)
        BASE_S3_used = np.where(valid_mask, BASE_S3, invalid_base_s3_value)
        KQ3 = np.clip(self.paths.KQ3, KQ3min, 1.0)  # 将 kq3 限制在 [KQ3min, 1.0]  
        KQ3_x_Sy = np.where(valid_mask, KQ3 * storage_coefficient, 0.0)
        KQ3_x_Sy_AR = np.where(BASE_S3_used != invalid_base_s3_value, KQ3_x_Sy * self.paths.cellarea, 0.0)
        KQ3_x_Sy_AR = np.where(np.isnan(self.paths.landmask), 0, KQ3_x_Sy_AR)
        nrow, ncol = self.ibound.shape
        cellids = [(0, i, j) for i in range(nrow) for j in range(ncol)]    
        DRNstress_period_data = []
        for cellid, elev, cond in zip(cellids, BASE_S3_used.flatten(), KQ3_x_Sy_AR.flatten()):
            if np.isnan(cond) or cond ==0:
                continue
            cellid_1, cellid_2, cellid_3 = cellid
            DRNstress_period_data.append([cellid_1, cellid_2, cellid_3, elev,cond]) 
        return DRNstress_period_data

    def get_chd_input(self):
        nrow, ncol = self.ibound.shape
        CHDstress_period_data = []

        if getattr(self.paths, 'case_name', 'global') != 'global':
            steady = self._get_non_global_steady_head()
            for layer in range(2):
                cellids = [(layer, i, j) for i in range(nrow) for j in range(ncol)]
                for cellid, icell, head in zip(cellids, self.ibound.flatten(), steady[layer].flatten()):
                    if icell != 2 or np.isnan(head):
                        continue
                    cellid_1, cellid_2, cellid_3 = cellid
                    CHDstress_period_data.append([cellid_1, cellid_2, cellid_3, float(head)])
            return CHDstress_period_data

        cellids = [(0, i, j) for i in range(nrow) for j in range(ncol)]
        for cellid, icell in zip(cellids, self.ibound.flatten()):
            cellid_1, cellid_2, cellid_3 = cellid
            if icell ==2:
                CHDstress_period_data.append([cellid_1, cellid_2, cellid_3, 0])
        cellids = [(1, i, j) for i in range(nrow) for j in range(ncol)]
        for cellid, icell in zip(cellids, self.ibound.flatten()):
            cellid_1, cellid_2, cellid_3 = cellid
            if icell == 2:
                CHDstress_period_data.append([cellid_1, cellid_2, cellid_3, 0])
        return CHDstress_period_data      
    
    def get_cpr_param(self): # TODO: not yet generated 
        CPRstress_period_data = []
        cpsurface = self.paths.capillary['cpsurface'].values
        cprate = self.paths.capillary['cprate'].values
        cpdistinct = self.paths.capillary['cpdistinct'].values

        
        nrow, ncol = self.paths.landmask.shape
        cellids = [(0, i, j) for i in range(nrow) for j in range(ncol)]        
        for cellid,surface,rate,distinction,landmask  in zip(cellids,cpsurface.flatten(),cprate.flatten(),cpdistinct.flatten(),self.paths.landmask.flatten()):
            cellid_1, cellid_2, cellid_3 = cellid
            if np.isnan(surface) or np.isnan(rate) or np.isnan(distinction) or np.isnan(landmask) or rate==0:
                continue
            CPRstress_period_data.append([(cellid_1, cellid_2, cellid_3), surface, rate, distinction])
        
        return CPRstress_period_data
    
        

class config_indus(config):
    def __init__(self):
        super().__init__(pathconfig_cls=PathconfigIndus)


#%%
config_global_nat_poc = config()
config_global_nat_poc.set_foc(False)
config_global_nat_poc.set_humanimpact(False)

config_global_nat_foc = config()
config_global_nat_foc.set_foc(True)
config_global_nat_foc.set_humanimpact(False)

config_global_human_poc = config()
config_global_human_poc.set_foc(False)
config_global_human_poc.set_humanimpact(True)

config_global_human_foc = config()
config_global_human_foc.set_foc(True)
config_global_human_foc.set_humanimpact(True)

def build_indus_config(humanimpact=False, foc=False):
    cfg = config_indus()
    cfg.set_foc(foc)
    cfg.set_humanimpact(humanimpact)
    return cfg
