#include <vic_driver_image.h>
#include <routing.h>

#include "plugin.h"

void
rout_forcing(void)
{
    extern size_t              current;
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern domain_struct global_domain;
    extern dmy_struct         *dmy;
    extern plugin_filenames_struct plugin_filenames;
    extern rout_force_struct *rout_force;
    extern size_t NF;
    extern int mpi_rank;
    
    int status;
    
    double *dvar;
    
    size_t  d3count[3];
    size_t  d3start[3];
    
    size_t i;
    size_t j;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error."); 
    
    // Open forcing file if it is the first time step
    if (current == 0 ) {
        if (mpi_rank == VIC_MPI_ROOT) {  
            // open new forcing file
            sprintf(plugin_filenames.routing_forcing.nc_filename, "%s%4d.nc",
                    plugin_filenames.rf_path_pfx, dmy[current].year);        
            status = nc_open(plugin_filenames.routing_forcing.nc_filename, NC_NOWRITE,
                             &(plugin_filenames.routing_forcing.nc_id));
            check_nc_status(status, "Error opening %s",
                            plugin_filenames.routing_forcing.nc_filename);
        }
    }
    // Open forcing file if it is a new year
    else if (current > 0 && dmy[current].year != dmy[current - 1].year) {
        if (mpi_rank == VIC_MPI_ROOT) {            
            // close previous forcing file
            status = nc_close(plugin_filenames.routing_forcing.nc_id);
            check_nc_status(status, "Error closing %s",
                            plugin_filenames.routing_forcing.nc_filename);

            // open new forcing file
            sprintf(plugin_filenames.routing_forcing.nc_filename, "%s%4d.nc",
                    plugin_filenames.rf_path_pfx, dmy[current].year);        
            status = nc_open(plugin_filenames.routing_forcing.nc_filename, NC_NOWRITE,
                             &(plugin_filenames.routing_forcing.nc_id));
            check_nc_status(status, "Error opening %s",
                            plugin_filenames.routing_forcing.nc_filename);
        }
    }

    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    // Get forcing data
    for (j = 0; j < NF; j++) {
        d3start[0] = global_param.forceskip[0] +
                     global_param.forceoffset[0] + j - NF;

        get_scatter_nc_field_double(&(plugin_filenames.routing_forcing), 
            "discharge", d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {            
            rout_force[i].discharge[j] = dvar[i];
        }
    }

    // Average forcing data
    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_force[i].discharge[NR] = average(rout_force[i].discharge, NF);
    }   

    // Close forcing file if it is the last time step
    if (current == global_param.nrecs - 1) {
        if (mpi_rank == VIC_MPI_ROOT) {         
            // close previous forcing file
            status = nc_close(plugin_filenames.routing_forcing.nc_id);
            check_nc_status(status, "Error closing %s",
                            plugin_filenames.routing_forcing.nc_filename);
        }
    }
    
    free(dvar);
}