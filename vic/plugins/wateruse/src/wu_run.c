/******************************************************************************
 * @section DESCRIPTION
 *
 * Run water-use
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#include <vic_driver_image.h>
#include <plugin.h>

/******************************************
* @brief   Reset water-use from sectors
******************************************/
void
reset_wu(size_t iCell)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct **wu_var;
    extern wu_con_map_struct *wu_con_map;
    
    size_t i;
    int iSector;
    
    for(i = 0; i < plugin_options.NWUTYPES; i ++){
        iSector = wu_con_map[iCell].sidx[i];        
        if(iSector == NODATA_WU){
            continue;
        }
        
        wu_var[iCell][iSector].available_surf = 0.0;
        wu_var[iCell][iSector].available_gw = 0.0;
        wu_var[iCell][iSector].available_dam = 0.0;
        wu_var[iCell][iSector].available_comp = 0.0;
        wu_var[iCell][iSector].available_remote = 0.0;
        wu_var[iCell][iSector].demand_surf = 0.0;
        wu_var[iCell][iSector].demand_gw = 0.0;
        wu_var[iCell][iSector].demand_comp = 0.0;
        wu_var[iCell][iSector].demand_remote = 0.0;
        wu_var[iCell][iSector].withdrawn_surf = 0.0;
        wu_var[iCell][iSector].withdrawn_gw = 0.0;
        wu_var[iCell][iSector].withdrawn_dam = 0.0;
        wu_var[iCell][iSector].withdrawn_comp = 0.0;
        wu_var[iCell][iSector].withdrawn_remote = 0.0;
        wu_var[iCell][iSector].withdrawn_nonrenew = 0.0;
        wu_var[iCell][iSector].consumed = 0.0;
        wu_var[iCell][iSector].returned = 0.0;
    }
}

/******************************************
* @brief   Get (cell) demand from sectors
******************************************/
void
calculate_demand(size_t iCell, 
        double *demand_gw, 
        double *demand_surf, 
        double *demand_remote)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct **wu_var;
    extern wu_con_struct *wu_con;
    extern wu_force_struct **wu_force;
    extern wu_con_map_struct *wu_con_map;
    
    size_t i;
    size_t j;
    int iSector;
    size_t iCell2;
    int iSector2;
    
    for(i = 0; i < plugin_options.NWUTYPES; i ++){
        iSector = wu_con_map[iCell].sidx[i];        
        if(iSector == NODATA_WU){
            continue;
        }
        
        // groundwater
        wu_var[iCell][iSector].demand_gw = wu_force[iCell][iSector].demand * 
                wu_force[iCell][iSector].groundwater_frac;
        (*demand_gw) += wu_var[iCell][iSector].demand_gw;
        
        // surface water
        wu_var[iCell][iSector].demand_surf = wu_force[iCell][iSector].demand * 
                (1 - wu_force[iCell][iSector].groundwater_frac);
        (*demand_surf) += wu_var[iCell][iSector].demand_surf;
        
        // remote
        // for water-use receiving cells the demand is calculated based on
        // their demand minus their local withdrawals
        wu_var[iCell][iSector].demand_remote = 0.0;
        for(j = 0; j < wu_con[iCell].nreceiving; j++){
            iCell2 = wu_con[iCell].receiving[j];

            iSector2 = wu_con_map[iCell2].sidx[i];        
            if(iSector2 == NODATA_WU){
                continue;
            }

            wu_var[iCell2][iSector2].demand_remote = 
                    wu_var[iCell2][iSector2].demand_surf - 
                    wu_var[iCell2][iSector2].withdrawn_surf - 
                    wu_var[iCell2][iSector2].withdrawn_dam; 
            (*demand_remote) += wu_var[iCell2][iSector2].demand_remote;       
        }
    }
}

