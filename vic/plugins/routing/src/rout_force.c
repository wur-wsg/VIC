/******************************************************************************
 * @section DESCRIPTION
 *
 * Force routing inflow
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
* @brief   Force routing inflow
******************************************/
void
rout_forcing(void)
{
    extern domain_struct              local_domain;
    extern plugin_global_param_struct plugin_global_param;
    extern domain_struct              global_domain;
    extern plugin_filenames_struct    plugin_filenames;
    extern rout_force_struct         *rout_force;

    double                           *dvar;

    size_t                            d3count[3];
    size_t                            d3start[3];

    size_t                            i;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    // Get forcing data
    d3start[0] = plugin_global_param.forceskip[FORCING_DISCHARGE] +
                 plugin_global_param.forceoffset[FORCING_DISCHARGE];

    if (plugin_global_param.forcerun[FORCING_DISCHARGE]) {
        get_scatter_nc_field_double(
            &(plugin_filenames.forcing[
                  FORCING_DISCHARGE]),
            plugin_filenames.f_varname[
                FORCING_DISCHARGE], d3start,
            d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            rout_force[i].discharge = dvar[i];
        }
    }

    free(dvar);
}
