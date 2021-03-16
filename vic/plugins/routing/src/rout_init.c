/******************************************************************************
 * @section DESCRIPTION
 *
 * Routing setup functions (from input file)
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
* @brief   Setup unit hydrograph
******************************************/
void
rout_set_uh(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct    plugin_options;
    extern domain_struct           global_domain;
    extern domain_struct           local_domain;
    extern rout_con_struct        *rout_con;

    char                           locstr[MAXSTRING];
    double                         uh_sum;
    double                        *dvar = NULL;

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

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    for (j = 0; j < plugin_options.UH_LENGTH; j++) {
        d3start[0] = j;

        get_scatter_nc_field_double(&(plugin_filenames.routing),
                                    "uh_inflow", d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            rout_con[i].inflow_uh[j] = dvar[i];
        }

        get_scatter_nc_field_double(&(plugin_filenames.routing),
                                    "uh_runoff", d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            rout_con[i].runoff_uh[j] = dvar[i];
        }
    }

    for (i = 0; i < local_domain.ncells_active; i++) {
        sprint_location(locstr, &(local_domain.locations[i]));

        uh_sum = 0.0;
        for (j = 0; j < plugin_options.UH_LENGTH; j++) {
            uh_sum += rout_con[i].inflow_uh[j];
        }

        for (j = 0; j < plugin_options.UH_LENGTH; j++) {
            rout_con[i].inflow_uh[j] /= uh_sum;
        }
        if (!assert_close_double(uh_sum, 1., 0., AREA_SUM_ERROR_THRESH)) {
            sprint_location(locstr, &(local_domain.locations[i]));
            log_warn("Sum of inflow unit hydrograph !=  1.0 (%.16f) at grid "
                     "cell %zd. Adjusting fractions ...\n%s", uh_sum, i,
                     locstr);
        }

        uh_sum = 0.0;
        for (j = 0; j < plugin_options.UH_LENGTH; j++) {
            uh_sum += rout_con[i].runoff_uh[j];
        }

        for (j = 0; j < plugin_options.UH_LENGTH; j++) {
            rout_con[i].runoff_uh[j] /= uh_sum;
        }
        if (!assert_close_double(uh_sum, 1., 0., AREA_SUM_ERROR_THRESH)) {
            sprint_location(locstr, &(local_domain.locations[i]));
            log_warn("Sum of runoff unit hydrograph !=  1.0 (%.16f) at grid "
                     "cell %zd. Adjusting fractions ...\n%s", uh_sum, i,
                     locstr);
        }
    }

    free(dvar);
}

/******************************************
* @brief   Setup downstream cells (local node)
******************************************/
void
rout_basin_set_downstream(void)
{
    extern domain_struct           global_domain;
    extern domain_struct           local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern rout_con_struct        *rout_con;

    int                           *id;
    int                           *downstream;
    size_t                         error_count;

    bool                           found;

    size_t                         i;
    size_t                         j;

    size_t                         d2count[2];
    size_t                         d2start[2];

    downstream = malloc(local_domain.ncells_active * sizeof(*downstream));
    check_alloc_status(downstream, "Memory allocation error.");
    id = malloc(local_domain.ncells_active * sizeof(*id));
    check_alloc_status(id, "Memory allocation error.");

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    get_scatter_nc_field_int(&(plugin_filenames.routing), "downstream_id",
                             d2start, d2count, id);

    get_scatter_nc_field_int(&(plugin_filenames.routing), "downstream",
                             d2start, d2count, downstream);

    error_count = 0;
    for (i = 0; i < local_domain.ncells_active; i++) {
        found = false;

        for (j = 0; j < local_domain.ncells_active; j++) {
            if (downstream[i] == id[j]) {
                rout_con[i].downstream = j;
                found = true;
            }
        }

        if (!found) {
            error_count++;
            rout_con[i].downstream = i;
        }
    }

    if (error_count > 0) {
        log_warn("No downstream cell was found for %zu cells; "
                 "Probably the ID was outside of the mask or "
                 "the ID was not set; "
                 "Setting cell as outflow point",
                 error_count);
    }

    free(downstream);
    free(id);
}

