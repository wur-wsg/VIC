/******************************************************************************
 * @section DESCRIPTION
 *
 * Populate dams
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
* @brief   Generate default dam state
******************************************/
void
dam_generate_default_state(void)
{
    extern domain_struct            local_domain;
    extern plugin_option_struct     plugin_options;
    extern dam_con_map_struct      *local_dam_con_map;
    extern dam_con_struct         **local_dam_con;
    extern dam_var_struct         **local_dam_var;
    extern dam_con_map_struct      *global_dam_con_map;
    extern dam_con_struct         **global_dam_con;
    extern dam_var_struct         **global_dam_var;
    extern plugin_parameters_struct plugin_param;

    size_t                          i;
    size_t                          j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < plugin_options.NDAMTYPES; j++) {
            if (local_dam_con_map[i].didx[j] != NODATA_DAM) {
                local_dam_var[i][j].storage = local_dam_con[i][j].capacity * 0;
            }
            if (global_dam_con_map[i].didx[j] != NODATA_DAM) {
                global_dam_var[i][j].storage = global_dam_con[i][j].capacity *
                                               plugin_param.DAM_ALPHA;
            }
        }
    }
}
