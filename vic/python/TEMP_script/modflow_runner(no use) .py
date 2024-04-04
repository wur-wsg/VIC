#%%

import os
os.chdir('/lustre/nobackup/WUR/ESG/liu297/gitrepo/VIC-WUR-GWM-1910/vic_online/python')
%env LD_LIBRARY_PATH=/shared/legacyapps/netcdf/gcc/64/4.6.1/lib:$LD_LIBRARY_PATH   # for netcdf
import numpy as np
import flopy
import pandas as pd
import netCDF4 as nc
import xarray as xr
import subprocess  # for calling shell commands
from datetime import datetime, timedelta
from dateutil.relativedelta import relativedelta
import calendar
import support_function as sf
from config_module import config_indus_ubuntu
from osgeo import gdal
from netCDF4 import Dataset, date2num
from matplotlib import pyplot as plt
#%%
class mfinput:
    def __init__(self):

    
qbank = mfinput().qbank
#%%
def stress_period_data(startdate,enddate):

    start_date = datetime.strptime(startdate, '%Y-%m-%d')  # start date
    end_date = datetime.strptime(enddate, '%Y-%m-%d')  # end data
    totalsp = (end_date.year - start_date.year) * 12 + end_date.month - start_date.month  # calculate stress period, in this sm: 395
    month_lengths = []
    sp_date = []
    current_date = start_date
    while current_date <= end_date:
        year = current_date.year
        month = current_date.month
        _, num_days = calendar.monthrange(year, month)
        last_day = datetime(year,month,num_days)
        month_lengths.append(num_days)
        sp_date.append(last_day.strftime('%Y-%m-%d'))
        #update to next date
        current_date = last_day + timedelta(days=1)
        
    nstp = 1 # number of timesteps per stress period
    tsmult = 1 # timestep multiplier
    perioddata = [(days,nstp,tsmult) for days in month_lengths] #a list containing the period length for all stress period
    sp = list(range(totalsp+1)) # list(396)
    return sp, sp_date, perioddata

sp, sp_date,perioddata = stress_period_data('1968-01-01','1968-03-01') # format are all xxxx-xx-01，here is until 2000-12-31 (but write -01) TODO

def dis_ic_parameter():
    global aqdepth, topl2,Nlay,Nrow,Ncol,delrow,delcol,botm,startinghead
    aqdepth = aqdepth_ini.copy()
    # aqdepth[landmask==0] = 200
    aqdepth[bdmask==-1] = 200
    aqdepth[bdmask==0] = 200
    aqdepth[bdmask==-2] = 200
    aqdepth[aqdepth==missingvalue] = 200
    aqdepth = np.where(aqdepth >0, aqdepth , 200)
    # dem = np.where(landmask==1, dem_ini, 0)
    # 创建一个与 bdmask 形状相同的掩码数组，用于检查条件
    mask_1 = (bdmask == 1)
    mask_2 = (bdmask == 2)

    # 使用 logical_or 同时检查两个条件
    combined_mask = np.logical_or(mask_1, mask_2)

    # 根据条件选择对应元素
    dem = np.where(combined_mask, dem_ini, 0)

    # dem = np.where(dem_ini==missingvalue, dem_ini, 0)
    topl2 = dem
    topl2 = np.where(topl2==missingvalue, 0 ,topl2) 
    botl2 = topl1 = topl2-(aqdepth*0.1) #second layer is 10% total thickness
    botl2 = np.where(botl2==missingvalue, -20 ,botl2)
    topl1 = np.where(topl1==missingvalue, -20 ,topl1)
    botl1 = topl2-aqdepth
    botl1 = np.where(botl1==missingvalue, -200 ,botl1)
        
    botm = [botl2,botl1]        
    Nlay = 2
    # Nrow, Ncol = landmask.shape
    Nrow, Ncol = bdmask.shape
    global delrow, delcol
    delrow = clonemap.GetGeoTransform()[1]*111*1000
    delcol = abs(clonemap.GetGeoTransform()[5]*111*1000)
    return Nlay,Nrow,Ncol,delrow,delcol,topl2,botm

