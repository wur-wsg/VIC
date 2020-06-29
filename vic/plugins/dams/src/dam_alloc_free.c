/******************************************************************************
 * @section DESCRIPTION
 *
 * Dam allocate and finalize functions
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
* @brief   Set the dam mapping
******************************************/
void
dam_set_ndamtypes(void)
{
    extern domain_struct           local_domain;
    extern domain_struct           global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct    plugin_options;
    extern dam_con_map_struct     *local_dam_con_map;
    extern dam_con_map_struct     *global_dam_con_map;

    int                           *ivar;

    size_t                         i;
    size_t                         j;

    size_t                         d2count[2];
    size_t                         d2start[2];

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    get_scatter_nc_field_int(&(plugin_filenames.dams),
                             "Ndam_local", d2start, d2count, ivar);

    for (i = 0; i < local_domain.ncells_active; i++) {
        local_dam_con_map[i].nd_types = plugin_options.NDAMTYPES;
        local_dam_con_map[i].nd_active = ivar[i];

        for (j = 0; j < local_dam_con_map[i].nd_types; j++) {
            if (j < local_dam_con_map[i].nd_active) {
                local_dam_con_map[i].didx[j] = j;
            }
            else {
                local_dam_con_map[i].didx[j] = NODATA_DAM;
            }
        }
    }

    get_scatter_nc_field_int(&(plugin_filenames.dams),
                             "Ndam_global", d2start, d2count, ivar);

    for (i = 0; i < local_domain.ncells_active; i++) {
        global_dam_con_map[i].nd_types = plugin_options.NDAMTYPES;
        global_dam_con_map[i].nd_active = ivar[i];

        for (j = 0; j < global_dam_con_map[i].nd_types; j++) {
            if (j < global_dam_con_map[i].nd_active) {
                global_dam_con_map[i].didx[j] = j;
            }
            else {
                global_dam_con_map[i].didx[j] = NODATA_DAM;
            }
        }
    }

    free(ivar);
}

/******************************************
* @brief   Set the dam service mapping
******************************************/
void
dam_set_ndamservice(void)
{
    extern domain_struct           local_domain;
    extern domain_struct           global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct    plugin_options;
    extern dam_con_map_struct     *local_dam_con_map;
    extern dam_con_struct        **local_dam_con;
    extern dam_con_map_struct     *global_dam_con_map;
    extern dam_con_struct        **global_dam_con;

    int                           *ivar;

    size_t                         i;
    size_t                         j;

    size_t                         d3count[3];
    size_t                         d3start[3];

    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        d3start[0] = j;

        get_scatter_nc_field_int(&(plugin_filenames.dams),
                                 "Nservice_local", d3start, d3count, ivar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (local_dam_con_map[i].didx[j] != NODATA_DAM) {
                local_dam_con[i][j].nservice = ivar[i];
            }
        }

        get_scatter_nc_field_int(&(plugin_filenames.dams),
                                 "Nservice_global", d3start, d3count, ivar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (global_dam_con_map[i].didx[j] != NODATA_DAM) {
                global_dam_con[i][j].nservice = ivar[i];
            }
        }
    }

    free(ivar);
}

