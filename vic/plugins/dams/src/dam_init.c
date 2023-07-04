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
    extern dam_con_struct     **dam_con;
    extern dam_con_map_struct  *dam_con_map;

    size_t                      i;
    size_t                      j;
    int                         dam_index;


    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < plugin_options.NDAMTYPES; j++) {
            dam_index = dam_con_map[i].didx[j];

            if (dam_index != NODATA_DAM) {
                // actually dam_class is the dam_class index
                dam_con[i][dam_index].dam_class = j;
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
    extern dam_con_map_struct     *dam_con_map;
    extern dam_con_struct        **dam_con;

    int                           *id_map;
    int                           *ivar;
    double                        *dvar;
    size_t                        *nservicing_tmp;

    size_t                         error_count;

    size_t                         i;
    size_t                         j;
    size_t                         k;
    int                            dam_index;

    size_t                         d2count[2];
    size_t                         d2start[2];
    size_t                         d3count[3];
    size_t                         d3start[3];

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    id_map = malloc(local_domain.ncells_active * sizeof(*id_map));
    check_alloc_status(id_map, "Memory allocation error.");
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");
    nservicing_tmp = malloc(plugin_options.NDAMTYPES * sizeof(*nservicing_tmp));
    check_alloc_status(nservicing_tmp, "Memory allocation error.");

    get_scatter_nc_field_int(&(plugin_filenames.dams),
                             "id_map", d2start, d2count, id_map);

    error_count = 0;
    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        nservicing_tmp[j] = 0;
    }

    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        d3start[0] = j;

        get_scatter_nc_field_int(&(plugin_filenames.dams),
                                 "service", d3start, d3count, ivar);
        get_scatter_nc_field_double(&(plugin_filenames.dams),
                                    "service_fraction", d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (ivar[i] > 0) {
                for (k = 0; k < local_domain.ncells_active; k++) {
                    if (ivar[i] != id_map[k]) {
                        continue;
                    }

                    dam_index = dam_con_map[k].didx[j];
                    if (dam_index == NODATA_DAM) {
                        log_warn("Servicing cell %zu is references a dam (%zu)"
                                 " that does not exit",
                                 i, k);
                        continue;
                    }

                    if (nservicing_tmp[j] >= dam_con[k][dam_index].nservice) {
                        log_err("number of servicing cells (%zu) is larger "
                                "than specified in the parameter file (%zu)",
                                nservicing_tmp[j],
                                dam_con[k][dam_index].nservice);
                    }

                    dam_con[k][dam_index].service[nservicing_tmp[j]] = i;
                    dam_con[k][dam_index].service_frac[nservicing_tmp[j]] =
                        dvar[i];
                    nservicing_tmp[j]++;
                }
            }
        }
    }

    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            dam_index = dam_con_map[i].didx[j];

            if (dam_index != NODATA_DAM) {
                if (nservicing_tmp[j] != dam_con[i][dam_index].nservice) {
                    error_count++;
                }
                dam_con[i][dam_index].nservice = nservicing_tmp[j];
            }
        }
    }

    if (error_count > 0) {
        log_warn("Several servicing cells were not found for %zu dams; "
                 "Probably the ID was outside of the mask or "
                 "the ID was not set; "
                 "Removing servicing cells",
                 error_count);
    }

    free(id_map);
    free(ivar);
    free(dvar);
    free(nservicing_tmp);
}

/******************************************
* @brief   Setup info
******************************************/
void
dam_set_info(void)
{
    extern domain_struct           local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct    plugin_options;
    extern dam_con_map_struct     *dam_con_map;
    extern dam_con_struct        **dam_con;
    extern int                    *dam_classes;
    extern MPI_Comm                MPI_COMM_VIC;
    extern int                     mpi_rank;

    int                           *ivar;
    double                        *dvar;

    int                            status;
    size_t                         i;
    size_t                         j;
    int                            dam_index;

    size_t                         d1count[1];
    size_t                         d1start[1];

    d1start[0] = 0;
    d1count[0] = plugin_options.NDAMTYPES;

    ivar = malloc(plugin_options.NDAMTYPES * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    dvar = malloc(plugin_options.NDAMTYPES * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        get_nc_field_int(&(plugin_filenames.dams), "type", d1start, d1count,
                         ivar);
    }
    status = MPI_Bcast(ivar, plugin_options.NDAMTYPES, MPI_INT,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            dam_index = dam_con_map[i].didx[j];

            if (dam_index != NODATA_DAM) {
                if (ivar[j] == 0) {
                    dam_con[i][dam_index].type = DAM_LOCAL;
                }
                else if (ivar[j] == 1) {
                    dam_con[i][dam_index].type = DAM_GLOBAL;
                }
                else {
                    log_err("Unknown dam type %d", ivar[j]);
                }
            }
        }
    }

    if (mpi_rank == VIC_MPI_ROOT) {
        get_nc_field_int(&(plugin_filenames.dams), "year", d1start, d1count,
                         ivar);
    }
    status = MPI_Bcast(ivar, plugin_options.NDAMTYPES, MPI_INT,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            dam_index = dam_con_map[i].didx[j];

            if (dam_index != NODATA_DAM) {
                dam_con[i][dam_index].year = ivar[j];
            }
        }
    }

    if (mpi_rank == VIC_MPI_ROOT) {
        get_nc_field_int(&(plugin_filenames.dams), "dam_class", d1start,
                         d1count,
                         dam_classes);
    }
    status = MPI_Bcast(dam_classes, plugin_options.NDAMTYPES, MPI_INT,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            dam_index = dam_con_map[i].didx[j];

            if (dam_index != NODATA_DAM) {
                dam_con[i][dam_index].dam_class = dam_classes[j];
            }
        }
    }

    if (mpi_rank == VIC_MPI_ROOT) {
        get_nc_field_double(&(plugin_filenames.dams), "capacity", d1start,
                            d1count, dvar);
    }
    status = MPI_Bcast(dvar, plugin_options.NDAMTYPES, MPI_INT,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            dam_index = dam_con_map[i].didx[j];

            if (dam_index != NODATA_DAM) {
                dam_con[i][dam_index].capacity = dvar[j];
            }
        }
    }

    if (mpi_rank == VIC_MPI_ROOT) {
        get_nc_field_double(&(plugin_filenames.dams), "inflow_fraction",
                            d1start, d1count, dvar);
    }
    status = MPI_Bcast(dvar, plugin_options.NDAMTYPES, MPI_INT,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            dam_index = dam_con_map[i].didx[j];

            if (dam_index != NODATA_DAM) {
                dam_con[i][dam_index].inflow_frac = dvar[j];
            }
        }
    }

    // if (mpi_rank == VIC_MPI_ROOT){
    // get_nc_field_int(&(plugin_filenames.dams), "dam_class",
    // d1start, d1count, ivar);
    // for (j = 0; j < plugin_options.NDAMTYPES; j++){
    // for (i = 0; i < local_domain.ncells_active; i++){
    // dam_index = dam_con_map[i].didx[j];

    // if (dam_index != NODATA_DAM) {
    // dam_con[i][dam_index].inflow_frac = dvar[j];
    // }
    // }
    // }
    // }

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

    // dam_set_mapping();
    dam_set_info();
    dam_set_service();

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.dams.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.dams.nc_filename);
    }
}