def initial_head():
    startingheadl2 = gwll2[0].copy()  #top layer #indus case: run steady-state model first, then use the computed head as the starting head for stress period 0
    startingheadl1 = gwll2[0].copy()  #bot layer 
    initial_head = [startingheadl2,startingheadl1]
    return initial_head

Nlay,Nrow,Ncol,delrow,delcol,topl2,botm = dis_ic_parameter()

def npf_parameter():
    global aqdepth,delcol,delrow,spe_yi_inp
    rho_water,miu_water,g_gravity = 1000,0.001,9.81
    dikte_l1_ini = aqdepth *0.1  #top layer
    dikte_l2_ini = aqdepth -dikte_l1_ini  #bottom layer
    dikte_l1= dikte_l1_ini.copy() 
    dikte_l1[bdmask == -1] = missingvalue
    dikte_l1[bdmask == 0] = missingvalue
    dikte_l1[bdmask == -2] = missingvalue
    dikte_l2 = dikte_l2_ini.copy()
    dikte_l2[bdmask == -1] = missingvalue
    dikte_l2[bdmask == 0] = missingvalue
    dikte_l2[bdmask == -2] = missingvalue
    
    
    # dikte_l2_ini = aqdepth *0.1  #top layer
    # dikte_l1_ini = aqdepth -dikte_l2_ini  #bottom layer
    # dikte_l2= dikte_l2_ini.copy() 
    # dikte_l2[landmask ==0] = missingvalue
    # dikte_l2[bdmask == -1] = missingvalue # TODO
    # dikte_l2[bdmask == 0] = missingvalue
    # dikte_l2[bdmask == -2] = missingvalue
    # dikte_l1 = dikte_l1_ini.copy()
    # dikte_l1[landmask == 0] = missingvalue
    # dikte_l1[bdmask == -1] = missingvalue
    # dikte_l1[bdmask == 0] = missingvalue
    # dikte_l1[bdmask == -2] = missingvalue
    conflayers[conflayers !=1 ]  = 0 
    #k for no confining layer area:
    ksat_inp = rho_water * (10 ** ksat_log) * (g_gravity / miu_water) * 24.0 * 3600.0
    # k for confining layer area:
    ksat_l2_conf=rho_water* (10**ksat_l2_conf_log)* (g_gravity/ miu_water) *24.0 *3600.0 # coarse grained
    ksat_l1_conf=rho_water* (10**ksat_l1_conf_log)* (g_gravity/ miu_water) *24.0 *3600.0 # fine grained
    khoriz_l2_ini = np.where(ksat_l2_conf != 0, ksat_l2_conf, ksat_inp)
    khoriz_l1_ini =	np.where(ksat_l1_conf != 0, ksat_l1_conf, ksat_inp)
    khoriz_l2_ini = np.clip(khoriz_l2_ini,a_min = 0.01, a_max = None)
    khoriz_l1_ini = np.clip(khoriz_l1_ini,a_min = 0.01, a_max = None)    
    # ksat_l1_conf=rho_water* (10**ksat_l1_conf_log)* (g_gravity/ miu_water) *24.0 *3600.0# fine grained
    # ksat_l2_conf=rho_water* (10**ksat_l2_conf_log)* (g_gravity/ miu_water) *24.0 *3600.0# coarse grained
    # khoriz_l1_ini = np.where(ksat_l1_conf != 0, ksat_l1_conf, ksat_inp)
    # khoriz_l2_ini =	np.where(ksat_l2_conf != 0, ksat_l2_conf, ksat_inp)
    # khoriz_l1_ini = np.clip(khoriz_l1_ini,a_min = 0.01, a_max = None)
    # khoriz_l2_ini = np.clip(khoriz_l2_ini,a_min = 0.01, a_max = None)
    #k vertical
    kvert_l1_ini = np.where(khoriz_l1_ini >-999,khoriz_l1_ini*cellarea/(delcol*delrow), (10.0*cellarea/(delcol*delrow)))
    kvert_l2 = np.where(khoriz_l2_ini>-999, (10.0*cellarea/(delcol*delrow)),(10.0*cellarea/(delcol*delrow)))
    kvert_l1 = np.maximum(dikte_l1/5000,kvert_l1_ini)
    kd_l1_ini = khoriz_l1_ini*(dikte_l1)
    kd_l2_ini = khoriz_l2_ini*(dikte_l2)	
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
    # kvert_l2_ini = np.where(khoriz_l2_ini >-999,khoriz_l2_ini*cellarea/(delcol*delrow), (10.0*cellarea/(delcol*delrow)))
    # kvert_l1 = np.where(khoriz_l1_ini>-999, (10.0*cellarea/(delcol*delrow)),(10.0*cellarea/(delcol*delrow)))
    # kvert_l2 = np.maximum(dikte_l2/5000,kvert_l2_ini)
    # kd_l2_ini = khoriz_l2_ini*(dikte_l2)
    # kd_l1_ini = khoriz_l1_ini*(dikte_l1)	
    # kd_l2 = np.maximum(30,kd_l2_ini)
    # kd_l1 = np.maximum(30,kd_l1_ini)
    # khoriz_l2 = kd_l2/dikte_l2
    # khoriz_l1 = kd_l1/dikte_l1
    # khoriz_l2[khoriz_l2<0]  = 20
    # khoriz_l1[khoriz_l1<0]  = 190
    # kvert_l2[kvert_l2<0] = 1e10
    # kvert_l2[kvert_l2<0] = 1e10
    # k_hor = [khoriz_l2, khoriz_l1]
    # k_ver = [khoriz_l2*0.1,khoriz_l1*0.1]
    # Storage Coefficient
    spe_yi_inp = spe_yi_inp
    # spe_yi_inp = np.where(landmask ==1, spe_yi_inp,missingvalue)
    spe_yi_inp = np.where(bdmask >=1, spe_yi_inp,missingvalue)
    spe_yi_inp = np.maximum(0.01,spe_yi_inp)
    spe_yi_inp = np.minimum(1,spe_yi_inp)
    spe_yi_inp = np.where(aqdepth >-999.9,np.maximum(spe_yi_inp, 0.11),spe_yi_inp)
    stor_prim = spe_yi_inp
    stor_sec = spe_yi_inp
    stor=[stor_prim,stor_prim]
    return k_hor,k_ver,stor

