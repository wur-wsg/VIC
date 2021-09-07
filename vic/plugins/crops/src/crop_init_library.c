/******************************************************************************
 * @section DESCRIPTION
 *
 * Irrigation initialization functions
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
* @brief   Initialize the crop constants
******************************************/
void
initialize_crop_con(crop_con_struct *crop_con)
{
    extern plugin_option_struct plugin_options;

    size_t                      i;

    crop_con->tsum1 = 0.;
    crop_con->tsum2 = 0.;
    for (i = 0; i < plugin_options.NFERTTIMES; i++) {
        crop_con->DVS_point[i] = 0.;
        crop_con->N_amount[i] = 0.;
        crop_con->P_amount[i] = 0.;
        crop_con->K_amount[i] = 0.;
    }
}

/******************************************
* @brief   Initialize the irrigation constants
******************************************/
void
initialize_crop_force(crop_force_struct *crop_force,
                      size_t             ncrops)
{
    extern plugin_option_struct plugin_options;

    size_t                      iCrop;
    size_t                      iFert;

    for (iCrop = 0; iCrop < ncrops; iCrop++) {
        crop_force->tsum1[iCrop] = 0.;
        crop_force->tsum2[iCrop] = 0.;
        for (iFert = 0; iFert < plugin_options.NFERTTIMES; iFert++) {
            crop_force->DVS_point[iCrop][iFert] = 0.;
            crop_force->N_amount[iCrop][iFert] = 0.;
            crop_force->P_amount[iCrop][iFert] = 0.;
            crop_force->K_amount[iCrop][iFert] = 0.;
        }
    }
}

/******************************************
* @brief   Initialize the irrigation structures
******************************************/
void
crop_initialize_local_structures(void)
{
    extern domain_struct        local_domain;
    extern crop_con_struct    **crop_con;
    extern crop_con_map_struct *crop_con_map;
    extern crop_force_struct   *crop_force;

    size_t                      i;
    size_t                      j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_crop_force(&crop_force[i], crop_con_map[i].nc_active);
        for (j = 0; j < crop_con_map[i].nc_active; j++) {
            initialize_crop_con(&crop_con[i][j]);
        }
    }
}
