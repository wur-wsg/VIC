/******************************************************************************
 * @section DESCRIPTION
 *
 * Dam setup functions (from input file)
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
* @brief   Setup dam mapping
******************************************/
void
dam_set_mapping(void)
{
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;
    extern dam_con_struct     **local_dam_con;
    extern dam_con_map_struct  *local_dam_con_map;
    extern dam_con_struct     **global_dam_con;
    extern dam_con_map_struct  *global_dam_con_map;

    size_t                      i;
    size_t                      j;
    int                         dam_index;


    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < plugin_options.NDAMTYPES; j++) {
            dam_index = local_dam_con_map[i].didx[j];

            if (dam_index != NODATA_DAM) {
                local_dam_con[i][dam_index].dam_class = j;
            }
        }
        for (j = 0; j < plugin_options.NDAMTYPES; j++) {
            dam_index = global_dam_con_map[i].didx[j];

            if (dam_index != NODATA_DAM) {
                global_dam_con[i][dam_index].dam_class = j;
            }
        }
    }
}

/******************************************
* @brief   Setup service mapping
******************************************/
void
dam_set_service(void)
{
    extern domain_struct           local_domain;
    extern domain_struct           global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct    plugin_options;
    extern dam_con_map_struct     *local_dam_con_map;
    extern dam_con_struct        **local_dam_con;
    extern dam_con_map_struct     *global_dam_con_map;
    extern dam_con_struct        **global_dam_con;

    int                           *service_id;
    int                          **adjustment;
    int                           *ivar;
    double                        *dvar;

    size_t                         error_count;
    bool                           done;

    size_t                         i;
    size_t                         j;
    size_t                         k;
    size_t                         l;

    size_t                         d2count[2];
    size_t                         d2start[2];
    size_t                         d4count[4];
    size_t                         d4start[4];

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    d4start[0] = 0;
    d4start[1] = 0;
    d4start[2] = 0;
    d4start[3] = 0;
    d4count[0] = 1;
    d4count[1] = 1;
    d4count[2] = global_domain.n_ny;
    d4count[3] = global_domain.n_nx;

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");
    service_id = malloc(local_domain.ncells_active * sizeof(*service_id));
    check_alloc_status(service_id, "Memory allocation error.");

    adjustment = malloc(local_domain.ncells_active * sizeof(*adjustment));
    check_alloc_status(adjustment, "Memory allocation error.");
    for (i = 0; i < local_domain.ncells_active; i++) {
        adjustment[i] =
            malloc(plugin_options.NDAMTYPES * sizeof(*adjustment[i]));
        check_alloc_status(adjustment[i], "Memory allocation error.");
    }

    get_scatter_nc_field_int(&(plugin_filenames.dams),
                             "service_id", d2start, d2count, service_id);

    error_count = 0;
    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < plugin_options.NDAMTYPES; j++) {
            adjustment[i][j] = 0;
        }
    }
    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        d4start[0] = j;

        for (k = 0; k < plugin_options.NDAMSERVICE; k++) {
            d4start[1] = k;

            get_scatter_nc_field_int(&(plugin_filenames.dams),
                                     "service_local", d4start, d4count, ivar);
            get_scatter_nc_field_double(&(plugin_filenames.dams),
                                        "service_fraction_local", d4start,
                                        d4count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                if (local_dam_con_map[i].didx[j] != NODATA_DAM &&
                    k - adjustment[i][j] < local_dam_con[i][j].nservice) {
                    done = false;
                    for (l = 0; l < local_domain.ncells_active; l++) {
                        if (ivar[i] == service_id[l]) {
                            local_dam_con[i][j].service[k -
                                                        adjustment[i][j]] = l;
                            local_dam_con[i][j].service_frac[k -
                                                             adjustment[i][j]] =
                                dvar[i];
                            done = true;
                        }
                    }

                    if (!done) {
                        error_count++;
                        local_dam_con[i][j].nservice--;
                        adjustment[i][j]++;
                    }
                }
            }
        }
    }

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < plugin_options.NDAMTYPES; j++) {
            adjustment[i][j] = 0;
        }
    }
    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        d4start[0] = j;

        for (k = 0; k < (size_t)plugin_options.NDAMSERVICE; k++) {
            d4start[1] = k;

            get_scatter_nc_field_int(&(plugin_filenames.dams),
                                     "service_global", d4start, d4count, ivar);
            get_scatter_nc_field_double(&(plugin_filenames.dams),
                                        "service_fraction_global", d4start,
                                        d4count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                if (global_dam_con_map[i].didx[j] != NODATA_DAM &&
                    k - adjustment[i][j] < global_dam_con[i][j].nservice) {
                    done = false;
                    for (l = 0; l < local_domain.ncells_active; l++) {
                        if (ivar[i] == service_id[l]) {
                            global_dam_con[i][j].service[k -
                                                         adjustment[i][j]] = l;
                            global_dam_con[i][j].service_frac[k -
                                                              adjustment[i][j]]
                                =
                                    dvar[i];
                            done = true;
                        }
                    }

                    if (!done) {
                        error_count++;
                        global_dam_con[i][j].nservice--;
                        adjustment[i][j]++;
                    }
                }
            }
        }
    }

    if (error_count > 0) {
        log_warn("Dams service cell id not found for %zu cells; "
                 "Probably the ID was outside of the mask or "
                 "the ID was not set; "
                 "Removing from dam service", error_count);
    }

    free(ivar);
    free(dvar);
    free(service_id);
    for (i = 0; i < local_domain.ncells_active; i++) {
        free(adjustment[i]);
    }
    free(adjustment);
}

/******************************************
* @brief   Setup info
******************************************/
void
dam_set_info(void)
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
    double                        *dvar;

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

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        get_scatter_nc_field_int(&(plugin_filenames.dams),
                                 "year_local", d3start, d3count, ivar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (local_dam_con_map[i].didx[j] != NODATA_DAM) {
                local_dam_con[i][j].year = ivar[i];
            }
        }

        get_scatter_nc_field_double(&(plugin_filenames.dams),
                                    "capacity_local", d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (local_dam_con_map[i].didx[j] != NODATA_DAM) {
                local_dam_con[i][j].capacity = dvar[i];
            }
        }

        get_scatter_nc_field_double(&(plugin_filenames.dams),
                                    "inflow_fraction_local", d3start, d3count,
                                    dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (local_dam_con_map[i].didx[j] != NODATA_DAM) {
                local_dam_con[i][j].inflow_frac = dvar[i];
            }
        }


        get_scatter_nc_field_int(&(plugin_filenames.dams),
                                 "year_global", d3start, d3count, ivar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (global_dam_con_map[i].didx[j] != NODATA_DAM) {
                global_dam_con[i][j].year = ivar[i];
            }
        }

        get_scatter_nc_field_double(&(plugin_filenames.dams),
                                    "capacity_global", d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (global_dam_con_map[i].didx[j] != NODATA_DAM) {
                global_dam_con[i][j].capacity = dvar[i];
            }
        }

        get_scatter_nc_field_double(&(plugin_filenames.dams),
                                    "inflow_fraction_global", d3start, d3count,
                                    dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (global_dam_con_map[i].didx[j] != NODATA_DAM) {
                global_dam_con[i][j].inflow_frac = dvar[i];
            }
        }
    }

    free(ivar);
    free(dvar);
}

/******************************************
* @brief   Setup the dam module
******************************************/
void
dam_init(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern int                     mpi_rank;

    int                            status;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.dams.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.dams.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.dams.nc_filename);
    }

    dam_set_mapping();
    dam_set_info();
    dam_set_service();

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.dams.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.dams.nc_filename);
    }
}