# RCH package
def rch_input(monthly_recharge):
    rch_nat = monthly_recharge
    # input changes with time
    rcmissingvalue = rch_nat[179][1]
    rch_nat[bdmask ==2 & (rch_nat==rcmissingvalue)]= 0
    recharge_inp = ((rch_nat/(1000*365)*cellarea)/(delcol*delrow))
    recharge_inp = np.where(recharge_inp < 10000, recharge_inp, 0)
    npzero = np.zeros_like(recharge_inp)
    # recharge on the top layer    
    recharge_inp = np.maximum(recharge_inp,npzero)
    # recharge_inp[idomain == -1]=np.nan
    recharge_inp[idomain == -1]=0
    recharge_inp[idomain == -2]=0
    recharge_inp[idomain == 0]=np.nan
    nrow, ncol = recharge_inp.shape
    cellids = [(0, i, j) for i in range(nrow) for j in range(ncol)]
    # Create stress_period_data as a list of lists (leave it for mf6)
    RCHstress_period_data = []
    for cellid, value, bdmask_ind in zip(cellids, recharge_inp.flatten(), bdmask.flatten()):
        if np.isnan(value) or value ==0 or bdmask_ind != 2:
        # if np.isnan(value) or value ==0:
            continue
        cellid_1, cellid_2, cellid_3 = cellid
        RCHstress_period_data.append([cellid_1, cellid_2, cellid_3, value])
    
    return RCHstress_period_data

