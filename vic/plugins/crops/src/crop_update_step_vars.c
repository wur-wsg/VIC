#include <vic_driver_image.h>
#include <plugin.h>

void
crop_update_step_vars(size_t iCell)
{
    extern option_struct options;
    extern parameters_struct param;
    extern veg_con_map_struct  *veg_con_map;
    extern crop_con_map_struct *crop_con_map;
    extern all_vars_struct  *all_vars;
    extern veg_lib_struct **veg_lib;
    extern dmy_struct *dmy;
    extern size_t current;
    
    double Cc;
    double height;
    double lai_coverage;
    
    size_t iBand;
    size_t crop_class;
    int veg_class;
    int iVeg;
    
    SimUnit *cgrid;
    veg_var_struct *cveg_var;
    veg_lib_struct *cveg_lib;
    
    // Clear VIC variables
    for(crop_class = 0; crop_class < plugin_options.NCROPTYPES; crop_class++) {
        veg_class = crop_con_map[iCell].veg_class[crop_class];
        if(veg_class != NODATA_VEG) {
        
            cveg_lib =  &(veg_lib[iCell][veg_class]);
            cveg_lib->overstory = false;
            cveg_lib->trunk_ratio = 0.;
            cveg_lib->wind_atten = 0.;
            cveg_lib->wind_h = 0.;
            cveg_lib->rmin = 0.;
            cveg_lib->rarc = 0.;
            cveg_lib->rad_atten = 0.;
            cveg_lib->RGL = 0.;

            iVeg = veg_con_map[iCell].vidx[veg_class];
            if(iVeg != NODATA_VEG) {
                for(iBand = 0; iBand < options.SNOW_BAND; iBand++){

                    cveg_var = &(all_vars[iCell].veg_var[iVeg][iBand]);
                    cveg_var->fcanopy = 0.;
                    cveg_var->LAI = 0.;
                    cveg_var->albedo = 0.;
                    cveg_var->displacement = 0.;
                    cveg_var->roughness = 0.;
                }
            }
        }
    }

    // Set VIC variables
    for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
        cgrid = Grid[iCell][iBand];
        
        while(cgrid){
            crop_class = cgrid->vic->crop_class;
            veg_class = crop_con_map[iCell].veg_class[crop_class];
            iVeg = veg_con_map[iCell].vidx[veg_class];

            Cc = crop_con_map[iCell].Cc[crop_class][dmy[current].month - 1];
            height = cgrid->crp->prm.MaxHeight * min(cgrid->crp->st.Development, 1.0);
            lai_coverage = min(cgrid->crp->st.LAI, 1.0);

            cveg_var->fcanopy += Cc;
            cveg_var->LAI += cgrid->crp->st.LAI * Cc;
            cveg_var->albedo += ((1 - lai_coverage) * param.ALBEDO_BARE_SOIL + lai_coverage * cgrid->crp->prm.Albedo) * Cc;
            cveg_var->displacement += height * param.VEG_RATIO_DH_HEIGHT * Cc;
            cveg_var->roughness += height * param.VEG_RATIO_RL_HEIGHT * Cc;

            cveg_lib->rarc += cgrid->crp->prm.MaxArchitecturalResistance * min(cgrid->crp->st.Development, 1.0) * Cc;

            cveg_lib->trunk_ratio += cgrid->crp->prm.TrunkRatio * Cc;
            cveg_lib->wind_atten += cgrid->crp->prm.WindAttenuation * Cc;
            cveg_lib->wind_h += (cgrid->crp->prm.MaxHeight + 1) * Cc;
            cveg_lib->rmin += cgrid->crp->prm.MinStomatalResistance * Cc;
            cveg_lib->rad_atten += cgrid->crp->prm.RadiationAttenuation * Cc;
            cveg_lib->RGL += cgrid->crp->prm.RGL * Cc;

            cgrid = cgrid->next;
        }
    }
    
    // Transform VIC variables
    for(crop_class = 0; crop_class < plugin_options.NCROPTYPES; crop_class++) {
        veg_class = crop_con_map[iCell].veg_class[crop_class];
        if(veg_class != NODATA_VEG) {

            iVeg = veg_con_map[iCell].vidx[veg_class];
            if(iVeg != NODATA_VEG) {
                for(iBand = 0; iBand < options.SNOW_BAND; iBand++){

                    cveg_var = &(all_vars[iCell].veg_var[iVeg][iBand]);
                    cveg_var->albedo += param.ALBEDO_BARE_SOIL * 
                            (1 - cveg_var->fcanopy);
                }
            }
        }
    }
}