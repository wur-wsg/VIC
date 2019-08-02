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
* @brief   Initialize the irrigation variables
******************************************/
void
initialize_irr_var(irr_var_struct *irr_var)
{
    irr_var->leftover = 0.0;
    irr_var->shortage = 0.0;
    irr_var->requirement = 0.0;
    irr_var->flag_req = false;
}

/******************************************
* @brief   Initialize the irrigation constants
******************************************/
void
initialize_irr_con(irr_con_struct *irr_con)
{
    irr_con->paddy = false;
    irr_con->irr_class = MISSING_USI;
    irr_con->veg_class = MISSING_USI;
    irr_con->veg_index = MISSING_USI;
    irr_con->groundwater_fraction = 0.0;
    irr_con->irrigation_efficiency = 0.0;
}

/******************************************
* @brief   Initialize the irrigation structures
******************************************/
void
irr_initialize_local_structures(void)
{
    extern domain_struct        local_domain;
    extern irr_var_struct          ***irr_var;
    extern irr_con_struct           **irr_con;
    extern irr_con_map_struct        *irr_con_map;
    extern option_struct options;

    size_t                      i;
    size_t                      j;
    size_t                      k;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            initialize_irr_con(&irr_con[i][j]);
            for(k = 0; k < options.SNOW_BAND; k++){
                initialize_irr_var(&(irr_var[i][j][k]));
            }
        }
    }
}
