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
    extern option_struct       options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct    **irr_con;
    extern irr_var_struct   ***irr_var;
    extern soil_con_struct    *soil_con;
    extern veg_con_struct    **veg_con;
    extern wu_con_map_struct  *wu_con_map;
    extern wu_force_struct   **wu_force;

    double                     demand;
    double                     demand_crop;
    double                     efficiency;
    double                     groundwater;
    double                     consumed;
    double                     area_fract;
    double                     veg_fract;

    size_t                     iSector;
    size_t                     i;
    size_t                     j;

    irr_con_struct            *cirr_con;
    irr_var_struct            *cirr_var;
    soil_con_struct           *csoil_con;
    veg_con_struct            *cveg_con;

    // get demand
    demand = 0.0;
    efficiency = 0.0;
    groundwater = 0.0;
    consumed = 0.0;
    
    csoil_con = &(soil_con[iCell]);
    for (i = 0; i < irr_con_map[iCell].ni_active; i++) {
        cirr_con = &(irr_con[iCell][i]);
        cveg_con = &(veg_con[iCell][cirr_con->veg_index]);
        veg_fract = cveg_con->Cv;

        if (veg_fract > 0) {
            for (j = 0; j < options.SNOW_BAND; j++) {
                cirr_var = &(irr_var[iCell][i][j]);
                area_fract = csoil_con->AreaFract[j];

                if (area_fract > 0) {
                    if (cirr_var->flag_req) {
                        demand_crop = cirr_var->requirement * veg_fract *
                                  area_fract;
                        if (cirr_con->paddy) {
                            demand_crop += PADDY_FLOOD_HEIGHT * veg_fract *
                                      area_fract;
                        }
                        consumed += demand_crop;
                        demand += demand_crop * 
                                cirr_con->irrigation_efficiency;
                        groundwater += demand_crop * 
                                cirr_con->irrigation_efficiency * 
                                cirr_con->groundwater_fraction;
                    }
                }
            }
        }
    }
    if (demand > 0) {
        efficiency = consumed / demand;
        groundwater = groundwater / demand;
    }
    // efficiency = 1.0;

    if (irr_con_map[iCell].ni_active > 0) {
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
    extern option_struct       options;
    extern domain_struct       local_domain;
    extern global_param_struct global_param;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct    **irr_con;
    extern irr_var_struct   ***irr_var;
    extern veg_con_struct    **veg_con;
    extern soil_con_struct    *soil_con;

    double                     leftover;
    double                     area_fract;
    double                     veg_fract;
    double                     available_discharge_tmp;

    size_t                     iStep;
    size_t                     i;
    size_t                     j;
    size_t                     rout_steps_per_dt;

    irr_con_struct            *cirr_con;
    irr_var_struct            *cirr_var;
    soil_con_struct           *csoil_con;
    veg_con_struct            *cveg_con;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    leftover = 0.0;
    csoil_con = &(soil_con[iCell]);
    for (i = 0; i < irr_con_map[iCell].ni_active; i++) {
        cirr_con = &(irr_con[iCell][i]);
        cveg_con = &(veg_con[iCell][cirr_con->veg_index]);
        veg_fract = cveg_con->Cv;

        if (veg_fract > 0) {
            for (j = 0; j < options.SNOW_BAND; j++) {
                cirr_var = &(irr_var[iCell][i][j]);
                area_fract = csoil_con->AreaFract[j];

                if (area_fract > 0) {
                    if (!cirr_con->paddy) {
                        leftover += cirr_var->leftover * veg_fract * area_fract;
                        cirr_var->leftover = 0.0;
                    }
                }
            }
        }
    }

    if (leftover > 0.) {
        available_discharge_tmp = 0.;
        for (iStep = rout_steps_per_dt;
             iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
             iStep++) {
            available_discharge_tmp += rout_var[iCell].dt_discharge[iStep];
        }

        leftover =
            leftover /
            MM_PER_M * local_domain.locations[iCell].area / global_param.dt;

        for (iStep = rout_steps_per_dt;
             iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
             iStep++) {
            if (available_discharge_tmp > 0) {
                // Scale withdrawal proportionally to availability
                rout_var[iCell].dt_discharge[iStep] +=
                    leftover *
                    (rout_var[iCell].dt_discharge[iStep] /
                     available_discharge_tmp);
            }
            else {
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
        for (iStep = 0;
             iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
             iStep++) {
            // Recalculate discharge and stream moisture
            if (iStep < rout_steps_per_dt) {
                rout_var[iCell].discharge +=
                    rout_var[iCell].dt_discharge[iStep];
            }
            else {
                rout_var[iCell].stream += rout_var[iCell].dt_discharge[iStep];
            }
        }
    }
}

/******************************************
* @brief   Apply irrigation water withdrawals
******************************************/
void
irr_apply(size_t iCell, 
          size_t iVeg, 
          size_t iBand,
          double received, 
          double *applied, 
          double *leftover)
{
    extern soil_con_struct    *soil_con;
    extern all_vars_struct    *all_vars;
    
    double                     applied_tmp;
    double                     leftover_tmp;
    double                     max_added;

    soil_con_struct           *csoil_con;
    cell_data_struct          *ccell_var;
    
    csoil_con = &(soil_con[iCell]);
    ccell_var = &(all_vars[iCell].cell[iVeg][iBand]);
    
    max_added = csoil_con->max_moist[0] - ccell_var->layer[0].moist;
    if (received < max_added) {
        applied_tmp = received;
    }
    else {
        applied_tmp = max_added;
    }
    leftover_tmp = received - applied_tmp;

    ccell_var->layer[0].moist += applied_tmp;
    
    (*applied) += applied_tmp;
    (*leftover) += leftover_tmp;
}

/******************************************
* @brief   Get leftover irrigation
******************************************/
void
irr_leftover(size_t iCell)
{
    extern option_struct       options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct    **irr_con;
    extern irr_var_struct   ***irr_var;
    extern veg_con_struct    **veg_con;
    extern soil_con_struct    *soil_con;
    
    double                     area_fract;
    double                     veg_fract;
    double                     leftover_tmp;

    size_t                     iIrr;
    size_t                     iBand;
    size_t                     iVeg;

    irr_con_struct            *cirr_con;
    irr_var_struct            *cirr_var;
    soil_con_struct           *csoil_con;
    veg_con_struct            *cveg_con;

    csoil_con = &(soil_con[iCell]);
    
    for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
        cirr_con = &(irr_con[iCell][iIrr]);
        iVeg = cirr_con->veg_index;
        cveg_con = &(veg_con[iCell][iVeg]);
        veg_fract = cveg_con->Cv;

        if (veg_fract > 0) {
            for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
                cirr_var = &(irr_var[iCell][iIrr][iBand]);
                area_fract = csoil_con->AreaFract[iBand];

                if (area_fract > 0) {

                    if (cirr_var->leftover > 0) {
                        leftover_tmp = cirr_var->leftover;
                        cirr_var->leftover = 0.0;
                        
                        irr_apply(iCell, iVeg, iBand,
                                  leftover_tmp,
                                &(cirr_var->applied),
                                &(cirr_var->leftover));
                    }
                }
            }
        }
    }
}

