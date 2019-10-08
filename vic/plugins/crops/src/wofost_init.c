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
    char start[MAXSTRING];
    char cf[MAXSTRING], mf[MAXSTRING];
        
    size_t i;
    size_t k;
    size_t m;
    size_t l;
    int iCrop;
    SimUnit *iGrid;
    SimUnit *tmpGrid;
    
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
    tmpGrid->vic = malloc(sizeof(*(tmpGrid->vic)));
    check_alloc_status(tmpGrid->vic, "Memory allocation error");
    tmpGrid->next = NULL;
    
    initialize_wofost_grid(tmpGrid);

    if ((ifp = fopen(list, "r")) == NULL) {
        log_err("Can't open wofost text file, %s\n", list);
    }

    k = 0;
    while(fgets(line, MAXSTRING, ifp)) {
        if(line[0] == '*' || line[0] == ' ' || line[0] == '\n' || line[0] == '\r'){
            continue;
        }

        sscanf(line, "%s %s %s %s %d" ,
            path, cf, mf, start, &Emergence);

        if(k >= plugin_options.NCROPTYPES) {
            log_err("Crops in wofost text (%zu) file are more than "
                    "crops in wofost netcdf file (%d)",
                    k, plugin_options.NCROPTYPES);
        }

        GetSimInput(path, cf, mf, start, 
                Emergence, k, tmpGrid);
        tmpGrid->vic->crop_class = k;
        tmpGrid->vic->CycleLength = 300;
    
        for(i = 0; i < local_domain.ncells_active; i++) {
            iCrop = crop_con_map[i].cidx[k];

            if(iCrop != NODATA_VEG){
                tmpGrid->vic->Latitude = local_domain.locations[i].latitude;
                tmpGrid->vic->Longitude = local_domain.locations[i].longitude;
                    
                for(l = 0; l < options.SNOW_BAND; l++){
                    iGrid = Grid[i][l];
                    for(m = 0; m < (size_t)iCrop; m++){
                        iGrid = iGrid->next;
                    }
                    
                    copy_wofost_grid(tmpGrid, iGrid);
                }
            }
        }

        k++;
    }

    if(k + 1 < plugin_options.NCROPTYPES) {
        log_err("Crops in wofost text file are less than "
                "crops in wofost netcdf file");
    }

    fclose(ifp);
    
    Clean(tmpGrid);
}