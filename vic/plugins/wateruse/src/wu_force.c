/******************************************************************************
 * @section DESCRIPTION
 *
 * Force water-use demand
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
* @brief   Force water-use demand
******************************************/
void
wu_forcing(void)
{
    extern domain_struct local_domain;
    extern plugin_global_param_struct plugin_global_param;
    extern domain_struct global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern wu_force_struct **wu_force;
    
    double *dvar;
    int demand_idx;
    int groundwater_idx;
    int consumption_idx;
    
    size_t  d3count[3];
    size_t  d3start[3];
    
    size_t i;
    size_t k;
    int iSector;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    // Get forcing data
    for(k = 0; k < plugin_options.NWUTYPES; k++){
        if(plugin_options.WU_INPUT[k] == WU_FROM_FILE){
            if(k == WU_IRRIGATION){
                demand_idx = FORCING_IRR_DEMAND;
                groundwater_idx = FORCING_IRR_GROUNDWATER;
                consumption_idx = FORCING_IRR_CONSUMPTION;
            } else if (k == WU_MUNICIPAL){
                demand_idx = FORCING_MUN_DEMAND;
                groundwater_idx = FORCING_MUN_GROUNDWATER;
                consumption_idx = FORCING_MUN_CONSUMPTION;
            } else if (k == WU_LIVESTOCK){
                demand_idx = FORCING_LIV_DEMAND;
                groundwater_idx = FORCING_LIV_GROUNDWATER;
                consumption_idx = FORCING_LIV_CONSUMPTION;
            } else if (k == WU_ENERGY){
                demand_idx = FORCING_ENE_DEMAND;
                groundwater_idx = FORCING_ENE_GROUNDWATER;
                consumption_idx = FORCING_ENE_CONSUMPTION;
            } else if (k == WU_MANUFACTURING){
                demand_idx = FORCING_MAN_DEMAND;
                groundwater_idx = FORCING_MAN_GROUNDWATER;
                consumption_idx = FORCING_MAN_CONSUMPTION;
            } else {
                log_err("Unknown water-use sector");
            }
            
            d3start[0] = plugin_global_param.forceskip[demand_idx] +
                         plugin_global_param.forceoffset[demand_idx];

            if (plugin_global_param.forcerun[demand_idx]) {
                get_scatter_nc_field_double(&(plugin_filenames.forcing[demand_idx]), 
                    plugin_filenames.f_varname[demand_idx], d3start, d3count, dvar);

                for (i = 0; i < local_domain.ncells_active; i++) {
                    iSector = wu_con_map[i].sidx[k];

                    if(iSector != NODATA_WU){
                        wu_force[i][iSector].demand = dvar[i];
                    }
                }
            }
            
            d3start[0] = plugin_global_param.forceskip[groundwater_idx] +
                         plugin_global_param.forceoffset[groundwater_idx];

            if (plugin_global_param.forcerun[groundwater_idx]) {
                get_scatter_nc_field_double(&(plugin_filenames.forcing[groundwater_idx]), 
                    plugin_filenames.f_varname[groundwater_idx], d3start, d3count, dvar);

                for (i = 0; i < local_domain.ncells_active; i++) {
                    iSector = wu_con_map[i].sidx[k];

                    if(iSector != NODATA_WU){
                        wu_force[i][iSector].groundwater_frac = dvar[i];
                    }
                }
            }
            
            d3start[0] = plugin_global_param.forceskip[consumption_idx] +
                         plugin_global_param.forceoffset[consumption_idx];

            if (plugin_global_param.forcerun[consumption_idx]) {
                get_scatter_nc_field_double(&(plugin_filenames.forcing[consumption_idx]), 
                    plugin_filenames.f_varname[consumption_idx], d3start, d3count, dvar);

                for (i = 0; i < local_domain.ncells_active; i++) {
                    iSector = wu_con_map[i].sidx[k];

                    if(iSector != NODATA_WU){
                        wu_force[i][iSector].consumption_frac = dvar[i];
                    }
                }
            }
        }
    }
    
    free(dvar);
}
