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

    plugin_force_start();
    if (plugin_options.ROUTING && plugin_options.FORCE_ROUTING) {
        rout_forcing();
    }
    plugin_force_end();
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

    if (plugin_options.ROUTING) {
        if (plugin_options.DECOMPOSITION == BASIN_DECOMPOSITION ||
            plugin_options.DECOMPOSITION == FILE_DECOMPOSITION) {
            for (i = 0; i < local_domain.ncells_active; i++) {
                iCell = routing_order[i];

                rout_basin_run(iCell);
            }
        }
        else if (plugin_options.DECOMPOSITION == RANDOM_DECOMPOSITION) {
            rout_random_run();
        }
    }
}

/******************************************
* @brief    Write plugins
******************************************/
void
plugin_put_data()
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
}
