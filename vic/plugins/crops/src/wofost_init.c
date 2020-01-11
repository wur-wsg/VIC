#include <vic_driver_image.h>
#include <plugin.h>

void
wofost_set_data_text(char *list)
{
    extern option_struct options;
    extern domain_struct local_domain;
    extern plugin_option_struct       plugin_options;
    extern crop_con_map_struct *crop_con_map;
    extern SimUnit ***Grid;
    
    FILE *ifp;
    int Emergence;
    char line[MAXSTRING];
    char path[MAXSTRING];
    char cf[MAXSTRING], mf[MAXSTRING];
        
    size_t i;
    size_t k;
    size_t m;
    size_t l;
    int iCrop;
    SimUnit *iGrid;
    SimUnit *tmpGrid;

    if ((ifp = fopen(list, "r")) == NULL) {
        log_err("Can't open wofost text file, %s\n", list);
    }

    k = 0;
    while(fgets(line, MAXSTRING, ifp)) {
        if(line[0] == '*' || line[0] == ' ' || line[0] == '\n' || line[0] == '\r'){
            continue;
        }

        tmpGrid = malloc(sizeof(*tmpGrid));
        check_alloc_status(tmpGrid, "Memory allocation error");
        tmpGrid->crp = malloc(sizeof(*(tmpGrid->crp)));
        check_alloc_status(tmpGrid->crp, "Memory allocation error");
        tmpGrid->ste = malloc(sizeof(*(tmpGrid->ste)));
        check_alloc_status(tmpGrid->ste, "Memory allocation error");
        tmpGrid->mng = malloc(sizeof(*(tmpGrid->mng)));
        check_alloc_status(tmpGrid->mng, "Memory allocation error");
        tmpGrid->soil = malloc(sizeof(*(tmpGrid->soil)));
        check_alloc_status(tmpGrid->soil, "Memory allocation error");
        tmpGrid->met = malloc(sizeof(*(tmpGrid->met)));
        check_alloc_status(tmpGrid->met, "Memory allocation error");
        tmpGrid->next = NULL;

        initialize_wofost_grid(tmpGrid);
    
        sscanf(line, "%s %s %s %d", path, cf, mf, &Emergence);

        if(k >= plugin_options.NCROPTYPES) {
            log_err("Crops in wofost text (%zu) file are more than "
                    "crops in wofost netcdf file (%d)",
                    k, plugin_options.NCROPTYPES);
        }

        GetSimInput(path, cf, mf, Emergence, k, tmpGrid);
        tmpGrid->met->crop_class = k;
    
        for(i = 0; i < local_domain.ncells_active; i++) {
            iCrop = crop_con_map[i].cidx[k];

            if(iCrop != NODATA_VEG){
                tmpGrid->met->Latitude = local_domain.locations[i].latitude;
                tmpGrid->met->Longitude = local_domain.locations[i].longitude;
                    
                for(l = 0; l < options.SNOW_BAND; l++){
                    iGrid = Grid[i][l];
                    for(m = 0; m < (size_t)iCrop; m++){
                        iGrid = iGrid->next;
                    }
                    
                    copy_wofost_grid(tmpGrid, iGrid);
                }
            }
        }

        Clean(tmpGrid);
        k++;
    }

    if(k + 1 < plugin_options.NCROPTYPES) {
        log_err("Crops in wofost text file are less than "
                "crops in wofost netcdf file");
    }

    fclose(ifp);
}

