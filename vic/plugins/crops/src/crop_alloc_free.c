#include <vic_driver_image.h>
#include <plugin.h>

void
crop_set_ncroptypes(void)
{
    extern option_struct options;
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct       plugin_options;
    extern crop_con_map_struct **crop_con_map;
    extern veg_con_map_struct  *veg_con_map;
    
    double *dvar;
    bool present;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    int iVeg;
    
    size_t  d5count[5];
    size_t  d5start[5];
    
    d5start[0] = 0;
    d5start[1] = 0;
    d5start[2] = 0;
    d5start[3] = 0;
    d5start[4] = 0;
    d5count[0] = 1;
    d5count[1] = 1;
    d5count[2] = 1;
    d5count[3] = global_domain.n_ny;
    d5count[4] = global_domain.n_nx;
    
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j< veg_con_map[i].nv_active; j++){
            crop_con_map[i][j].nc_types = plugin_options.NCROPTYPES;
            crop_con_map[i][j].nc_active = 0;
        }
    }
    
    for(l = 0; l < MONTHS_PER_YEAR; l++){
        d5start[0] = l;
        for(j = 0; j < plugin_options.NCROPTYPES; j++){
            d5start[1] = j;
            for(k = 0; k < options.NVEGTYPES; k++){
                d5start[2] = k;
                
                get_scatter_nc_field_double(&plugin_filenames.wofost,
                                          "Cc", d5start, d5count, dvar);

                for (i = 0; i < local_domain.ncells_active; i++) {
                    iVeg = veg_con_map[i].vidx[k];

                    if(iVeg != NODATA_VEG) {
                        crop_con_map[i][iVeg].Cc[j][l] = dvar[i];
                    }
                }
            }
        }
    }
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(k = 0; k < options.NVEGTYPES; k++){
            iVeg = veg_con_map[i].vidx[k];
            if(iVeg != NODATA_VEG) {
                for(j = 0; j < plugin_options.NCROPTYPES; j++){
                    present = false;
                    for(l = 0; l < MONTHS_PER_YEAR; l++){
                        if(crop_con_map[i][iVeg].Cc[j][l] > 0){
                            crop_con_map[i][iVeg].cidx[j] = crop_con_map[i][iVeg].nc_active;
                            crop_con_map[i][iVeg].vidx[j] = iVeg;
                            crop_con_map[i][iVeg].nc_active++;
                            present = true;
                            break;
                        }
                    }
                    if(!present) {
                        crop_con_map[i][iVeg].cidx[j] = NODATA_VEG;
                        crop_con_map[i][iVeg].vidx[j] = NODATA_VEG;
                    }
                }
            }
        }
    }
    
    free(dvar);
}

void
crop_late_alloc(void)
{
    extern option_struct options;
    extern domain_struct              local_domain;
    extern plugin_option_struct       plugin_options;
    extern SimUnit                  ****Grid;
    extern veg_con_map_struct       *veg_con_map;
    extern crop_con_map_struct       **crop_con_map;
    extern crop_force_struct    *crop_force;
    extern int                 mpi_rank;
    extern size_t NF;

    SimUnit *iGrid;
    int                        status;

    size_t                            i;
    size_t                            j;
    size_t                            k;
    size_t                            l;

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
        crop_force[i].CO2 = malloc(NF * sizeof(*crop_force[i].CO2));
        check_alloc_status(crop_force[i].CO2, "Memory allocation error");
        
        crop_con_map[i] = malloc(veg_con_map[i].nv_active * sizeof(*crop_con_map[i]));
        check_alloc_status(crop_con_map[i], "Memory allocation error");
        
        Grid[i] = malloc(veg_con_map[i].nv_active * sizeof(*Grid[i]));
        check_alloc_status(Grid[i], "Memory allocation error");
    
        for(j = 0; j < veg_con_map[i].nv_active; j++) {
            crop_con_map[i][j].cidx = malloc(plugin_options.NCROPTYPES * sizeof(*crop_con_map[i][j].cidx));
            check_alloc_status(crop_con_map[i][j].cidx, "Memory allocation error");

            crop_con_map[i][j].vidx = malloc(plugin_options.NCROPTYPES * sizeof(*crop_con_map[i][j].vidx));
            check_alloc_status(crop_con_map[i][j].vidx, "Memory allocation error");

            crop_con_map[i][j].Cc = malloc(plugin_options.NCROPTYPES * sizeof(*crop_con_map[i][j].Cc));
            check_alloc_status(crop_con_map[i][j].Cc, "Memory allocation error");
            
            for(k = 0; k < plugin_options.NCROPTYPES; k++){
                crop_con_map[i][j].Cc[k] = malloc(MONTHS_PER_YEAR * sizeof(*crop_con_map[i][j].Cc[k]));
                check_alloc_status(crop_con_map[i][j].Cc, "Memory allocation error");
            }
        }
        
        for(j = 0; j < veg_con_map[i].nv_active; j++){
            Grid[i][j] = malloc(options.SNOW_BAND * sizeof(*Grid[i][j]));
            check_alloc_status(Grid[i][j], "Memory allocation error");
        }
    }
    
    crop_set_ncroptypes();
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < veg_con_map[i].nv_active; j++) {
            for(l = 0; l < options.SNOW_BAND; l++){
                Grid[i][j][l] = iGrid = NULL;

                for(k = 0; k < crop_con_map[i][j].nc_active; k++){
                    if(iGrid == NULL){
                        Grid[i][j][l] = malloc(sizeof(*Grid[i][j][l]));
                        check_alloc_status(Grid[i][j][l], "Memory allocation error");
                        iGrid = Grid[i][j][l];
                    } else {
                        iGrid->next= malloc(sizeof(*iGrid->next));
                        check_alloc_status(iGrid->next, "Memory allocation error");
                        iGrid = iGrid->next;
                    }

                    iGrid->crp = malloc(sizeof(*(iGrid->crp)));
                    check_alloc_status(iGrid->crp, "Memory allocation error");
                    iGrid->ste = malloc(sizeof(*(iGrid->ste)));
                    check_alloc_status(iGrid->ste, "Memory allocation error");
                    iGrid->mng = malloc(sizeof(*(iGrid->mng)));
                    check_alloc_status(iGrid->mng, "Memory allocation error");
                    iGrid->soil = malloc(sizeof(*(iGrid->soil)));
                    check_alloc_status(iGrid->soil, "Memory allocation error");
                    iGrid->vic = malloc(sizeof(*(iGrid->vic)));
                    check_alloc_status(iGrid->vic, "Memory allocation error");
                    iGrid->next = NULL;
                }
            }
        }
    }
    
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
    extern option_struct options;
    extern plugin_option_struct plugin_options;
    extern domain_struct        local_domain;
    extern SimUnit                  ****Grid;
    extern crop_con_map_struct       **crop_con_map;
    extern veg_con_map_struct       *veg_con_map;

    size_t                      i;
    size_t                      j;
    size_t k;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < veg_con_map[i].nv_active; j++){ 
            for(k = 0; k < plugin_options.NCROPTYPES; k++){
                free(crop_con_map[i][j].Cc[k]);
            }
            for(k = 0; k < options.SNOW_BAND; k++){ 
                Clean(Grid[i][j][k]);
            }
            
            free(crop_con_map[i][j].cidx);
            free(crop_con_map[i][j].vidx);
            free(crop_con_map[i][j].Cc);
            free(Grid[i][j]);
        }
        
        free(Grid[i]);
        free(crop_con_map[i]);
        free(crop_force[i].CO2);
    }
    free(Grid);
    free(crop_force);
    free(crop_con_map);
}
