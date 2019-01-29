#include <vic_driver_image.h>

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

void
plugin_put_data(void)
{
    extern domain_struct       local_domain;
    
    size_t                     i;
    
    // If running with OpenMP, run this for loop using multiple threads
    #pragma omp parallel for default(shared) private(i)
    for (i = 0; i < local_domain.ncells_active; i++) {
        
    }
}