# RIV package

def riv_input(monthly_discharge,qaverage):
    qaverage = qaverage # 
    monthly_discharge = monthly_discharge 
    riv_manning,resistance,riv_bedres_inp = 0.045,1.0,1.0000
    min_dem2 = np.where(min_dem<0,0,min_dem)
    
    
    Z0_floodplain1 = np.maximum(min_dem2, Z0_floodplain)
    Z0_floodplain2 = np.where(Z0_floodplain < 0.0, Z0_floodplain1, Z0_floodplain)
    riv_width = 4.8* ((qaverage)**0.5)
    riv_width = np.maximum(riv_width, 0.5)
    
    
    riv_slope = np.where(riv_width > 0.0, riv_slope1,missingvalue)
    riv_slope_used = np.where(riv_slope>0.00005, riv_slope,missingvalue)
    riv_head_ini = np.where(riv_width > 30.0, Z0_floodplain2, topl1)
    # riv_head_ini = np.where(riv_width > 30.0, Z0_floodplain2, topl2)
    riv_head_ini[bdmask==-1] = 0  
    riv_head_ini[bdmask==0] = 0
    riv_head_ini[bdmask==-2] = 0   
    
    
    riv_depth_bkfl = ((riv_manning*(qaverage)**0.5)/(riv_width*riv_slope_used**0.5))**(3.0/5.0)
    riv_depth_bkfl = np.where(np.isnan(riv_depth_bkfl),missingvalue,riv_depth_bkfl)
    riv_bot_bkfl = min_dem2 - riv_depth_bkfl
    
    riv_depth_avg1 = ((riv_manning*((monthly_discharge)**0.5))/(riv_width*riv_slope_used**0.5))**(3.0/5.0) #  
    riv_depth_avg = np.where(riv_depth_avg1 < 0.01, 0.0, riv_depth_avg1)
    riv_depth_avg = np.where(np.isnan(riv_depth_avg),0,riv_depth_avg)
    riv_head=riv_bot_bkfl + riv_depth_avg
    riv_head2 = np.where(np.isnan(riv_head), riv_head_ini, riv_head)
    riv_head2 = np.where(riv_head == -1*missingvalue, riv_head_ini, riv_head)
    denominator = np.where(riv_width >= 30.0, riv_width * (cellarea*2.0) ** 0.5, 0.0)
    riv_cond =  1 / resistance * denominator
    riv_cond[bdmask == -1]=0
    riv_cond[bdmask == 0]=0
    riv_cond[bdmask == -2]=0
    drn_width1 = np.maximum(10,riv_width)
    drn_width = np.where(riv_cond == 0.0, drn_width1, 0.0)
    drn_cond = np.where(riv_cond ==0, (1.0/resistance)*drn_width*(cellarea*2)**0.5,0.0)
    drn_cond = np.where(drn_cond<1e-20, 0,drn_cond)
    riv_head_comb = np.where(riv_cond > 0.0, riv_head2, riv_head_ini)
    riv_head_comb = np.where(np.isnan(riv_head_comb),0,riv_head_comb)
    riv_head_comb[bdmask==-1]=np.nan
    riv_head_comb[bdmask==0]=np.nan
    riv_head_comb[bdmask==-2]=np.nan
    riv_bot_comb = np.where(riv_cond >0.0, riv_bot_bkfl,riv_head_ini)#TODO
    riv_bot_comb[np.isinf(riv_bot_comb)] = 0
    riv_bot_comb = np.where(riv_bot_comb == -missingvalue, np.nan, riv_bot_comb) #TODO
    riv_bot_comb[bdmask==-1]=np.nan
    riv_bot_comb[bdmask==0]=np.nan
    riv_bot_comb[bdmask==-2]=np.nan
    riv_cond_comb = np.where(riv_cond>0, riv_cond, drn_cond)
    riv_cond_comb[bdmask==-1]=np.nan
    riv_cond_comb[bdmask==0]=np.nan
    riv_cond_comb[bdmask==-2]=np.nan

    
    nrow, ncol = bdmask.shape
    cellids = [(0, i, j) for i in range(nrow) for j in range(ncol)]
    RIVstress_period_data = []
    for cellid, stage, rbot, cond, bdmask_ind in zip(cellids, riv_head_comb.flatten(), riv_bot_comb.flatten(), riv_cond_comb.flatten(), bdmask.flatten()): 
        # Skip cellids with NaN values in stage or rbot
        # if np.isnan(stage) or np.isnan(rbot) or np.isnan(cond) or cond == 0 or bdmask_ind == 0 or bdmask_ind == -1:
        if np.isnan(stage) or np.isnan(rbot) or np.isnan(cond) or cond == 0 or bdmask_ind != 2:
            continue
        cellid_1, cellid_2, cellid_3 = cellid
        RIVstress_period_data.append([cellid_1, cellid_2, cellid_3, stage, cond, rbot])
    return RIVstress_period_data