/******************************************
* @brief   Setup downstream cells (master node)
******************************************/
void
rout_random_set_downstream(void)
{
    extern domain_struct           global_domain;
    extern domain_struct           local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern rout_con_struct        *rout_con;
    extern int                     mpi_rank;

    int                           *id;
    int                           *downstream;
    int                           *down_global;
    int                           *down_local;
    size_t                         error_count;

    bool                           found;

    size_t                         i;
    size_t                         j;

    size_t                         d2count[2];
    size_t                         d2start[2];

    down_local = malloc(local_domain.ncells_active * sizeof(*down_local));
    check_alloc_status(down_local, "Memory allocation error.");

    downstream = malloc(global_domain.ncells_active * sizeof(*downstream));
    check_alloc_status(downstream, "Memory allocation error.");

    id = malloc(global_domain.ncells_active * sizeof(*id));
    check_alloc_status(id, "Memory allocation error.");

    down_global = malloc(global_domain.ncells_active * sizeof(*down_global));
    check_alloc_status(down_global, "Memory allocation error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        d2start[0] = 0;
        d2start[1] = 0;
        d2count[0] = global_domain.n_ny;
        d2count[1] = global_domain.n_nx;

        get_active_nc_field_int(&(plugin_filenames.routing), "downstream_id",
                                d2start, d2count, id);

        get_active_nc_field_int(&(plugin_filenames.routing), "downstream",
                                d2start, d2count, downstream);

        error_count = 0;
        for (i = 0; i < global_domain.ncells_active; i++) {
            found = false;

            for (j = 0; j < global_domain.ncells_active; j++) {
                if (downstream[i] == id[j]) {
                    down_global[i] = j;
                    found = true;
                }
            }

            if (!found) {
                error_count++;
                down_global[i] = i;
            }
        }

        if (error_count > 0) {
            log_warn("No downstream cell was found for %zu cells; "
                     "Probably the ID was outside of the mask or "
                     "the ID was not set; "
                     "Setting cell as outflow point",
                     error_count);
        }
    }

    scatter_int(down_global, down_local);

    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_con[i].downstream = down_local[i];
    }

    free(down_local);
    free(down_global);
    free(downstream);
    free(id);
}

/******************************************
* @brief   Setup upstream cells
******************************************/
void
rout_basin_set_upstream(void)
{
    extern domain_struct    local_domain;
    extern rout_con_struct *rout_con;

    size_t                  upstream[MAX_UPSTREAM];

    size_t                  i;
    size_t                  j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < local_domain.ncells_active; j++) {
            if (rout_con[j].downstream == i && i != j) {
                upstream[rout_con[i].Nupstream] = j;
                rout_con[i].Nupstream++;

                if (rout_con[i].Nupstream > MAX_UPSTREAM) {
                    log_err(
                        "Number of upstream cells [%zu] is bigger "
                        "than the maximum number of upstream cells possible [%d]",
                        rout_con[i].Nupstream,
                        MAX_UPSTREAM);
                }
            }
        }

        rout_con[i].upstream =
            malloc(rout_con[i].Nupstream * sizeof(*rout_con[i].upstream));
        check_alloc_status(rout_con[i].upstream, "Memory allocation error.");

        for (j = 0; j < rout_con[i].Nupstream; j++) {
            rout_con[i].upstream[j] = upstream[j];
        }
    }
}