/******************************************
* @brief   Get (cell) availability
******************************************/
void
calculate_availability(size_t iCell, 
        double *available_gw, 
        double *available_surf, 
        double *available_dam, 
        double *available_comp, 
        double *available_remote, 
        double **av_gw,
        double *av_dam,
        double demand_gw,
        double demand_surf,
        double demand_remote)
{
    extern plugin_global_param_struct plugin_global_param;
    extern global_param_struct global_param;
    extern domain_struct  local_domain;
    extern all_vars_struct     *all_vars;
    extern option_struct options;
    extern global_param_struct global_param;
    extern soil_con_struct     *soil_con;
    extern veg_con_map_struct     *veg_con_map;
    extern veg_con_struct     **veg_con;
    extern dam_con_map_struct *local_dam_con_map;
    extern dam_var_struct **local_dam_var;
    
    double resid_moist;
    double ice;
    double frac;
    
    size_t rout_steps_per_dt;
    size_t iStep;
    size_t iVeg;
    size_t iBand;
    size_t iFrost;
    size_t iLayer;
    size_t iDam;
    
    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;
    
    // groundwater
    iLayer = options.Nlayer - 1;
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){        
        if (veg_con[iCell][iVeg].Cv <= 0.0) {
            continue;
        }
        
        for(iBand = 0; iBand < options.SNOW_BAND; iBand++){        
            if (soil_con[iCell].AreaFract[iBand] <= 0.0) {
                continue;
            }
            
            av_gw[iVeg][iBand] = all_vars[iCell].cell[iVeg][iBand].layer[iLayer].moist;
            if (plugin_options.EFR) {
                av_gw[iVeg][iBand] -= efr_force[iCell].moist[iVeg][iBand];
            }

            resid_moist = soil_con[iCell].resid_moist[iLayer] *
                          soil_con[iCell].depth[iLayer] * MM_PER_M;
            av_gw[iVeg][iBand] -= resid_moist;

            ice = 0;
            for(iFrost = 0; iFrost < options.Nfrost; iFrost ++){
                ice += all_vars[iCell].cell[iVeg][iBand].layer[iLayer].ice[iFrost] *
                       soil_con[iCell].frost_fract[iFrost];
            }
            av_gw[iVeg][iBand] -= ice;

            av_gw[iVeg][iBand] *= soil_con[iCell].AreaFract[iBand] *
                        veg_con[iCell][iVeg].Cv;

            if(av_gw[iVeg][iBand] < 0){
                av_gw[iVeg][iBand] = 0;
            }

            (*available_gw) += av_gw[iVeg][iBand];
        }
    }
    
    // surface water
    for(iStep = 0; iStep < plugin_options.UH_LENGTH + rout_steps_per_dt + 1; iStep++) {
        (*available_surf) += rout_var[iCell].dt_discharge[iStep] * global_param.dt / 
                local_domain.locations[iCell].area * MM_PER_M;
    }
    if (plugin_options.EFR) {
        frac = min(efr_force[iCell].discharge / rout_var[iCell].discharge, 1.0);
        (*available_surf) *= (1 - frac);
    }
    if((*available_surf) < 0){
        (*available_surf) = 0;
    }
    
    // dam
    if (plugin_options.DAMS) {
        for (iDam = 0; iDam < local_dam_con_map[iCell].nd_active; iDam++) {
            av_dam[iDam] = 0;
            
            if(local_dam_var[iCell][iDam].active){
                av_dam[iDam] = local_dam_var[iCell][iDam].storage * M3_PER_HM3 / 
                local_domain.locations[iCell].area * MM_PER_M;
                (*available_dam) += av_dam[iDam];
            }
        }
    }
    if((*available_dam) < 0){
        (*available_dam) = 0;
    }
    
    // comp
    if(plugin_options.COMP_WITH){
        // for comp the leftover surface water after withdrawals is available
        // if necessary the dam water is abstracted first so more surface water is
        // available for comp
        if((*available_dam) > demand_surf){
            (*available_comp) = min(demand_gw - (*available_gw), (*available_surf));
        } else if ((*available_dam) + (*available_surf) > demand_surf) {
            (*available_comp) = min(demand_gw - (*available_gw), (*available_surf) + (*available_dam) - demand_surf);
        }
        if((*available_comp) < 0){
            (*available_comp) = 0;
        }
        (*available_surf) -= (*available_comp);
    }
        
    // remote
    // for remote cells the leftover surface water after withdrawals is available
    // if necessary the dam water is abstracted first so more surface water is
    // available for remote cells
    if((*available_dam) > demand_surf){
        (*available_remote) = min(demand_remote, (*available_surf));
    } else if ((*available_dam) + (*available_surf) > demand_surf) {
        (*available_remote) = min(demand_remote, (*available_surf) + (*available_dam) - demand_surf);
    }
    if((*available_remote) < 0){
        (*available_remote) = 0;
    }
    (*available_surf) -= (*available_remote);
}