#CHD package
def chd_input(stress_period):
    CHDstress_period_data = []
    # first layer (top layer)
    nrow, ncol = bdmask.shape
    cellids = [(0, i, j) for i in range(nrow) for j in range(ncol)] # 创建一个包含元组的列表，每个元组包含三个值：0、i 和 j
    for cellid, id_num in zip(cellids, idomain.flatten()):
        cellid_1, cellid_2, cellid_3 = cellid
        # if id_num != 1:# 
        if id_num != 1 and id_num != 4:# 
            continue
        if id_num == 4:# 
            # gwll2[stress_period][cellid_2][cellid_3] = 0
            gwll1[stress_period][cellid_2][cellid_3] = 0
        # if np.isnan(gwll2[stress_period][cellid_2][cellid_3]):
        if np.isnan(gwll1[stress_period][cellid_2][cellid_3]):
            # gwll2[stress_period][cellid_2][cellid_3] = 0
            gwll1[stress_period][cellid_2][cellid_3] = 0
        # CHDstress_period_data.append([cellid_1, cellid_2, cellid_3, gwll2[stress_period][cellid_2][cellid_3]]) # cellid_1 is layer number: 0 for top, 1 for bot
        CHDstress_period_data.append([cellid_1, cellid_2, cellid_3, gwll1[stress_period][cellid_2][cellid_3]]) # cellid_1 is layer number: 0 for top, 1 for bot

    # second layer (bot layer)
    nrow, ncol = bdmask.shape
    cellids = [(1, i, j) for i in range(nrow) for j in range(ncol)]
    for cellid, id_num in zip(cellids, idomain.flatten()):
        cellid_1, cellid_2, cellid_3 = cellid
        # if id_num != 1:
        if id_num != 1 and id_num != 4:# 
            continue
        if id_num == 4:# 
            # gwll1[stress_period][cellid_2][cellid_3] = 0
            gwll2[stress_period][cellid_2][cellid_3] = 0
        # if np.isnan(gwll1[stress_period][cellid_2][cellid_3]):
        if np.isnan(gwll2[stress_period][cellid_2][cellid_3]):
            # gwll1[stress_period][cellid_2][cellid_3] = 0
            gwll2[stress_period][cellid_2][cellid_3] = 0
        # CHDstress_period_data.append([cellid_1, cellid_2, cellid_3, gwll1[stress_period][cellid_2][cellid_3]]) # cellid_1 is layer number: 0 for top, 1 for bot
        CHDstress_period_data.append([cellid_1, cellid_2, cellid_3, gwll2[stress_period][cellid_2][cellid_3]]) # cellid_1 is layer number: 0 for top, 1 for bot
    return CHDstress_period_data # list of list: 

