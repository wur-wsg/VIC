/******************************************************************************
 * @section DESCRIPTION
 *
 * Land-use initialization functions
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
* @brief   Initialize the land-use forcing
******************************************/
void
initialize_lu_force(lu_force_struct *lu_force,
                    size_t           nveg)
{
    size_t i;

    for (i = 0; i < nveg; i++) {
        lu_force->Cv[i] = 0.0;
        lu_force->veg_class[i] = 0.0;
    }
}

/******************************************
* @brief   Initialize the land-use structures
******************************************/
void
lu_initialize_local_structures(void)
{
    extern domain_struct       local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern lu_force_struct    *lu_force;

    size_t                     i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_lu_force(&lu_force[i], veg_con_map[i].nv_active);
    }
}