/******************************************
* @brief   Get potential irrigation
******************************************/
void
irr_potential(size_t iCell)
{
    extern option_struct       options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct    **irr_con;
    extern irr_var_struct   ***irr_var;
    extern veg_con_struct    **veg_con;
    extern soil_con_struct    *soil_con;

    double                     area_fract;
    double                     veg_fract;
    double received_tmp;

    size_t                     iIrr;
    size_t                     iBand;
    size_t                     iVeg;

    irr_con_struct            *cirr_con;
    irr_var_struct            *cirr_var;
    soil_con_struct           *csoil_con;
    veg_con_struct            *cveg_con;
    
    csoil_con = &(soil_con[iCell]);
    
    for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
        cirr_con = &(irr_con[iCell][iIrr]);
        iVeg = cirr_con->veg_index;
        cveg_con = &(veg_con[iCell][iVeg]);
        veg_fract = cveg_con->Cv;

        if (veg_fract > 0) {
            for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
                cirr_var = &(irr_var[iCell][iIrr][iBand]);
                area_fract = csoil_con->AreaFract[iBand];

                if (area_fract > 0) {
                    
                    received_tmp = 0.0;
                    if(cirr_var->flag_req){
                        received_tmp = cirr_var->requirement;
                        if (cirr_con->paddy) {
                            received_tmp += PADDY_FLOOD_HEIGHT;
                        }
                        
                        irr_apply(iCell, iVeg, iBand, 
                                  received_tmp, 
                                &(cirr_var->applied), 
                                &(cirr_var->leftover));
                        cirr_var->received += received_tmp;
                    }
                }
            }
        }
    }
}

