/******************************************************************************
 * @section DESCRIPTION
 *
 * Irrigation allocate and finalize functions
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
* @brief   Set the irrigation mapping
******************************************/
void
irr_set_nirrtypes(void)
{
    extern domain_struct           local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct    plugin_options;
    extern irr_con_map_struct     *irr_con_map;
    extern veg_con_map_struct     *veg_con_map;
    extern MPI_Comm                MPI_COMM_VIC;
    extern int                     mpi_rank;

    int                           *ivar;

    size_t                         i;
    size_t                         j;
    int                            status;
    size_t                         veg_class;

    size_t                         d1count[1];
    size_t                         d1start[1];

    d1start[0] = 0;
    d1count[0] = plugin_options.NIRRTYPES;

    ivar = malloc(plugin_options.NIRRTYPES * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        get_nc_field_int(&(plugin_filenames.irrigation),
                         "veg_class", d1start, d1count, ivar);
    }

    status = MPI_Bcast(ivar, plugin_options.NIRRTYPES, MPI_INT,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    for (i = 0; i < local_domain.ncells_active; i++) {
        irr_con_map[i].ni_types = plugin_options.NIRRTYPES;
        irr_con_map[i].ni_active = 0;

        for (j = 0; j < plugin_options.NIRRTYPES; j++) {
            veg_class = ivar[j] - 1;

            if (veg_con_map[i].vidx[veg_class] != NODATA_VEG) {
                irr_con_map[i].iidx[j] = irr_con_map[i].ni_active;
                irr_con_map[i].vidx[j] = veg_con_map[i].vidx[veg_class];
                irr_con_map[i].ni_active++;
            }
            else {
                irr_con_map[i].iidx[j] = NODATA_VEG;
                irr_con_map[i].vidx[j] = NODATA_VEG;
            }
        }
    }

    free(ivar);
}

/******************************************
* @brief   Allocate (late) irrigation module
******************************************/
void
irr_late_alloc(void)
{
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;
    extern irr_var_struct    ***irr_var;
    extern irr_con_struct     **irr_con;
    extern irr_con_map_struct  *irr_con_map;
    extern option_struct        options;
    extern int                  mpi_rank;

    int                         status;

    size_t                      i;
    size_t                      j;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.irrigation.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.irrigation.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.irrigation.nc_filename);
    }

    irr_var = malloc(local_domain.ncells_active * sizeof(*irr_var));
    check_alloc_status(irr_var, "Memory allocation error");

    irr_con = malloc(local_domain.ncells_active * sizeof(*irr_con));
    check_alloc_status(irr_con, "Memory allocation error");

    irr_con_map = malloc(local_domain.ncells_active * sizeof(*irr_con_map));
    check_alloc_status(irr_con_map, "Memory allocation error");

    for (i = 0; i < local_domain.ncells_active; i++) {
        irr_con_map[i].iidx =
            malloc(plugin_options.NIRRTYPES * sizeof(*irr_con_map[i].iidx));
        check_alloc_status(irr_con_map[i].iidx, "Memory allocation error");

        irr_con_map[i].vidx =
            malloc(plugin_options.NIRRTYPES * sizeof(*irr_con_map[i].vidx));
        check_alloc_status(irr_con_map[i].vidx, "Memory allocation error");
    }

    irr_set_nirrtypes();

    for (i = 0; i < local_domain.ncells_active; i++) {
        irr_con[i] =
            malloc(irr_con_map[i].ni_active * sizeof(*irr_con[i]));
        check_alloc_status(irr_con[i], "Memory allocation error");

        irr_var[i] =
            malloc(irr_con_map[i].ni_active * sizeof(*irr_var[i]));
        check_alloc_status(irr_var[i], "Memory allocation error");

        for (j = 0; j < irr_con_map[i].ni_active; j++) {
            irr_var[i][j] =
                malloc(options.SNOW_BAND * sizeof(*irr_var[i][j]));
            check_alloc_status(irr_var[i][j], "Memory allocation error");
        }
    }

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.irrigation.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.irrigation.nc_filename);
    }

    irr_initialize_local_structures();
}

/******************************************
* @brief   Finalize irrigation module
******************************************/
void
irr_finalize(void)
{
    extern domain_struct       local_domain;
    extern irr_var_struct   ***irr_var;
    extern irr_con_struct    **irr_con;
    extern irr_con_map_struct *irr_con_map;

    size_t                     i;
    size_t                     j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < irr_con_map[i].ni_active; j++) {
            free(irr_var[i][j]);
        }
        free(irr_con_map[i].iidx);
        free(irr_con_map[i].vidx);
        free(irr_con[i]);
        free(irr_var[i]);
    }
    free(irr_var);
    free(irr_con);
    free(irr_con_map);
}
