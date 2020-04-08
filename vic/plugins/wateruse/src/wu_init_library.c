/******************************************************************************
 * @section DESCRIPTION
 *
 * Water-use initialization functions
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
* @brief   Initialize the water-use forcing
******************************************/
void
initialize_wu_force(wu_force_struct *wu_force)
{
    wu_force->pumping_capacity = 0.0;
    wu_force->consumption_frac = 0.0;
    wu_force->groundwater_frac = 0.0;
    wu_force->demand = 0.0;
}

/******************************************
* @brief   Initialize the water-use constants
******************************************/
void
initialize_wu_var(wu_var_struct *wu_var)
{
    wu_var->available_gw = 0.0;
    wu_var->available_surf = 0.0;
    wu_var->available_dam = 0.0;
    wu_var->available_comp = 0.0;
    wu_var->available_remote = 0.0;
    wu_var->demand_gw = 0.0;
    wu_var->demand_surf = 0.0;
    wu_var->demand_comp = 0.0;
    wu_var->demand_remote = 0.0;
    wu_var->withdrawn_gw = 0.0;
    wu_var->withdrawn_surf = 0.0;
    wu_var->withdrawn_dam = 0.0;
    wu_var->withdrawn_comp = 0.0;
    wu_var->withdrawn_remote = 0.0;
    wu_var->withdrawn_nonrenew = 0.0;
    wu_var->returned = 0.0;
    wu_var->consumed = 0.0;
}

/******************************************
* @brief   Initialize the water-use variables
******************************************/
void
initialize_wu_con(wu_con_struct *wu_con)
{
    size_t               i;
    
    wu_con->pumping_capacity = 0.0;
    for(i = 0; i < wu_con->nreceiving; i++){
        wu_con->receiving[i] = MISSING_USI;
    }
}

/******************************************
* @brief   Initialize the water-use structures
******************************************/
void
wu_initialize_local_structures(void)
{
    extern domain_struct    local_domain;
    extern plugin_option_struct plugin_options;
    extern wu_con_map_struct *wu_con_map;
    extern wu_var_struct **wu_var;
    extern wu_con_struct *wu_con;
    extern wu_force_struct **wu_force;

    size_t                  i;
    size_t                  j;
    int                  iSector;

    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_wu_con(&wu_con[i]);
        
        for(j = 0; j < plugin_options.NWUTYPES; j++){
            iSector = wu_con_map[i].sidx[j];
            if(iSector != NODATA_WU){
                initialize_wu_var(&(wu_var[i][iSector]));
                initialize_wu_force(&(wu_force[i][iSector]));
            }
        }
    }
}