/******************************************
* @brief   Allocate dam module
******************************************/
void
dam_alloc(void)
{
    extern domain_struct           local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct    plugin_options;
    extern dam_con_map_struct     *local_dam_con_map;
    extern dam_con_struct        **local_dam_con;
    extern dam_var_struct        **local_dam_var;
    extern dam_con_map_struct     *global_dam_con_map;
    extern dam_con_struct        **global_dam_con;
    extern dam_var_struct        **global_dam_var;
    extern int                     mpi_rank;

    int                            status;

    size_t                         i;
    size_t                         j;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.dams.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.dams.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.dams.nc_filename);
    }

    local_dam_con_map =
        malloc(local_domain.ncells_active * sizeof(*local_dam_con_map));
    check_alloc_status(local_dam_con_map, "Memory allocation error");
    local_dam_con = malloc(local_domain.ncells_active * sizeof(*local_dam_con));
    check_alloc_status(local_dam_con, "Memory allocation error");
    local_dam_var = malloc(local_domain.ncells_active * sizeof(*local_dam_var));
    check_alloc_status(local_dam_var, "Memory allocation error");
    for (i = 0; i < local_domain.ncells_active; i++) {
        local_dam_con_map[i].didx =
            malloc(plugin_options.NDAMTYPES *
                   sizeof(*local_dam_con_map[i].didx));
        check_alloc_status(local_dam_con_map[i].didx,
                           "Memory allocation error");
    }

    global_dam_con_map =
        malloc(local_domain.ncells_active * sizeof(*global_dam_con_map));
    check_alloc_status(global_dam_con_map, "Memory allocation error");
    global_dam_con =
        malloc(local_domain.ncells_active * sizeof(*global_dam_con));
    check_alloc_status(global_dam_con, "Memory allocation error");
    global_dam_var =
        malloc(local_domain.ncells_active * sizeof(*global_dam_var));
    check_alloc_status(global_dam_var, "Memory allocation error");
    for (i = 0; i < local_domain.ncells_active; i++) {
        global_dam_con_map[i].didx =
            malloc(
                plugin_options.NDAMTYPES * sizeof(*global_dam_con_map[i].didx));
        check_alloc_status(global_dam_con_map[i].didx,
                           "Memory allocation error");
    }

    dam_set_ndamtypes();

    for (i = 0; i < local_domain.ncells_active; i++) {
        local_dam_con[i] =
            malloc(local_dam_con_map[i].nd_active * sizeof(*local_dam_con[i]));
        check_alloc_status(local_dam_con[i], "Memory allocation error");
        local_dam_var[i] =
            malloc(local_dam_con_map[i].nd_active * sizeof(*local_dam_var[i]));
        check_alloc_status(local_dam_var[i], "Memory allocation error");

        global_dam_con[i] =
            malloc(global_dam_con_map[i].nd_active *
                   sizeof(*global_dam_con[i]));
        check_alloc_status(global_dam_con[i], "Memory allocation error");
        global_dam_var[i] =
            malloc(global_dam_con_map[i].nd_active *
                   sizeof(*global_dam_var[i]));
        check_alloc_status(global_dam_var[i], "Memory allocation error");
    }

    dam_set_ndamservice();

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < plugin_options.NDAMTYPES; j++) {
            if (local_dam_con_map[i].didx[j] != NODATA_DAM) {
                local_dam_con[i][j].service = malloc(
                    local_dam_con[i][j].nservice *
                    sizeof(*local_dam_con[i][j].service));
                check_alloc_status(local_dam_con[i][j].service,
                                   "Memory allocation error");
                local_dam_con[i][j].service_frac = malloc(
                    local_dam_con[i][j].nservice *
                    sizeof(*local_dam_con[i][j].service_frac));
                check_alloc_status(local_dam_con[i][j].service_frac,
                                   "Memory allocation error");
            }

            if (global_dam_con_map[i].didx[j] != NODATA_DAM) {
                global_dam_con[i][j].service = malloc(
                    global_dam_con[i][j].nservice *
                    sizeof(*global_dam_con[i][j].service));
                check_alloc_status(global_dam_con[i][j].service,
                                   "Memory allocation error");
                global_dam_con[i][j].service_frac = malloc(
                    global_dam_con[i][j].nservice *
                    sizeof(*global_dam_con[i][j].service_frac));
                check_alloc_status(global_dam_con[i][j].service_frac,
                                   "Memory allocation error");
            }
        }
    }

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.dams.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.dams.nc_filename);
    }

    dam_initialize_local_structures();
}

/******************************************
* @brief   Finalize dam module
******************************************/
void
dam_finalize(void)
{
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;
    extern dam_con_map_struct  *local_dam_con_map;
    extern dam_con_struct     **local_dam_con;
    extern dam_var_struct     **local_dam_var;
    extern dam_con_map_struct  *global_dam_con_map;
    extern dam_con_struct     **global_dam_con;
    extern dam_var_struct     **global_dam_var;

    size_t                      i;
    size_t                      j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < plugin_options.NDAMTYPES; j++) {
            if (local_dam_con_map[i].didx[j] != NODATA_DAM) {
                free(local_dam_con[i][j].service);
                free(local_dam_con[i][j].service_frac);
            }
            if (global_dam_con_map[i].didx[j] != NODATA_DAM) {
                free(global_dam_con[i][j].service);
                free(global_dam_con[i][j].service_frac);
            }
        }

        free(local_dam_con_map[i].didx);
        free(local_dam_con[i]);
        free(local_dam_var[i]);
        free(global_dam_con_map[i].didx);
        free(global_dam_con[i]);
        free(global_dam_var[i]);
    }

    free(local_dam_con_map);
    free(local_dam_con);
    free(local_dam_var);
    free(global_dam_con_map);
    free(global_dam_con);
    free(global_dam_var);
}