/******************************************
* @brief   Get water-use irrigation
******************************************/
void
irr_wateruse(size_t iCell)
{
    extern option_struct       options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct    **irr_con;
    extern irr_var_struct   ***irr_var;
    extern veg_con_struct    **veg_con;
    extern soil_con_struct    *soil_con;
    extern wu_con_map_struct  *wu_con_map;

    double received_tmp;
    double                     demand;
    double                     available;
    double                     received;
    double                     applied;
    double                     prev_applied;
    double                     leftover;
    double                     prev_leftover;

    double                     avail_frac;
    double                     area_fract;
    double                     veg_fract;

    size_t                     iSector;
    size_t                     iIrr;
    size_t                     iBand;
    size_t                     iVeg;

    irr_con_struct            *cirr_con;
    irr_var_struct            *cirr_var;
    soil_con_struct           *csoil_con;
    veg_con_struct            *cveg_con;

    csoil_con = &(soil_con[iCell]);
    
    // initialize
    demand = 0.0;
    available = 0.0;
    received = 0.0;
    applied = 0.0;
    prev_applied = 0.0;
    leftover = 0.0;
    prev_leftover = 0.0;
    
    // get demand
    for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
        cirr_con = &(irr_con[iCell][iIrr]);
        iVeg = cirr_con->veg_index;
        cveg_con = &(veg_con[iCell][iVeg]);
        veg_fract = cveg_con->Cv;

        if (veg_fract > 0) {
            for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
                cirr_var = &(irr_var[iCell][iIrr][iBand]);
                area_fract = csoil_con->AreaFract[iBand];

                if (area_fract > 0) {
                    
                    if (cirr_var->flag_req) {
                        demand += cirr_var->requirement * veg_fract *
                                  area_fract;
                        if (cirr_con->paddy) {
                            demand += PADDY_FLOOD_HEIGHT * veg_fract *
                                      area_fract;
                        }
                    }
                }
            }
        }
    }

    // get availability
    iSector = wu_con_map[iCell].sidx[WU_IRRIGATION];
    available = wu_var[iCell][iSector].consumed;

    // calculate fraction
    // this is used to distribute irrigation relative to demand
    // for each irrigated vegetation type and each snow band
    if (available > 0) {
        avail_frac = available / demand;
    }
    else {
        avail_frac = 0;
    }

    // do irrigation
    if (available > 0) {
        for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
            cirr_con = &(irr_con[iCell][iIrr]);
            iVeg = cirr_con->veg_index;
            cveg_con = &(veg_con[iCell][iVeg]);
            veg_fract = cveg_con->Cv;

            if (veg_fract > 0) {
                for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
                    cirr_var = &(irr_var[iCell][iIrr][iBand]);
                    area_fract = csoil_con->AreaFract[iBand];

                    if (area_fract > 0) {

                        received_tmp = 0.0;
                        if (cirr_var->flag_req && avail_frac > 0) {
                            received_tmp = cirr_var->requirement *
                                                 avail_frac;
                            if (cirr_con->paddy) {
                                received_tmp += PADDY_FLOOD_HEIGHT *
                                                      avail_frac;
                            }
                            
                            prev_leftover += cirr_var->leftover * veg_fract * area_fract;
                            prev_applied += cirr_var->applied * veg_fract * area_fract;
                            
                            irr_apply(iCell, iVeg, iBand,
                                    received_tmp,
                                    &(cirr_var->applied),
                                    &(cirr_var->leftover));
                            cirr_var->received += received_tmp;

                            received += cirr_var->received * veg_fract * area_fract;
                            applied += cirr_var->applied * veg_fract * area_fract;
                            leftover += cirr_var->leftover * veg_fract * area_fract;
                        }
                    }
                }
            }
        }
    }
    
    // check water balance
    if (received - demand > WU_BALANCE_ERROR_THRESH ||
        (applied - prev_applied) + (leftover - prev_leftover) - received >
        WU_BALANCE_ERROR_THRESH ||
        received - (applied - prev_applied) - (leftover - prev_leftover) >
        WU_BALANCE_ERROR_THRESH) {
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
}