void
wofost_set_data(void)
{
    extern option_struct options;
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct       plugin_options;
    extern crop_con_map_struct *crop_con_map;
    extern SimUnit ***Grid;
    
    double *dvar;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    int iCrop;
    SimUnit *iGrid;
    
    size_t  d3count[3];
    size_t  d3start[3];
    
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;
    
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");
    
    
    // Set plant day
    for(j = 0; j < plugin_options.NCROPTYPES; j++){
        d3start[0] = j;
                        
        get_scatter_nc_field_double(&plugin_filenames.crop,
                                 "plant_day", d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            iCrop = crop_con_map[i].cidx[j];
            if (iCrop != NODATA_VEG) {
                for (k = 0; k < options.SNOW_BAND; k++) {
                    iGrid = Grid[i][k];
                    for(l = 0; l < (size_t)iCrop; l++){
                        iGrid = iGrid->next;
                    }
                    
                    // plant is set to a random non-leap year
                    dmy_doy(dvar[i], 1900, global_param.calendar, &(iGrid->start));
                }
            }
        }
    }
    
    // Set harvest day
    for(j = 0; j < plugin_options.NCROPTYPES; j++){
        d3start[0] = j;
                        
        get_scatter_nc_field_double(&plugin_filenames.crop,
                                 "harvest_day", d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            iCrop = crop_con_map[i].cidx[j];
            if (iCrop != NODATA_VEG) {
                for (k = 0; k < options.SNOW_BAND; k++) {
                    iGrid = Grid[i][k];
                    for(l = 0; l < (size_t)iCrop; l++){
                        iGrid = iGrid->next;
                    }
                    
                    // harvest is set to a random non-leap year
                    dmy_doy(dvar[i], 1900, global_param.calendar, &(iGrid->end));
                }
            }
        }
    }
    
    // Set cycle length
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < plugin_options.NCROPTYPES; j++){
            iCrop = crop_con_map[i].cidx[j];
            if (iCrop != NODATA_VEG) {
                for (k = 0; k < options.SNOW_BAND; k++) {
                    iGrid = Grid[i][k];
                    for(l = 0; l < (size_t)iCrop; l++){
                        iGrid = iGrid->next;
                    }
                    
                    if(iGrid->start.day_in_year < iGrid->end.day_in_year){
                        iGrid->met->CycleLength = iGrid->end.day_in_year - iGrid->start.day_in_year;
                    } else {
                        iGrid->met->CycleLength = DAYS_PER_YEAR - iGrid->end.day_in_year + iGrid->start.day_in_year;
                    }
                }
            }
        }
    }
    
    // Set TSUM1
    for(j = 0; j < plugin_options.NCROPTYPES; j++){
        d3start[0] = j;
                        
        get_scatter_nc_field_double(&plugin_filenames.crop,
                                 "TSUM1", d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            iCrop = crop_con_map[i].cidx[j];
            if (iCrop != NODATA_VEG) {
                for (k = 0; k < options.SNOW_BAND; k++) {
                    iGrid = Grid[i][k];
                    for(l = 0; l < (size_t)iCrop; l++){
                        iGrid = iGrid->next;
                    }
                    
                    iGrid->crp->prm.TempSum1 = dvar[i];
                }
            }
        }
    }
    
    // Set TSUM2
    for(j = 0; j < plugin_options.NCROPTYPES; j++){
        d3start[0] = j;
                        
        get_scatter_nc_field_double(&plugin_filenames.crop,
                                 "TSUM2", d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            iCrop = crop_con_map[i].cidx[j];
            if (iCrop != NODATA_VEG) {
                for (k = 0; k < options.SNOW_BAND; k++) {
                    iGrid = Grid[i][k];
                    for(l = 0; l < (size_t)iCrop; l++){
                        iGrid = iGrid->next;
                    }
                    
                    iGrid->crp->prm.TempSum2 = dvar[i];
                }
            }
        }
    }
    
    free(dvar);
}

void
wofost_check_data(void)
{
    extern option_struct options;
    extern domain_struct local_domain;
    extern SimUnit ***Grid;
    
    size_t i;
    size_t k;
    SimUnit *iGrid;
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for (k = 0; k < options.SNOW_BAND; k++) {
            iGrid = Grid[i][k];

            while(iGrid) {
                
                if (iGrid->crp->prm.TempSum1 <= 0) {
                    log_err("TempSum1 [%.2f] should be in range [0, inf)",
                            iGrid->crp->prm.TempSum1);
                } else if (iGrid->crp->prm.TempSum2 <= 0) {
                    log_err("TempSum2 [%.2f] should be in range [0, inf)",
                            iGrid->crp->prm.TempSum2);
                }
                
                iGrid = iGrid->next;
            }
        }
    }
}