/******************************************
* @brief   Divide available resources over sectors
******************************************/
void
calculate_division(size_t iCell, 
        double available_gw, 
        double available_surf, 
        double available_dam, 
        double available_comp, 
        double available_remote, 
        double demand_gw, 
        double demand_surf, 
        double demand_remote)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct **wu_var;
    extern wu_con_struct *wu_con;
    extern wu_con_map_struct *wu_con_map;
    
    double available_gw_tmp;
    double available_surf_tmp;
    double available_dam_tmp;
    double available_comp_tmp;
    double available_remote_tmp;
    double available_surf_tot;
    
    double frac;
    
    size_t i;
    size_t j;
    int iSector;
    size_t iCell2;
    int iSector2;
    
    available_gw_tmp = available_gw;
    available_surf_tmp = available_surf;
    available_dam_tmp = available_dam;
    available_comp_tmp = available_comp;
    available_remote_tmp = available_remote;
    
    for(i = 0; i < plugin_options.NWUTYPES; i ++){
        iSector = wu_con_map[iCell].sidx[i];        
        if(iSector == NODATA_WU){
            continue;
        }
        
        // groundwater
        if(demand_gw > 0){
            if(wu_var[iCell][iSector].demand_gw >= available_gw_tmp){
                wu_var[iCell][iSector].available_gw = available_gw_tmp;
                available_gw_tmp = 0.0;
            } else {
                frac = max(available_gw / demand_gw, 1.0);
                
                wu_var[iCell][iSector].available_gw = wu_var[iCell][iSector].demand_gw * frac;
                available_gw_tmp -= wu_var[iCell][iSector].demand_gw;
            }
        }
        
        // surface water & dam
        available_surf_tot = available_surf_tmp + available_dam_tmp;
        if(demand_surf > 0) {
            if(wu_var[iCell][iSector].demand_surf >= available_surf_tmp + available_dam_tmp){
                wu_var[iCell][iSector].available_surf = available_surf_tmp;
                wu_var[iCell][iSector].available_dam = available_dam_tmp;
                available_surf_tmp = 0.0;
                available_dam_tmp = 0.0;
            } else {
                frac = max((available_surf + available_dam) / demand_surf, 1.0);
                
                wu_var[iCell][iSector].available_surf = wu_var[iCell][iSector].demand_surf *
                        (available_surf_tmp / available_surf_tot) * frac;
                wu_var[iCell][iSector].available_dam = wu_var[iCell][iSector].demand_surf *
                        (available_dam_tmp / available_surf_tot) * frac;

                available_surf_tmp -= wu_var[iCell][iSector].demand_surf *
                        (available_surf_tmp / available_surf_tot);
                available_dam_tmp -= wu_var[iCell][iSector].demand_surf *
                        (available_dam_tmp / available_surf_tot);
            }
        }
        
        // comp
        if(plugin_options.COMP_WITH){
            wu_var[iCell][iSector].demand_comp = max(0.0, wu_var[iCell][iSector].demand_gw - wu_var[iCell][iSector].available_gw);            
            if(wu_var[iCell][iSector].demand_comp > 0) {
                if(wu_var[iCell][iSector].demand_comp >= available_comp_tmp) {
                    wu_var[iCell][iSector].available_comp = available_comp_tmp;
                    available_comp_tmp = 0.0;
                } else {
                    wu_var[iCell][iSector].available_comp = wu_var[iCell][iSector].demand_comp;
                    available_comp_tmp -= wu_var[iCell][iSector].demand_comp;
                }
            }
        }
        
        // remote
        for(j = 0; j < wu_con[iCell].nreceiving; j++){
            iCell2 = wu_con[iCell].receiving[j];

            iSector2 = wu_con_map[iCell2].sidx[i];        
            if(iSector2 == NODATA_WU){
                continue;
            }

            if (demand_remote > 0){
                if(wu_var[iCell2][iSector2].demand_remote >= available_remote_tmp){
                    wu_var[iCell2][iSector2].available_remote = available_remote_tmp;
                    available_remote_tmp = 0.0;
                } else {
                    frac = max(available_remote / demand_remote, 1.0);

                    wu_var[iCell2][iSector2].available_remote = wu_var[iCell2][iSector2].demand_remote * frac;
                    available_remote_tmp -= wu_var[iCell2][iSector2].demand_remote;
                }
            }
        }
    }
}

