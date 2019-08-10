#include <vic_driver_image.h>
#include <plugin.h>

void
rout_alloc(void)
{
    extern domain_struct              global_domain;
    extern domain_struct              local_domain;
    extern size_t                     NF;
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct       plugin_options;

    extern rout_var_struct           *rout_var;
    extern rout_con_struct           *rout_con;
    extern rout_force_struct         *rout_force;
    extern size_t                    *routing_order;

    size_t                            rout_steps_per_dt;
    size_t                            i;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    rout_var = malloc(local_domain.ncells_active * sizeof(*rout_var));
    check_alloc_status(rout_var, "Memory allocation error");

    rout_con = malloc(local_domain.ncells_active * sizeof(*rout_con));
    check_alloc_status(rout_con, "Memory allocation error");

    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_con[i].inflow_uh =
            malloc(plugin_options.UH_LENGTH * sizeof(*rout_con[i].inflow_uh));
        check_alloc_status(rout_con[i].inflow_uh, "Memory allocation error");

        rout_con[i].runoff_uh =
            malloc(plugin_options.UH_LENGTH * sizeof(*rout_con[i].runoff_uh));
        check_alloc_status(rout_con[i].runoff_uh, "Memory allocation error");

        rout_var[i].dt_discharge =
            malloc((plugin_options.UH_LENGTH + rout_steps_per_dt) *
                   sizeof(*rout_var[i].dt_discharge));
        check_alloc_status(rout_var[i].dt_discharge, "Memory allocation error");
    }

    if (plugin_options.FORCE_ROUTING) {
        rout_force = malloc(local_domain.ncells_active * sizeof(*rout_force));
        check_alloc_status(rout_force, "Memory allocation error");

        for (i = 0; i < local_domain.ncells_active; i++) {
            rout_force[i].discharge =
                malloc(NF * sizeof(*rout_force[i].discharge));
            check_alloc_status(rout_force[i].discharge,
                               "Memory allocation error");
        }
    }

    if (plugin_options.DECOMPOSITION == BASIN_DECOMPOSITION ||
        plugin_options.DECOMPOSITION == FILE_DECOMPOSITION) {
        routing_order =
            malloc(local_domain.ncells_active * sizeof(*routing_order));
        check_alloc_status(routing_order, "Memory allocation error");
    }
    else if (plugin_options.DECOMPOSITION == RANDOM_DECOMPOSITION) {
        routing_order =
            malloc(global_domain.ncells_active * sizeof(*routing_order));
        check_alloc_status(routing_order, "Memory allocation error");
    }

    rout_initialize_local_structures();
}

void
rout_finalize(void)
{
    extern domain_struct        local_domain;
    extern rout_var_struct     *rout_var;
    extern rout_con_struct     *rout_con;
    extern size_t              *routing_order;
    extern rout_force_struct   *rout_force;
    extern plugin_option_struct plugin_options;

    size_t                      i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        free(rout_con[i].inflow_uh);
        free(rout_con[i].runoff_uh);
        free(rout_con[i].upstream);
        free(rout_var[i].dt_discharge);
    }
    free(rout_var);
    free(rout_con);

    if (plugin_options.FORCE_ROUTING) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            free(rout_force[i].discharge);
        }
        free(rout_force);
    }

    free(routing_order);
}
