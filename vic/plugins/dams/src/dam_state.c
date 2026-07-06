#include <vic_driver_image.h>
#include <plugin.h>
#include <dams.h>

/******************************************
* @brief   Set dam state metadata
******************************************/
void
dam_set_state_meta_data_info(void)
{
    extern metadata_struct state_metadata[];

    // STATE_DAM_ACTIVE
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_ACTIVE].varname, MAXSTRING, "STATE_DAM_ACTIVE");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_ACTIVE].long_name, MAXSTRING, "dam_active");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_ACTIVE].standard_name, MAXSTRING, "dam_active");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_ACTIVE].units, MAXSTRING, "-");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_ACTIVE].description, MAXSTRING, "dam active flag");

    // STATE_DAM_OP_MONTH
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].varname, MAXSTRING, "STATE_DAM_OP_MONTH");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].long_name, MAXSTRING, "dam_op_month");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].standard_name, MAXSTRING, "dam_op_month");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].units, MAXSTRING, "-");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].description, MAXSTRING, "dam operational month");

    // STATE_DAM_STORAGE
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].varname, MAXSTRING, "STATE_DAM_STORAGE");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].long_name, MAXSTRING, "dam_storage");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].standard_name, MAXSTRING, "dam_storage");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].description, MAXSTRING, "dam storage");

    // STATE_DAM_TOTAL_INFLOW
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW].varname, MAXSTRING, "STATE_DAM_TOTAL_INFLOW");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW].long_name, MAXSTRING, "dam_total_inflow");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW].standard_name, MAXSTRING, "dam_total_inflow");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW].description, MAXSTRING, "dam accumulated inflow");

    // STATE_DAM_TOTAL_DEMAND
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND].varname, MAXSTRING, "STATE_DAM_TOTAL_DEMAND");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND].long_name, MAXSTRING, "dam_total_demand");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND].standard_name, MAXSTRING, "dam_total_demand");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND].description, MAXSTRING, "dam accumulated demand");

    // STATE_DAM_TOTAL_EFR
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_EFR].varname, MAXSTRING, "STATE_DAM_TOTAL_EFR");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_EFR].long_name, MAXSTRING, "dam_total_efr");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_EFR].standard_name, MAXSTRING, "dam_total_efr");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_EFR].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_EFR].description, MAXSTRING, "dam accumulated environmental requirments");

    // STATE_DAM_REGISTER_STEPS
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_REGISTER_STEPS].varname, MAXSTRING, "STATE_DAM_REGISTER_STEPS");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_REGISTER_STEPS].long_name, MAXSTRING, "dam_register_steps");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_REGISTER_STEPS].standard_name, MAXSTRING, "dam_register_steps");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_REGISTER_STEPS].units, MAXSTRING, "-");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_REGISTER_STEPS].description, MAXSTRING, "dam number of accumulation steps");

    // STATE_DAM_MONTHS_RUNNING
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING].varname, MAXSTRING, "STATE_DAM_MONTHS_RUNNING");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING].long_name, MAXSTRING, "dam_months_running");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING].standard_name, MAXSTRING, "dam_months_running");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING].units, MAXSTRING, "-");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING].description, MAXSTRING, "dam number of months history is registered");

    // STATE_DAM_HISTORY_INFLOW
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].varname, MAXSTRING, "STATE_DAM_HISTORY_INFLOW");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].long_name, MAXSTRING, "dam_history_inflow");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].standard_name, MAXSTRING, "dam_history_inflow");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].description, MAXSTRING, "dam historical inflow array");

    // STATE_DAM_HISTORY_DEMAND
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].varname, MAXSTRING, "STATE_DAM_HISTORY_DEMAND");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].long_name, MAXSTRING, "dam_history_demand");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].standard_name, MAXSTRING, "dam_history_demand");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].description, MAXSTRING, "dam historical demand array");

    // STATE_DAM_HISTORY_EFR
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].varname, MAXSTRING, "STATE_DAM_HISTORY_EFR");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].long_name, MAXSTRING, "dam_history_efr");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].standard_name, MAXSTRING, "dam_history_efr");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].description, MAXSTRING, "dam historical environmental requirment array");

    // STATE_DAM_OP_RELEASE
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].varname, MAXSTRING, "STATE_DAM_OP_RELEASE");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].long_name, MAXSTRING, "dam_op_release");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].standard_name, MAXSTRING, "dam_op_release");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].description, MAXSTRING, "dam calculated release array");

    // STATE_DAM_OP_STORAGE
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].varname, MAXSTRING, "STATE_DAM_OP_STORAGE");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].long_name, MAXSTRING, "dam_op_storage");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].standard_name, MAXSTRING, "dam_op_storage");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].description, MAXSTRING, "dam calculated storage array");
}