#%% loop for mfrun
# layer2_head = {} # top
layer1_head = {} # top
# layer1_head = {} # bot
layer2_head = {} # bot

for stress_period,date in zip(sp, sp_date):
    if (stress_period==0):
        print('assign steady-state computed head as the starting head for stress period 0')
        startinghead = initial_head()
    else:
        startingheadl2 = layer2_head[stress_period-1] # bot
        startingheadl1 = layer1_head[stress_period-1] # top
        # startinghead = [startingheadl2,startingheadl2]
        startinghead = [startingheadl1,startingheadl2]
        print(f"start assigning the variables for stress period {stress_period}...\n")
    Nlay,Nrow,Ncol,delrow,delcol,topl1,botm = dis_ic_parameter()
    k_hor,k_ver,stor = npf_parameter()
    RCHstress_period_data = rch_input(ts_gwrecharge[stress_period])
    RIVstress_period_data = riv_input(ts_discharge[stress_period],qbank)
    CHDstress_period_data = chd_input(stress_period)
    idomain1 = [idomain,idomain]
    name = '2Ltrans_off'
    print('finish assigning variables \n')
    print(f"writing the flopy variables for stress period {stress_period}...\n")
    sim = flopy.mf6.MFSimulation(
        sim_name=name,version='mf6', sim_ws=workspace, exe_name = exedir,
        verbosity_level = 1, continue_ = False, nocheck = True,
        memory_print_option = None, write_headers = True)
    tdis = flopy.mf6.ModflowTdis(
        sim,time_units= "DAYS", start_date_time = None,nper= 1,                     
        perioddata= [(perioddata[stress_period][0],perioddata[stress_period][1],perioddata[stress_period][2])]
        )
    rclose = 1000
    ims = flopy.mf6.ModflowIms(
        sim, complexity='COMPLEX', print_option="SUMMARY",
            outer_dvclose=50, inner_dvclose=50, under_relaxation="simple",
            under_relaxation_gamma=0.9, relaxation_factor=0,
            linear_acceleration="bicgstab", outer_maximum = 100, 
            inner_maximum = 500, rcloserecord="{} strict".format(rclose) )
    model_nam_file = f"{name}.nam"   
    gwf = flopy.mf6.ModflowGwf(
        sim, modelname = name, model_nam_file = model_nam_file,
        exe_name = exedir, model_rel_path = '.', list= None,
        print_input = False, print_flows = False, save_flows = True)
    dis = flopy.mf6.ModflowGwfdis(
        gwf,length_units = "METERS",nogrb = True, xorigin = 0,yorigin = 0,
        angrot = 0,nlay=Nlay, nrow=Nrow, ncol=Ncol,
        idomain = idomain1, delr=delrow,delc=delcol,top=topl1, botm=botm)
    npf = flopy.mf6.ModflowGwfnpf(
        gwf, save_flows = False, print_flows = False, save_specific_discharge =False,
        save_saturation = False, perioddata = None, dev_no_newton = False,
        icelltype = 0,  k = k_hor, k33 = k_ver)
    sto = flopy.mf6.ModflowGwfsto(
        gwf, save_flows=None, storagecoefficient=True, 
        ss_confined_only=None, iconvert=0,ss=stor, #steady_state=None,
        transient=True)
    chd = flopy.mf6.ModflowGwfchd(
        gwf,  print_input = False, print_flows = False, save_flows = True,
        stress_period_data=CHDstress_period_data)
    ic = flopy.mf6.ModflowGwfic(
        gwf, strt = startinghead)
    rch = flopy.mf6.ModflowGwfrch(
        gwf,stress_period_data=RCHstress_period_data)
    riv = flopy.mf6.ModflowGwfriv(
        gwf,stress_period_data= RIVstress_period_data)
    saverecord = [("HEAD", "ALL"), ("BUDGET", "ALL")]
    printrecord = [("HEAD", "ALL"), ("BUDGET", "ALL")]
    headfile = "{}_{}.hds".format(name,date)
    head_filerecord = [headfile]
    budgetfile = "{}.cbb".format(name)
    budget_filerecord = [budgetfile]
    
    oc = flopy.mf6.modflow.mfgwfoc.ModflowGwfoc(
        gwf,saverecord=saverecord,head_filerecord=head_filerecord,
        budget_filerecord=budget_filerecord)
    print('finish writing the flopy variables \n')
    print(f"writing the simulation for stress period {stress_period}...\n")
    sim.write_simulation()
    print('finish writing the simulation \n')
    print(f"running the simulation for stress period {stress_period}...\n")
    success, buff = sim.run_simulation()
    if not success:
        raise Exception(f"MODFLOW 6 did not terminate normally for stressperiod: {stress_period}\n")
    else:
        print(f"MODFLOW 6 terminated normally for stressperiod: {stress_period}\n")
        print(f"extracting the head and budget files for stress period {stress_period}...\n")
        head = flopy.utils.binaryfile.HeadFile(outputdir+headfile)
        layer1_head[stress_period] = head.get_data()[0] # top
        layer2_head[stress_period] = head.get_data()[1] # bot

