/******************************************************************************
 * @section DESCRIPTION
 *
 * Environmental requirement initialization functions
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
* @brief   Initialize the environmental requirement forcing
******************************************/
void
initialize_efr_force(efr_force_struct   *efr_force,
                     veg_con_map_struct *veg_con_map)
{
    extern option_struct options;

    size_t               i;
    size_t               j;

    efr_force->discharge = 0.0;
    efr_force->baseflow = 0.0;

    for (i = 0; i < veg_con_map->nv_active; i++) {
        for (j = 0; j < options.SNOW_BAND; j++) {
            efr_force->moist[i][j] = 0.0;
        }
    }
}

/******************************************
* @brief   Initialize the environmental requirement structures
******************************************/
void
efr_initialize_local_structures(void)
{
    extern domain_struct       local_domain;
    extern efr_force_struct   *efr_force;
    extern veg_con_map_struct *veg_con_map;

    size_t                     i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_efr_force(&(efr_force[i]), &(veg_con_map[i]));
    }
}
