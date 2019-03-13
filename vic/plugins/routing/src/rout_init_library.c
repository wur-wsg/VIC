#include <vic_driver_image.h>
#include <plugin.h>

void
initialize_rout_force(rout_force_struct *rout_force)
{
    extern size_t NF;

    size_t        i;

    for (i = 0; i < NF; i++) {
        rout_force->discharge[i] = 0.0;
    }
}

void
initialize_rout_var(rout_var_struct *rout_var)
{
    extern plugin_option_struct       plugin_options;
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;

    size_t                            i;
    size_t                            rout_steps_per_dt;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    rout_var->stream = 0.0;
    rout_var->discharge = 0.0;
    for (i = 0; i < plugin_options.UH_LENGTH + rout_steps_per_dt; i++) {
        rout_var->dt_discharge[i] = 0.0;
    }
}

void
initialize_rout_con(rout_con_struct *rout_con)
{
    extern plugin_option_struct plugin_options;

    size_t                      i;

    rout_con->downstream = MISSING_USI;
    rout_con->Nupstream = 0;
    rout_con->upstream = NULL;

    for (i = 0; i < plugin_options.UH_LENGTH; i++) {
        rout_con->inflow_uh[i] = 0.0;
        rout_con->runoff_uh[i] = 0.0;
    }
}

void
rout_initialize_local_structures(void)
{
    extern domain_struct        local_domain;
    extern rout_var_struct     *rout_var;
    extern rout_con_struct     *rout_con;
    extern rout_force_struct   *rout_force;
    extern plugin_option_struct plugin_options;

    size_t                      i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_rout_con(&rout_con[i]);
        initialize_rout_var(&(rout_var[i]));
    }

    if (plugin_options.FORCE_ROUTING) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            initialize_rout_force(&(rout_force[i]));
        }
    }
}
