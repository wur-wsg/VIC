/******************************************************************************
 * @section DESCRIPTION
 *
 * Run routing on master node (random decomposition)
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
* @brief   Run routing on master node (random decomposition)
******************************************/
void
rout_random_run()
{
    extern domain_struct              local_domain;
    extern domain_struct              global_domain;
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct       plugin_options;
    extern plugin_option_struct       plugin_options;
    extern rout_var_struct           *rout_var;
    extern rout_con_struct           *rout_con;
    extern rout_force_struct         *rout_force;
    extern double                  ***out_data;
    extern size_t                    *routing_order;
    extern int                        mpi_rank;

    size_t                           *nup_global;
    size_t                          **up_global;
    double                          **ruh_global;
    double                          **iuh_global;
    double                           *run_global;
    double                          **dt_dis_global;
    double                           *dis_global;
    double                           *stream_global;
    double                           *force_global;

    size_t                           *nup_local;
    size_t                          **up_local;
    double                          **ruh_local;
    double                          **iuh_local;
    double                           *run_local;
    double                          **dt_dis_local;
    double                           *dis_local;
    double                           *stream_local;
    double                           *force_local;

    size_t                            iCell;
    double                            in_runoff;
    double                            in_baseflow;
    double                            inflow;
    double                            inflow_wb;
    double                            dt_inflow;
    double                            runoff;
    double                            dt_runoff;
    size_t                            rout_steps_per_dt;
    double                            prev_stream;

    size_t                            i;
    size_t                            j;
    size_t                            k;

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
    dt_dis_global =
        malloc(global_domain.ncells_active * sizeof(*dt_dis_global));
    check_alloc_status(dt_dis_global, "Memory allocation error");
    dis_global = malloc(global_domain.ncells_active * sizeof(*dis_global));
    check_alloc_status(dis_global, "Memory allocation error");
    stream_global =
        malloc(global_domain.ncells_active * sizeof(*stream_global));
    check_alloc_status(stream_global, "Memory allocation error");

    for (i = 0; i < global_domain.ncells_active; i++) {
        up_global[i] = malloc(MAX_UPSTREAM * sizeof(*up_global[i]));
        check_alloc_status(up_global[i], "Memory allocation error");
        ruh_global[i] =
            malloc(plugin_options.UH_LENGTH * sizeof(*ruh_global[i]));
        check_alloc_status(ruh_global[i], "Memory allocation error");
        iuh_global[i] =
            malloc(plugin_options.UH_LENGTH * sizeof(*iuh_global[i]));
        check_alloc_status(iuh_global[i], "Memory allocation error");
        dt_dis_global[i] = malloc(
            (plugin_options.UH_LENGTH + rout_steps_per_dt) *
            sizeof(*dt_dis_global[i]));
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
        dt_dis_local[i] = malloc(
            (plugin_options.UH_LENGTH + rout_steps_per_dt) *
            sizeof(*dt_dis_local[i]));
        check_alloc_status(dt_dis_local[i], "Memory allocation error");
    }

    if (plugin_options.FORCE_ROUTING) {
        force_global = malloc(
            global_domain.ncells_active * sizeof(*force_global));
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
        
        in_runoff = out_data[i][OUT_RUNOFF][0];
        in_baseflow = out_data[i][OUT_BASEFLOW][0];
        if (in_baseflow > rout_var[i].nonrenew_deficit) {
            rout_var[i].nonrenew_deficit = 0.;
            in_baseflow -= rout_var[i].nonrenew_deficit;
        } else {
            rout_var[i].nonrenew_deficit -= in_baseflow;
            in_baseflow = 0.;
        }
        
        run_local[i] =
            (in_runoff + in_baseflow) *
            local_domain.locations[i].area /
            (global_param.dt * MM_PER_M);
        for (j = 0; j < plugin_options.UH_LENGTH + rout_steps_per_dt; j++) {
            dt_dis_local[i][j] = rout_var[i].dt_discharge[j];
        }

        dis_local[i] = rout_var[i].discharge;
        stream_local[i] = rout_var[i].stream;
        if (plugin_options.FORCE_ROUTING) {
            force_local[i] = rout_force[i].discharge;
        }
    }

    // Gather
    gather_size_t(nup_global, nup_local);
    gather_size_t_2d(up_global, up_local, MAX_UPSTREAM);
    gather_double_2d(ruh_global, ruh_local, plugin_options.UH_LENGTH);
    gather_double_2d(iuh_global, iuh_local, plugin_options.UH_LENGTH);
    gather_double(run_global, run_local);
    gather_double_2d(dt_dis_global, dt_dis_local,
                     plugin_options.UH_LENGTH + rout_steps_per_dt);
    gather_double(dis_global, dis_local);
    gather_double(stream_global, stream_local);
    if (plugin_options.FORCE_ROUTING) {
        gather_double(force_global, force_local);
    }

    // Calculate
    if (mpi_rank == VIC_MPI_ROOT) {
        for (i = 0; i < global_domain.ncells_active; i++) {
            iCell = routing_order[i];

    	    /* Shift and clear previous discharge data */
            for (j = 0; j < rout_steps_per_dt; j++) {
                dt_dis_global[iCell][0] = 0.0;
                cshift(dt_dis_global[iCell],
                       plugin_options.UH_LENGTH + rout_steps_per_dt + 1, 1, 0, 1);
            }

    	    /* RUNOFF*/
            // Gather runoff from VIC
            runoff = run_global[iCell];
            // Calculate delta-time runoff (equal contribution)
            dt_runoff = runoff / rout_steps_per_dt;
            // Convolute current runoff
            for (j = 0; j < rout_steps_per_dt; j++) {
                convolute(dt_runoff, ruh_global[iCell], dt_dis_global[iCell],
                          plugin_options.UH_LENGTH, j);
            }

    	    /* INFLOW*/
            // Gather inflow from VIC
            inflow = 0.0;
            if (plugin_options.FORCE_ROUTING) {
                inflow += force_global[iCell];
            }
            // Convolute current inflow
            inflow_wb = 0.;
            for (j = 0; j < rout_steps_per_dt; j++) {

            	// Calculate delta-time inflow (equal contribution)
            	dt_inflow = inflow / rout_steps_per_dt;

                for (k = 0; k < nup_global[iCell]; k++) {
                    dt_inflow += dt_dis_global[up_global[iCell][k]][j];
                }
		
                inflow_wb += dt_inflow;
                convolute(dt_inflow, iuh_global[iCell], 
                          dt_dis_global[iCell],
                          plugin_options.UH_LENGTH, j);
            }

            // Aggregate current timestep discharge & stream moisture
            dis_global[iCell] = 0.0;
            prev_stream = stream_global[iCell];
            stream_global[iCell] = 0.0;
            for (j = 0; j < plugin_options.UH_LENGTH + rout_steps_per_dt + 1; j++) {
                if (j < rout_steps_per_dt) {
                    dis_global[iCell] += dt_dis_global[iCell][j];
                }
                else {
                    stream_global[iCell] += dt_dis_global[iCell][j];
                }
            }

            // Check water balance
            if (abs(prev_stream + (inflow_wb + runoff) -
                    (dis_global[iCell] + stream_global[iCell])) >
                DBL_EPSILON) {
                log_err(
                    "Discharge water balance error [%.4f]. "
                    "in: %.4f out: %.4f prev_storage: %.4f cur_storage %.4f",
                    prev_stream + (inflow_wb + runoff) -
                    (dis_global[iCell] + stream_global[iCell]),
                    (inflow_wb + runoff),
                    dis_global[iCell],
                    prev_stream,
                    stream_global[iCell]);
            }
        }
    }

    // Scatter discharge
    scatter_double_2d(dt_dis_global, dt_dis_local,
                      plugin_options.UH_LENGTH + rout_steps_per_dt);
    scatter_double(stream_global, stream_local);
    scatter_double(dis_global, dis_local);

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

    if (plugin_options.FORCE_ROUTING) {
        free(force_global);
        free(force_local);
    }
}