## save head to nc
indus_simhead_path = '/lustre/nobackup/WUR/ESG/yuan018/domain_Indus.nc' # prepare a indus domain nc file, put somewhere as indus_simhead_path
nc_indus_simhead = nc.Dataset(indus_simhead_path, 'a')
lon = nc_indus_simhead.variables['lon'][:]
lat = nc_indus_simhead.variables['lat'][:]
mask = nc_indus_simhead.variables['mask'][:]
mask = mask.mask

# create time dim and variable
start_date = datetime(1968, 1, 1)
end_date = datetime(2000, 12, 31)
num_time_steps = (end_date.year - start_date.year) * 12 + end_date.month - start_date.month + 1
time_dim = nc_indus_simhead.createDimension('time', num_time_steps)
time_var = nc_indus_simhead.createVariable('time', np.float64, ('time',))
time_var.units = 'days since 1968-01-01'  # set time unit

# create time step
time_steps = [start_date + relativedelta(months=i) for i in range(0, (end_date.year - start_date.year) * 12 + end_date.month - start_date.month + 1)]
time_data = date2num(time_steps, units=time_var.units, calendar='gregorian')
time_var[:] = time_data

# create water level variable, add it to time dim
# l2_gw_head = nc_indus_simhead.createVariable('bothead', np.float32, ('time','lat', 'lon'))# top layer water head
# l2_gw_head.units = 'meters'
# l1_gw_head = nc_indus_simhead.createVariable('tophead', np.float32, ('time','lat', 'lon'))
# l1_gw_head.units = 'meters'

l1_gw_head = nc_indus_simhead.createVariable('tophead', np.float32, ('time','lat', 'lon'))# top layer water head
l1_gw_head.units = 'meters'
l2_gw_head = nc_indus_simhead.createVariable('bothead', np.float32, ('time','lat', 'lon'))# bot layer water head
l2_gw_head.units = 'meters'

for key in range(9999): # write a big number
    if key in layer1_head: # layer1_head: dict
        layer1_head_2d_array = layer1_head[key] # key: month
        rows, cols = len(layer1_head_2d_array), len(layer1_head_2d_array[0])
        layer1_head_3d_array = layer1_head_2d_array.reshape(1, rows, cols) # reshape to a 3D array
        l1_gw_head[key, :, :] = layer1_head_3d_array

for key in range(9999): # write a big number
    if key in layer2_head: # layer2_head: dict
        layer2_head_2d_array = layer2_head[key] # key: month
        rows, cols = len(layer2_head_2d_array), len(layer2_head_2d_array[0])
        layer2_head_3d_array = layer2_head_2d_array.reshape(1, rows, cols) # reshape to a 3D array
        l2_gw_head[key, :, :] = layer2_head_3d_array


topl1_gwl.close()
topl2_gwl.close()
