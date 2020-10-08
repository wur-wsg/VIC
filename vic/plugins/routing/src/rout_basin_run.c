/******************************************************************************
 * @section DESCRIPTION
 *
 * Run routing on local node (basin decomposition)
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
* @brief   Run routing on local node (basin decomposition)
******************************************/
void
rout_basin_run(size_t iCell)
{
    extern domain_struct              local_domain;
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct       plugin_options;
    extern rout_var_struct           *rout_var;
    extern rout_con_struct           *rout_con;
    extern rout_force_struct         *rout_force;
    extern double                  ***out_data;

    double                            in_runoff;
    double                            in_baseflow;
    double                            inflow;
    double                           *dt_inflow;
    double                            runoff;
    double                           *dt_runoff;
    size_t                            rout_steps_per_dt;
    double                            prev_stream;
    double                           *convoluted;

    size_t                            i;
    size_t                            j;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;
    
    /* Allocate data */
    dt_inflow = malloc(rout_steps_per_dt * sizeof(*dt_inflow));
    check_alloc_status(dt_inflow, "Memory allocation error");
    dt_runoff = malloc(rout_steps_per_dt * sizeof(*dt_runoff));
    check_alloc_status(dt_runoff, "Memory allocation error");
    convoluted = malloc((plugin_options.UH_LENGTH + rout_steps_per_dt - 1) * sizeof(*convoluted));
    check_alloc_status(convoluted, "Memory allocation error");
    
    /* Shift and clear previous discharge data */
    for (i = 0; i < rout_steps_per_dt; i++) {
        rout_var[iCell].dt_discharge[0] = 0.0;
        cshift(rout_var[iCell].dt_discharge,
               plugin_options.UH_LENGTH + rout_steps_per_dt - 1, 1, 0, 1);
    }
    for (i = 0; i < rout_steps_per_dt + plugin_options.UH_LENGTH - 1; i++) {
        if(rout_var[iCell].dt_discharge[i] != rout_var[iCell].dt_discharge[i]){
            log_err("dt_discharge NaN 1");
        }
    }
    
    /* RUNOFF*/
    // Gather runoff from VIC
    in_runoff = out_data[iCell][OUT_RUNOFF][0];
    if(in_runoff != in_runoff){
        log_err("in_runoff NaN 1");
    }
    in_baseflow = out_data[iCell][OUT_BASEFLOW][0];
    if(in_baseflow != in_baseflow){
        log_err("in_baseflow NaN 1");
    }
    
    if(rout_var[iCell].nonrenew_deficit != rout_var[iCell].nonrenew_deficit){
        log_err("nonrenew_deficit NaN 1");
    }
    if(rout_var[iCell].nonrenew_deficit > 0) {
        if (in_baseflow > rout_var[iCell].nonrenew_deficit) {
            in_baseflow -= rout_var[iCell].nonrenew_deficit;
            rout_var[iCell].nonrenew_deficit = 0.;
            if(in_baseflow != in_baseflow){
                log_err("in_baseflow NaN 2");
            }
            if(rout_var[iCell].nonrenew_deficit != rout_var[iCell].nonrenew_deficit){
                log_err("nonrenew_deficit NaN 2");
            }
        } else {
            rout_var[iCell].nonrenew_deficit -= in_baseflow;
            in_baseflow = 0.;
            if(in_baseflow != in_baseflow){
                log_err("in_baseflow NaN 3");
            }
            if(rout_var[iCell].nonrenew_deficit != rout_var[iCell].nonrenew_deficit){
                log_err("nonrenew_deficit NaN 3");
            }
        }
        
        if(plugin_options.NONRENEW_RUNOFF) {
            if (in_runoff > rout_var[iCell].nonrenew_deficit) {
                in_runoff -= rout_var[iCell].nonrenew_deficit;
                rout_var[iCell].nonrenew_deficit = 0.;
                if(in_runoff != in_runoff){
                    log_err("in_runoff NaN 2");
                }
                if(rout_var[iCell].nonrenew_deficit != rout_var[iCell].nonrenew_deficit){
                    log_err("nonrenew_deficit in_runoff NaN 2");
                }
            } else {
                rout_var[iCell].nonrenew_deficit -= in_runoff;
                in_runoff = 0.;
                if(in_runoff != in_runoff){
                    log_err("in_runoff NaN 3");
                }
                if(rout_var[iCell].nonrenew_deficit != rout_var[iCell].nonrenew_deficit){
                    log_err("nonrenew_deficit in_runoff NaN 3");
                }
            }
        }
    }
    
    runoff =
        (in_runoff + in_baseflow) *
        local_domain.locations[iCell].area /
        (global_param.dt * MM_PER_M);
    if(runoff != runoff){
        log_err("runoff NaN 1");
    }
    
    // Calculate delta-time runoff (equal contribution)
    for (i = 0; i < rout_steps_per_dt; i++) {
        dt_runoff[i] = runoff / rout_steps_per_dt;
        if(dt_runoff[i] != dt_runoff[i]){
            log_err("dt_runoff NaN 1");
        }
    }
    
    // Convolute current runoff
    convolve(dt_runoff, rout_steps_per_dt, 
             rout_con[iCell].runoff_uh, plugin_options.UH_LENGTH,
             convoluted);
    for (i = 0; i < rout_steps_per_dt; i++) {
        if(convoluted[i] != convoluted[i]){
            log_err("convoluted NaN 1");
        }
    }
    for (i = 0; i < rout_steps_per_dt + plugin_options.UH_LENGTH - 1; i++) {
        rout_var[iCell].dt_discharge[i] += convoluted[i];
    }
    for (i = 0; i < rout_steps_per_dt + plugin_options.UH_LENGTH - 1; i++) {
        if(rout_var[iCell].dt_discharge[i] != rout_var[iCell].dt_discharge[i]){
            log_err("dt_discharge NaN 2");
        }
    }

    /* INFLOW*/
    // Gather inflow from forcing
    inflow = 0.0;
    if (plugin_options.FORCE_ROUTING) {
        inflow += rout_force[iCell].discharge;
    }
    if(inflow != inflow){
        log_err("inflow NaN 1");
    }
    
    // Calculate delta-time inflow (equal contribution)
    rout_var[iCell].inflow = 0.;
    for (i = 0; i < rout_steps_per_dt; i++) {
        dt_inflow[i] = inflow / rout_steps_per_dt;
        if(dt_inflow[i] != dt_inflow[i]){
            log_err("dt_inflow NaN 1");
        }
        for (j = 0; j < rout_con[iCell].Nupstream; j++) {
            dt_inflow[i] += rout_var[rout_con[iCell].upstream[j]].dt_discharge[i];
            if(dt_inflow[i] != dt_inflow[i]){
                log_err("dt_inflow NaN 2");
            }
        }
        rout_var[iCell].inflow += dt_inflow[i];
    }
    
    // Convolute current inflow
    convolve(dt_inflow, rout_steps_per_dt, 
            rout_con[iCell].runoff_uh, plugin_options.UH_LENGTH,
            convoluted);
    for (i = 0; i < rout_steps_per_dt; i++) {
        if(convoluted[i] != convoluted[i]){
            log_err("convoluted NaN 1");
        }
    }
    for (i = 0; i < rout_steps_per_dt + plugin_options.UH_LENGTH - 1; i++) {
        rout_var[iCell].dt_discharge[i] += convoluted[i];
    }
    for (i = 0; i < rout_steps_per_dt + plugin_options.UH_LENGTH - 1; i++) {
        if(rout_var[iCell].dt_discharge[i] != rout_var[iCell].dt_discharge[i]){
            log_err("dt_discharge NaN 3");
        }
    }

    // Aggregate current timestep discharge & stream moisture
    prev_stream = rout_var[iCell].stream;
    rout_var[iCell].discharge = 0.0;
    rout_var[iCell].stream = 0.0;
    for (i = 0; i < plugin_options.UH_LENGTH + rout_steps_per_dt - 1; i++) {
        if(rout_var[iCell].dt_discharge[i] != rout_var[iCell].dt_discharge[i]){
            log_err("dt_discharge NaN 4");
        }
        if (i < rout_steps_per_dt) {
            rout_var[iCell].discharge += rout_var[iCell].dt_discharge[i];
        }
        else {
            rout_var[iCell].stream += rout_var[iCell].dt_discharge[i];
        }
    }
    if(rout_var[iCell].discharge != rout_var[iCell].discharge){
        log_err("discharge NaN 1");
    }
    if(rout_var[iCell].stream != rout_var[iCell].stream){
        log_err("stream NaN 1");
    }

    // Check water balance
    if (abs(prev_stream + (rout_var[iCell].inflow + runoff) -
            (rout_var[iCell].discharge + rout_var[iCell].stream)) >
        DBL_EPSILON) {
        log_err("Discharge water balance error [%.4f]. "
                "in: %.4f out: %.4f prev_storage: %.4f cur_storage %.4f",
                prev_stream + (rout_var[iCell].inflow + runoff) -
                (rout_var[iCell].discharge + rout_var[iCell].stream),
                (rout_var[iCell].inflow + runoff),
                rout_var[iCell].discharge,
                prev_stream,
                rout_var[iCell].stream);
    }
    
    /* Free */
    free(dt_inflow);
    free(dt_runoff);
    free(convoluted);
}
