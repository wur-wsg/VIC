/******************************************************************************
 * @section DESCRIPTION
 *
 * Plugin simulation functions
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
* @brief    Force plugins
******************************************/
void
plugin_force(void)
{
    extern plugin_option_struct plugin_options;

    plugin_start_forcing();
    if (plugin_options.ROUTING && plugin_options.FORCE_ROUTING) {
        rout_forcing();
    }
    if (plugin_options.EFR) {
        efr_forcing();
    }
    if (plugin_options.WATERUSE) {
        wu_forcing();
    }
    plugin_end_forcing();
}

/******************************************
* @brief    Run plugins
******************************************/
void
plugin_run(void)
{
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;
    extern size_t              *routing_order;

    size_t                      i;
    size_t                      iCell;

    // If running with OpenMP, run this for loop using multiple threads
    #pragma omp parallel for default(shared) private(i)
    for (i = 0; i < local_domain.ncells_active; i++) {
    	if (plugin_options.IRRIGATION) {
            irr_run_requirement(i);
            if (plugin_options.WATERUSE) {
                irr_set_demand(i);
            }
        }
    }
    
    if (plugin_options.ROUTING) {
        if (plugin_options.DECOMPOSITION == BASIN_DECOMPOSITION ||
            plugin_options.DECOMPOSITION == FILE_DECOMPOSITION) {
            for (i = 0; i < local_domain.ncells_active; i++) {
                iCell = routing_order[i];
		
                if (plugin_options.DAMS) {
                    local_dam_run(iCell);
                }
                rout_basin_run(iCell);
                if (plugin_options.WATERUSE) {
                    wu_run(iCell);
        	}
                if (plugin_options.DAMS) {
                    global_dam_run(iCell);
                }
            }
        }
        else if (plugin_options.DECOMPOSITION == RANDOM_DECOMPOSITION) {
            rout_random_run();
        }
    }
    
    // If running with OpenMP, run this for loop using multiple threads
    #pragma omp parallel for default(shared) private(i)
    for (i = 0; i < local_domain.ncells_active; i++) {
    	if (plugin_options.IRRIGATION) {
            if(plugin_options.POTENTIAL_IRRIGATION || plugin_options.WATERUSE){
                irr_get_withdrawn(i);
            }
            irr_run_shortage(i);
        }
    }
}

/******************************************
* @brief    Store plugin error
******************************************/
void
plugin_store_error(size_t iCell)
{
    extern global_param_struct global_param;
    extern domain_struct        local_domain;
    extern plugin_save_data_struct *plugin_save_data;
    extern double           ***out_data;
    
    double inflow;
    double outflow;
    double storage;
    
    inflow = out_data[iCell][OUT_RUNOFF][0] + out_data[iCell][OUT_BASEFLOW][0];
    inflow += out_data[iCell][N_OUTVAR_TYPES + OUT_STREAM_INFLOW][0] * 
            global_param.dt / local_domain.locations[iCell].area * MM_PER_M;
    
    outflow = out_data[iCell][N_OUTVAR_TYPES + OUT_DISCHARGE][0] * 
            global_param.dt / local_domain.locations[iCell].area * MM_PER_M;
    
    storage = out_data[iCell][N_OUTVAR_TYPES + OUT_STREAM_MOIST][0] - 
            out_data[iCell][N_OUTVAR_TYPES + OUT_NONREN_DEFICIT][0];
    storage += (out_data[iCell][N_OUTVAR_TYPES + OUT_GDAM_STORAGE][0] +
               out_data[iCell][N_OUTVAR_TYPES + OUT_LDAM_STORAGE][0]) * 
            M3_PER_HM3 / local_domain.locations[iCell].area * MM_PER_M;
        
    out_data[iCell][N_OUTVAR_TYPES + OUT_ROUTING_ERROR][0] = \
            calc_water_balance_error(inflow, 
                                     outflow, 
                                     storage, 
                                     plugin_save_data[iCell].total_moist_storage);
    
    plugin_save_data[iCell].total_moist_storage = storage;
}

/******************************************
* @brief    Write plugins
******************************************/
void
plugin_put_data(void)
{
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;

    size_t                      i;

    // If running with OpenMP, run this for loop using multiple threads
    #pragma omp parallel for default(shared) private(i)
    for (i = 0; i < local_domain.ncells_active; i++) {
        if (plugin_options.ROUTING) {
            rout_put_data(i);
        }
        if (plugin_options.EFR) {
            efr_put_data(i);
        }
        if (plugin_options.DAMS) {
            dam_put_data(i);
        }
        if (plugin_options.WATERUSE) {
            wu_put_data(i);
        }
        if (plugin_options.IRRIGATION) {
            irr_put_data(i);
        }
        
        plugin_store_error(i);
    }
}

/******************************************
* @brief    Store plugins
******************************************/
void
plugin_store(nc_file_struct *state_file)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.ROUTING) {
        rout_store(state_file);
    }
    if (plugin_options.DAMS) {
        log_warn("DAM state restore not implemented yet...");
    }
}
