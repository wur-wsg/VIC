/******************************************************************************
 * @section DESCRIPTION
 *
 * Run dams
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
* @brief   Operate inflow, storage and release
******************************************/
void
dam_operate(dam_con_struct *dam_con,
            dam_var_struct *dam_var)
{
    double prev_storage; //reservoir storage at the previous time step

    // Fill reservoir
    prev_storage = dam_var->storage;
    dam_var->storage += dam_var->inflow;

    // Calculate discharge
    dam_var->release = dam_var->op_release[0];
    dam_var->release = dam_corr_release(dam_var->op_release[0],
                                        dam_var->storage,
                                        dam_con->capacity);

    // reduce reservoir
    dam_var->storage -= dam_var->release;

    // Empty reservoir
    if (dam_var->storage < 0) {
        dam_var->release += dam_var->storage;
        dam_var->storage = 0.0;
    }

    // Full reservoir
    if (dam_var->storage > dam_con->capacity) {
        dam_var->release += dam_var->storage - dam_con->capacity;
        dam_var->storage = dam_con->capacity;
    }

    if (abs(dam_var->storage - prev_storage -
            dam_var->inflow + dam_var->release) > DBL_EPSILON) {
        log_err("Dam operation error:\n"
                "Prev storage [%.4f]\tStorage [%.4f]\t"
                "Inflow [%.4f]\tRelease [%.4f]\t",
                prev_storage, dam_var->storage,
                dam_var->inflow, dam_var->release);
    }
}

/******************************************
* @brief   Operate local dam
******************************************/
void
local_dam_operate(dam_con_struct *dam_con,
                  dam_var_struct *dam_var,
                  size_t          iCell)
{
    extern domain_struct local_domain;
    extern double     ***out_data;

    dam_operate(dam_con, dam_var);
     //update the runoff by substracting the water intercept by the dam 
    //or add the water released from the dam to the runoff. 

    out_data[iCell][OUT_RUNOFF][0] *= 1 - dam_con->inflow_frac;
    out_data[iCell][OUT_RUNOFF][0] += dam_var->release * M3_PER_HM3 *
                                      MM_PER_M /
                                      local_domain.locations[iCell].area;
}

/******************************************
* @brief   Operate global dam
******************************************/
void
global_dam_operate(dam_con_struct *dam_con,
                   dam_var_struct *dam_var,
                   size_t          iCell)
{
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern rout_var_struct           *rout_var;

    size_t                            rout_steps_per_dt;
    size_t                            i;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    dam_operate(dam_con, dam_var);

    for (i = 0; i < rout_steps_per_dt; i++) {
        rout_var[iCell].dt_discharge[i] *= 1 - dam_con->inflow_frac;
        rout_var[iCell].dt_discharge[i] += dam_var->release * M3_PER_HM3 /
                                           global_param.dt / rout_steps_per_dt;

        if (rout_var[iCell].dt_discharge[i] < 0) {
            rout_var[iCell].dt_discharge[i] = 0.;
        }
    }

    rout_var[iCell].discharge = 0.0;
    for (i = 0; i < rout_steps_per_dt; i++) {
        rout_var[iCell].discharge += rout_var[iCell].dt_discharge[i];
    }
}

/******************************************
* @brief   Run local dam
******************************************/
void
local_dam_run(size_t iCell)
{
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct    **dam_con;
    extern dam_var_struct    **dam_var;

    size_t                     iDam;

    for (iDam = 0; iDam < dam_con_map[iCell].nd_active; iDam++) {
        if (dam_con[iCell][iDam].type == DAM_LOCAL) {
            local_dam_register(&dam_con[iCell][iDam],
                               &dam_var[iCell][iDam], iCell);

            if (dam_var[iCell][iDam].active) {
                local_dam_operate(&dam_con[iCell][iDam],
                                  &dam_var[iCell][iDam], iCell);
            }
            else {
                dam_var[iCell][iDam].inflow = 0.;
                dam_var[iCell][iDam].demand = 0.;
                dam_var[iCell][iDam].efr = 0.;
            }
        }
    }
}

/******************************************
* @brief   Run global dam
******************************************/
void
global_dam_run(size_t iCell)
{
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct    **dam_con;
    extern dam_var_struct    **dam_var;

    size_t                     iDam;

    for (iDam = 0; iDam < dam_con_map[iCell].nd_active; iDam++) {
        if (dam_con[iCell][iDam].type == DAM_GLOBAL) {
            global_dam_register(&dam_con[iCell][iDam],
                                &dam_var[iCell][iDam], iCell);

            if (dam_var[iCell][iDam].active) {
                global_dam_operate(&dam_con[iCell][iDam],
                                   &dam_var[iCell][iDam], iCell);
            }
            else {
                dam_var[iCell][iDam].inflow = 0.;
                dam_var[iCell][iDam].demand = 0.;
                dam_var[iCell][iDam].efr = 0.;
            }
        }
    }
}
