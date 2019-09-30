#include <vic_driver_image.h>
#include <plugin.h>

void
crop_check_mapping(void)
{
    extern option_struct options;
    extern domain_struct local_domain;
    extern plugin_option_struct       plugin_options;
    extern crop_con_map_struct **crop_con_map;
    extern veg_con_map_struct  *veg_con_map;
    
    double **Cc_sum;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    size_t m;
    int iVeg;
    int iCrop;
    
    Cc_sum = malloc(options.NVEGTYPES * sizeof(*Cc_sum));
    check_alloc_status(Cc_sum, "Memory allocation error.");
    for(j = 0; j < options.NVEGTYPES; j++){
        Cc_sum[j] = malloc(MONTHS_PER_YEAR * sizeof(*Cc_sum[j]));
        check_alloc_status(Cc_sum[j], "Memory allocation error.");
        for(m = 0; m < MONTHS_PER_YEAR; m++){
            Cc_sum[j][m] = 0.;
        }
    }
    
    for(i = 0; i < local_domain.ncells_active; i++){
        for(j = 0; j < options.NVEGTYPES; j++){
            iVeg = veg_con_map[i].vidx[j];
            l = 0;
            
            if(iVeg != NODATA_VEG){
                for(k = 0; k < plugin_options.NCROPTYPES; k++) {
                    iCrop = crop_con_map[i][iVeg].cidx[k];
                    
                    if(iCrop != NODATA_VEG) {
                        for(m = 0; m < MONTHS_PER_YEAR; m++){
                            Cc_sum[j][m] += crop_con_map[i][iVeg].Cc[k][m];
                        }
                        l++;
                    }
                }

                // check the number of nonzero crop tiles
                if (l > crop_con_map[i][iVeg].nc_active) {
                    log_err("Number of crop tiles with nonzero area (%zu) > nc_active "
                            "(%zu)", l, crop_con_map[i][iVeg].nc_active);
                }
                else if (l < crop_con_map[i][iVeg].nc_active) {
                    log_err("Number of crop tiles with nonzero area (%zu) < nc_active "
                            "(%zu)", l, crop_con_map[i][iVeg].nc_active);
                }
            }
        }
        
        for (j = 0; j < options.NVEGTYPES; j++) {
            iVeg = veg_con_map[i].vidx[j];
            
            if(iVeg != NODATA_VEG){
                for(m = 0; m < MONTHS_PER_YEAR; m++){
                    if (Cc_sum[j][m] - 1 > AREA_SUM_ERROR_THRESH) {
                        log_warn("Sum of crop tile area fractions !=  1.0 (%.16f) at grid "
                                 "cell %zd, veg class %zd. Adjusting fractions ...", Cc_sum[j][m], i, j);
                        for(k = 0; k < plugin_options.NCROPTYPES; k++) {
                            iCrop = crop_con_map[i][iVeg].cidx[k];

                            if(iCrop != NODATA_VEG) {
                                crop_con_map[i][iVeg].Cc[k][m] /= Cc_sum[j][m];
                            }
                        }
                    }
                }
            }
        }
    }
    
    for(j = 0; j < options.NVEGTYPES; j++){
        free(Cc_sum[j]);
    }
    free(Cc_sum);
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
