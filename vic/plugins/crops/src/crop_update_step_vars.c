#include <vic_driver_image.h>
#include <plugin.h>

void
crop_update_step_vars(size_t iCell)
{
    extern option_struct options;
    extern parameters_struct param;
    extern veg_con_map_struct  *veg_con_map;
    extern crop_con_map_struct *crop_con_map;
    extern veg_lib_struct **veg_lib;
    extern veg_con_struct **veg_con;
    extern veg_hist_struct **veg_hist;
    extern soil_con_struct *soil_con;
    extern dmy_struct *dmy;
    extern size_t current;
    extern size_t NR;
    
    double tmp_min;
    double Cc;
    double height;
    double lai_coverage;
    
    size_t iBand;
    size_t iLayer;
    size_t crop_class;
    int veg_class;
    int iVeg;
    
    SimUnit *cgrid;
    veg_lib_struct *cveg_lib;
    veg_hist_struct *cveg_hist;
    veg_con_struct *cveg_con;
    soil_con_struct *csoil_con;
    
    csoil_con = &(soil_con[iCell]);
    
    // Clear VIC variables
    for(crop_class = 0; crop_class < plugin_options.NCROPTYPES; crop_class++) {
        veg_class = crop_con_map[iCell].veg_class[crop_class];
        
        if(veg_class != NODATA_VEG) {
            iVeg = veg_con_map[iCell].vidx[veg_class];
            cveg_lib = &(veg_lib[iCell][veg_class]);
            
            cveg_lib->overstory = false;
            cveg_lib->trunk_ratio = 0.;
            cveg_lib->wind_atten = 0.;
            cveg_lib->wind_h = 0.;
            cveg_lib->rmin = 0.;
            cveg_lib->rarc = 0.;
            cveg_lib->rad_atten = 0.;
            cveg_lib->RGL = 0.;

            if(iVeg != NODATA_VEG) {
                cveg_con = &(veg_con[iCell][iVeg]);
                cveg_hist = &(veg_hist[iCell][iVeg]);
            
                for(iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                    cveg_con->root[iLayer] = 0.;
                }
                    
                cveg_hist->fcanopy[NR] = 0.;
                cveg_hist->LAI[NR] = 0.;
                cveg_hist->albedo[NR] = 0.;
                cveg_hist->displacement[NR] = 0.;
                cveg_hist->roughness[NR] = 0.;
            }
        }
    }

    // Set VIC variables
    for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
        cgrid = Grid[iCell][iBand];
        
        while(cgrid){
            crop_class = cgrid->met->crop_class;
            veg_class = crop_con_map[iCell].veg_class[crop_class];
            iVeg = veg_con_map[iCell].vidx[veg_class];

            cveg_lib = &(veg_lib[iCell][veg_class]);
            cveg_con = &(veg_con[iCell][iVeg]);
            cveg_hist = &(veg_hist[iCell][iVeg]);
            
            Cc = crop_con_map[iCell].Cc[crop_class][dmy[current].month - 1];
            tmp_min = min(cgrid->crp->st.Development, 1.0);
            height = cgrid->crp->prm.MaxHeight * tmp_min;
            lai_coverage = min(cgrid->crp->st.LAI, 1.0);

            cveg_hist->fcanopy[NR] += Cc;
            cveg_hist->LAI[NR] += cgrid->crp->st.LAI * Cc;
            cveg_hist->albedo[NR] += ((1 - lai_coverage) * param.ALBEDO_BARE_SOIL + lai_coverage * cgrid->crp->prm.Albedo) * Cc;
            cveg_hist->displacement[NR] += height * param.VEG_RATIO_DH_HEIGHT * Cc;
            cveg_hist->roughness[NR] += height * param.VEG_RATIO_RL_HEIGHT * Cc;

            tmp_min = min(cgrid->crp->st.Development, 1.0);
            cveg_lib->rarc += cgrid->crp->prm.MaxArchitecturalResistance * tmp_min * Cc;
            cveg_lib->trunk_ratio += cgrid->crp->prm.TrunkRatio * Cc;
            cveg_lib->wind_atten += cgrid->crp->prm.WindAttenuation * Cc;
            cveg_lib->wind_h += (cgrid->crp->prm.MaxHeight + 1) * Cc;
            cveg_lib->rmin += cgrid->crp->prm.MinStomatalResistance * Cc;
            cveg_lib->rad_atten += cgrid->crp->prm.RadiationAttenuation * Cc;
            cveg_lib->RGL += cgrid->crp->prm.RGL * Cc;
            
            if(cgrid->crp->st.RootDepth > csoil_con->depth[0]) {
                cveg_con->root[0] = csoil_con->depth[0] / cgrid->crp->st.RootDepth;
                cveg_con->root[1] = 1 - cveg_con->root[0];
                cveg_con->root[0] = 0.66;
                cveg_con->root[1] = 0.34;
            } else {
                cveg_con->root[0] = 1;
            }

            cgrid = cgrid->next;
        }
    }
    
    // Transform VIC variables
    for(crop_class = 0; crop_class < plugin_options.NCROPTYPES; crop_class++) {
        veg_class = crop_con_map[iCell].veg_class[crop_class];
        
        if(veg_class != NODATA_VEG) {
            iVeg = veg_con_map[iCell].vidx[veg_class];
            
            if(iVeg != NODATA_VEG) {
                cveg_hist = &(veg_hist[iCell][iVeg]);
                    
                cveg_hist->albedo[NR] += param.ALBEDO_BARE_SOIL * 
                        (1 - cveg_hist->fcanopy[NR]);
                
                if(cveg_hist->LAI[NR] / cveg_hist->fcanopy[NR] < 1.) {
                    cveg_hist->fcanopy[NR] *= cveg_hist->LAI[NR] / cveg_hist->fcanopy[NR];
                    cveg_hist->LAI[NR] = cveg_hist->fcanopy[NR];
                }

                if(cveg_hist->fcanopy[NR] < MIN_FCANOPY){
                    cveg_hist->fcanopy[NR] = MIN_FCANOPY;
                }
            }
        }
    }
}
