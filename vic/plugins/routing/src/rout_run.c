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
* @brief   Run runoff routing on local node
******************************************/
void
rout_run(size_t iCell)
{
    extern domain_struct        local_domain;
    extern global_param_struct  global_param;
    extern plugin_option_struct plugin_options;
    extern rout_var_struct     *rout_var;
    extern double            ***out_data;
    extern option_struct        options;
    double                      in_runoff;
    double                      in_baseflow;

    /* RUNOFF*/
    // Gather runoff from VIC
    in_runoff = out_data[iCell][OUT_RUNOFF][0];
    if (options.GWM){
        in_baseflow = 0;
    }else{
        in_baseflow = out_data[iCell][OUT_BASEFLOW][0];
    }
    
    if (rout_var[iCell].nonrenew_deficit > 0 && options.GWM == false) {
        if (in_baseflow > rout_var[iCell].nonrenew_deficit) {
            in_baseflow -= rout_var[iCell].nonrenew_deficit;
            rout_var[iCell].nonrenew_deficit = 0.;
        }
        else {
            rout_var[iCell].nonrenew_deficit -= in_baseflow;
            in_baseflow = 0.;
        }

        if (plugin_options.NONRENEW_RUNOFF ) {
            if (in_runoff > rout_var[iCell].nonrenew_deficit) {
                in_runoff -= rout_var[iCell].nonrenew_deficit;
                rout_var[iCell].nonrenew_deficit = 0.;
            }
            else {
                rout_var[iCell].nonrenew_deficit -= in_runoff;
                in_runoff = 0.;
            }
        }
    }

    rout_var[iCell].runoff =
        (in_runoff + in_baseflow) *
        local_domain.locations[iCell].area /
        (global_param.dt * MM_PER_M);
}
