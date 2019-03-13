#include <vic_driver_image.h>
#include <plugin.h>

/******************************************
 Output initialization
******************************************/
// Set output metadata
void
plugin_set_output_met_data_info(void)
{
    extern plugin_option_struct    plugin_options;
    extern metadata_struct out_metadata[];
    
    size_t v;
    
    // Set missing and/or default values
    for (v = N_OUTVAR_TYPES; v < N_OUTVAR_TYPES + PLUGIN_N_OUTVAR_TYPES; v++) {
        // Set default string values
        snprintf(out_metadata[v].varname, MAXSTRING, "%s", MISSING_S);
        snprintf(out_metadata[v].long_name, MAXSTRING, "%s", MISSING_S);
        snprintf(out_metadata[v].standard_name, MAXSTRING, "%s", MISSING_S);
        snprintf(out_metadata[v].units, MAXSTRING, "%s", MISSING_S);
        snprintf(out_metadata[v].description, MAXSTRING, "%s", MISSING_S);
        // Set default number of elements
        out_metadata[v].nelem = 1;
    }
    
    if(plugin_options.ROUTING)
        rout_set_output_met_data_info();
}

// Initialize outfile dimension size & id
void
plugin_initialize_nc_file(nc_file_struct  *nc_file)
{
    /* Unused variables */
    UNUSED(nc_file);    
    
}

// Add dimensions to outfile
void
plugin_add_hist_dim(nc_file_struct *nc,
                    stream_struct  *stream)
{
    /* Unused variables */
    UNUSED(nc);    
    UNUSED(stream);    
}

// Set output variable dimension count
void
 plugin_set_nc_var_info(unsigned int       varid,
                       unsigned short int dtype,
                       nc_file_struct    *nc_hist_file,
                       nc_var_struct     *nc_var)
{
    
    /* Unused variables */
    UNUSED(varid);

    /* Local variables */    
    size_t i;

    // set datatype
    nc_var->nc_type = get_nc_dtype(dtype);

    for (i = 0; i < MAXDIMS; i++) {
        nc_var->nc_dimids[i] = -1;
        nc_var->nc_counts[i] = 0;
    }

    // Set the number of dimensions and the count sizes
    nc_var->nc_dims = 3;
    nc_var->nc_counts[1] = nc_hist_file->nj_size;
    nc_var->nc_counts[2] = nc_hist_file->ni_size;
    
}

// Set output variable dimension ids
void
plugin_set_nc_var_dimids(unsigned int    varid,
                         nc_file_struct *nc_hist_file,
                         nc_var_struct  *nc_var)
{
    
    /* Unused variables */
    UNUSED(varid);

    /* Local variables */
    size_t i;

    for (i = 0; i < MAXDIMS; i++) {
        nc_var->nc_dimids[i] = -1;
    }
    
    // Set the non-default ones
    nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
    nc_var->nc_dimids[1] = nc_hist_file->nj_dimid;
    nc_var->nc_dimids[2] = nc_hist_file->ni_dimid;
}

void
plugin_get_default_outvar_aggtype(unsigned int varid, unsigned int *agg_type)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.ROUTING)
        rout_history(varid, agg_type);
}
/******************************************
 State file initialization
******************************************/
// Set output metadata
void
plugin_set_state_meta_data_info(void)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.ROUTING)
        rout_set_state_meta_data_info();
    
}

// Initialize outfile dimension size & id
void
plugin_set_nc_state_file_info(nc_file_struct  *nc_state_file)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.ROUTING)
        rout_set_nc_state_file_info(nc_state_file);
}

// Add dimensions to outfile
void
plugin_add_state_dim(char           *filename,
                             nc_file_struct *nc_state_file)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.ROUTING)
        rout_add_state_dim(filename, nc_state_file);
}

// Add dimensions variable to outfile
void
plugin_add_state_dim_var(char           *filename,
                             nc_file_struct *nc_state_file)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.ROUTING)
        rout_add_state_dim_var(filename, nc_state_file);
}

// Add dimensions variable data to outfile
void
plugin_add_state_dim_var_data(char           *filename,
                             nc_file_struct *nc_state_file)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.ROUTING)
        rout_add_state_dim_var_data(filename, nc_state_file);
}

// Set output variable dimension count & ids
void
plugin_set_nc_state_var_info(nc_file_struct *nc)
{
    extern plugin_option_struct    plugin_options;
    
    size_t i;
    size_t j;

    for (i = N_STATE_VARS; i < (N_STATE_VARS + PLUGIN_N_STATE_VARS); i++) {
        nc->nc_vars[i].nc_varid = i;
        
        nc->nc_vars[i].nc_type = NC_DOUBLE;
        for (j = 0; j < MAXDIMS; j++) {
            nc->nc_vars[i].nc_dimids[j] = -1;
            nc->nc_vars[i].nc_counts[j] = 0;
        }
        
        // Set the number of dimensions and dimids for each state variable
        nc->nc_vars[i].nc_dims = 2;
        nc->nc_vars[i].nc_dimids[0] = nc->nj_dimid;
        nc->nc_vars[i].nc_dimids[1] = nc->ni_dimid;
        nc->nc_vars[i].nc_counts[0] = nc->nj_size;
        nc->nc_vars[i].nc_counts[1] = nc->ni_size;
        
        if(plugin_options.ROUTING)
            rout_set_nc_state_var_info(nc, i);
        
        if (nc->nc_vars[i].nc_dims > MAXDIMS) {
            log_err("Too many dimensions specified in variable %zu", i);
        }
    }
}

void
plugin_store(nc_file_struct *state_file)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.ROUTING)
        rout_store(state_file);
}
