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
    double                            inflow_wb;
    double                            dt_inflow;
    double                            runoff;
    double                            dt_runoff;
    size_t                            rout_steps_per_dt;
    double                            prev_stream;

    size_t                            i;
    size_t                            j;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;
    
    /* Shift and clear previous discharge data */
    for (i = 0; i < rout_steps_per_dt; i++) {
        rout_var[iCell].dt_discharge[0] = 0.0;
        cshift(rout_var[iCell].dt_discharge,
               plugin_options.UH_LENGTH + rout_steps_per_dt + 1, 1, 0, 1);
    }
    
    /* RUNOFF*/
    // Gather runoff from VIC
    in_runoff = out_data[iCell][OUT_RUNOFF][0];
    in_baseflow = out_data[iCell][OUT_BASEFLOW][0];
    if (in_baseflow > rout_var[iCell].nonrenew_deficit) {
        in_baseflow -= rout_var[iCell].nonrenew_deficit;
        rout_var[iCell].nonrenew_deficit = 0.;
    } else {
        rout_var[iCell].nonrenew_deficit -= in_baseflow;
        in_baseflow = 0.;
    }
    runoff =
        (in_runoff + in_baseflow) *
        local_domain.locations[iCell].area /
        (global_param.dt * MM_PER_M);
    // Calculate delta-time runoff (equal contribution)
    dt_runoff = runoff / rout_steps_per_dt;
    // Convolute current runoff
    for (i = 0; i < rout_steps_per_dt; i++) {
        convolute(dt_runoff, rout_con[iCell].runoff_uh,
                  rout_var[iCell].dt_discharge,
                  plugin_options.UH_LENGTH, i);
    }

    /* INFLOW*/
    // Gather inflow from forcing
    inflow = 0.0;
    if (plugin_options.FORCE_ROUTING) {
        inflow += rout_force[iCell].discharge;
    }
    // Calculate delta-time inflow (equal contribution)
    dt_inflow = inflow / rout_steps_per_dt;
    // Convolute current inflow
    inflow_wb = 0.;
    for (i = 0; i < rout_steps_per_dt; i++) {
        
        for (j = 0; j < rout_con[iCell].Nupstream; j++) {
            dt_inflow += rout_var[rout_con[iCell].upstream[j]].dt_discharge[i];
        }
        
        inflow_wb += dt_inflow;
        convolute(dt_inflow, rout_con[iCell].inflow_uh,
                  rout_var[iCell].dt_discharge,
                  plugin_options.UH_LENGTH, i);
    }
    
    // Aggregate current timestep discharge & stream moisture
    rout_var[iCell].discharge = 0.0;
    prev_stream = rout_var[iCell].stream;
    rout_var[iCell].stream = 0.0;
    for (i = 0; i < plugin_options.UH_LENGTH + rout_steps_per_dt + 1; i++) {
        if (i < rout_steps_per_dt) {
            rout_var[iCell].discharge += rout_var[iCell].dt_discharge[i];
        }
        else {
            rout_var[iCell].stream += rout_var[iCell].dt_discharge[i];
        }
    }

    // Check water balance
    if (abs(prev_stream + (inflow_wb + runoff) -
            (rout_var[iCell].discharge + rout_var[iCell].stream)) >
        DBL_EPSILON) {
        log_err("Discharge water balance error [%.4f]. "
                "in: %.4f out: %.4f prev_storage: %.4f cur_storage %.4f",
                prev_stream + (inflow + runoff) -
                (rout_var[iCell].discharge + rout_var[iCell].stream),
                (inflow + runoff),
                rout_var[iCell].discharge,
                prev_stream,
                rout_var[iCell].stream);
    }
}