/******************************************
* @brief   Calculate (cell) withdrawals and return flows from sectors
******************************************/
void
calculate_use(size_t iCell,
        double *withdrawn_gw, 
        double *withdrawn_surf, 
        double *withdrawn_dam, 
        double *withdrawn_comp,
        double *withdrawn_remote,
        double *withdrawn_nonrenew,
        double *returned)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct **wu_var;
    extern wu_con_struct *wu_con;
    extern wu_force_struct **wu_force;
    extern wu_con_map_struct *wu_con_map;
    
    double frac;
    
    size_t i;
    size_t j;
    int iSector;
    size_t iCell2;
    int iSector2;
    
    for(i = 0; i < plugin_options.NWUTYPES; i ++){
        iSector = wu_con_map[iCell].sidx[i];        
        if(iSector == NODATA_WU){
            continue;
        }
        
        wu_var[iCell][iSector].returned = 0.0;
        wu_var[iCell][iSector].consumed = 0.0;
        
        // groundwater
        if(wu_var[iCell][iSector].available_gw > 0){
            frac = wu_var[iCell][iSector].demand_gw / 
                    wu_var[iCell][iSector].available_gw;
            frac = min(frac, 1);
            wu_var[iCell][iSector].withdrawn_gw = 
                    wu_var[iCell][iSector].available_gw * frac;
        } else {
            wu_var[iCell][iSector].withdrawn_gw = 0;
        }

        wu_var[iCell][iSector].returned += 
                wu_var[iCell][iSector].withdrawn_gw * 
                (1 - wu_force[iCell][iSector].consumption_frac);
        wu_var[iCell][iSector].consumed += 
                wu_var[iCell][iSector].withdrawn_gw * 
                wu_force[iCell][iSector].consumption_frac;
        
        // surface water & dams
        if(wu_var[iCell][iSector].available_surf + 
                wu_var[iCell][iSector].available_dam > 0){
            
            // surface water
            frac = wu_var[iCell][iSector].demand_surf * 
                    (wu_var[iCell][iSector].available_surf / 
                    (wu_var[iCell][iSector].available_surf + 
                    wu_var[iCell][iSector].available_dam));
            frac = frac / wu_var[iCell][iSector].available_surf;
            frac = min(frac, 1);
            wu_var[iCell][iSector].withdrawn_surf = 
                    wu_var[iCell][iSector].available_surf * frac;
            
            // dams
            frac = wu_var[iCell][iSector].demand_surf * 
                    (wu_var[iCell][iSector].available_dam / 
                    (wu_var[iCell][iSector].available_surf + 
                    wu_var[iCell][iSector].available_dam));
            frac = frac / wu_var[iCell][iSector].available_dam;
            frac = min(frac, 1);
            wu_var[iCell][iSector].withdrawn_dam = 
                    wu_var[iCell][iSector].available_dam * frac;
        } else {
            wu_var[iCell][iSector].withdrawn_surf = 0;
            wu_var[iCell][iSector].withdrawn_dam = 0;
        }
        
        wu_var[iCell][iSector].returned += 
                wu_var[iCell][iSector].withdrawn_surf * 
                (1 - wu_force[iCell][iSector].consumption_frac);
        wu_var[iCell][iSector].returned += 
                wu_var[iCell][iSector].withdrawn_dam * 
                (1 - wu_force[iCell][iSector].consumption_frac);
        wu_var[iCell][iSector].consumed += 
                wu_var[iCell][iSector].withdrawn_surf * 
                wu_force[iCell][iSector].consumption_frac;
        wu_var[iCell][iSector].consumed += 
                wu_var[iCell][iSector].withdrawn_dam * 
                wu_force[iCell][iSector].consumption_frac;
        
        // comps
        if(plugin_options.COMP_WITH) {
            if(wu_var[iCell][iSector].available_comp > 0){
                frac = wu_var[iCell][iSector].demand_comp / 
                        wu_var[iCell][iSector].available_comp;
                frac = min(frac, 1);
                wu_var[iCell][iSector].withdrawn_comp = 
                        wu_var[iCell][iSector].available_comp * frac;
            } else {
                wu_var[iCell][iSector].withdrawn_comp = 0;
            }

            wu_var[iCell][iSector].returned += 
                    wu_var[iCell][iSector].withdrawn_comp * 
                    (1 - wu_force[iCell][iSector].consumption_frac);
            wu_var[iCell][iSector].consumed += 
                    wu_var[iCell][iSector].withdrawn_comp * 
                    wu_force[iCell][iSector].consumption_frac;
        }
        
        // remote
        for(j = 0; j < wu_con[iCell].nreceiving; j++){
            iCell2 = wu_con[iCell].receiving[j];

            iSector2 = wu_con_map[iCell2].sidx[i];        
            if(iSector2 == NODATA_WU){
                continue;
            }

            if(wu_var[iCell2][iSector2].available_remote > 0){
                frac = wu_var[iCell2][iSector2].demand_remote / 
                        wu_var[iCell2][iSector2].available_remote;
                frac = min(frac, 1);
                wu_var[iCell2][iSector2].withdrawn_remote = 
                        wu_var[iCell2][iSector2].available_remote * frac;
            } else {
                wu_var[iCell2][iSector2].withdrawn_remote = 0;
            }

            wu_var[iCell][iSector].returned += 
                    wu_var[iCell2][iSector2].withdrawn_remote * 
                    (1 - wu_force[iCell2][iSector2].consumption_frac);
            wu_var[iCell2][iSector2].consumed += 
                    wu_var[iCell2][iSector2].withdrawn_remote * 
                    wu_force[iCell2][iSector2].consumption_frac;
        }
        
        // non-renewable
        if(plugin_options.NONRENEW_WITH) {
            wu_var[iCell][iSector].withdrawn_nonrenew = 
                    wu_var[iCell][iSector].demand_gw - wu_var[iCell][iSector].withdrawn_gw - wu_var[iCell][iSector].withdrawn_comp;
            if(wu_var[iCell][iSector].withdrawn_nonrenew > wu_con[iCell].pumping_capacity) {
                wu_var[iCell][iSector].withdrawn_nonrenew = wu_con[iCell].pumping_capacity;
            }

            wu_var[iCell][iSector].returned += 
                    wu_var[iCell][iSector].withdrawn_nonrenew * 
                    (1 - wu_force[iCell][iSector].consumption_frac);
            wu_var[iCell][iSector].consumed += 
                    wu_var[iCell][iSector].withdrawn_nonrenew * 
                    wu_force[iCell][iSector].consumption_frac;
        }

        
        (*withdrawn_gw) += wu_var[iCell][iSector].withdrawn_gw;
        (*withdrawn_surf) += wu_var[iCell][iSector].withdrawn_surf;
        (*withdrawn_dam) += wu_var[iCell][iSector].withdrawn_dam;
        (*withdrawn_comp) += wu_var[iCell][iSector].withdrawn_comp;
        (*withdrawn_nonrenew) += wu_var[iCell][iSector].withdrawn_nonrenew;
        
        for(j = 0; j < wu_con[iCell].nreceiving; j++){
            iCell2 = wu_con[iCell].receiving[j];

            iSector2 = wu_con_map[iCell2].sidx[i];        
            if(iSector2 == NODATA_WU){
                continue;
            }
            
            (*withdrawn_remote) += wu_var[iCell2][iSector2].withdrawn_remote;
        }
        
        (*returned) += wu_var[iCell][iSector].returned;
    }
}

