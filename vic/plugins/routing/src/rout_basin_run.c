#include <vic_driver_image.h>
#include <plugin.h>

void
rout_basin_run(size_t iCell)
{
    extern domain_struct              local_domain;
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct       plugin_options;
    extern rout_var_struct           *rout_var;
    extern rout_con_struct           *rout_con;
    extern rout_force_struct         *rout_force;
    extern double                  ***out_data;
    extern size_t                     NR;

    double                            inflow;
    double                            dt_inflow;
    double                            runoff;
    double                            dt_runoff;
    size_t                            rout_steps_per_dt;
    double                            prev_stream;

    size_t                            i;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    // Gather inflow from upstream cells
    inflow = 0;
    for (i = 0; i < rout_con[iCell].Nupstream; i++) {
        inflow += rout_var[rout_con[iCell].upstream[i]].discharge;
    }

    // Gather inflow from forcing
    if (plugin_options.FORCE_ROUTING) {
        inflow += rout_force[iCell].discharge[NR];
    }

    // Gather runoff from VIC
    runoff =
        (out_data[iCell][OUT_RUNOFF][0] +
         out_data[iCell][OUT_BASEFLOW][0]) *
        local_domain.locations[iCell].area /
        (global_param.dt * MM_PER_M);

    // Calculate delta-time inflow & runoff (equal contribution)
    dt_inflow = inflow / rout_steps_per_dt;
    dt_runoff = runoff / rout_steps_per_dt;

    // Shift and clear previous discharge data
    for (i = 0; i < rout_steps_per_dt; i++) {
        rout_var[iCell].dt_discharge[0] = 0.0;
        cshift(rout_var[iCell].dt_discharge,
               plugin_options.UH_LENGTH + rout_steps_per_dt, 1, 0, 1);
    }

    // Convolute current inflow & runoff
    for (i = 0; i < rout_steps_per_dt; i++) {
        convolute(dt_inflow, rout_con[iCell].inflow_uh,
                  rout_var[iCell].dt_discharge,
                  plugin_options.UH_LENGTH, i);
        convolute(dt_runoff, rout_con[iCell].runoff_uh,
                  rout_var[iCell].dt_discharge,
                  plugin_options.UH_LENGTH, i);
    }

    // Aggregate current timestep discharge & stream moisture
    rout_var[iCell].discharge = 0.0;
    prev_stream = rout_var[iCell].stream;
    rout_var[iCell].stream = 0.0;
    for (i = 0; i < plugin_options.UH_LENGTH + rout_steps_per_dt; i++) {
        if (i < rout_steps_per_dt) {
            rout_var[iCell].discharge += rout_var[iCell].dt_discharge[i];
        }
        else {
            rout_var[iCell].stream += rout_var[iCell].dt_discharge[i];
        }
    }

    // Check water balance
    if (abs(prev_stream + (inflow + runoff) -
            (rout_var[iCell].discharge + rout_var[iCell].stream)) >
        DBL_EPSILON) {
        log_err("Discharge water balance error [%.4f]. "
                "in: %.4f out: %.4f prev_storage: %.4f cur_storage %.4f",
                prev_stream + (inflow + runoff) -
                (rout_var[iCell].discharge + rout_var[iCell].stream),
                (inflow + runoff),
                rout_var[iCell].discharge,
                prev_stream,
                rout_var[iCell].stream);
    }
}
