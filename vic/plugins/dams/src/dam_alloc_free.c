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
    extern dam_con_map_struct     *dam_con_map;
    extern MPI_Comm                MPI_COMM_VIC;
    extern int                     mpi_rank;

    int                           *id_map;
    int                           *id;

    int                            status;
    size_t                         i;
    size_t                         j;

    size_t                         d1count[1];
    size_t                         d1start[1];
    size_t                         d2count[2];
    size_t                         d2start[2];

    d1start[0] = 0;
    d1count[0] = plugin_options.NDAMTYPES;
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    id_map = malloc(local_domain.ncells_active * sizeof(*id_map));
    check_alloc_status(id_map, "Memory allocation error.");
    id = malloc(plugin_options.NDAMTYPES * sizeof(*id));
    check_alloc_status(id, "Memory allocation error.");

    get_scatter_nc_field_int(&(plugin_filenames.dams),
                             "id_map", d2start, d2count, id_map);

    if (mpi_rank == VIC_MPI_ROOT) {
        get_nc_field_int(&(plugin_filenames.dams), "id", d1start, d1count, id);
    }
    status = MPI_Bcast(id, plugin_options.NDAMTYPES, MPI_INT,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    for (i = 0; i < local_domain.ncells_active; i++) {
        dam_con_map[i].nd_types = plugin_options.NDAMTYPES;
        dam_con_map[i].nd_active = 0;
    }

    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (id[j] == id_map[i]) {
                dam_con_map[i].didx[j] = dam_con_map[i].nd_active;
                dam_con_map[i].nd_active++;
            }
            else {
                dam_con_map[i].didx[j] = NODATA_DAM;
            }
        }
    }

    free(id_map);
    free(id);
}

/******************************************
* @brief   Set the dam service mapping
******************************************/
void
dam_set_ndamservice(void)
{
    extern domain_struct           local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct    plugin_options;
    extern dam_con_map_struct     *dam_con_map;
    extern dam_con_struct        **dam_con;
    extern MPI_Comm                MPI_COMM_VIC;
    extern int                     mpi_rank;

    int                           *ivar;

    int                            status;
    size_t                         i;
    size_t                         j;
    int                            iDam;

    size_t                         d1count[1];
    size_t                         d1start[1];

    d1start[0] = 0;
    d1count[0] = plugin_options.NDAMTYPES;

    ivar = malloc(plugin_options.NDAMTYPES * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        get_nc_field_int(&(plugin_filenames.dams), "Nservice", d1start, d1count,
                         ivar);
    }
    status = MPI_Bcast(ivar, plugin_options.NDAMTYPES, MPI_INT,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    for (j = 0; j < plugin_options.NDAMTYPES; j++) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            iDam = dam_con_map[i].didx[j];
            if (iDam != NODATA_DAM) {
                dam_con[i][iDam].nservice = ivar[j];
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
    extern dam_con_map_struct     *dam_con_map;
    extern dam_con_struct        **dam_con;
    extern dam_var_struct        **dam_var;
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

    dam_con_map =
        malloc(local_domain.ncells_active * sizeof(*dam_con_map));
    check_alloc_status(dam_con_map, "Memory allocation error");
    dam_con = malloc(local_domain.ncells_active * sizeof(*dam_con));
    check_alloc_status(dam_con, "Memory allocation error");
    dam_var = malloc(local_domain.ncells_active * sizeof(*dam_var));
    check_alloc_status(dam_var, "Memory allocation error");
    for (i = 0; i < local_domain.ncells_active; i++) {
        dam_con_map[i].didx =
            malloc(plugin_options.NDAMTYPES * sizeof(*dam_con_map[i].didx));
        check_alloc_status(dam_con_map[i].didx, "Memory allocation error");
    }

    dam_set_ndamtypes();

    for (i = 0; i < local_domain.ncells_active; i++) {
        dam_con[i] =
            malloc(dam_con_map[i].nd_active * sizeof(*dam_con[i]));
        check_alloc_status(dam_con[i], "Memory allocation error");
        dam_var[i] =
            malloc(dam_con_map[i].nd_active * sizeof(*dam_var[i]));
        check_alloc_status(dam_var[i], "Memory allocation error");
    }

    dam_set_ndamservice();

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < dam_con_map[i].nd_active; j++) {
            dam_con[i][j].service = malloc(dam_con[i][j].nservice *
                                           sizeof(*dam_con[i][j].service));
            check_alloc_status(dam_con[i][j].service,
                               "Memory allocation error");
            dam_con[i][j].service_frac = malloc(dam_con[i][j].nservice *
                                                sizeof(*dam_con[i][j].
                                                       service_frac));
            check_alloc_status(dam_con[i][j].service_frac,
                               "Memory allocation error");
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
    extern domain_struct       local_domain;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct    **dam_con;
    extern dam_var_struct    **dam_var;

    size_t                     i;
    size_t                     j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < dam_con_map[i].nd_active; j++) {
            free(dam_con[i][j].service);
            free(dam_con[i][j].service_frac);
        }

        free(dam_con_map[i].didx);
        free(dam_con[i]);
        free(dam_var[i]);
    }

    free(dam_con_map);
    free(dam_con);
    free(dam_var);
}
