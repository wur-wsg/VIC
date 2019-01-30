#include <vic_driver_image.h>
#include <plugin.h>

/******************************************
 Global parameters & parameters
******************************************/
bool
plugin_get_global_param(char cmdstr[MAXSTRING])
{
    if(rout_get_global_param(cmdstr)){} 
    else {
        return false;
    }
    
    return true;
}

void
plugin_validate_global_param(void)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.ROUTING)
        rout_validate_global_param();
}

bool
plugin_get_parameters(char optstr[MAXSTRING])
{
    return false;
}

void
plugin_validate_parameters(void)
{
    extern plugin_option_struct    plugin_options;
    
}

/******************************************
 Start
******************************************/
void
plugin_mpi_map_decomp_domain(size_t   ncells,
                             size_t   mpi_size,
                             int    **mpi_map_local_array_sizes,
                             int    **mpi_map_global_array_offsets,
                             size_t **mpi_map_mapping_array)
{
    
}

void
plugin_start(void)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.ROUTING)
        rout_start();
}

/******************************************
 Allocation
******************************************/
void
plugin_alloc(void)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.ROUTING)
        rout_alloc();
}

/******************************************
 Initialization
******************************************/
void
plugin_init(void)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.ROUTING)
        rout_init();
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
 Finalization
******************************************/
void
plugin_finalize(void)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.ROUTING) 
        rout_finalize();
}