/******************************************
* @brief   Add dam dimensions to state file
******************************************/
void
dam_add_state_dim(char *filename, nc_file_struct *nc_state_file)
{
    extern plugin_option_struct plugin_options;
    int status;

    nc_state_file->dam_size = plugin_options.NDAMTYPES;
    nc_state_file->dam_hist_size = MONTHS_PER_YEAR * DAM_HIST_YEARS;
    nc_state_file->dam_month_size = MONTHS_PER_YEAR;

    status = nc_def_dim(nc_state_file->nc_id, "dam_class",
                        nc_state_file->dam_size,
                        &(nc_state_file->dam_dimid));
    check_nc_status(status, "Error defining dam_class dim in %s", filename);

    status = nc_def_dim(nc_state_file->nc_id, "dam_hist",
                        nc_state_file->dam_hist_size,
                        &(nc_state_file->dam_hist_dimid));
    check_nc_status(status, "Error defining dam_hist dim in %s", filename);

    status = nc_def_dim(nc_state_file->nc_id, "dam_month",
                        nc_state_file->dam_month_size,
                        &(nc_state_file->dam_month_dimid));
    check_nc_status(status, "Error defining dam_month dim in %s", filename);
}

/******************************************
* @brief   Set state file dimension size
******************************************/
void
dam_set_nc_state_file_info(nc_file_struct *nc_state_file)
{
    extern plugin_option_struct plugin_options;

    nc_state_file->dam_size = plugin_options.NDAMTYPES;
    nc_state_file->dam_hist_size = MONTHS_PER_YEAR * DAM_HIST_YEARS;
    nc_state_file->dam_month_size = MONTHS_PER_YEAR;
}

/******************************************
* @brief   Set state variable NC dimensions for dam variables
******************************************/
void
dam_set_nc_state_var_info(nc_file_struct *nc, size_t varid)
{
    switch(varid) {
        case N_STATE_VARS + STATE_DAM_ACTIVE:
        case N_STATE_VARS + STATE_DAM_OP_MONTH:
        case N_STATE_VARS + STATE_DAM_STORAGE:
        case N_STATE_VARS + STATE_DAM_TOTAL_INFLOW:
        case N_STATE_VARS + STATE_DAM_TOTAL_DEMAND:
        case N_STATE_VARS + STATE_DAM_TOTAL_EFR:
        case N_STATE_VARS + STATE_DAM_REGISTER_STEPS:
        case N_STATE_VARS + STATE_DAM_MONTHS_RUNNING:
            nc->nc_vars[varid].nc_dims = 3;
            nc->nc_vars[varid].nc_dimids[0] = nc->dam_dimid;
            nc->nc_vars[varid].nc_dimids[1] = nc->nj_dimid;
            nc->nc_vars[varid].nc_dimids[2] = nc->ni_dimid;
            nc->nc_vars[varid].nc_counts[0] = 1;
            nc->nc_vars[varid].nc_counts[1] = nc->nj_size;
            nc->nc_vars[varid].nc_counts[2] = nc->ni_size;
            break;
            
        case N_STATE_VARS + STATE_DAM_HISTORY_INFLOW:
        case N_STATE_VARS + STATE_DAM_HISTORY_DEMAND:
        case N_STATE_VARS + STATE_DAM_HISTORY_EFR:
            nc->nc_vars[varid].nc_dims = 4;
            nc->nc_vars[varid].nc_dimids[0] = nc->dam_dimid;
            nc->nc_vars[varid].nc_dimids[1] = nc->dam_hist_dimid;
            nc->nc_vars[varid].nc_dimids[2] = nc->nj_dimid;
            nc->nc_vars[varid].nc_dimids[3] = nc->ni_dimid;
            nc->nc_vars[varid].nc_counts[0] = 1;
            nc->nc_vars[varid].nc_counts[1] = 1;
            nc->nc_vars[varid].nc_counts[2] = nc->nj_size;
            nc->nc_vars[varid].nc_counts[3] = nc->ni_size;
            break;

        case N_STATE_VARS + STATE_DAM_OP_RELEASE:
        case N_STATE_VARS + STATE_DAM_OP_STORAGE:
            nc->nc_vars[varid].nc_dims = 4;
            nc->nc_vars[varid].nc_dimids[0] = nc->dam_dimid;
            nc->nc_vars[varid].nc_dimids[1] = nc->dam_month_dimid;
            nc->nc_vars[varid].nc_dimids[2] = nc->nj_dimid;
            nc->nc_vars[varid].nc_dimids[3] = nc->ni_dimid;
            nc->nc_vars[varid].nc_counts[0] = 1;
            nc->nc_vars[varid].nc_counts[1] = 1;
            nc->nc_vars[varid].nc_counts[2] = nc->nj_size;
            nc->nc_vars[varid].nc_counts[3] = nc->ni_size;
            break;
    }
}

