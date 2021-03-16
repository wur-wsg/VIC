/******************************************************************************
 * @section DESCRIPTION
 *
 * Routing state functions
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
* @brief   Set state metadata
******************************************/
void
rout_set_state_meta_data_info(void)
{
    extern metadata_struct state_metadata[];

    snprintf(state_metadata[N_STATE_VARS + STATE_DISCHARGE_DT].varname,
             MAXSTRING, "%s", "STATE_DISCHARGE_DT");
    snprintf(state_metadata[N_STATE_VARS + STATE_DISCHARGE_DT].long_name,
             MAXSTRING, "%s", "discharge_dt");
    snprintf(state_metadata[N_STATE_VARS + STATE_DISCHARGE_DT].standard_name,
             MAXSTRING, "%s", "discharge_dt");
    snprintf(state_metadata[N_STATE_VARS + STATE_DISCHARGE_DT].units, MAXSTRING,
             "%s", "m3/s");
    snprintf(state_metadata[N_STATE_VARS + STATE_DISCHARGE_DT].description,
             MAXSTRING, "%s", "sub-step discharge at the cell outflow point");
}

/******************************************
* @brief   Initialize state dimension size and ids
******************************************/
void
rout_set_nc_state_file_info(nc_file_struct *nc_state_file)
{
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct       plugin_options;

    size_t                            rout_steps_per_dt;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    nc_state_file->rdt_dimid = MISSING;
    nc_state_file->rdt_size = plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
}

/******************************************
* @brief   Add state dimensions
******************************************/
void
rout_add_state_dim(char           *filename,
                   nc_file_struct *nc_state_file)
{
    int status;

    // Define dimension
    status = nc_def_dim(nc_state_file->nc_id, "routing_dt",
                        nc_state_file->rdt_size,
                        &(nc_state_file->rdt_dimid));
    check_nc_status(status, "Error defining lake_node in %s", filename);
}

/******************************************
* @brief   Add state dimension variables
******************************************/
void
rout_add_state_dim_var(char           *filename,
                       nc_file_struct *nc_state_file)
{
    int dimids[MAXDIMS];
    int var_id;
    int status;

    // Initialize
    dimids[0] = nc_state_file->rdt_dimid;

    // Define dimension variable
    status = nc_def_var(nc_state_file->nc_id, "routing_dt",
                        NC_INT, 1, dimids, &(var_id));
    check_nc_status(status, "Error defining routing_dt variable in %s",
                    filename);
    status = nc_put_att_text(nc_state_file->nc_id, var_id,
                             "long_name",
                             strlen("routing_dt"), "routing_dt");
    check_nc_status(status, "Error adding attribute in %s", filename);
    status = nc_put_att_text(nc_state_file->nc_id, var_id,
                             "standard_name",
                             strlen("routing_delta_time"),
                             "routing_delta_time");
    check_nc_status(status, "Error adding attribute in %s", filename);
}

/******************************************
* @brief   Add state dimension variable data
******************************************/
void
rout_add_state_dim_var_data(char           *filename,
                            nc_file_struct *nc_state_file)
{
    size_t dstart[MAXDIMS];
    size_t dcount[MAXDIMS];
    int    var_id;
    int   *ivar;
    int    status;

    size_t i;

    // Initialize
    dstart[0] = 0;
    dcount[0] = nc_state_file->rdt_size;

    status = nc_inq_varid(nc_state_file->nc_id,
                          "routing_dt", &var_id);
    check_nc_status(status, "Unable to find variable \"%s\" in %s",
                    "routing_dt",
                    filename);

    // Fill dimension variable
    ivar = malloc(nc_state_file->rdt_size * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error");

    for (i = 0; i < nc_state_file->rdt_size; i++) {
        ivar[i] = (int) i * plugin_global_param.rout_dt;
    }
    status = nc_put_vara_int(nc_state_file->nc_id, var_id, dstart,
                             dcount, ivar);
    check_nc_status(status, "Error writing routing_dt id in %s",
                    filename);
    free(ivar);
}

/******************************************
* @brief   Set state variable dimensions
******************************************/
void
rout_set_nc_state_var_info(nc_file_struct *nc,
                           size_t          varid)
{
    // Set the number of dimensions and dimids for each state variable
    switch (varid) {
    case N_STATE_VARS + STATE_DISCHARGE_DT:
        nc->nc_vars[varid].nc_dims = 3;
        nc->nc_vars[varid].nc_dimids[0] = nc->rdt_dimid;
        nc->nc_vars[varid].nc_dimids[1] = nc->nj_dimid;
        nc->nc_vars[varid].nc_dimids[2] = nc->ni_dimid;
        nc->nc_vars[varid].nc_counts[0] = 1;
        nc->nc_vars[varid].nc_counts[1] = nc->nj_size;
        nc->nc_vars[varid].nc_counts[2] = nc->ni_size;
    }
}

/******************************************
* @brief   Store states
******************************************/
void
rout_store(nc_file_struct *state_file)
{
    extern domain_struct              local_domain;
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct       plugin_options;
    extern rout_var_struct           *rout_var;

    size_t                            i;
    size_t                            j;
    double                           *dvar = NULL;
    size_t                            d3start[3];
    nc_var_struct                    *nc_var;
    size_t                            rout_steps_per_dt;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    // write state variables

    // allocate memory for variables to be stored
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error");

    // initialize starts and counts
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;

    nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DISCHARGE_DT]);
    for (j = 0; j < plugin_options.UH_LENGTH + rout_steps_per_dt - 1; j++) {
        d3start[0] = j;
        for (i = 0; i < local_domain.ncells_active; i++) {
            dvar[i] = (double) rout_var[i].dt_discharge[j];
        }
        gather_put_nc_field_double(state_file->nc_id,
                                   nc_var->nc_varid,
                                   state_file->d_fillvalue,
                                   d3start, nc_var->nc_counts, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            dvar[i] = state_file->d_fillvalue;
        }
    }

    free(dvar);
}
