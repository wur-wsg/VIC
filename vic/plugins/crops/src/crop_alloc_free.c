#include <vic_driver_image.h>
#include <plugin.h>
void
crop_set_mapping(void)
{
    extern option_struct options;
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct       plugin_options;
    extern crop_con_map_struct *crop_con_map;
    extern veg_con_map_struct  *veg_con_map;
    
    int *ivar;
    double *dvar;
    double Cc_sum;
    
    size_t i;
    size_t j;
    size_t k;
    int iVeg;
    
    size_t  d3count[3];
    size_t  d3start[3];
    size_t  d4count[4];
    size_t  d4start[4];
    
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;
    
    d4start[0] = 0;
    d4start[1] = 0;
    d4start[2] = 0;
    d4start[3] = 0;
    d4count[0] = 1;
    d4count[1] = 1;
    d4count[2] = global_domain.n_ny;
    d4count[3] = global_domain.n_nx;
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");
    
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        crop_con_map[i].nc_active = 0;
    }
    
    // Set vegetation classes
    for(j = 0; j < plugin_options.NCROPTYPES; j++){
        d3start[0] = j;
                        
        get_scatter_nc_field_int(&plugin_filenames.wofost,
                                 "crop_veg_class", d3start, d3count, ivar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (ivar[i] > 0) {
                if((size_t)ivar[i] > options.NVEGTYPES){
                    log_err("Crop %zu veg class %d is larger than number of "
                            "vegetation classes %zu in cell %zu",
                            j, ivar[i], options.NVEGTYPES, i);
                }
                crop_con_map[i].veg_class[j] = ivar[i];
            } else {
                crop_con_map[i].veg_class[j] = NODATA_VEG;
            }
        }
    }
    
    // Set crop coverage
    for(k = 0; k < MONTHS_PER_YEAR; k++) {
        for(j = 0; j < plugin_options.NCROPTYPES; j++){
            d3start[0] = j;

            get_scatter_nc_field_double(&plugin_filenames.wofost,
                                        "Cc", d4start, d4count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                if (dvar[i] > 0) {
                    crop_con_map[i].Cc[j][k] = dvar[i];
                }
            }
        }
    }
    
    // Set crop indexes and nc_active
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < plugin_options.NCROPTYPES; j++){
            iVeg = veg_con_map[i].vidx[crop_con_map[i].veg_class[j]];
            
            Cc_sum = 0;
            for(k = 0; k < MONTHS_PER_YEAR; k++) {
                Cc_sum = crop_con_map[i].Cc[j][k];
            }
            
            if(Cc_sum > 0 && iVeg == NODATA_VEG){
                log_err("Crop %zu coverage %.4f is > 0 for "
                        "empty vegetation tile %zu in cell %zu", 
                         j, Cc_sum, k, i);
            }
            
            crop_con_map[i].cidx[j] = crop_con_map[i].nc_active;
            crop_con_map[i].nc_active++;
        }
    }
    
    free(dvar);
    free(ivar);
}

void
crop_late_alloc(void)
{
    extern option_struct options;
    extern domain_struct              local_domain;
    extern plugin_option_struct       plugin_options;
    extern SimUnit                  ***Grid;
    extern crop_con_map_struct       *crop_con_map;
    extern crop_force_struct    *crop_force;
    extern int                 mpi_rank;
    
    int                        status;

    size_t                            i;
    size_t                            j;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.wofost.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.wofost.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.wofost.nc_filename);
    }
    
    Grid = malloc(local_domain.ncells_active * sizeof(*Grid));
    check_alloc_status(Grid, "Memory allocation error");

    crop_con_map = malloc(local_domain.ncells_active * sizeof(*crop_con_map));
    check_alloc_status(crop_con_map, "Memory allocation error");

    crop_force = malloc(local_domain.ncells_active * sizeof(*crop_force));
    check_alloc_status(crop_force, "Memory allocation error");
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        crop_con_map[i].nc_types = plugin_options.NCROPTYPES;
    
        crop_con_map[i].cidx = malloc(crop_con_map[i].nc_types * sizeof(*crop_con_map[i].cidx));
        check_alloc_status(crop_con_map[i].cidx, "Memory allocation error");

        crop_con_map[i].veg_class = malloc(crop_con_map[i].nc_types * sizeof(*crop_con_map[i].veg_class));
        check_alloc_status(crop_con_map[i].veg_class, "Memory allocation error");

        crop_con_map[i].Cc = malloc(crop_con_map[i].nc_types * sizeof(*crop_con_map[i].Cc));
        check_alloc_status(crop_con_map[i].Cc, "Memory allocation error");
            
        Grid[i] = malloc(options.SNOW_BAND * sizeof(*Grid[i]));
        check_alloc_status(Grid[i], "Memory allocation error");
            
        for(j = 0; j < crop_con_map[i].nc_types; j++){
            crop_con_map[i].Cc[j] = malloc(MONTHS_PER_YEAR * sizeof(*crop_con_map[i].Cc[j]));
            check_alloc_status(crop_con_map[i].Cc[j], "Memory allocation error");
        }
    }
    
    crop_set_mapping();
    
    wofost_late_alloc();
    
    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.wofost.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.wofost.nc_filename);
    }

    crop_initialize_local_structures();
}

void
crop_finalize(void)
{
    extern domain_struct        local_domain;
    extern crop_con_map_struct       *crop_con_map;

    size_t                      i;
    size_t                      j;

    wofost_finalize();
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < crop_con_map[i].nc_types; j++){
            free(crop_con_map[i].Cc[j]);
        }

        free(crop_con_map[i].cidx);
        free(crop_con_map[i].veg_class);
        free(crop_con_map[i].Cc);
    }
    free(crop_con_map);
    free(crop_force);
}