/******************************************
* @brief   Store dam state
******************************************/
void
dam_store(nc_file_struct *state_file)
{
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;
    extern dam_con_map_struct  *dam_con_map;
    extern dam_var_struct     **dam_var;

    size_t         i, iDam, m;
    double        *dvar = NULL;
    size_t         d3start[3];
    size_t         d4start[4];
    nc_var_struct *nc_var;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error");

    for (i = 0; i < 3; i++) d3start[i] = 0;
    for (i = 0; i < 4; i++) d4start[i] = 0;

    /* ----- SCALARS ----- */
    for (iDam = 0; iDam < plugin_options.NDAMTYPES; iDam++) {
        d3start[0] = iDam;

        // ACTIVE
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_ACTIVE]);
        for (i = 0; i < local_domain.ncells_active; i++) {
            dvar[i] = (iDam < dam_con_map[i].nd_active) ? (double)dam_var[i][iDam].active : state_file->d_fillvalue;
        }
        gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d3start, nc_var->nc_counts, dvar);

        // OP_MONTH
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_OP_MONTH]);
        for (i = 0; i < local_domain.ncells_active; i++) {
            dvar[i] = (iDam < dam_con_map[i].nd_active) ? (double)dam_var[i][iDam].op_month : state_file->d_fillvalue;
        }
        gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d3start, nc_var->nc_counts, dvar);

        // STORAGE
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_STORAGE]);
        for (i = 0; i < local_domain.ncells_active; i++) {
            dvar[i] = (iDam < dam_con_map[i].nd_active) ? dam_var[i][iDam].storage : state_file->d_fillvalue;
        }
        gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d3start, nc_var->nc_counts, dvar);

        // TOTAL_INFLOW
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW]);
        for (i = 0; i < local_domain.ncells_active; i++) {
            dvar[i] = (iDam < dam_con_map[i].nd_active) ? dam_var[i][iDam].total_inflow : state_file->d_fillvalue;
        }
        gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d3start, nc_var->nc_counts, dvar);

        // TOTAL_DEMAND
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND]);
        for (i = 0; i < local_domain.ncells_active; i++) {
            dvar[i] = (iDam < dam_con_map[i].nd_active) ? dam_var[i][iDam].total_demand : state_file->d_fillvalue;
        }
        gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d3start, nc_var->nc_counts, dvar);

        // TOTAL_EFR
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_TOTAL_EFR]);
        for (i = 0; i < local_domain.ncells_active; i++) {
            dvar[i] = (iDam < dam_con_map[i].nd_active) ? dam_var[i][iDam].total_efr : state_file->d_fillvalue;
        }
        gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d3start, nc_var->nc_counts, dvar);

        // REGISTER_STEPS
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_REGISTER_STEPS]);
        for (i = 0; i < local_domain.ncells_active; i++) {
            dvar[i] = (iDam < dam_con_map[i].nd_active) ? (double)dam_var[i][iDam].register_steps : state_file->d_fillvalue;
        }
        gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d3start, nc_var->nc_counts, dvar);

        // MONTHS_RUNNING
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING]);
        for (i = 0; i < local_domain.ncells_active; i++) {
            dvar[i] = (iDam < dam_con_map[i].nd_active) ? (double)dam_var[i][iDam].months_running : state_file->d_fillvalue;
        }
        gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d3start, nc_var->nc_counts, dvar);
    }

    /* ----- ARRAYS ----- */
    for (iDam = 0; iDam < plugin_options.NDAMTYPES; iDam++) {
        d4start[0] = iDam;

        // HISTORY_INFLOW
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW]);
        for (m = 0; m < state_file->dam_hist_size; m++) {
            d4start[1] = m;
            for (i = 0; i < local_domain.ncells_active; i++) {
                dvar[i] = (iDam < dam_con_map[i].nd_active) ? dam_var[i][iDam].history_inflow[m] : state_file->d_fillvalue;
            }
            gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d4start, nc_var->nc_counts, dvar);
        }

        // HISTORY_DEMAND
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND]);
        for (m = 0; m < state_file->dam_hist_size; m++) {
            d4start[1] = m;
            for (i = 0; i < local_domain.ncells_active; i++) {
                dvar[i] = (iDam < dam_con_map[i].nd_active) ? dam_var[i][iDam].history_demand[m] : state_file->d_fillvalue;
            }
            gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d4start, nc_var->nc_counts, dvar);
        }

        // HISTORY_EFR
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_HISTORY_EFR]);
        for (m = 0; m < state_file->dam_hist_size; m++) {
            d4start[1] = m;
            for (i = 0; i < local_domain.ncells_active; i++) {
                dvar[i] = (iDam < dam_con_map[i].nd_active) ? dam_var[i][iDam].history_efr[m] : state_file->d_fillvalue;
            }
            gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d4start, nc_var->nc_counts, dvar);
        }

        // OP_RELEASE
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_OP_RELEASE]);
        for (m = 0; m < state_file->dam_month_size; m++) {
            d4start[1] = m;
            for (i = 0; i < local_domain.ncells_active; i++) {
                dvar[i] = (iDam < dam_con_map[i].nd_active) ? dam_var[i][iDam].op_release[m] : state_file->d_fillvalue;
            }
            gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d4start, nc_var->nc_counts, dvar);
        }

        // OP_STORAGE
        nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_OP_STORAGE]);
        for (m = 0; m < state_file->dam_month_size; m++) {
            d4start[1] = m;
            for (i = 0; i < local_domain.ncells_active; i++) {
                dvar[i] = (iDam < dam_con_map[i].nd_active) ? dam_var[i][iDam].op_storage[m] : state_file->d_fillvalue;
            }
            gather_put_nc_field_double(state_file->nc_id, nc_var->nc_varid, state_file->d_fillvalue, d4start, nc_var->nc_counts, dvar);
        }
    }

    free(dvar);
}

