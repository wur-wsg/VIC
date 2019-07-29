#include <vic_driver_image.h>
#include <plugin.h>

/******************************************
   Forcing
******************************************/
void
plugin_force(void)
{
    extern plugin_option_struct       plugin_options;
    extern global_param_struct global_param;
    extern size_t              current;
    extern dmy_struct         *dmy;
    extern plugin_filenames_struct plugin_filenames;
    extern int mpi_rank;
    
    int status;
    size_t f;
    
    for(f = 0; f < PLUGIN_N_FORCING_TYPES; f++){
        if(strcasecmp(plugin_filenames.f_path_pfx[f], MISSING_S) == 0){
            continue;
        }
        
        // Open forcing file if it is the first time step
        if (current == 0 ) {
            if (mpi_rank == VIC_MPI_ROOT) {  
                // open new forcing file
                sprintf(plugin_filenames.forcing[f].nc_filename, "%s%4d.nc",
                        plugin_filenames.f_path_pfx[f], dmy[current].year);        
                status = nc_open(plugin_filenames.forcing[f].nc_filename, NC_NOWRITE,
                                 &(plugin_filenames.forcing[f].nc_id));
                check_nc_status(status, "Error opening %s",
                                plugin_filenames.forcing[f].nc_filename);
            }
        }
        // Open forcing file if it is a new year
        else if (current > 0 && dmy[current].year != dmy[current - 1].year) {
            if (mpi_rank == VIC_MPI_ROOT) {            
                // close previous forcing file
                status = nc_close(plugin_filenames.forcing[f].nc_id);
                check_nc_status(status, "Error closing %s",
                                plugin_filenames.forcing[f].nc_filename);

                // open new forcing file
                sprintf(plugin_filenames.forcing[f].nc_filename, "%s%4d.nc",
                        plugin_filenames.f_path_pfx[f], dmy[current].year);        
                status = nc_open(plugin_filenames.forcing[f].nc_filename, NC_NOWRITE,
                                 &(plugin_filenames.forcing[f].nc_id));
                check_nc_status(status, "Error opening %s",
                                plugin_filenames.forcing[f].nc_filename);
            }
        }
    }
    
    if (plugin_options.ROUTING && plugin_options.FORCE_ROUTING) {
        rout_forcing();
    }
    
    for(f = 0; f < PLUGIN_N_FORCING_TYPES; f++){
        if(strcasecmp(plugin_filenames.f_path_pfx[f], MISSING_S) == 0){
            continue;
        }
        
        // Close forcing file if it is the last time step
        if (current == global_param.nrecs - 1) {
            if (mpi_rank == VIC_MPI_ROOT) {         
                // close previous forcing file
                status = nc_close(plugin_filenames.forcing[f].nc_id);
                check_nc_status(status, "Error closing %s",
                                plugin_filenames.forcing[f].nc_filename);
            }
        }
    }
}

/******************************************
   Running
******************************************/
void
plugin_run(void)
{
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;
    extern size_t              *routing_order;

    size_t                      i;
    size_t                      iCell;

    if (plugin_options.ROUTING) {
        if (plugin_options.DECOMPOSITION == BASIN_DECOMPOSITION ||
            plugin_options.DECOMPOSITION == FILE_DECOMPOSITION) {
            for (i = 0; i < local_domain.ncells_active; i++) {
                iCell = routing_order[i];

                rout_basin_run(iCell);
            }
        }
        else if (plugin_options.DECOMPOSITION == RANDOM_DECOMPOSITION) {
            rout_random_run();
        }
    }
}

void
plugin_put_data()
{
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;

    size_t                      i;

    // If running with OpenMP, run this for loop using multiple threads
    #pragma omp parallel for default(shared) private(i)
    for (i = 0; i < local_domain.ncells_active; i++) {
        if (plugin_options.ROUTING) {
            rout_put_data(i);
        }
    }
}
