/******************************************************************************
 * @section DESCRIPTION
 *
 * Run irrigation
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#include <vic_driver_image.h>
#include <plugin.h>

/******************************************
* @brief   Calculate irrigation requirement (between field capacity and critical soil moisture point)
******************************************/
void
irr_run_requirement(size_t iCell)
{
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern plugin_parameters_struct plugin_param;
    
    double moist[MAX_LAYERS];
    double total_moist;
    double total_wcr;
    double total_wfc;
    double veg_fract;
    double area_fract;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    
    irr_con_struct *cirr_con;
    irr_var_struct *cirr_var;
    soil_con_struct *csoil_con;
    veg_con_struct *cveg_con;
    veg_var_struct *cveg_var;
    cell_data_struct *ccell_var;
    
    csoil_con = &soil_con[iCell];
    
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        veg_fract = cveg_con->Cv;
        
        if(veg_fract > 0) {
            
            for(j = 0; j < options.SNOW_BAND; j++){
                cirr_var = &irr_var[iCell][i][j];
                ccell_var = &all_vars[iCell].cell[cirr_con->veg_index][j];
                cveg_var = &all_vars[iCell].veg_var[cirr_con->veg_index][j];
                area_fract = csoil_con->AreaFract[j];

                if(area_fract > 0){

                    // Reset values
                    cirr_var->requirement = 0.0;
                    cirr_var->flag_req = false;
                    ccell_var->layer[0].Ksat = csoil_con->Ksat[0];
                    ccell_var->layer[1].Ksat = csoil_con->Ksat[1];

                    if(cveg_var->fcanopy > 0.01){

                        /**********************************************************************
                        * Initialize
                        **********************************************************************/
                        // Reduce Ksat
                        if (cirr_con->paddy){
                            ccell_var->layer[0].Ksat = pow(csoil_con->Ksat[0], plugin_param.Ksat_expt);
                            ccell_var->layer[1].Ksat = pow(csoil_con->Ksat[1], plugin_param.Ksat_expt);
                        }

                        // Get moisture content and critical moisture content of every layer
                        total_moist = 0.0;
                        total_wcr = 0.0;
                        total_wfc = 0.0;
                        for(k = 0; k < options.Nlayer; k++){
                            moist[k] = 0.0;
                        } 

                        for(k = 0; k < options.Nlayer; k++){                
                            for (l = 0; l < options.Nfrost; l++) {
                                moist[k] += (ccell_var->layer[k].moist - 
                                             ccell_var->layer[k].ice[l]) *
                                             csoil_con->frost_fract[l];
                            }

                            if(cveg_con->root[k] > 0.){
                                total_moist += moist[k];
                                total_wcr += ccell_var->layer[k].Wcr;
                                total_wfc += csoil_con->Wfc[k];
                            }
                        }

                        /**********************************************************************
                        * Requirement
                        **********************************************************************/
                        // Calculate water requirement to fill up to field capacity
                        if(cirr_con->paddy){
                            // With ponding the moisture of the top layer should
                            // always be saturated
                            cirr_var->requirement = 
                                    csoil_con->max_moist[0] - 
                                    (moist[0] + cirr_var->leftover);
                        }
                        else{
                            // Without ponding the moisture should be at
                            // field capacity
                            cirr_var->requirement = 
                                    total_wfc - 
                                    (total_moist + cirr_var->leftover);
                        }

                        /**********************************************************************
                        * Flag Demand
                        **********************************************************************/
                        // Calculate whether irrigation water is required to 
                        // prevent suboptimal evapotranspiration
                        if(cirr_con->paddy && cirr_var->requirement > 0){
                            cirr_var->flag_req = true;
                        }
                        else if (total_wcr - (total_moist + cirr_var->leftover) > 0){
                            cirr_var->flag_req = true;
                        }
                    }
                }
            }
        }   
    }
}

/******************************************
* @brief   Calculate irrigation shortage (below critical soil moisture point)
******************************************/
void
irr_run_shortage(size_t iCell)
{
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    
    double moist[MAX_LAYERS];
    double total_moist;
    double total_wcr;
    double veg_fract;
    double area_fract;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    
    irr_con_struct *cirr_con;
    irr_var_struct *cirr_var;
    soil_con_struct *csoil_con;
    veg_con_struct *cveg_con;
    veg_var_struct *cveg_var;
    cell_data_struct *ccell_var;
    
    csoil_con = &soil_con[iCell];
    
    for(i = 0; i < irr_con_map[iCell].ni_active; i++){
        cirr_con = &irr_con[iCell][i];
        cveg_con = &veg_con[iCell][cirr_con->veg_index];
        veg_fract = cveg_con->Cv;
        
        if(veg_fract > 0) {
        
            for(j = 0; j < options.SNOW_BAND; j++){
                cirr_var = &irr_var[iCell][i][j];
                ccell_var = &all_vars[iCell].cell[cirr_con->veg_index][j];
                cveg_var = &all_vars[iCell].veg_var[cirr_con->veg_index][j];
                area_fract = csoil_con->AreaFract[j];

                if(area_fract > 0){ 

                    // Reset values
                    cirr_var->shortage = 0.0;

                    if(cveg_var->fcanopy > 0.01){

                        /**********************************************************************
                        * Initialize
                        **********************************************************************/
                        // Get moisture content and critical moisture content of every layer
                        total_moist = 0.0;
                        total_wcr = 0.0;        
                        for(k = 0; k < options.Nlayer; k++){
                            moist[k] = 0.0;
                        } 

                        for(k = 0; k < options.Nlayer; k++){                
                            for (l = 0; l < options.Nfrost; l++) {
                                moist[k] += (ccell_var->layer[k].moist - 
                                             ccell_var->layer[k].ice[l]) *
                                             csoil_con->frost_fract[l];
                            }

                            if(cveg_con->root[k] > 0.){
                                total_moist += moist[k];
                                total_wcr += ccell_var->layer[k].Wcr;
                            }
                        }

                        /**********************************************************************
                        * Shortage & Deficit
                        **********************************************************************/
                        // Calculate shortage - suboptimal evapotranspiration
                        // (based on VIC equations for evapotranspiration)
                        if(total_moist < total_wcr){
                            cirr_var->shortage = total_wcr - total_moist;
                        }

                        if(cirr_var->shortage < 0.0){
                            cirr_var->shortage = 0.0;
                        }
                    }
                }
            }
        }
    }
}
