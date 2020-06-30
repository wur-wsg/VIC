/******************************************************************************
 * @section DESCRIPTION
 *
 * Water-use allocate and finalize functions
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
* @brief   Set the water-use mapping
******************************************/
void
wu_set_nsectors(void)
{
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;
    extern wu_con_map_struct   *wu_con_map;

    int                        *ivar;

    size_t                      i;
    size_t                      j;

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_con_map[i].ns_types = plugin_options.NWUTYPES;
        wu_con_map[i].ns_active = 0;

        for (j = 0; j < plugin_options.NWUTYPES; j++) {
            if (plugin_options.WU_INPUT[j] == WU_SKIP) {
                wu_con_map[i].sidx[j] = NODATA_WU;
            }
            else {
                wu_con_map[i].sidx[j] = wu_con_map[i].ns_active;
                wu_con_map[i].ns_active++;
            }
        }
    }

    free(ivar);
}

/******************************************
* @brief   Set the water-use remote mapping
******************************************/
void
wu_set_nreceiving(void)
{
    extern domain_struct           local_domain;
    extern domain_struct           global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern wu_con_struct          *wu_con;

    int                           *ivar;

    size_t                         i;

    size_t                         d2count[2];
    size_t                         d2start[2];

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    get_scatter_nc_field_int(&(plugin_filenames.wateruse), "Nreceiving",
                             d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_con[i].nreceiving = ivar[i];
    }

    free(ivar);
}

/******************************************
* @brief   Allocate water-use module
******************************************/
void
wu_alloc(void)
{
    extern domain_struct      local_domain;
    extern wu_con_map_struct *wu_con_map;
    extern wu_var_struct    **wu_var;
    extern wu_con_struct     *wu_con;
    extern wu_force_struct  **wu_force;
    extern int                mpi_rank;

    size_t                    i;
    int                       status;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.wateruse.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.wateruse.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.wateruse.nc_filename);
    }

    wu_con_map = malloc(local_domain.ncells_active * sizeof(*wu_con_map));
    check_alloc_status(wu_con_map, "Memory allocation error");
    wu_force = malloc(local_domain.ncells_active * sizeof(*wu_force));
    check_alloc_status(wu_force, "Memory allocation error");
    wu_con = malloc(local_domain.ncells_active * sizeof(*wu_con));
    check_alloc_status(wu_con, "Memory allocation error");
    wu_var = malloc(local_domain.ncells_active * sizeof(*wu_var));
    check_alloc_status(wu_var, "Memory allocation error");
    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_con_map[i].sidx =
            malloc(plugin_options.NWUTYPES * sizeof(*wu_con_map[i].sidx));
        check_alloc_status(wu_con_map[i].sidx, "Memory allocation error");
    }

    wu_set_nsectors();

    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_force[i] = malloc(wu_con_map[i].ns_active * sizeof(*wu_force[i]));
        check_alloc_status(wu_force[i], "Memory allocation error");
        wu_var[i] = malloc(wu_con_map[i].ns_active * sizeof(*wu_var[i]));
        check_alloc_status(wu_var[i], "Memory allocation error");
    }

    if (plugin_options.REMOTE_WITH) {
        wu_set_nreceiving();
    }
    else {
        for (i = 0; i < local_domain.ncells_active; i++) {
            wu_con[i].nreceiving = 0;
        }
    }

    for (i = 0; i < local_domain.ncells_active; i++) {
        wu_con[i].receiving =
            malloc(wu_con[i].nreceiving * sizeof(*wu_con[i].receiving));
        check_alloc_status(wu_con[i].receiving, "Memory allocation error");
    }

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.wateruse.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.wateruse.nc_filename);
    }

    wu_initialize_local_structures();
}

/******************************************
* @brief   Finalize water-use module
******************************************/
void
wu_finalize(void)
{
    extern domain_struct      local_domain;
    extern wu_con_map_struct *wu_con_map;
    extern wu_var_struct    **wu_var;
    extern wu_con_struct     *wu_con;
    extern wu_force_struct  **wu_force;

    size_t                    i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        free(wu_con_map[i].sidx);
        free(wu_con[i].receiving);
        free(wu_var[i]);
        free(wu_force[i]);
    }

    free(wu_con_map);
    free(wu_con);
    free(wu_var);
    free(wu_force);
}
