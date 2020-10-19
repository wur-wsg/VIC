#include <vic_driver_image.h>
#include <plugin.h>

void
crop_update_step_vars(size_t iCell)
{
    extern option_struct options;
    extern parameters_struct param;
    extern veg_con_map_struct  *veg_con_map;
    extern crop_con_map_struct *crop_con_map;
    extern all_vars_struct *all_vars;
    extern veg_lib_struct **veg_lib;
    extern veg_con_struct **veg_con;
    extern veg_hist_struct **veg_hist;
    extern soil_con_struct *soil_con;
    extern dmy_struct *dmy;
    extern size_t current;
    extern size_t NR;
    
    double tmp_min;
    double Cc;
    double area_fract;
    double height;
    double root_sum;
    double Wcr_FRAC;
    
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
    cell_data_struct *ccell_data;
    
    csoil_con = &(soil_con[iCell]);
    
    // Clear VIC variables
    for(veg_class = 0; veg_class < (int)options.NVEGTYPES; veg_class++) {
        iVeg = veg_con_map[iCell].vidx[veg_class];
        cveg_lib = &(veg_lib[iCell][veg_class]);
        
        if(iVeg != NODATA_VEG) {
            cveg_con = &(veg_con[iCell][iVeg]);
            cveg_hist = &(veg_hist[iCell][iVeg]);
            
            for(crop_class = 0; crop_class < plugin_options.NCROPTYPES; crop_class++) {
                if (crop_con_map[iCell].veg_class[crop_class] == veg_class) {
                    
                    cveg_lib->overstory = false;
                    cveg_lib->trunk_ratio = 0.;
                    cveg_lib->wind_atten = 0.;
                    cveg_lib->wind_h = 0.;
                    cveg_lib->rmin = 0.;
                    cveg_lib->rarc = 0.;
                    cveg_lib->rad_atten = 0.;
                    cveg_lib->RGL = 0.;

                    for(iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                        cveg_con->root[iLayer] = 0.;
                    }

                    for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
                        ccell_data = &(all_vars[iCell].cell[iVeg][iBand]);

                        for(iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                            ccell_data->layer[iLayer].Wcr = 0.;
                        }
                    }

                    cveg_hist->fcanopy[NR] = 0.;
                    cveg_hist->LAI[NR] = 0.;
                    cveg_hist->albedo[NR] = 0.;
                    cveg_hist->displacement[NR] = 0.;
                    cveg_hist->roughness[NR] = 0.;
                    
                    break;
                }
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
            ccell_data = &(all_vars[iCell].cell[iVeg][iBand]);
            
            Cc = crop_con_map[iCell].Cc[crop_class][dmy[current].month - 1];
            area_fract = Cc * csoil_con->AreaFract[iBand];
            area_fract *= cgrid->crp->prm.Fcanopy;
            
            tmp_min = min(cgrid->crp->st.Development, 1.0);
            height = cgrid->crp->prm.MaxHeight * tmp_min;

            cveg_hist->fcanopy[NR] += area_fract;
            cveg_hist->LAI[NR] += cgrid->crp->st.LAI * area_fract;
            cveg_hist->albedo[NR] += cgrid->crp->prm.Albedo * area_fract;
            cveg_hist->displacement[NR] += height * param.VEG_RATIO_DH_HEIGHT * area_fract;
            cveg_hist->roughness[NR] += height * param.VEG_RATIO_RL_HEIGHT * area_fract;

            cveg_lib->trunk_ratio += cgrid->crp->prm.TrunkRatio * area_fract;
            cveg_lib->wind_atten += cgrid->crp->prm.WindAttenuation * area_fract;
            cveg_lib->wind_h += (cgrid->crp->prm.MaxHeight + 1) * area_fract;
            cveg_lib->rad_atten += cgrid->crp->prm.RadiationAttenuation * area_fract;
            cveg_lib->RGL += cgrid->crp->prm.RGL * area_fract;
            
            cveg_lib->rarc += cgrid->crp->prm.MaxArchitecturalResistance / 
                    cgrid->crp->prm.CorrectionTransp * tmp_min * area_fract;
            cveg_lib->rmin += cgrid->crp->prm.MinStomatalResistance /
                    cgrid->crp->prm.CorrectionTransp * area_fract;
            
            if(cgrid->crp->st.RootDepth > csoil_con->depth[0] * CM_PER_M) {
                cveg_con->root[0] += (csoil_con->depth[0] * CM_PER_M / cgrid->crp->st.RootDepth) * area_fract;
                cveg_con->root[1] += (1 - cveg_con->root[0]) * area_fract;
            } else {
                cveg_con->root[0] += area_fract;
            }
            
            Wcr_FRAC = (1 - sweaf(cgrid->crp->prm.CropGroupNumber, cgrid->met->PotEvaptrans));
            for(iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                if(cveg_con->root[iLayer] > 0.) {
                    ccell_data->layer[iLayer].Wcr += (csoil_con->Wpwp[iLayer] + 
                            (csoil_con->Wfc[iLayer] - csoil_con->Wpwp[iLayer]) *  Wcr_FRAC) * area_fract;
                }
            }
            
            cgrid = cgrid->next;
        }
    }
    
    // Transform VIC variables
    for(veg_class = 0; veg_class < (int)options.NVEGTYPES; veg_class++) {
        iVeg = veg_con_map[iCell].vidx[veg_class];
        cveg_lib = &(veg_lib[iCell][veg_class]);
        
        if(iVeg != NODATA_VEG) {
            cveg_con = &(veg_con[iCell][iVeg]);
            cveg_hist = &(veg_hist[iCell][iVeg]);
            
            for(crop_class = 0; crop_class < plugin_options.NCROPTYPES; crop_class++) {
                if (crop_con_map[iCell].veg_class[crop_class] == veg_class) {
                
                    /* Adjust vegetation specific variables for partial crop coverage.
                     * NOTE: LAI and albedo are assumed to be cell averages (including bare soil)
                     * all other parameters are assumed to be vegetation specific. */
                    
                    for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
                        ccell_data = &(all_vars[iCell].cell[iVeg][iBand]);

                        for(iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                            if(cveg_con->root[iLayer] > 0.) {
                                if (cveg_hist->fcanopy[NR] == 0 || cveg_hist->LAI[NR] == 0) {
                                    ccell_data->layer[iLayer].Wcr = soil_con[iCell].Wcr[iLayer];
                                } else {
                                    ccell_data->layer[iLayer].Wcr /= cveg_hist->fcanopy[NR];
                                }
                            }
                        }
                    }

                    if (cveg_hist->fcanopy[NR] == 0 || cveg_hist->LAI[NR] == 0) {
                        cveg_lib->rarc = param.SOIL_RARC;
                        cveg_lib->trunk_ratio = 0.0;
                        cveg_lib->wind_atten = 0.5;
                        cveg_lib->wind_h = param.SOIL_WINDH;
                        cveg_lib->rmin = 0.0;
                        cveg_lib->rad_atten = 0.5;
                        cveg_lib->RGL = 100;

                        cveg_hist->displacement[NR] = csoil_con->rough;
                        cveg_hist->roughness[NR] = csoil_con->rough;

                        cveg_con->root[0] = 1.0;
                        cveg_con->root[1] = 0.0;
                    } else {
                        cveg_lib->rarc /= cveg_hist->fcanopy[NR];
                        cveg_lib->trunk_ratio /= cveg_hist->fcanopy[NR];
                        cveg_lib->wind_atten /= cveg_hist->fcanopy[NR];
                        cveg_lib->wind_h /= cveg_hist->fcanopy[NR];
                        cveg_lib->rmin /= cveg_hist->fcanopy[NR];
                        cveg_lib->rad_atten /= cveg_hist->fcanopy[NR];
                        cveg_lib->RGL /= cveg_hist->fcanopy[NR];

                        cveg_hist->displacement[NR] /= cveg_hist->fcanopy[NR];
                        cveg_hist->roughness[NR] /= cveg_hist->fcanopy[NR];

                        root_sum = 0.;
                        for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                            root_sum += cveg_con->root[iLayer];
                        }
                        for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                            cveg_con->root[iLayer] /= root_sum;
                        }
                    }

                    /* If crop area LAI < 1, adjust fcanopy for partial crop coverage */
                    if (cveg_hist->LAI[NR] / cveg_hist->fcanopy[NR] < 1.) {
                        cveg_hist->fcanopy[NR] *= cveg_hist->LAI[NR] / cveg_hist->fcanopy[NR];
                    }
                    cveg_hist->albedo[NR] = 
                            cveg_hist->fcanopy[NR] * cveg_hist->albedo[NR] + 
                            (1 - cveg_hist->fcanopy[NR]) * param.ALBEDO_BARE_SOIL;

                    if(cveg_hist->fcanopy[NR] < 0){
                        if(-cveg_hist->fcanopy[NR] > AREA_SUM_ERROR_THRESH){
                            log_err("fcanopy cannot be < 0");
                        }
                        cveg_hist->fcanopy[NR] = 0.0;
                    } else if(cveg_hist->fcanopy[NR] > 1){
                        if(cveg_hist->fcanopy[NR] - 1 > AREA_SUM_ERROR_THRESH){
                            log_err("fcanopy cannot be > 1");
                        }
                        cveg_hist->fcanopy[NR] = 1.0;
                    } else if(cveg_hist->fcanopy[NR] < MIN_FCANOPY){
                        cveg_hist->fcanopy[NR] = MIN_FCANOPY;
                    }

                    break;
                }
            }
        }
    }
}
