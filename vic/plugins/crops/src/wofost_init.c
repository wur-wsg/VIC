#include <vic_driver_image.h>
#include <plugin.h>

void
wofost_set_data_text(char *list)
{
    extern option_struct options;
    extern domain_struct local_domain;
    extern plugin_option_struct       plugin_options;
    extern crop_con_map_struct **crop_con_map;
    extern veg_con_map_struct  *veg_con_map;
    extern SimUnit ****Grid;
    
    FILE *foutput;    
    FILE *ifp;
    int Emergence;
    char path[MAXSTRING];
    char output[MAXSTRING];
    char start[MAXSTRING];
    char cf[MAXSTRING], sf[MAXSTRING], mf[MAXSTRING], site[MAXSTRING];
        
    size_t i;
    size_t j;
    size_t k;
    size_t m;
    size_t l;
    int iVeg;
    int iCrop;
    SimUnit *iGrid;
    
    for(i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < options.NVEGTYPES; j++) {
            iVeg = veg_con_map[i].vidx[j];
            
            if(iVeg != NODATA_VEG){
                if(crop_con_map[i][iVeg].nc_active > 0) {

                    if ((ifp = fopen(list, "r")) == NULL) {
                        log_err("Can't open wofost text file, %s\n", list);
                    }

                    k = 0;
                    while (fscanf(ifp, "%s %s %s %s %s %s %d %s" ,
                            path, cf, sf, mf, site, start, &Emergence, output)
                            != EOF) {
                        if(k >= plugin_options.NCROPTYPES) {
                            log_err("Crops in wofost text (%zu) file are more than "
                                    "crops in wofost netcdf file (%d)",
                                    k, plugin_options.NCROPTYPES);
                        }

                        iCrop = crop_con_map[i][iVeg].cidx[k];

                        if(iCrop != NODATA_VEG){
                            for(l = 0; l < options.SNOW_BAND; l++){
                                iGrid = Grid[i][iVeg][l];
                                for(m = 0; m < (size_t)iCrop; m++){
                                    iGrid = iGrid->next;
                                }

                                iGrid->vic->veg_class = j;
                                iGrid->vic->snow_band = l;
                                iGrid->vic->crop_class = k;
                                iGrid->vic->Latitude = local_domain.locations[i].latitude;
                                iGrid->vic->Longitude = local_domain.locations[i].longitude;
                                iGrid->vic->CycleLength = 300;

                                GetSimInputSingle(path, cf, mf, start, 
                                        Emergence, k, output, iGrid);

                                foutput = fopen(iGrid->output, "w");
                                header(foutput);
                                fclose(foutput);
                            }
                        }

                        k++;
                    }
                    if(k + 1 < plugin_options.NCROPTYPES) {
                        log_err("Crops in wofost text file are less than "
                                "crops in wofost netcdf file");
                    }

                    fclose(ifp);
                }
            }
        }
    }
}