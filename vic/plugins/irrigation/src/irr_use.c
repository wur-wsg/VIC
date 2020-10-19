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
    
    double demand;
    double efficiency;
    double groundwater;
    double total;
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
    efficiency = 0.0;
    groundwater = 0.0;
    total = 0.0;
    csoil_con = &soil_con[iCell];
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        veg_fract = cveg_con->Cv;
        
        if(veg_fract > 0) {
        
            for(j = 0; j < options.SNOW_BAND; j++){
                cirr_var = &irr_var[iCell][i][j];
                area_fract = csoil_con->AreaFract[j];

                if(area_fract > 0){
                    if(cirr_var->flag_req){
                        demand += cirr_var->requirement * veg_fract * area_fract * cirr_con->irrigation_efficiency;
                        if(cirr_con->paddy) {
                            demand += PADDY_FLOOD_HEIGHT * veg_fract * area_fract * cirr_con->irrigation_efficiency;
                        }
                        efficiency += demand * cirr_con->irrigation_efficiency;
                        groundwater += demand * cirr_con->groundwater_fraction;
                        total += demand;
                    }
                }
            }
        }
    }
    if(total > 0){
        efficiency /= total;
        efficiency = 1 / efficiency;
        groundwater /= total;
    }
    efficiency = 1.0;
    
    if(irr_con_map[iCell].ni_active > 0){
        iSector = wu_con_map[iCell].sidx[WU_IRRIGATION];
        wu_force[iCell][iSector].demand = demand;
        wu_force[iCell][iSector].consumption_frac = efficiency;
        wu_force[iCell][iSector].groundwater_frac = groundwater;
    }
}

