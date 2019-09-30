#include <vic_driver_image.h>
#include <plugin.h>

void
get_wofost_veg_vars(size_t iCell, size_t veg_class, size_t iVeg, size_t iBand)
{
    extern parameters_struct param;
    extern SimUnit ****Grid;
    extern crop_con_map_struct **crop_con_map;
    extern all_vars_struct  *all_vars;
    extern veg_lib_struct **veg_lib;
    extern dmy_struct current;
    
    double cc;
    double albedo;
    double lai;
    double fcanopy;
    double displacement;
    double roughness;
    double overstory;
    double trunk_ratio;
    double wind_atten;
    double wind_h;
    double rmin;
    double rarc;
    double rad_atten;
    double RGL;
    
    SimUnit *iGrid;
    size_t crop_class;
    
    veg_var_struct *cveg_var;
    veg_lib_struct *cveg_lib;
    
    cveg_var = &(all_vars[iCell].veg_var[iVeg][iBand]);
    cveg_lib = &(veg_lib[iCell][veg_class]);
    
    albedo = 0;
    lai = 0;
    fcanopy = 0;
    displacement = 0;
    roughness = 0;
    
    overstory = 0;
    trunk_ratio = 0;
    wind_atten = 0;
    wind_h = 0;
    rmin = 0;
    rarc = 0;
    rad_atten = 0;
    RGL = 0;
    
    iGrid = Grid[iCell][iVeg][iBand];
    while(iGrid) {
        crop_class = iGrid->vic->crop_class;
        cc = crop_con_map[iCell][iVeg].Cc[crop_class][current.month];
                
        fcanopy += cc;
        lai += iGrid->vic->LAI * cc;
        albedo += iGrid->vic->albedo * cc;
        displacement += iGrid->vic->displacement * cc;
        roughness += iGrid->vic->roughness * cc;
        
        overstory += iGrid->vic->overstory * cc;
        trunk_ratio += iGrid->vic->trunk_ratio * cc;
        wind_atten += iGrid->vic->wind_atten * cc;
        wind_h += iGrid->vic->wind_h * cc;
        rmin += iGrid->vic->rmin * cc;
        rarc += iGrid->vic->rarc * cc;
        rad_atten += iGrid->vic->rad_atten * cc;
        RGL += iGrid->vic->RGL * cc;
        
        iGrid = iGrid->next;
    }
    
    // Convert values from global (cell) to local (crop)
    albedo += param.ALBEDO_BARE_SOIL * (1 - fcanopy);
    displacement /= (1 - fcanopy);
    roughness /= (1 - fcanopy);
    
    overstory /= (1 - fcanopy);
    trunk_ratio /= (1 - fcanopy);
    wind_atten /= (1 - fcanopy);
    wind_h /= (1 - fcanopy);
    rmin /= (1 - fcanopy);
    rarc /= (1 - fcanopy);
    rad_atten /= (1 - fcanopy);
    RGL /= (1 - fcanopy);
    
    cveg_var->fcanopy = fcanopy;
    cveg_var->LAI = lai;
    cveg_var->albedo = albedo;
    cveg_var->displacement = displacement;
    cveg_var->roughness = roughness;
    
    cveg_lib->overstory = (int)round(overstory);
    cveg_lib->trunk_ratio = trunk_ratio;
    cveg_lib->wind_atten = wind_atten;
    cveg_lib->wind_h = wind_h;
    cveg_lib->rmin = rmin;
    cveg_lib->rarc = rarc;
    cveg_lib->rad_atten = rad_atten;
    cveg_lib->RGL = RGL;
}

void
crop_update_step_vars(size_t iCell)
{
    extern option_struct options;
    extern crop_con_map_struct **crop_con_map;
    extern veg_con_map_struct  *veg_con_map;
    extern soil_con_struct *soil_con;
    
    int veg_idx;
    size_t veg_class;
    size_t elev_class;
    
    for(veg_class = 0; veg_class < options.NVEGTYPES; veg_class++){
        veg_idx = veg_con_map[iCell].vidx[veg_class];
        if(veg_idx != NODATA_VEG){
            if(crop_con_map[iCell][veg_idx].nc_active > 0){
                for(elev_class = 0; elev_class < options.SNOW_BAND; elev_class++){
                    if(soil_con[iCell].AreaFract[elev_class] > 0){
                        get_wofost_veg_vars(iCell, veg_class, (size_t)veg_idx, elev_class);
                    }
                }
            }
        }
    }
}