/******************************************
* @brief   Handle withdrawals and return flows
******************************************/
void
calculate_hydrology(size_t iCell, 
        double available_gw,
        double available_dam,
        double **av_gw,
        double *av_dam,
        double withdrawn_gw, 
        double withdrawn_surf, 
        double withdrawn_dam,  
        double withdrawn_comp, 
        double withdrawn_remote,
        double withdrawn_nonrenew,
        double returned)
{
    extern plugin_global_param_struct plugin_global_param;
    extern global_param_struct global_param;
    extern domain_struct  local_domain;
    extern all_vars_struct     *all_vars;
    extern option_struct options;
    extern soil_con_struct     *soil_con;
    extern veg_con_map_struct     *veg_con_map;
    extern veg_con_struct **veg_con;
    extern dam_con_map_struct *local_dam_con_map;
    extern dam_var_struct **local_dam_var;
    
    double ice;
    double withdrawn_discharge_tmp;
    double available_discharge_tmp;
    
    size_t rout_steps_per_dt;
    size_t iStep;
    size_t iVeg;
    size_t iBand;
    size_t iFrost;
    size_t iLayer;
    size_t iDam;
    
    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;
    
    // groundwater
    if(withdrawn_gw > 0.){
        iLayer = options.Nlayer - 1;
        for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){        
            if (veg_con[iCell][iVeg].Cv <= 0.0) {
                continue;
            }
            
            for(iBand = 0; iBand < options.SNOW_BAND; iBand++){        
                if (soil_con[iCell].AreaFract[iBand] <= 0.0) {
                    continue;
                }

                all_vars[iCell].cell[iVeg][iBand].layer[iLayer].moist -=
                        withdrawn_gw / 
                        (soil_con[iCell].AreaFract[iBand] *
                        veg_con[iCell][iVeg].Cv) *
                        av_gw[iVeg][iBand] /
                        available_gw;
                
                if (all_vars[iCell].cell[iVeg][iBand].layer[iLayer].moist < 0) {
                    all_vars[iCell].cell[iVeg][iBand].layer[iLayer].moist = 0.;
                }

                ice = 0;
                for(iFrost = 0; iFrost < options.Nfrost; iFrost ++){
                    ice += all_vars[iCell].cell[iVeg][iBand].layer[iLayer].ice[iFrost] *
                           soil_con[iCell].frost_fract[iFrost];
                }

                if(all_vars[iCell].cell[iVeg][iBand].layer[iLayer].moist < ice){
                    ice = all_vars[iCell].cell[iVeg][iBand].layer[iLayer].moist;
                }
            }
        }
    }

    // surface
    if(withdrawn_surf + withdrawn_remote + withdrawn_comp - returned != 0.) {
        available_discharge_tmp = 0.;
        for(iStep = 0; iStep < plugin_options.UH_LENGTH + rout_steps_per_dt + 1; iStep++) {
            available_discharge_tmp += rout_var[iCell].dt_discharge[iStep];
        }
        
        withdrawn_discharge_tmp = withdrawn_surf + withdrawn_remote + withdrawn_comp - returned;
        rout_var[iCell].discharge = 0.;
        rout_var[iCell].stream = 0.;
        for(iStep = 0; iStep < plugin_options.UH_LENGTH + rout_steps_per_dt + 1; iStep++) {
            if(available_discharge_tmp > 0) {
                rout_var[iCell].dt_discharge[iStep] -= 
                        withdrawn_discharge_tmp * 
                        (rout_var[iCell].dt_discharge[iStep] / available_discharge_tmp);
            } else {
                rout_var[iCell].dt_discharge[iStep] -= 
                    withdrawn_discharge_tmp / (plugin_options.UH_LENGTH + rout_steps_per_dt + 1);
            }
            if (rout_var[iCell].dt_discharge[iStep] < 0) {
                rout_var[iCell].dt_discharge[iStep] = 0.;
            }
            
            if (iStep < rout_steps_per_dt) {
                rout_var[iCell].discharge += rout_var[iCell].dt_discharge[iStep];
            }
            else {
                rout_var[iCell].stream += rout_var[iCell].dt_discharge[iStep];
            }
        }
    }
    
    // dam
    if(withdrawn_dam > 0.){
        if (plugin_options.DAMS) {
            for (iDam = 0; iDam < local_dam_con_map[iCell].nd_active; iDam++) {
                if(local_dam_var[iCell][iDam].active){
                    local_dam_var[iCell][iDam].storage -= 
                            withdrawn_dam * 
                            (av_dam[iDam] / available_dam) / 
                            MM_PER_M * 
                            local_domain.locations[iCell].area / 
                            M3_PER_HM3;
                    
                    if (local_dam_var[iCell][iDam].storage < 0) {
                        local_dam_var[iCell][iDam].storage = 0.;
                    }
                }
            }
        }
    }
    
    // non-renewable
    if(withdrawn_nonrenew > 0.) {
        rout_var[iCell].nonrenew_deficit += withdrawn_nonrenew;
        
        if(rout_var[iCell].nonrenew_deficit < 0){
            rout_var[iCell].nonrenew_deficit = 0;
        }
    }
}

