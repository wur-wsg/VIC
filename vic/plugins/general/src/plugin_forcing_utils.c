#include <vic_driver_image.h>
#include <plugin.h>

bool
plugin_force_get_global_param(char *cmdstr)
{
    char                              optstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("PLUGIN_FORCE_TYPE", optstr) == 0) {
        plugin_set_force_type(cmdstr);
        return true;
    }
    
    return false;
}

void
plugin_force_validate_global_param(void)
{
    extern global_param_struct global_param;
    extern plugin_filenames_struct plugin_filenames;

    int status;
    
    size_t i;
    
    for (i = 0; i < PLUGIN_N_FORCING_TYPES; i++) {
        // Validate forcing files and variables
        if (strcmp(plugin_filenames.f_path_pfx[i], MISSING_S) == 0) {
            continue;
        }

        // Get information from the forcing file(s)
        // Open first-year forcing files and get info
        snprintf(plugin_filenames.forcing[i].nc_filename, MAXSTRING, "%s%4d.nc",
                 plugin_filenames.f_path_pfx[i], global_param.startyear);
        status = nc_open(plugin_filenames.forcing[i].nc_filename, NC_NOWRITE,
                         &(plugin_filenames.forcing[i].nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.forcing[i].nc_filename);
        
        plugin_get_forcing_file_info(i);
        
        // Close first-year forcing files
        status = nc_close(plugin_filenames.forcing[i].nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.forcing[i].nc_filename);
    }
}

void
plugin_force_init(void) 
{
    extern global_param_struct global_param;
    extern plugin_filenames_struct plugin_filenames;

    int status;
    
    size_t i;
    
    for (i = 0; i < PLUGIN_N_FORCING_TYPES; i++) {
        // Validate forcing files and variables
        if (strcmp(plugin_filenames.f_path_pfx[i], MISSING_S) == 0) {
            continue;
        }

        // Get information from the forcing file(s)
        // Open first-year forcing files and get info
        snprintf(plugin_filenames.forcing[i].nc_filename, MAXSTRING, "%s%4d.nc",
                 plugin_filenames.f_path_pfx[i], global_param.startyear);
        status = nc_open(plugin_filenames.forcing[i].nc_filename, NC_NOWRITE,
                         &(plugin_filenames.forcing[i].nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.forcing[i].nc_filename);
        
        plugin_get_forcing_file_skip(i);
        compare_ncdomain_with_global_domain(&plugin_filenames.forcing[i]);
        
        // Close first-year forcing files
        status = nc_close(plugin_filenames.forcing[i].nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.forcing[i].nc_filename);
    }
}

void
plugin_force_start(void)
{
    extern global_param_struct global_param;
    extern plugin_global_param_struct plugin_global_param;
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
            plugin_global_param.forcerun[f] = true;
        }
        // Open forcing file if it is a new year
        else if (current > 0 && dmy[current].year != dmy[current - 1].year) {
            // global_param.forceoffset resets every year since the met file restarts
            // every year
            // global_param.forceskip should also reset to 0 after the first year
            plugin_global_param.forceoffset[f] = 0;
            plugin_global_param.forceskip[f] = 0;
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
}

void
plugin_force_end(void)
{
    extern global_param_struct global_param;
    extern plugin_global_param_struct plugin_global_param;
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
        
        // Close forcing file if it is the last time step
        if (current == global_param.nrecs - 1) {
            if (mpi_rank == VIC_MPI_ROOT) {         
                // close previous forcing file
                status = nc_close(plugin_filenames.forcing[f].nc_id);
                check_nc_status(status, "Error closing %s",
                                plugin_filenames.forcing[f].nc_filename);
            }
        }

        // Update the offset counter and run flag
        plugin_global_param.forcerun[f] = false;
        if (plugin_global_param.forcefreq[f] == FORCE_STEP) {
            plugin_global_param.forceoffset[f] += NF;
            plugin_global_param.forcerun[f] = true;
        } else if (plugin_global_param.forcefreq[f] == FORCE_DAY) {
            if(current != global_param.nrecs - 1 && dmy[current].day != dmy[current + 1].day){
                plugin_global_param.forceoffset[f] += 1;
                plugin_global_param.forcerun[f] = true;
            }
        } else if (plugin_global_param.forcefreq[f] == FORCE_MONTH) {
            if(current != global_param.nrecs - 1 && dmy[current].month != dmy[current + 1].month){
                plugin_global_param.forceoffset[f] += 1;
                plugin_global_param.forcerun[f] = true;
            }
        } else if (plugin_global_param.forcefreq[f] == FORCE_YEAR) {
            if(current != global_param.nrecs - 1 && dmy[current].year != dmy[current + 1].year){
                plugin_global_param.forceoffset[f] += 1;
                plugin_global_param.forcerun[f] = true;
            }
        }
    }
}