/******************************************
* @brief   Restore dam state
******************************************/
void
dam_restore(void)
{
    extern domain_struct        global_domain;
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;
    extern dam_con_map_struct  *dam_con_map;
    extern dam_var_struct     **dam_var;
    extern filenames_struct     filenames;
    extern metadata_struct      state_metadata[];

    size_t  i, iDam, m;
    double *dvar = NULL;
    size_t  dstart[4];
    size_t  d3count[3];
    size_t  d4count[4];
    size_t  dam_hist_size = MONTHS_PER_YEAR * DAM_HIST_YEARS;
    size_t  dam_month_size = MONTHS_PER_YEAR;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error");

    for (i = 0; i < 4; i++) dstart[i] = 0;
    
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;
    
    d4count[0] = 1;
    d4count[1] = 1;
    d4count[2] = global_domain.n_ny;
    d4count[3] = global_domain.n_nx;

    /* ----- SCALARS ----- */
    for (iDam = 0; iDam < plugin_options.NDAMTYPES; iDam++) {
        dstart[0] = iDam;
        dstart[1] = 0; // lat
        dstart[2] = 0; // lon

        // ACTIVE
        get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_ACTIVE].varname, dstart, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].active = (bool)dvar[i];
        }

        // OP_MONTH
        get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].varname, dstart, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].op_month = (int)dvar[i];
        }

        // STORAGE
        get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].varname, dstart, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].storage = dvar[i];
        }

        // TOTAL_INFLOW
        get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW].varname, dstart, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].total_inflow = dvar[i];
        }

        // TOTAL_DEMAND
        get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND].varname, dstart, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].total_demand = dvar[i];
        }

        // TOTAL_EFR
        get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_EFR].varname, dstart, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].total_efr = dvar[i];
        }

        // REGISTER_STEPS
        get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_REGISTER_STEPS].varname, dstart, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].register_steps = (size_t)dvar[i];
        }

        // MONTHS_RUNNING
        get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING].varname, dstart, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].months_running = (size_t)dvar[i];
        }
    }

    /* ----- ARRAYS ----- */
    for (iDam = 0; iDam < plugin_options.NDAMTYPES; iDam++) {
        dstart[0] = iDam;
        dstart[2] = 0; // lat
        dstart[3] = 0; // lon

        // HISTORY_INFLOW
        for (m = 0; m < dam_hist_size; m++) {
            dstart[1] = m;
            get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].varname, dstart, d4count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].history_inflow[m] = dvar[i];
            }
        }

        // HISTORY_DEMAND
        for (m = 0; m < dam_hist_size; m++) {
            dstart[1] = m;
            get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].varname, dstart, d4count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].history_demand[m] = dvar[i];
            }
        }

        // HISTORY_EFR
        for (m = 0; m < dam_hist_size; m++) {
            dstart[1] = m;
            get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].varname, dstart, d4count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].history_efr[m] = dvar[i];
            }
        }

        // OP_RELEASE
        for (m = 0; m < dam_month_size; m++) {
            dstart[1] = m;
            get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].varname, dstart, d4count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].op_release[m] = dvar[i];
            }
        }

        // OP_STORAGE
        for (m = 0; m < dam_month_size; m++) {
            dstart[1] = m;
            get_scatter_nc_field_double(&(filenames.init_state), state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].varname, dstart, d4count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iDam < dam_con_map[i].nd_active) dam_var[i][iDam].op_storage[m] = dvar[i];
            }
        }
    }

    free(dvar);
}