/******************************************
* @brief   Check the water-use water balance
******************************************/
void
check_water_use_balance(size_t iCell, 
        double available_gw, 
        double available_surf, 
        double available_dam, 
        double available_comp, 
        double available_remote, 
        double demand_gw, 
        double demand_surf, 
        double demand_remote,
        double withdrawn_gw, 
        double withdrawn_surf, 
        double withdrawn_dam, 
        double withdrawn_comp, 
        double withdrawn_remote,
        double withdrawn_nonrenew)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct **wu_var;
    extern wu_con_struct *wu_con;
    extern wu_con_map_struct *wu_con_map;
    
    size_t i;
    size_t j;
    int iSector;
    size_t iCell2;
    int iSector2;
    
    for(i = 0; i < plugin_options.NWUTYPES; i ++){
        iSector = wu_con_map[iCell].sidx[i];        
        if(iSector == NODATA_WU){
            continue;
        }
                
        if (wu_var[iCell][iSector].withdrawn_gw - wu_var[iCell][iSector].available_gw > WU_BALANCE_ERROR_THRESH ||
                wu_var[iCell][iSector].withdrawn_surf - wu_var[iCell][iSector].available_surf > WU_BALANCE_ERROR_THRESH ||
                wu_var[iCell][iSector].withdrawn_dam - wu_var[iCell][iSector].available_dam > WU_BALANCE_ERROR_THRESH ||
                wu_var[iCell][iSector].withdrawn_comp - wu_var[iCell][iSector].available_comp > WU_BALANCE_ERROR_THRESH ||
                wu_var[iCell][iSector].withdrawn_surf + wu_var[iCell][iSector].withdrawn_dam - wu_var[iCell][iSector].demand_surf > WU_BALANCE_ERROR_THRESH ||
                wu_var[iCell][iSector].withdrawn_gw + wu_var[iCell][iSector].withdrawn_nonrenew + wu_var[iCell][iSector].withdrawn_comp - wu_var[iCell][iSector].demand_gw > WU_BALANCE_ERROR_THRESH) {
            log_err("Water-use water balance error for sector %zu:\n"
                    "groundwater:\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n"
                    "surface-water:\twithdrawn [%.4f]\tdemand (s + d) [%.4f]\tavailable [%.4f]\n"
                    "dam:\t\twithdrawn [%.4f]\tdemand (s + d) [%.4f]\tavailable [%.4f]\n"
                    "comp:\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n"
                    "non-renewable:\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [Inf]\n",
                    i,
                    wu_var[iCell][iSector].withdrawn_gw, 
                    wu_var[iCell][iSector].demand_gw, 
                    wu_var[iCell][iSector].available_gw,
                    wu_var[iCell][iSector].withdrawn_surf, 
                    wu_var[iCell][iSector].demand_surf, 
                    wu_var[iCell][iSector].available_surf,
                    wu_var[iCell][iSector].withdrawn_dam,
                    wu_var[iCell][iSector].demand_surf,
                    wu_var[iCell][iSector].available_dam,
                    wu_var[iCell][iSector].withdrawn_comp,
                    wu_var[iCell][iSector].demand_gw - wu_var[iCell][iSector].available_gw,
                    wu_var[iCell][iSector].available_comp,
                    wu_var[iCell][iSector].withdrawn_nonrenew,
                    wu_var[iCell][iSector].demand_gw - wu_var[iCell][iSector].available_gw - wu_var[iCell][iSector].available_comp);
        }
        
        
        for(j = 0; j < wu_con[iCell].nreceiving; j++){
            iCell2 = wu_con[iCell].receiving[j];

            iSector2 = wu_con_map[iCell2].sidx[i];        
            if(iSector2 == NODATA_WU){
                continue;
            }
            
            if(wu_var[iCell2][iSector2].withdrawn_remote - wu_var[iCell2][iSector2].available_remote > WU_BALANCE_ERROR_THRESH ||
                wu_var[iCell2][iSector2].withdrawn_remote - wu_var[iCell2][iSector2].demand_remote > WU_BALANCE_ERROR_THRESH) {
                log_err("Water-use water balance error for remote %zu:\n"
                    "groundwater:\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n"
                    "surface-water:\twithdrawn [%.4f]\tdemand (s + d) [%.4f]\tavailable [%.4f]\n"
                    "dam:\t\twithdrawn [%.4f]\tdemand (s + d) [%.4f]\tavailable [%.4f]\n"
                    "remote:\t\twithdrawn [%.4f]\tdemand (s + d) [%.4f]\tavailable [%.4f]\n",
                    i,
                    wu_var[iCell2][iSector2].withdrawn_gw, 
                    wu_var[iCell2][iSector2].demand_gw, 
                    wu_var[iCell2][iSector2].available_gw,
                    wu_var[iCell2][iSector2].withdrawn_surf, 
                    wu_var[iCell2][iSector2].demand_surf, 
                    wu_var[iCell2][iSector2].available_surf,
                    wu_var[iCell2][iSector2].withdrawn_dam,
                    wu_var[iCell2][iSector2].demand_surf,
                    wu_var[iCell2][iSector2].available_dam,
                    wu_var[iCell2][iSector2].withdrawn_remote, 
                    wu_var[iCell2][iSector2].demand_remote, 
                    wu_var[iCell2][iSector2].available_remote);
            }
        }
    }
    if(withdrawn_gw - available_gw  > WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES ||
            withdrawn_surf - available_surf  > WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES ||
            withdrawn_dam - available_dam  > WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES ||
            withdrawn_comp - available_comp  > WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES ||
            withdrawn_remote - available_remote  > WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES ||
            withdrawn_surf + withdrawn_dam - demand_surf > WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES ||
            withdrawn_gw + withdrawn_nonrenew + withdrawn_comp - demand_gw > WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES ||
            withdrawn_remote - demand_remote > WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES){ 
        log_err("Water-use water balance error for cell %zu:\n"
                "groundwater:\twithdrawn [%.4f]\tdemand [%.4f]\t\t\tavailable [%.4f]\n"
                "surface-water:\twithdrawn [%.4f]\tdemand (s + d) [%.4f]\tavailable [%.4f]\n"
                "dam:\t\twithdrawn [%.4f]\tdemand (s + d) [%.4f]\tavailable [%.4f]\n"
                "comp:\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n"
                "remote:\t\twithdrawn [%.4f]\tdemand [%.4f]\t\t\tavailable [%.4f]\n"
                "non-renewable:\twithdrawn [%.4f]\tdemand [%.4f]\t\t\tavailable [Inf]\n",
                iCell,
                withdrawn_gw, 
                demand_gw, 
                available_gw,
                withdrawn_surf, 
                demand_surf, 
                available_surf,
                withdrawn_dam,
                demand_surf, 
                available_dam,
                withdrawn_comp, 
                demand_gw - available_gw, 
                available_comp,
                withdrawn_remote, 
                demand_remote, 
                available_remote,
                withdrawn_nonrenew, 
                demand_gw - available_gw - available_comp);
    }
}

