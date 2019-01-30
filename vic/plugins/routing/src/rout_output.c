#include <vic_driver_image.h>
#include <plugin.h>

void
rout_set_nc_state_file_info(nc_file_struct  *nc_state_file)
{
    extern global_param_struct global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct plugin_options;
    
    size_t                     rout_steps_per_dt;
        
    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                          global_param.model_steps_per_day;
    
    nc_state_file->rdt_dimid = MISSING;
    nc_state_file->rdt_size = plugin_options.UH_LENGTH + rout_steps_per_dt;
}

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

void
rout_add_state_dim_var(char           *filename,
                           nc_file_struct *nc_state_file)
{
    int                        dimids[MAXDIMS];
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

void
rout_add_state_dim_var_data(char           *filename,
                           nc_file_struct *nc_state_file)
{
    size_t                     dstart[MAXDIMS];
    size_t                     dcount[MAXDIMS];
    int var_id;
    int *ivar;
    int status;
    
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

void
rout_set_nc_state_var_info(nc_file_struct *nc, size_t varid)
{
    
    // Set the number of dimensions and dimids for each state variable
    switch(varid){
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

void
rout_set_output_met_data_info(void)
{
    extern metadata_struct out_metadata[];
    
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].varname, "OUT_DISCHARGE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].long_name, "discharge");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].standard_name, "discharge");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].units, "m3/s");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].description, "discharge at the cell outflow point");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].varname, "OUT_STREAM_MOIST");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].long_name, "stream_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].standard_name, "stream storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].description, "moisture storage in stream flow");

    out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].nelem = 1;
}

void
rout_set_state_meta_data_info(void)
{
    extern metadata_struct state_metadata[];
    
    strcpy(state_metadata[N_STATE_VARS + STATE_DISCHARGE_DT].varname, "STATE_DISCHARGE_DT");
    strcpy(state_metadata[N_STATE_VARS + STATE_DISCHARGE_DT].long_name, "discharge_dt");
    strcpy(state_metadata[N_STATE_VARS + STATE_DISCHARGE_DT].standard_name, "discharge_dt");
    strcpy(state_metadata[N_STATE_VARS + STATE_DISCHARGE_DT].units, "m3/s");
    strcpy(state_metadata[N_STATE_VARS + STATE_DISCHARGE_DT].description, "sub-step discharge at the cell outflow point");
}

void
rout_store(nc_file_struct *state_file)
{
    extern domain_struct       local_domain;
    extern global_param_struct global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct plugin_options;
    extern rout_var_struct *rout_var;

    size_t                     i;
    size_t                     j;
    double                    *dvar = NULL;
    size_t                     d3start[3];
    nc_var_struct             *nc_var;
    size_t                     rout_steps_per_dt;
        
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
    for (j = 0; j < plugin_options.UH_LENGTH + rout_steps_per_dt; j++) {
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