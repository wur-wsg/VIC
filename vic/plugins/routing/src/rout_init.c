#include <vic_driver_image.h>
#include <plugin.h>

void
rout_set_uh(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct    plugin_options;
    extern domain_struct    global_domain;
    extern domain_struct    local_domain;
    extern rout_con_struct *rout_con;

    double                 *dvar = NULL;

    size_t                  i;
    size_t                  j;

    size_t                  d3count[3];
    size_t                  d3start[3];

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

    free(dvar);
}

void
rout_basin_set_downstream(void)
{
    extern domain_struct    global_domain;
    extern domain_struct    local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern rout_con_struct *rout_con;

    int                    *id;
    int                    *downstream;
    size_t                  rout_count;
    
    bool                    found;

    size_t                  i;
    size_t                  j;

    size_t                  d2count[2];
    size_t                  d2start[2];

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
    
    rout_count = 0;
    for (i = 0; i < local_domain.ncells_active; i++) {
        found = false;
        
        for (j = 0; j < local_domain.ncells_active; j++) {
            if (downstream[i] == id[j]) {
                rout_con[i].downstream = j;
                found = true;
            }
        }
        
        if(!found){
            rout_count++;
            rout_con[i].downstream = i;
        }
    }
    
    if(rout_count > 0){
        log_warn("No downstream cell was found for %zu cells; "
                "Probably the ID was outside of the mask or "
                "the ID was not set; "
                "Setting cell as outflow point",
                rout_count);
    }

    free(downstream);
    free(id);
}

void
rout_random_set_downstream(void)
{
    extern domain_struct    global_domain;
    extern domain_struct    local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern rout_con_struct *rout_con;
    extern int mpi_rank;

    int                    *id;
    int                    *downstream;
    int                    *down_global;
    int                    *down_local;
    size_t                  error_count;
    
    bool                    found;

    size_t                  i;
    size_t                  j;

    size_t                  d2count[2];
    size_t                  d2start[2];
    
    down_local = malloc(local_domain.ncells_active * sizeof(*down_local));
    check_alloc_status(down_local, "Memory allocation error.");
    
    downstream = malloc(global_domain.ncells_active * sizeof(*downstream));
    check_alloc_status(downstream, "Memory allocation error.");
    
    id = malloc(global_domain.ncells_active * sizeof(*id));
    check_alloc_status(id, "Memory allocation error.");
    
    down_global = malloc(global_domain.ncells_active * sizeof(*down_global));
    check_alloc_status(down_global, "Memory allocation error.");

    if(mpi_rank == VIC_MPI_ROOT){
    
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

            if(!found){
                error_count++;
                down_global[i] = i;
            }
        }
    
        if(error_count > 0){
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

void
rout_init(void)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;
    extern int              mpi_rank;

    int                     status;

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
