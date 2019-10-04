#include <vic_driver_image.h>
#include <plugin.h>

void
crop_check_mapping(void)
{
    extern domain_struct local_domain;
    extern plugin_option_struct       plugin_options;
    extern crop_con_map_struct *crop_con_map;
    
    char locstr[MAXSTRING];
    double Cc_sum;
    
    size_t i;
    size_t j;
    size_t k;
    
    // Check crop coverage
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(k = 0; k < MONTHS_PER_YEAR; k++) {
            
            Cc_sum = 0;
            for(j = 0; j < plugin_options.NCROPTYPES; j++){
                Cc_sum = crop_con_map[i].Cc[j][k];
            }
            
            // If the sum of the tile fractions is not within a tolerance,
            // readjust Ccs to sum to 1.0
            for(j = 0; j < plugin_options.NCROPTYPES; j++){
                crop_con_map[i].Cc[j][k] /= Cc_sum;
            }
            if (!assert_close_double(Cc_sum, 1., 0., AREA_SUM_ERROR_THRESH)) {
                sprint_location(locstr, &(local_domain.locations[i]));
                log_warn("Sum of veg tile area fractions !=  1.0 (%.16f) at grid "
                         "cell %zd. Adjusting fractions ...\n%s", Cc_sum, i,
                         locstr);
            }
        }
    }
}

void
crop_init(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern int                     mpi_rank;

    int                            status;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.wofost.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.wofost.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.wofost.nc_filename);
    }
    
    crop_check_mapping();
    wofost_set_data_text(plugin_filenames.wofost_text);

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.wofost.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.wofost.nc_filename);
    }
}
