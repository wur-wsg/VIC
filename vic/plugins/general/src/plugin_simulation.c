#include <vic_driver_image.h>
#include <plugin.h>

/******************************************
 Forcing
******************************************/
void
plugin_force(void)
{
    extern plugin_option_struct       plugin_options;
    
    if(plugin_options.ROUTING && plugin_options.FORCE_ROUTING){
        rout_forcing();
    }
}

/******************************************
 Running
******************************************/
void
plugin_run(void)
{
    extern domain_struct       local_domain;
    extern plugin_option_struct       plugin_options;
    extern size_t             *routing_order;
    
    size_t                     i;
    size_t                     cur_cell;
    
    if(plugin_options.ROUTING){
        if(plugin_options.DECOMPOSITION == BASIN_DECOMPOSITION ||
                plugin_options.DECOMPOSITION == FILE_DECOMPOSITION) {
            for (i = 0; i < local_domain.ncells_active; i++) {
                cur_cell = routing_order[i];
                
                rout_basin_run(cur_cell);
            }
        } 
        else if (plugin_options.DECOMPOSITION == RANDOM_DECOMPOSITION){
            rout_random_run();
        }
    }
}

void
plugin_put_data()
{
    extern domain_struct       local_domain;
    extern plugin_option_struct       plugin_options;
    
    size_t                     i;
    
    // If running with OpenMP, run this for loop using multiple threads
    #pragma omp parallel for default(shared) private(i)
    for (i = 0; i < local_domain.ncells_active; i++) {
        if(plugin_options.ROUTING){
            rout_put_data(i);
        }
    }
}