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
    extern global_param_struct global_param;
    extern domain_struct        local_domain;
    extern plugin_save_data_struct *plugin_save_data;
    extern save_data_struct *save_data;
    extern double           ***out_data;
    
    double inflow;
    double outflow;
    double storage;
    
    inflow = out_data[iCell][OUT_RUNOFF][0] + out_data[iCell][OUT_BASEFLOW][0];
    inflow += out_data[iCell][N_OUTVAR_TYPES + OUT_STREAM_INFLOW][0] * 
            global_param.dt / local_domain.locations[iCell].area * MM_PER_M;
    inflow += out_data[iCell][N_OUTVAR_TYPES + OUT_RETURNED][0];
    
    outflow = out_data[iCell][N_OUTVAR_TYPES + OUT_DISCHARGE][0] * 
            global_param.dt / local_domain.locations[iCell].area * MM_PER_M;
    outflow += out_data[iCell][N_OUTVAR_TYPES + OUT_WI_SURF][0] +
            out_data[iCell][N_OUTVAR_TYPES + OUT_WI_COMP][0] +
            out_data[iCell][N_OUTVAR_TYPES + OUT_WI_REM][0] +
            out_data[iCell][N_OUTVAR_TYPES + OUT_WI_NREN][0];
    
    /* NOTE: local dams are not included in the routing error, since they modify runoff*/
    storage = out_data[iCell][N_OUTVAR_TYPES + OUT_STREAM_MOIST][0] - 
            out_data[iCell][N_OUTVAR_TYPES + OUT_NONREN_DEFICIT][0];
    storage += out_data[iCell][N_OUTVAR_TYPES + OUT_GDAM_STORAGE][0] * 
            M3_PER_HM3 / local_domain.locations[iCell].area * MM_PER_M;
        
    out_data[iCell][N_OUTVAR_TYPES + OUT_ROUTING_ERROR][0] = \
            calc_water_balance_error(inflow, 
                                     outflow, 
                                     storage, 
                                     plugin_save_data[iCell].total_moist_storage);
    
    plugin_save_data[iCell].total_moist_storage = storage;
    
    save_data[iCell].total_soil_moist -= out_data[iCell][N_OUTVAR_TYPES + OUT_WI_GW][0];
    save_data[iCell].total_moist_storage -= out_data[iCell][N_OUTVAR_TYPES + OUT_WI_GW][0];
    save_data[iCell].total_soil_moist += out_data[iCell][N_OUTVAR_TYPES + OUT_APPLIED][0];
    save_data[iCell].total_moist_storage += out_data[iCell][N_OUTVAR_TYPES + OUT_APPLIED][0];
}
