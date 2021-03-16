/******************************************************************************
 * @section DESCRIPTION
 *
 * Environmental requirements allocate and finalize functions
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
* @brief   Allocate environmental requirement module
******************************************/
void
efr_alloc(void)
{
    extern domain_struct       local_domain;
    extern option_struct       options;
    extern veg_con_map_struct *veg_con_map;
    extern efr_force_struct   *efr_force;

    size_t                     i;
    size_t                     j;

    efr_force = malloc(local_domain.ncells_active * sizeof(*efr_force));
    check_alloc_status(efr_force, "Memory allocation error");

    for (i = 0; i < local_domain.ncells_active; i++) {
        efr_force[i].moist =
            malloc(veg_con_map[i].nv_active * sizeof(*efr_force[i].moist));
        check_alloc_status(efr_force[i].moist,
                           "Memory allocation error");

        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            efr_force[i].moist[j] =
                malloc(options.SNOW_BAND * sizeof(*efr_force[i].moist[j]));
            check_alloc_status(efr_force[i].moist[j],
                               "Memory allocation error");
        }
    }

    efr_initialize_local_structures();
}

/******************************************
* @brief   Finalize environmental requirement module
******************************************/
void
efr_finalize(void)
{
    extern domain_struct       local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern efr_force_struct   *efr_force;

    size_t                     i;
    size_t                     j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            free(efr_force[i].moist[j]);
        }
        free(efr_force[i].moist);
    }
    free(efr_force);
}
