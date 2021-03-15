/******************************************************************************
 * @section DESCRIPTION
 *
 * Force land-use vegetation fractions
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
* @brief   Force land-use vegetation fractions
******************************************/
void
lu_forcing(void)
{
    extern domain_struct              local_domain;
    extern plugin_global_param_struct plugin_global_param;
    extern domain_struct              global_domain;
    extern plugin_filenames_struct    plugin_filenames;
    extern option_struct              options;
    extern veg_con_map_struct        *veg_con_map;
    extern lu_force_struct           *lu_force;

    double                           *dvar;

    size_t                            d4count[4];
    size_t                            d4start[4];

    size_t                            i;
    size_t                            j;
    int                               vidx;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    d4start[0] = 0;
    d4start[1] = 0;
    d4start[2] = 0;
    d4start[3] = 0;
    d4count[0] = 1;
    d4count[1] = 1;
    d4count[2] = global_domain.n_ny;
    d4count[3] = global_domain.n_nx;

    // Get forcing data
    d4start[0] = plugin_global_param.forceskip[FORCING_CV] +
                 plugin_global_param.forceoffset[FORCING_CV];

    if (plugin_global_param.forcerun[FORCING_CV]) {
        for (j = 0; j < options.NVEGTYPES; j++) {
            d4start[1] = j;

            get_scatter_nc_field_double(
                &(plugin_filenames.forcing[
                      FORCING_CV]),
                plugin_filenames.f_varname[
                    FORCING_CV], d4start,
                d4count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                vidx = veg_con_map[i].vidx[j];
                if (vidx != NODATA_VEG) {
                    lu_force[i].Cv[vidx] = dvar[i];
                    lu_force[i].veg_class[vidx] = j;
                }
                else if (dvar[i] > 0) {
                    log_err("Vegetation class %zu is not allocated, "
                            "but Cv forcing > 0 [%.4f] for cell %zu",
                            j, dvar[i], i);
                }
            }
        }

        lu_apply();
    }

    free(dvar);
}