/******************************************
* @brief   Return irrigation leftovers
******************************************/
void
irr_return_leftover(size_t iCell)
{
    extern option_struct options;
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern veg_con_struct **veg_con;
    extern soil_con_struct *soil_con;
    
    double leftover;
    double area_fract;
    double veg_fract;
    double available_discharge_tmp;
    
    size_t iStep;
    size_t i;
    size_t j;
    size_t rout_steps_per_dt;
    
    irr_con_struct *cirr_con;
    irr_var_struct *cirr_var;
    soil_con_struct *csoil_con;
    veg_con_struct *cveg_con;
    
    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;
    
    leftover = 0.0;
    csoil_con = &soil_con[iCell];
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        veg_fract = cveg_con->Cv;

        if(veg_fract > 0) {

            for(j = 0; j < options.SNOW_BAND; j++){
                cirr_var = &irr_var[iCell][i][j];
                area_fract = csoil_con->AreaFract[j];

                if(area_fract > 0){
                    leftover += cirr_var->leftover * veg_fract * area_fract;
                    cirr_var->leftover = 0.0;
                }
            }
        }
    }
    
    if(leftover > 0.) {
        available_discharge_tmp = 0.;
        for(iStep = rout_steps_per_dt; iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1; iStep++) {
            available_discharge_tmp += rout_var[iCell].dt_discharge[iStep];
        }
        
        leftover = 
		leftover /
                MM_PER_M * local_domain.locations[iCell].area / global_param.dt;
        
        for(iStep = rout_steps_per_dt; iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1; iStep++) {
            if(available_discharge_tmp > 0) {
                // Scale withdrawal proportionally to availability
                rout_var[iCell].dt_discharge[iStep] += 
                        leftover * 
                        (rout_var[iCell].dt_discharge[iStep] / available_discharge_tmp);
            } else {
                // Scale withdrawal proportionally to length
                rout_var[iCell].dt_discharge[iStep] += 
                    leftover / (plugin_options.UH_LENGTH - 1);
            }
            if (rout_var[iCell].dt_discharge[iStep] < 0) {
                rout_var[iCell].dt_discharge[iStep] = 0.;
            }
        }
            
        
        rout_var[iCell].discharge = 0.;
        rout_var[iCell].stream = 0.;
        for(iStep = 0; iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1; iStep++) {
            // Recalculate discharge and stream moisture
            if (iStep < rout_steps_per_dt) {
                rout_var[iCell].discharge += rout_var[iCell].dt_discharge[iStep];
            }
            else {
                rout_var[iCell].stream += rout_var[iCell].dt_discharge[iStep];
            }
        }
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
    extern wu_con_map_struct *wu_con_map;
    
    double demand;
    double available;
    double received;
    double applied;
    double leftover;
    double prev_leftover;
    
    double avail_frac;
    double area_fract;
    double veg_fract;
    double max_moist_1;
    double max_moist_2;
    double max_added;
    
    size_t iSector;
    size_t i;
    size_t j;
    
    irr_con_struct *cirr_con;
    irr_var_struct *cirr_var;
    soil_con_struct *csoil_con;
    veg_con_struct *cveg_con;
    cell_data_struct *ccell_var;
        
    // initialize
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        for(j = 0; j < options.SNOW_BAND; j++){
            cirr_var = &irr_var[iCell][i][j];
            
            cirr_var->received = 0.0;
            cirr_var->applied = 0.0;
        }
    }
    
    // get demand
    demand = 0.0;
    csoil_con = &soil_con[iCell];
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        veg_fract = cveg_con->Cv;
        
        if(veg_fract > 0) {
        
            for(j = 0; j < options.SNOW_BAND; j++){
                cirr_var = &irr_var[iCell][i][j];
                area_fract = csoil_con->AreaFract[j];

                if(area_fract > 0){
                    if(cirr_var->flag_req){
                        demand += cirr_var->requirement * veg_fract * area_fract * cirr_con->irrigation_efficiency;
                        if(cirr_con->paddy) {
                            demand += PADDY_FLOOD_HEIGHT * veg_fract * area_fract * cirr_con->irrigation_efficiency;
                        }
                    }
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
    
    // do irrigation
    received = 0.0;
    applied = 0.0;
    leftover = 0.0;
    prev_leftover = 0.0;
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        veg_fract = cveg_con->Cv;

        if(veg_fract > 0) {

            for(j = 0; j < options.SNOW_BAND; j++){
                cirr_var = &irr_var[iCell][i][j];
                ccell_var = &all_vars[iCell].cell[cirr_con->veg_index][j];
                area_fract = csoil_con->AreaFract[j];

                if(area_fract > 0){
                    max_moist_1 = csoil_con->max_moist[0] - ccell_var->layer[0].moist;
                    max_moist_2 = csoil_con->max_moist[1] - ccell_var->layer[1].moist;
                    max_added = max_moist_1;
                    
                    // leftover
                    if(cirr_var->leftover > 0){
                        if(cirr_var->leftover < max_added){
                            cirr_var->applied += cirr_var->leftover;
                        } else {
                            cirr_var->applied = max_added;
                        }
                    }
                    
                    // received
                    if(cirr_var->flag_req && avail_frac > 0){
                        cirr_var->received = cirr_var->requirement * cirr_con->irrigation_efficiency * avail_frac;
                        if(cirr_con->paddy) {
                            cirr_var->received += PADDY_FLOOD_HEIGHT * cirr_con->irrigation_efficiency * avail_frac;
                        }

                        if(cirr_var->received + cirr_var->applied < max_added){
                            cirr_var->applied += cirr_var->received;
                        } else {
                            cirr_var->applied = max_added;
                        }
                    }
                    
                    prev_leftover += cirr_var->leftover * veg_fract * area_fract;
                    
                    // apply
                    ccell_var->layer[0].moist += min(cirr_var->applied, max_moist_1);
                    cirr_var->leftover += cirr_var->received - cirr_var->applied;
                    
                    received += cirr_var->received * veg_fract * area_fract;
                    applied += cirr_var->applied * veg_fract * area_fract;
                    leftover += cirr_var->leftover * veg_fract * area_fract;
                }
            }
        }
    }
    
    // Check water balance
    if(received - demand > WU_BALANCE_ERROR_THRESH ||
       applied - received + (leftover - prev_leftover) > WU_BALANCE_ERROR_THRESH ||
       available - applied - (leftover - prev_leftover) > WU_BALANCE_ERROR_THRESH) {
        log_err("Irrigation water balance error for cell %zu: "
                "demand is %.10f mm\t%.10f is available\n"
                "%.10f mm is received\t%10f mm is applied\t%10f mm is leftover",
                iCell,
                demand,
                available,
                received,
                applied,
                leftover - prev_leftover);
    }
    
    irr_return_leftover(iCell);
}     
