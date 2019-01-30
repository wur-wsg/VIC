#include <vic_driver_image.h>
#include <plugin.h>

void
rout_random_run()
{
    extern domain_struct       local_domain;
    extern domain_struct       global_domain;
    extern global_param_struct global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct       plugin_options;
    extern plugin_option_struct       plugin_options;
    extern rout_var_struct    *rout_var;
    extern rout_con_struct    *rout_con;
    extern rout_force_struct  *rout_force;
    extern double           ***out_data;
    extern size_t             *routing_order;
    extern int                 mpi_rank;

    size_t                    *nup_global;
    size_t                   **up_global;
    double                   **ruh_global;
    double                   **iuh_global;
    double                    *run_global;
    double                   **dt_dis_global;
    double                    *dis_global;
    double                    *stream_global;
    double                    *force_global;

    size_t                    *nup_local;
    size_t                   **up_local;
    double                   **ruh_local;
    double                   **iuh_local;
    double                    *run_local;
    double                   **dt_dis_local;
    double                    *dis_local;
    double                    *stream_local;
    double                    *force_local;

    size_t                     cur_cell;
    double                     inflow;
    double                     dt_inflow;
    double                     runoff;
    double                     dt_runoff;
    size_t                     rout_steps_per_dt;
    double                     prev_stream;

    size_t                     i;
    size_t                     j;
        
    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                          global_param.model_steps_per_day;

    // Alloc
    nup_global = malloc(global_domain.ncells_active * sizeof(*nup_global));
    check_alloc_status(nup_global, "Memory allocation error");
    up_global = malloc(global_domain.ncells_active * sizeof(*up_global));
    check_alloc_status(up_global, "Memory allocation error");
    ruh_global = malloc(global_domain.ncells_active * sizeof(*ruh_global));
    check_alloc_status(ruh_global, "Memory allocation error");
    iuh_global = malloc(global_domain.ncells_active * sizeof(*iuh_global));
    check_alloc_status(iuh_global, "Memory allocation error");
    run_global = malloc(global_domain.ncells_active * sizeof(*run_global));
    check_alloc_status(run_global, "Memory allocation error");
    dt_dis_global = malloc(global_domain.ncells_active * sizeof(*dt_dis_global));
    check_alloc_status(dt_dis_global, "Memory allocation error");
    dis_global = malloc(global_domain.ncells_active * sizeof(*dis_global));
    check_alloc_status(dis_global, "Memory allocation error");
    stream_global = malloc(global_domain.ncells_active * sizeof(*stream_global));
    check_alloc_status(stream_global, "Memory allocation error");

    for (i = 0; i < global_domain.ncells_active; i++) {
        up_global[i] = malloc(MAX_UPSTREAM * sizeof(*up_global[i]));
        check_alloc_status(up_global[i], "Memory allocation error");
        ruh_global[i] = malloc(plugin_options.UH_LENGTH * sizeof(*ruh_global[i]));
        check_alloc_status(ruh_global[i], "Memory allocation error");
        iuh_global[i] = malloc(plugin_options.UH_LENGTH * sizeof(*iuh_global[i]));
        check_alloc_status(iuh_global[i], "Memory allocation error");
        dt_dis_global[i] = malloc((plugin_options.UH_LENGTH + rout_steps_per_dt) * sizeof(*dt_dis_global[i]));
        check_alloc_status(dt_dis_global[i], "Memory allocation error");
    }

    nup_local = malloc(local_domain.ncells_active * sizeof(*nup_local));
    check_alloc_status(nup_local, "Memory allocation error");
    up_local = malloc(local_domain.ncells_active * sizeof(*up_local));
    check_alloc_status(up_local, "Memory allocation error");
    ruh_local = malloc(local_domain.ncells_active * sizeof(*ruh_local));
    check_alloc_status(ruh_local, "Memory allocation error");
    iuh_local = malloc(local_domain.ncells_active * sizeof(*iuh_local));
    check_alloc_status(iuh_local, "Memory allocation error");
    run_local = malloc(local_domain.ncells_active * sizeof(*run_local));
    check_alloc_status(run_local, "Memory allocation error");
    dt_dis_local = malloc(local_domain.ncells_active * sizeof(*dt_dis_local));
    check_alloc_status(dt_dis_local, "Memory allocation error");
    dis_local = malloc(local_domain.ncells_active * sizeof(*dis_local));
    check_alloc_status(dis_local, "Memory allocation error");
    stream_local = malloc(local_domain.ncells_active * sizeof(*stream_local));
    check_alloc_status(stream_local, "Memory allocation error");

    for (i = 0; i < local_domain.ncells_active; i++) {
        up_local[i] = malloc(MAX_UPSTREAM * sizeof(*up_local[i]));
        check_alloc_status(up_local[i], "Memory allocation error");
        ruh_local[i] = malloc(plugin_options.UH_LENGTH * sizeof(*ruh_local[i]));
        check_alloc_status(ruh_local[i], "Memory allocation error");
        iuh_local[i] = malloc(plugin_options.UH_LENGTH * sizeof(*iuh_local[i]));
        check_alloc_status(iuh_local[i], "Memory allocation error");
        dt_dis_local[i] = malloc((plugin_options.UH_LENGTH + rout_steps_per_dt) * sizeof(*dt_dis_local[i]));
        check_alloc_status(dt_dis_local[i], "Memory allocation error");
    }
    
    if (plugin_options.FORCE_ROUTING) {
        force_global = malloc(global_domain.ncells_active * sizeof(*force_global));
        check_alloc_status(force_global, "Memory allocation error");
        force_local = malloc(local_domain.ncells_active * sizeof(*force_local));
        check_alloc_status(force_local, "Memory allocation error");
    }

    // Get
    for (i = 0; i < local_domain.ncells_active; i++) {
        nup_local[i] = rout_con[i].Nupstream;
        for (j = 0; j < rout_con[i].Nupstream; j++) {
            up_local[i][j] = rout_con[i].upstream[j];
        }
        for (j = 0; j < plugin_options.UH_LENGTH; j++) {
            ruh_local[i][j] = rout_con[i].runoff_uh[j];
        }
        for (j = 0; j < plugin_options.UH_LENGTH; j++) {
            iuh_local[i][j] = rout_con[i].inflow_uh[j];
        }
        run_local[i] =
            (out_data[i][OUT_RUNOFF][0] +
            out_data[i][OUT_BASEFLOW][0]) *
            local_domain.locations[i].area /
            (global_param.dt * MM_PER_M);
        for (j = 0; j < plugin_options.UH_LENGTH + rout_steps_per_dt; j++) {
            dt_dis_local[i][j] = rout_var[i].dt_discharge[j];
        }
        
        dis_local[i] = rout_var[i].discharge;
        stream_local[i] = rout_var[i].stream;
        if (plugin_options.FORCE_ROUTING) {
            force_local[i] = rout_force[i].discharge[NR];
        }
    }

    // Gather
    gather_size_t(nup_global, nup_local);
    gather_size_t_2d(up_global, up_local, MAX_UPSTREAM);
    gather_double_2d(ruh_global, ruh_local, plugin_options.UH_LENGTH);
    gather_double_2d(iuh_global, iuh_local, plugin_options.UH_LENGTH);
    gather_double(run_global, run_local);
    gather_double_2d(dt_dis_global, dt_dis_local, plugin_options.UH_LENGTH + rout_steps_per_dt);
    gather_double(dis_global, dis_local);
    gather_double(stream_global, stream_local);
    if (plugin_options.FORCE_ROUTING) {
        gather_double(force_global, force_local);
    }

    // Calculate
    if (mpi_rank == VIC_MPI_ROOT) {
        for (i = 0; i < global_domain.ncells_active; i++) {
            cur_cell = routing_order[i];

            // Gather inflow from upstream cells
            inflow = 0;
            for (j = 0; j < nup_global[cur_cell]; j++) {
                inflow += dis_global[up_global[cur_cell][j]];
            }
            if (plugin_options.FORCE_ROUTING) {
                inflow += force_global[cur_cell];
            }
            
            // Gather runoff from VIC
            runoff = run_global[cur_cell];
            
            // Calculate delta-time inflow & runoff (equal contribution)
            dt_inflow = inflow / rout_steps_per_dt;
            dt_runoff = runoff / rout_steps_per_dt;
            
            // Shift and clear previous discharge data
            for(j = 0; j < rout_steps_per_dt; j++){
                dt_dis_global[cur_cell][0] = 0.0;
                cshift(dt_dis_global[cur_cell], plugin_options.UH_LENGTH + rout_steps_per_dt, 1, 0, 1);
            }
            
            // Convolute current inflow & runoff
            for(j = 0; j < rout_steps_per_dt; j++){
                convolute(dt_inflow, iuh_global[cur_cell], dt_dis_global[cur_cell],
                     plugin_options.UH_LENGTH, j);
                convolute(dt_runoff, ruh_global[cur_cell], dt_dis_global[cur_cell],
                     plugin_options.UH_LENGTH, j);
            }
    
            // Aggregate current timestep discharge & stream moisture
            dis_global[cur_cell] = 0.0;
            prev_stream = stream_global[cur_cell];
            stream_global[cur_cell] = 0.0;
            for(j = 0; j < plugin_options.UH_LENGTH + rout_steps_per_dt; j++){
                if (j < rout_steps_per_dt) {
                    dis_global[cur_cell] += dt_dis_global[cur_cell][j];
                } else {
                    stream_global[cur_cell] += dt_dis_global[cur_cell][j];
                }
            }

            // Check water balance
            if(abs(prev_stream + (inflow + runoff) - 
                    (dis_global[cur_cell] + stream_global[cur_cell])) >
                    DBL_EPSILON){
                log_err("Discharge water balance error");
            }
        }
    }

    // Scatter discharge
    scatter_double_2d(dt_dis_global, dt_dis_local, plugin_options.UH_LENGTH + rout_steps_per_dt);
    scatter_double(dis_global, dis_local);
    scatter_double(stream_global, stream_local);

    // Set discharge
    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < plugin_options.UH_LENGTH + rout_steps_per_dt; j++) {
            rout_var[i].dt_discharge[j] = dt_dis_local[i][j];
        }
        rout_var[i].discharge = dis_local[i];
        rout_var[i].stream = stream_local[i];
    }

    // Free
    for (i = 0; i < global_domain.ncells_active; i++) {
        free(up_global[i]);
        free(ruh_global[i]);
        free(iuh_global[i]);
        free(dt_dis_global[i]);
    }
    free(nup_global);
    free(up_global);
    free(ruh_global);
    free(iuh_global);
    free(run_global);
    free(dt_dis_global);
    free(dis_global);
    free(stream_global);

    for (i = 0; i < local_domain.ncells_active; i++) {
        free(up_local[i]);
        free(ruh_local[i]);
        free(iuh_local[i]);
        free(dt_dis_local[i]);
    }
    free(nup_local);
    free(up_local);
    free(ruh_local);
    free(iuh_local);
    free(run_local);
    free(dt_dis_local);
    free(dis_local);
    free(stream_local);
    
    if (plugin_options.FORCE_ROUTING){
        free(force_global);
        free(force_local);
    }
}
