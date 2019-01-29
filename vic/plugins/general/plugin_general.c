#include <vic_driver_image.h>

/******************************************
 Global parameters & parameters
******************************************/
bool
plugin_get_global_param(char optstr[MAXSTRING])
{
    return false;
}

void
plugin_validate_global_param(void)
{
    
}

bool
plugin_get_parameters(void)
{
    return false;
}

void
plugin_validate_parameters(void)
{
    
}

/******************************************
 Start
******************************************/
void
plugin_compare_ncdomain_with_global_domain(void)
{
    
}

void
plugin_get_forcing_file_info(void)
{
    
}

void
plugin_start(void)
{
    
}

/******************************************
 Allocation
******************************************/
void
plugin_alloc(void)
{
    
}

/******************************************
 Initialization
******************************************/
void
plugin_init(void)
{
    
}

/******************************************
 Population
******************************************/
void
plugin_generate_default_state(void)
{
    
}

void
plugin_compute_derived_state_vars(void)
{
    
}

/******************************************
 Restoration
******************************************/
void
plugin_check_init_state_file(void)
{
    
}

void
plugin_restore(void)
{
    
}

/******************************************
 Output initialization
******************************************/
// Set output metadata
void
plugin_set_output_met_data_info(void)
{
    
}

// Add dimensions to outfile
void
plugin_add_history_dimensions(nc_file_struct *nc,
                              stream_struct  *stream)
{
    
}

// Initialize outfile dimensions
void
plugin_initialize_nc_file(nc_file_struct  *nc_file)
{
    
}

// Set output variable dimensions
void
plugin_set_nc_var_info(nc_file_struct    *nc_hist_file,
                       nc_var_struct     *nc_var)
{
    
}

// Set output variable dimension ids
void
plugin_set_nc_var_dimids(nc_file_struct    *nc_hist_file,
                         nc_var_struct     *nc_var)
{
    
}

/******************************************
 State file initialization
******************************************/
// Set output metadata
void
plugin_set_state_meta_data_info(void)
{
    
}

// Add dimensions to outfile
void
plugin_add_state_dimensions(nc_file_struct *nc,
                            stream_struct  *stream)
{
    
}

// Initialize outfile dimensions
void
plugin_set_nc_state_file_info(nc_file_struct  *nc_state_file)
{
    
}

// Set output variable dimension count & ids
void
plugin_set_nc_state_var_info(nc_var_struct     *nc_var)
{
    
}

/******************************************
 Printing
******************************************/
void
plugin_print_global_param(global_param_struct *gp)
{
    
}

void
plugin_print_options(option_struct *option)
{
    
}

void
plugin_print_parameters(parameters_struct *param)
{
    
}

/******************************************
 Forcing
******************************************/
void
plugin_force(void)
{
    
}

/******************************************
 Running
******************************************/
void
plugin_run(void)
{
    
}

/******************************************
 Writing
******************************************/
void
plugin_get_default_outvar_aggtype(unsigned int varid)
{
    
}

/******************************************
 Finalization
******************************************/
void
plugin_finalize(void)
{
    
}
