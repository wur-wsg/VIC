/******************************************************************************
 * @section DESCRIPTION
 *
 * Run irrigation water-use
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
* @brief   Set the irrigation demand (for water-use module)
******************************************/
void
irr_set_demand(size_t iCell)
{
    extern option_struct options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern wu_con_map_struct *wu_con_map;
    extern wu_force_struct **wu_force;
    extern size_t NR;
    
    double demand;
    double area_fract;
    double veg_fract;
    
    size_t iSector;
    size_t i;
    size_t j;
    
    irr_con_struct *cirr_con;
    irr_var_struct *cirr_var;
    soil_con_struct *csoil_con;
    veg_con_struct *cveg_con;
    
    // get demand
    demand = 0.0;
    csoil_con = &soil_con[iCell];
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        veg_fract = cveg_con->Cv;
        
        for(j = 0; j < options.SNOW_BAND; j++){
            cirr_var = &irr_var[iCell][i][j];
            area_fract = csoil_con->AreaFract[j];
            
            if(area_fract > 0){
                if(cirr_var->flag_req){
                    demand += cirr_var->requirement * veg_fract * area_fract * cirr_con->irrigation_efficiency;
                }
            }
        }
    }
    
    if(irr_con_map[iCell].ni_active > 0){
        iSector = wu_con_map[iCell].sidx[WU_IRRIGATION];
        wu_force[iCell][iSector].demand[NR] = demand;
        wu_force[iCell][iSector].consumption_frac[NR] = 1.0;
        wu_force[iCell][iSector].groundwater_frac[NR] = irr_con[iCell][0].groundwater_fraction;
    }
}

/******************************************
* @brief   Get irrigation water withdrawals
******************************************/
void
irr_get_withdrawn(size_t iCell)
{
    extern option_struct options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern all_vars_struct *all_vars;
    extern global_param_struct global_param;
    extern wu_con_map_struct *wu_con_map;
    
    double demand;
    double abs_demand;
    double available;
    double avail_frac;
    double avail_irr;
    double area_fract;
    double veg_fract;
    double max_infilt;
    double max_moist;
    double max_added;
    
    size_t iSector;
    size_t i;
    size_t j;
    
    irr_con_struct *cirr_con;
    irr_var_struct *cirr_var;
    soil_con_struct *csoil_con;
    veg_con_struct *cveg_con;
    cell_data_struct *ccell_var;
        
    // get demand
    demand = 0.0;
    csoil_con = &soil_con[iCell];
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        veg_fract = cveg_con->Cv;
        
        for(j = 0; j < options.SNOW_BAND; j++){
            cirr_var = &irr_var[iCell][i][j];
            area_fract = csoil_con->AreaFract[j];
            
            if(area_fract > 0){
                if(cirr_var->flag_req){
                    demand += cirr_var->requirement * veg_fract * area_fract * cirr_con->irrigation_efficiency;
                }
            }
        }
    }
    
    // get availability
    available = 0.0;
    if(plugin_options.POTENTIAL_IRRIGATION){
        available = demand;
    } else {
        iSector = wu_con_map[iCell].sidx[WU_IRRIGATION];
        available = wu_var[iCell][iSector].consumed;
    }
    
    if(available > 0){
        avail_frac = available / demand;
    } else {
        avail_frac = 0;
    }
    
    // do leftover
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];

        for(j = 0; j < options.SNOW_BAND; j++){
            cirr_var = &irr_var[iCell][i][j];
            ccell_var = &all_vars[iCell].cell[cirr_con->veg_index][j];
            area_fract = csoil_con->AreaFract[j];

            if(area_fract > 0){
                //max_infilt = ccell_var->layer[0].Ksat / global_param.model_steps_per_day;
                max_moist = csoil_con->max_moist[0] - ccell_var->layer[0].moist;
                //max_added = min(max_infilt, max_moist);
                max_added = max_moist;

                if(cirr_var->leftover > 0){
                    if(cirr_var->leftover < max_added){
                        ccell_var->layer[0].moist += cirr_var->leftover;
                        cirr_var->leftover = 0.0;
                    } else {
                        ccell_var->layer[0].moist += max_added;
                        cirr_var->leftover -= max_added;
                    }
                }
            }
        }
    }
    
    // do irrigation
    if(avail_frac > 0){
        
        abs_demand = 0.0;
        for(i = 0; i < irr_con_map[iCell].ni_active; i++){
            cirr_con = &irr_con[iCell][i];
            cveg_con = &veg_con[iCell][cirr_con->veg_index];
            veg_fract = cveg_con->Cv;

            for(j = 0; j < options.SNOW_BAND; j++){
                cirr_var = &irr_var[iCell][i][j];
                ccell_var = &all_vars[iCell].cell[cirr_con->veg_index][j];
                area_fract = csoil_con->AreaFract[j];

                if(area_fract > 0){
                    if(cirr_var->flag_req){
                        //max_infilt = ccell_var->layer[0].Ksat / global_param.model_steps_per_day;
                        max_moist = csoil_con->max_moist[0] - ccell_var->layer[0].moist;
                        //max_added = min(max_infilt, max_moist);
                        max_added = max_moist;
                        
                        avail_irr = cirr_var->requirement * cirr_con->irrigation_efficiency * avail_frac;
                        abs_demand += avail_irr * veg_fract * area_fract;
                        if(avail_irr < max_added){
                            ccell_var->layer[0].moist += avail_irr;
                            avail_irr = 0.0;
                        } else {
                            ccell_var->layer[0].moist += max_added;
                            cirr_var->leftover += avail_irr - max_added;
                        }
                    }
                }
            }
        }
        
        if(abs_demand - demand > WU_BALANCE_ERROR_THRESH){
            log_err("Irrigation water balance error for cell %zu: "
                    "demand is %.10f mm but %.10f mm is added to the soil",
                    iCell,
                    demand,
                    abs_demand);
        }
    }
}     
