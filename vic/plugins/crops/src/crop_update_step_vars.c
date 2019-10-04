#include <vic_driver_image.h>
#include <plugin.h>

void
get_wofost_veg_vars(SimUnit *cgrid, 
        veg_var_struct *cveg_var, 
        veg_lib_struct *cveg_lib, 
        double Cc)
{
    extern parameters_struct param;
    
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
    
    fcanopy += Cc;
    lai += cgrid->vic->LAI * Cc;
    albedo += cgrid->vic->albedo * Cc;
    displacement += cgrid->vic->displacement * Cc;
    roughness += cgrid->vic->roughness * Cc;

    overstory += cgrid->vic->overstory * Cc;
    trunk_ratio += cgrid->vic->trunk_ratio * Cc;
    wind_atten += cgrid->vic->wind_atten * Cc;
    wind_h += cgrid->vic->wind_h * Cc;
    rmin += cgrid->vic->rmin * Cc;
    rarc += cgrid->vic->rarc * Cc;
    rad_atten += cgrid->vic->rad_atten * Cc;
    RGL += cgrid->vic->RGL * Cc;
    
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
    extern veg_con_map_struct  *veg_con_map;
    extern crop_con_map_struct *crop_con_map;
    extern all_vars_struct  *all_vars;
    extern veg_lib_struct **veg_lib;
    extern dmy_struct current;
    
    size_t iBand;
    size_t crop_class;
    size_t veg_class;
    size_t iVeg;
    
    SimUnit *cgrid;

    for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
        cgrid = Grid[iCell][iBand];

        while(cgrid){
            crop_class = cgrid->vic->crop_class;
            veg_class = crop_con_map[iCell].veg_class[crop_class];
            iVeg = veg_con_map[iCell].vidx[veg_class];

            get_wofost_veg_vars(cgrid, 
                    &(all_vars[iCell].veg_var[iVeg][iBand]),
                    &(veg_lib[iCell][veg_class]), 
                    crop_con_map[iCell].Cc[crop_class][current.month]);

            cgrid = cgrid->next;
        }
    }
}