/******************************************
* @brief   Run water-use module
******************************************/
void
wu_run(size_t iCell)
{
    extern option_struct options;
    extern veg_con_map_struct     *veg_con_map;
    
    double **av_gw;
    double *av_dam;
    double available_gw;
    double available_surf;
    double available_dam;
    double available_comp;
    double available_remote;
    double demand_gw;
    double demand_surf;
    double demand_remote;
    double withdrawn_gw;
    double withdrawn_surf;
    double withdrawn_dam;
    double withdrawn_comp;
    double withdrawn_remote;
    double withdrawn_nonrenew;
    double returned;
    
    size_t iVeg;
    size_t iBand;
    
    /******************************************
     Alloc
    ******************************************/
    av_gw = malloc(veg_con_map[iCell].nv_active * sizeof(*av_gw));
    check_alloc_status(av_gw, "Memory allocation error.");
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        av_gw[iVeg] = malloc(options.SNOW_BAND * sizeof(*av_gw[iVeg]));
        check_alloc_status(av_gw[iVeg], "Memory allocation error.");
    }
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
            av_gw[iVeg][iBand] = 0.0;
        }
    }
    
    if(plugin_options.DAMS){
        av_dam = malloc(local_dam_con_map[iCell].nd_active * sizeof(*av_dam));
        check_alloc_status(av_dam, "Memory allocation error.");
    }
    
    /******************************************
     Init
    ******************************************/
    demand_gw = 0;
    demand_surf = 0;
    demand_remote = 0;
    available_gw = 0;
    available_surf = 0;
    available_dam = 0;
    available_comp = 0;
    available_remote = 0;
    withdrawn_gw = 0;
    withdrawn_surf = 0;
    withdrawn_dam = 0;
    withdrawn_comp = 0;
    withdrawn_remote = 0;
    withdrawn_nonrenew = 0;
    returned = 0;
    
    reset_wu(iCell);
    
    /******************************************
     Demand
    ******************************************/
    calculate_demand(iCell, 
                    &demand_gw, &demand_surf, &demand_remote);
    
    /******************************************
     Availability
    ******************************************/
    calculate_availability(iCell, 
                          &available_gw, &available_surf, &available_dam, &available_comp, &available_remote, av_gw, av_dam, 
                           demand_gw, demand_surf, demand_remote);
    
    /******************************************
     Devide
    ******************************************/ 
    calculate_division(iCell, 
                       available_gw, available_surf, available_dam, available_comp, available_remote, 
                       demand_gw, demand_surf, demand_remote);
    
    /******************************************
     Withdrawals & Consumption
    ******************************************/ 
    calculate_use(iCell, 
                 &withdrawn_gw, &withdrawn_surf, &withdrawn_dam, &withdrawn_comp, &withdrawn_remote, &withdrawn_nonrenew, 
                 &returned);
    
    /******************************************
     Return
    ******************************************/
    calculate_hydrology(iCell, 
                        available_gw, available_dam, av_gw, av_dam, 
                        withdrawn_gw, withdrawn_surf, withdrawn_dam, withdrawn_comp, withdrawn_remote, withdrawn_nonrenew,
                        returned);
    
    /******************************************
     Check balance
    ******************************************/
    check_water_use_balance(iCell, 
                            available_gw, available_surf, available_dam, available_comp, available_remote, 
                            demand_gw, demand_surf, demand_remote, 
                            withdrawn_gw, withdrawn_surf, withdrawn_dam, withdrawn_comp, withdrawn_remote, withdrawn_nonrenew);
        
    /******************************************
     Free
    ******************************************/
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        free(av_gw[iVeg]);
    }
    free(av_gw);
    
    if(plugin_options.DAMS){
        free(av_dam);
    }
}
