/******************************************************************************
 * @section DESCRIPTION
 *
 * Water-use setup functions (from input file)
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
* @brief   Setup remote mapping
******************************************/
void
wu_set_receiving(void)
{
    extern domain_struct           global_domain;
    extern domain_struct           local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern wu_con_struct          *wu_con;

    int                           *ivar;
    int                           *receiving_id;
    int                           *adjustment;
    size_t                         error_count;

    bool                           found;

    size_t                         i;
    size_t                         k;
    size_t                         l;

    size_t                         d2count[2];
    size_t                         d2start[2];
    size_t                         d3count[3];
    size_t                         d3start[3];

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");
    receiving_id = malloc(local_domain.ncells_active * sizeof(*receiving_id));
    check_alloc_status(receiving_id, "Memory allocation error.");
    adjustment = malloc(local_domain.ncells_active * sizeof(*adjustment));
    check_alloc_status(adjustment, "Memory allocation error.");

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

    get_scatter_nc_field_int(&(plugin_filenames.wateruse), "receiving_id",
                             d2start, d2count, receiving_id);

    error_count = 0;
    for (i = 0; i < local_domain.ncells_active; i++) {
        adjustment[i] = 0;
    }

    for (k = 0; k < plugin_options.NWURECEIVING; k++) {
        d3start[0] = k;

        get_scatter_nc_field_int(&(plugin_filenames.wateruse), "receiving",
                                 d3start, d3count, ivar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (k - adjustment[i] < wu_con[i].nreceiving) {
                found = false;
                for (l = 0; l < local_domain.ncells_active; l++) {
                    if (ivar[i] == receiving_id[l]) {
                        wu_con[i].receiving[k - adjustment[i]] = l;
                        found = true;
                    }
                }

                if (!found) {
                    error_count++;
                    wu_con[i].nreceiving--;
                    adjustment[i]++;
                }
            }
        }
    }

    if (error_count > 0) {
        log_warn("No receiving cell was found for %zu cells; "
                 "Probably the ID was outside of the mask or "
                 "the ID was not set; "
                 "Removing receiving cells",
                 error_count);
    }

    free(adjustment);
    free(ivar);
    free(receiving_id);
}

/******************************************
* @brief   Setup the water-use module
******************************************/
void
wu_init(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern int                     mpi_rank;

    int                            status;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.wateruse.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.wateruse.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.wateruse.nc_filename);
    }

    if (plugin_options.REMOTE_WITH) {
        wu_set_receiving();
    }

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.wateruse.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.wateruse.nc_filename);
    }
}