/******************************************
* @brief   Setup upstream cells (master node)
******************************************/
void
rout_random_set_upstream(void)
{
    extern domain_struct    local_domain;
    extern domain_struct    global_domain;
    extern rout_con_struct *rout_con;
    extern int              mpi_rank;

    size_t                  i;
    size_t                  j;

    size_t                 *down_global;
    size_t                 *down_local;
    size_t                **up_global;
    size_t                **up_local;
    size_t                 *nup_global;
    size_t                 *nup_local;

    // Alloc
    down_global = malloc(global_domain.ncells_active * sizeof(*down_global));
    check_alloc_status(down_global, "Memory allocation error");
    nup_global = malloc(global_domain.ncells_active * sizeof(*nup_global));
    check_alloc_status(nup_global, "Memory allocation error");
    up_global = malloc(global_domain.ncells_active * sizeof(*up_global));
    check_alloc_status(up_global, "Memory allocation error");
    for (i = 0; i < global_domain.ncells_active; i++) {
        up_global[i] = malloc(MAX_UPSTREAM * sizeof(*up_global[i]));
        check_alloc_status(up_global[i], "Memory allocation error");
    }
    down_local = malloc(local_domain.ncells_active * sizeof(*down_local));
    check_alloc_status(down_local, "Memory allocation error");
    nup_local = malloc(local_domain.ncells_active * sizeof(*nup_local));
    check_alloc_status(nup_local, "Memory allocation error");
    up_local = malloc(local_domain.ncells_active * sizeof(*up_local));
    check_alloc_status(up_local, "Memory allocation error");
    for (i = 0; i < local_domain.ncells_active; i++) {
        up_local[i] = malloc(MAX_UPSTREAM * sizeof(*up_local[i]));
        check_alloc_status(up_local[i], "Memory allocation error");
    }

    // Get downstream
    for (i = 0; i < local_domain.ncells_active; i++) {
        down_local[i] = rout_con[i].downstream;
    }

    // Gather downstream
    gather_size_t(down_global, down_local);

    // Get upstream
    if (mpi_rank == VIC_MPI_ROOT) {
        for (i = 0; i < global_domain.ncells_active; i++) {
            nup_global[i] = 0;

            for (j = 0; j < global_domain.ncells_active; j++) {
                if (down_global[j] == i && i != j) {
                    up_global[i][nup_global[i]] = j;
                    nup_global[i]++;

                    if (nup_global[i] > MAX_UPSTREAM) {
                        log_err(
                            "Number of upstream cells [%zu] is bigger "
                            "than the maximum number of upstream cells possible [%d]",
                            nup_global[i],
                            MAX_UPSTREAM);
                    }
                }
            }
        }
    }

    // Scatter upstream
    scatter_size_t(nup_global, nup_local);
    scatter_size_t_2d(up_global, up_local, MAX_UPSTREAM);

    // Set upstream
    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_con[i].Nupstream = nup_local[i];

        rout_con[i].upstream =
            malloc(rout_con[i].Nupstream * sizeof(*rout_con[i].upstream));
        check_alloc_status(rout_con[i].upstream, "Memory allocation error.");

        for (j = 0; j < rout_con[i].Nupstream; j++) {
            rout_con[i].upstream[j] = up_local[i][j];
        }
    }

    // Free
    for (i = 0; i < global_domain.ncells_active; i++) {
        free(up_global[i]);
    }
    free(down_global);
    free(nup_global);
    free(up_global);
    for (i = 0; i < local_domain.ncells_active; i++) {
        free(up_local[i]);
    }
    free(down_local);
    free(nup_local);
    free(up_local);
}

/******************************************
* @brief   Setup the upstream-downstream order (local node)
******************************************/
void
rout_basin_set_order()
{
    extern domain_struct    local_domain;
    extern rout_con_struct *rout_con;
    extern size_t          *routing_order;

    bool                    done_tmp[local_domain.ncells_active];
    bool                    done_fin[local_domain.ncells_active];
    size_t                  rank;
    bool                    has_upstream;

    size_t                  i;
    size_t                  j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        done_tmp[i] = false;
        done_fin[i] = false;
    }

    // Set cell_order_local for node
    rank = 0;
    while (rank < local_domain.ncells_active) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (done_fin[i]) {
                continue;
            }

            // count number of upstream cells that are not processed yet
            has_upstream = false;
            for (j = 0; j < rout_con[i].Nupstream; j++) {
                if (!done_fin[rout_con[i].upstream[j]]) {
                    has_upstream = true;
                    break;
                }
            }

            if (has_upstream) {
                continue;
            }

            // if no upstream, add as next order
            routing_order[rank] = i;
            done_tmp[i] = true;
            rank++;

            if (rank > local_domain.ncells_active) {
                log_err("Error in ordering and ranking cells");
            }
        }
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (done_tmp[i] == true) {
                done_fin[i] = true;
            }
        }
    }
}

