/******************************************************************************
 * @section DESCRIPTION
 *
 * Routing allocate and finalize functions
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
* @brief   Allocate routing module
******************************************/
void
rout_alloc(void)
{
    extern domain_struct              global_domain;
    extern domain_struct              local_domain;
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct       plugin_options;

    extern rout_var_struct           *rout_var;
    extern rout_con_struct           *rout_con;
    extern rout_force_struct         *rout_force;
    extern size_t                    *routing_order;

    size_t                            rout_steps_per_dt;
    size_t                            i;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    rout_var = malloc(local_domain.ncells_active * sizeof(*rout_var));
    check_alloc_status(rout_var, "Memory allocation error");

    rout_con = malloc(local_domain.ncells_active * sizeof(*rout_con));
    check_alloc_status(rout_con, "Memory allocation error");

    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_con[i].inflow_uh =
            malloc(plugin_options.UH_LENGTH * sizeof(*rout_con[i].inflow_uh));
        check_alloc_status(rout_con[i].inflow_uh, "Memory allocation error");

        rout_con[i].runoff_uh =
            malloc(plugin_options.UH_LENGTH * sizeof(*rout_con[i].runoff_uh));
        check_alloc_status(rout_con[i].runoff_uh, "Memory allocation error");

        rout_var[i].dt_discharge =
            malloc((plugin_options.UH_LENGTH + rout_steps_per_dt - 1) *
                   sizeof(*rout_var[i].dt_discharge));
        check_alloc_status(rout_var[i].dt_discharge, "Memory allocation error");
    }

    if (plugin_options.FORCE_ROUTING) {
        rout_force = malloc(local_domain.ncells_active * sizeof(*rout_force));
        check_alloc_status(rout_force, "Memory allocation error");
    }

    if (plugin_options.DECOMPOSITION == BASIN_DECOMPOSITION ||
        plugin_options.DECOMPOSITION == FILE_DECOMPOSITION) {
        routing_order =
            malloc(local_domain.ncells_active * sizeof(*routing_order));
        check_alloc_status(routing_order, "Memory allocation error");
    }
    else if (plugin_options.DECOMPOSITION == RANDOM_DECOMPOSITION) {
        routing_order =
            malloc(global_domain.ncells_active * sizeof(*routing_order));
        check_alloc_status(routing_order, "Memory allocation error");
    }

    rout_initialize_local_structures();
}

/******************************************
* @brief   Finalize routing module
******************************************/
void
rout_finalize(void)
{
    extern domain_struct        local_domain;
    extern rout_var_struct     *rout_var;
    extern rout_con_struct     *rout_con;
    extern size_t              *routing_order;
    extern rout_force_struct   *rout_force;
    extern plugin_option_struct plugin_options;

    size_t                      i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        free(rout_con[i].inflow_uh);
        free(rout_con[i].runoff_uh);
        free(rout_con[i].upstream);
        free(rout_var[i].dt_discharge);
    }
    free(rout_var);
    free(rout_con);

    if (plugin_options.FORCE_ROUTING) {
        free(rout_force);
    }

    free(routing_order);
}