/******************************************
* @brief   Get wofost irrigation
******************************************/
void
irr_wofost(size_t iCell)
{
    extern option_struct       options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct    **irr_con;
    extern irr_var_struct   ***irr_var;
    extern veg_con_struct    **veg_con;
    extern veg_con_map_struct *veg_con_map;
    extern soil_con_struct    *soil_con;
    extern SimUnit           ***Grid;
    extern dmy_struct           *dmy;
    extern size_t current;
    
    double                     area_fract;
    double                     veg_fract;
    double                     crop_fract;
    double received_tmp;
    bool irrigated;

    size_t                     crop_class;
    size_t                     veg_class;
    size_t                     iIrr;
    size_t                     iBand;
    size_t                     iVeg;

    irr_con_struct            *cirr_con;
    irr_var_struct            *cirr_var;
    soil_con_struct           *csoil_con;
    veg_con_struct            *cveg_con;
    SimUnit *cgrid;
    
    csoil_con = &(soil_con[iCell]);

    // get availability
    // this takes the wofost irrigation times the fractional crop cover
    // note this assumes irrigation to be spread over the entire tile
    csoil_con = &(soil_con[iCell]);
    
    for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
        cirr_con = &(irr_con[iCell][iIrr]);
        iVeg = cirr_con->veg_index;
        cveg_con = &(veg_con[iCell][iVeg]);
        veg_fract = cveg_con->Cv;

        if (veg_fract > 0) {
            for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
                cirr_var = &(irr_var[iCell][iIrr][iBand]);
                area_fract = csoil_con->AreaFract[iBand];
                cgrid = Grid[iCell][iBand];

                if (area_fract > 0) {

                    received_tmp = 0.0;
                    while (cgrid) {
                        crop_class = cgrid->met->crop_class;
                        veg_class = crop_con_map[iCell].veg_class[crop_class];
                        crop_fract = crop_con_map[iCell].Cc[crop_class][dmy[current].month - 1];
                        
                        if(veg_con_map[iCell].vidx[veg_class] == (int)iVeg) {
                            received_tmp += List(cgrid->mng->Irrigation) * crop_fract * MM_PER_CM;
                        }

                        cgrid = cgrid->next;
                    }
                    
                    if(received_tmp > 0){
                        log_info("test");
                    }
                    
                    irr_apply(iCell, iVeg, iBand,
                              received_tmp,
                             &cirr_var->applied,
                             &cirr_var->leftover);
                    cirr_var->received += received_tmp;
                }
            }
        }
    }
    
    // Check wofost crop that are not irrigated
    for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
        cirr_var = &(irr_var[iCell][iIrr][iBand]);
        area_fract = csoil_con->AreaFract[iBand];
        cgrid = Grid[iCell][iBand];

        if (area_fract > 0) {

            while (cgrid) {
                crop_class = cgrid->met->crop_class;
                veg_class = crop_con_map[iCell].veg_class[crop_class];
                iVeg = veg_con_map[iCell].vidx[veg_class];
                crop_fract = crop_con_map[iCell].Cc[crop_class][dmy[current].month - 1];

                received_tmp = List(cgrid->mng->Irrigation) * crop_fract * MM_PER_CM;
                
                irrigated = false;
                for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
                    if(irr_con_map[iCell].vidx[iIrr] == (int)iVeg){
                        irrigated = true;
                    }
                }
                
                if(received_tmp > 0 && !irrigated){
                    log_err("WOFOST crop %zu contains irrigation in management, "
                            "but irrigation module does not irrigate "
                            "vegetation type %zu", crop_class, veg_class);
                }
                
                cgrid = cgrid->next;
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
    
    size_t iIrr;
    size_t iBand;
    
    irr_var_struct            *cirr_var;
    
    // initialize
    for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            cirr_var = &(irr_var[iCell][iIrr][iBand]);

            cirr_var->received = 0.0;
            cirr_var->applied = 0.0;
        }
    }
    
    // apply leftover irrigation
    irr_leftover(iCell);
    
    // irrigate
    if (plugin_options.POTENTIAL_IRRIGATION) {
        irr_potential(iCell);
    }
    else {
        if(plugin_options.WATERUSE){
            irr_wateruse(iCell);
        }
        if(plugin_options.WOFOST){
            irr_wofost(iCell);
        }
    }
    
    // irr_return_leftover(iCell);
}