/******************************************
* @brief   Setup the upstream-downstream order (master node)
******************************************/
void
rout_random_set_order()
{
    extern domain_struct    local_domain;
    extern domain_struct    global_domain;
    extern rout_con_struct *rout_con;
    extern size_t          *routing_order;
    extern int              mpi_rank;

    size_t                **up_global;
    size_t                **up_local;
    size_t                 *nup_global;
    size_t                 *nup_local;

    bool                    done_tmp[global_domain.ncells_active];
    bool                    done_fin[global_domain.ncells_active];

    size_t                  rank;
    bool                    has_upstream;

    size_t                  i;
    size_t                  j;

    // Alloc
    if (mpi_rank == VIC_MPI_ROOT) {
        nup_global = malloc(global_domain.ncells_active * sizeof(*nup_global));
        check_alloc_status(nup_global, "Memory allocation error");
        up_global = malloc(global_domain.ncells_active * sizeof(*up_global));
        check_alloc_status(up_global, "Memory allocation error");
        for (i = 0; i < global_domain.ncells_active; i++) {
            up_global[i] = malloc(MAX_UPSTREAM * sizeof(*up_global[i]));
            check_alloc_status(up_global[i], "Memory allocation error");
        }
    }
    nup_local = malloc(local_domain.ncells_active * sizeof(*nup_local));
    check_alloc_status(nup_local, "Memory allocation error");
    up_local = malloc(local_domain.ncells_active * sizeof(*up_local));
    check_alloc_status(up_local, "Memory allocation error");
    for (i = 0; i < local_domain.ncells_active; i++) {
        up_local[i] = malloc(MAX_UPSTREAM * sizeof(*up_local[i]));
        check_alloc_status(up_local[i], "Memory allocation error");
    }

    // Set nupstream and upstream
    for (i = 0; i < local_domain.ncells_active; i++) {
        nup_local[i] = rout_con[i].Nupstream;

        for (j = 0; j < rout_con[i].Nupstream; j++) {
            up_local[i][j] = rout_con[i].upstream[j];
        }
    }

    // Gather nupstream and upstream
    gather_size_t(nup_global, nup_local);
    gather_size_t_2d(up_global, up_local, MAX_UPSTREAM);

    // Get order
    if (mpi_rank == VIC_MPI_ROOT) {
        for (i = 0; i < global_domain.ncells_active; i++) {
            done_tmp[i] = false;
            done_fin[i] = false;
        }

        rank = 0;
        while (rank < global_domain.ncells_active) {
            for (i = 0; i < global_domain.ncells_active; i++) {
                if (done_fin[i]) {
                    continue;
                }

                // count number of upstream cells that are not processed yet
                has_upstream = false;
                for (j = 0; j < nup_global[i]; j++) {
                    if (!done_fin[up_global[i][j]]) {
                        has_upstream = true;
                        break;
                    }
                }

                if (has_upstream) {
                    continue;
                }

                // if no upstream, add as next order
                routing_order[rank] = i;
                done_tmp[i] = true;
                rank++;

                if (rank > global_domain.ncells_active) {
                    log_err("Error in ordering and ranking cells");
                }
            }
            for (i = 0; i < global_domain.ncells_active; i++) {
                if (done_tmp[i] == true) {
                    done_fin[i] = true;
                }
            }
        }
    }

    // Free
    if (mpi_rank == VIC_MPI_ROOT) {
        for (i = 0; i < global_domain.ncells_active; i++) {
            free(up_global[i]);
        }
        free(up_global);
        free(nup_global);
    }
    for (i = 0; i < local_domain.ncells_active; i++) {
        free(up_local[i]);
    }
    free(up_local);
    free(nup_local);
}

/******************************************
* @brief   Setup the routing module
******************************************/
void
rout_init(void)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;
    extern int                     mpi_rank;

    int                            status;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.routing.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.routing.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.routing.nc_filename);
    }

    rout_set_uh();

    if (plugin_options.DECOMPOSITION == BASIN_DECOMPOSITION ||
        plugin_options.DECOMPOSITION == FILE_DECOMPOSITION) {
        rout_basin_set_downstream();
        rout_basin_set_upstream();
        rout_basin_set_order();
    }
    else if (plugin_options.DECOMPOSITION == RANDOM_DECOMPOSITION) {
        rout_random_set_downstream();
        rout_random_set_upstream();
        rout_random_set_order();
    }

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.routing.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.routing.nc_filename);
    }
}
