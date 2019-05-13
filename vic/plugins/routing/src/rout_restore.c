#include <vic_driver_shared_image.h>
#include <plugin.h>

void
rout_check_init_state_file(void)
{
    extern filenames_struct           filenames;
    extern plugin_option_struct       plugin_options;
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;

    size_t                            dimlen;
    size_t                            rout_steps_per_dt;

    if (mpi_rank == VIC_MPI_ROOT) {
        rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                            global_param.model_steps_per_day;

        dimlen = get_nc_dimension(&(filenames.init_state), "routing_dt");
        if (dimlen != plugin_options.UH_LENGTH + rout_steps_per_dt) {
            log_err("Rout delta time in state file does not "
                    "match parameter file");
        }
    }
}

void
rout_restore(void)
{
    extern domain_struct              global_domain;
    extern domain_struct              local_domain;
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct       plugin_options;
    extern rout_var_struct           *rout_var;
    extern filenames_struct           filenames;
    extern metadata_struct            state_metadata[];

    size_t                            i;
    size_t                            j;
    double                           *dvar = NULL;
    size_t                            d3count[3];
    size_t                            d3start[3];
    size_t                            rout_steps_per_dt;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    // allocate memory for variables to be stored
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error");

    // initialize starts and counts
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    // total soil moisture
    for (j = 0; j < plugin_options.UH_LENGTH + rout_steps_per_dt; j++) {
        d3start[0] = j;
        get_scatter_nc_field_double(&(filenames.init_state),
                                    state_metadata[N_STATE_VARS +
                                                   STATE_DISCHARGE_DT].varname,
                                    d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            rout_var[i].dt_discharge[j] = dvar[i];
        }
    }

    free(dvar);
}

void
rout_compute_derived_state_vars(void)
{
    extern domain_struct              local_domain;
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct       plugin_options;
    extern rout_var_struct           *rout_var;

    size_t                            i;
    size_t                            j;
    size_t                            rout_steps_per_dt;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    // Aggregate current timestep discharge & stream moisture
    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_var[i].discharge = 0.0;
        rout_var[i].stream = 0.0;
        for (j = 0; j < plugin_options.UH_LENGTH + rout_steps_per_dt; j++) {
            if (j < rout_steps_per_dt) {
                rout_var[i].discharge += rout_var[i].dt_discharge[j];
            }
            else {
                rout_var[i].stream += rout_var[i].dt_discharge[j];
            }
        }
    }
}
