/******************************************************************************
 * @section DESCRIPTION
 *
 * Dam initialization functions
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
* @brief   Initialize the dam variables
******************************************/
void
initialize_dam_var(dam_var_struct *dam_var)
{
    size_t i;

    dam_var->active = false;
    dam_var->inflow = 0.0;
    dam_var->demand = 0.0;
    dam_var->efr = 0.0;
    dam_var->release = 0.0;
    dam_var->storage = 0.0;

    dam_var->total_inflow = 0.0;
    dam_var->total_demand = 0.0;
    dam_var->total_efr = 0.0;
    dam_var->register_steps = 0;

    for (i = 0; i < MONTHS_PER_YEAR * DAM_HIST_YEARS; i++) {
        dam_var->history_inflow[i] = 0.0;
        dam_var->history_demand[i] = 0.0;
        dam_var->history_efr[i] = 0.0;
    }
    for (i = 0; i < MONTHS_PER_YEAR; i++) {
        dam_var->op_release[i] = 0.0;
        dam_var->op_storage[i] = 0.0;
    }

    dam_var->op_month = NODATA_DAM;
    dam_var->months_running = 0;
}

/******************************************
* @brief   Initialize the dam constants
******************************************/
void
initialize_dam_con(dam_con_struct *dam_con)
{
    size_t i;

    dam_con->dam_class = MISSING_USI;
    dam_con->type = -1;
    dam_con->year = 0;
    dam_con->capacity = 0;
    dam_con->inflow_frac = 0.0;

    for (i = 0; i < dam_con->nservice; i++) {
        dam_con->service[i] = MISSING_USI;
        dam_con->service_frac[i] = 0.0;
    }
}

/******************************************
* @brief   Initialize the dam structures
******************************************/
void
dam_initialize_local_structures(void)
{
    extern domain_struct        local_domain;
    extern dam_con_map_struct  *dam_con_map;
    extern dam_con_struct     **dam_con;
    extern dam_var_struct     **dam_var;

    size_t                      i;
    size_t                      j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < dam_con_map[i].nd_active; j++) {
            initialize_dam_con(&dam_con[i][j]);
            initialize_dam_var(&dam_var[i][j]);
        }
    }
}
