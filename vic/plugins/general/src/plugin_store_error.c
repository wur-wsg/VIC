/******************************************************************************
 * @section DESCRIPTION
 *
 * Functions to initialize and broadcast plugin structures
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
* @brief    Store plugin error
******************************************/
void
plugin_store_error(size_t iCell)
{
    extern global_param_struct      global_param;
    extern plugin_option_struct     plugin_options;
    extern domain_struct            local_domain;
    extern plugin_save_data_struct *plugin_save_data;
    extern save_data_struct        *save_data;
    extern double                ***out_data;
    extern option_struct            options;

    double                          inflow;
    double                          outflow;
    double                          storage;

    size_t                          i;

    // OUT_ROUTING_ERROR contains all plugin water states: river streamflow,
    // non-renewable storage, dam reservoir storage, and irrigation leftovers

    // Inflow
    inflow = 0.;
    if (plugin_options.ROUTING) {
        inflow += (out_data[iCell][N_OUTVAR_TYPES + OUT_STREAM_RUNOFF][0] +
                   out_data[iCell][N_OUTVAR_TYPES + OUT_STREAM_INFLOW][0]) *
                  global_param.dt / local_domain.locations[iCell].area *
                  MM_PER_M;
    }
    if (plugin_options.DAMS) {
        inflow += out_data[iCell][N_OUTVAR_TYPES + OUT_LDAM_INFLOW][0] *
                  M3_PER_HM3 / local_domain.locations[iCell].area * MM_PER_M;
    }
    if (plugin_options.IRRIGATION) {
        inflow += out_data[iCell][N_OUTVAR_TYPES + OUT_RECEIVED][0];
    }
    if (plugin_options.WATERUSE) {
        inflow += out_data[iCell][N_OUTVAR_TYPES + OUT_RETURNED][0];
    }

    // Outflow
    outflow = 0.;
    if (plugin_options.ROUTING) {
        outflow += out_data[iCell][N_OUTVAR_TYPES + OUT_DISCHARGE][0] *
                   global_param.dt / local_domain.locations[iCell].area *
                   MM_PER_M;
    }
    if (plugin_options.DAMS) {
        outflow += out_data[iCell][N_OUTVAR_TYPES + OUT_LDAM_RELEASE][0] *
                   M3_PER_HM3 / local_domain.locations[iCell].area * MM_PER_M;
    }
    if (plugin_options.IRRIGATION) {
        outflow += out_data[iCell][N_OUTVAR_TYPES + OUT_APPLIED][0];
    }
    if (plugin_options.WATERUSE && options.GWM == false) {
        for (i = 0; i < WU_NSECTORS; i++) {
            outflow += out_data[iCell][N_OUTVAR_TYPES + OUT_WI_SURF_SECT][i] +
                       out_data[iCell][N_OUTVAR_TYPES + OUT_WI_DAM_SECT][i] +
                       out_data[iCell][N_OUTVAR_TYPES + OUT_WI_REM_SECT][i] +
                       out_data[iCell][N_OUTVAR_TYPES + OUT_WI_NREN_SECT][i];
        }
    }
    if (plugin_options.WATERUSE && options.GWM == true) {
        for (i = 0; i < WU_NSECTORS; i++) {
            outflow += out_data[iCell][N_OUTVAR_TYPES + OUT_WI_SURF_SECT][i] +
                       out_data[iCell][N_OUTVAR_TYPES + OUT_WI_DAM_SECT][i] +
                       out_data[iCell][N_OUTVAR_TYPES + OUT_WI_REM_SECT][i];
        }
    }

    // Storage
    storage = 0.;
    if (plugin_options.ROUTING ||
        (plugin_options.WATERUSE && plugin_options.NONRENEW_WITH)) {
            if(options.GWM){
                storage += out_data[iCell][N_OUTVAR_TYPES + OUT_STREAM_MOIST][0];
            } else{
                storage +=out_data[iCell][N_OUTVAR_TYPES + OUT_STREAM_MOIST][0] - 
                out_data[iCell][N_OUTVAR_TYPES + OUT_NONREN_DEFICIT][0];
            }
    }
    if (plugin_options.DAMS) {
        storage += (out_data[iCell][N_OUTVAR_TYPES + OUT_GDAM_STORAGE][0] +
                    out_data[iCell][N_OUTVAR_TYPES + OUT_LDAM_STORAGE][0]) *
                   M3_PER_HM3 / local_domain.locations[iCell].area * MM_PER_M;
    }
    if (plugin_options.IRRIGATION) {
        storage += out_data[iCell][N_OUTVAR_TYPES + OUT_LEFTOVER][0];
    }

    out_data[iCell][N_OUTVAR_TYPES + OUT_ROUTING_ERROR][0] = \
        calc_water_balance_error(inflow,
                                 outflow,
                                 storage,
                                 plugin_save_data[iCell].total_moist_storage);

    plugin_save_data[iCell].total_moist_storage = storage;

    // Adapt variables for OUT_WATER_ERROR
    if (plugin_options.IRRIGATION) {
        save_data[iCell].total_moist_storage +=
            out_data[iCell][N_OUTVAR_TYPES + OUT_APPLIED][0];
        save_data[iCell].total_soil_moist +=
            out_data[iCell][N_OUTVAR_TYPES + OUT_APPLIED][0];
    }
    if (plugin_options.WATERUSE) {
        for (i = 0; i < WU_NSECTORS; i++) {
            if(options.GWM == false){
                save_data[iCell].total_soil_moist -=
                out_data[iCell][N_OUTVAR_TYPES + OUT_WI_GW_SECT][i];
                save_data[iCell].total_moist_storage -=
                out_data[iCell][N_OUTVAR_TYPES + OUT_WI_GW_SECT][i];
            }
            else{
                continue;

            }
            
        }
    }
}
