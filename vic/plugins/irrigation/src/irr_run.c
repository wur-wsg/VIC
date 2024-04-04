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
    extern option_struct            options;
    extern all_vars_struct         *all_vars;
    extern irr_con_map_struct      *irr_con_map;
    extern irr_con_struct         **irr_con;
    extern irr_var_struct        ***irr_var;
    extern soil_con_struct         *soil_con;
    extern veg_con_struct         **veg_con;
    extern plugin_parameters_struct plugin_param;
    extern dmy_struct              *dmy;
    extern size_t                   current; //current timestep

    double                          moist[MAX_LAYERS]; //soil moisture at each layer
    double                          total_moist; //total soil moiisture of the three soil columns
    double                          total_wcr; // soil water content at critical point
    double                          total_wfc; // soil water content at field capacity
    double                          veg_fract; // fraction of vegetation coverage
    double                          area_fract; // fraction of grid cell included in each snow elevation band

    size_t                          iIrr;  // irrigation type index
    size_t                          iBand; // snow band index
    size_t                          k; //temporary index for soil layer
    size_t                          l; //temporary index for frost subarea

    irr_con_struct                 *cirr_con;
    irr_var_struct                 *cirr_var;
    soil_con_struct                *csoil_con;
    veg_con_struct                 *cveg_con;
    veg_var_struct                 *cveg_var;
    cell_data_struct               *ccell_var;

    csoil_con = &(soil_con[iCell]);

    for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
        cirr_con = &(irr_con[iCell][iIrr]); // current  Irrigation Constants
        cveg_con = &(veg_con[iCell][cirr_con->veg_index]); // current vegetation type's constants

        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            // Get pointers to current irrigation and cell variables
            cirr_var = &(irr_var[iCell][iIrr][iBand]); //
            ccell_var = &(all_vars[iCell].cell[cirr_con->veg_index][iBand]);
            // Reset values
            cirr_var->requirement = 0.0;
            cirr_var->flag_req = false;
            ccell_var->layer[0].Ksat = csoil_con->Ksat[0]; 
        }

        veg_fract = cveg_con->Cv;

        if (current == 0 || //if it is the first day of the simulation
            dmy[current].day_in_year != dmy[current - 1].day_in_year) { //if it is the first day of the year
            for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
                cirr_var = &(irr_var[iCell][iIrr][iBand]);

                // Assume a new irrigation season starts
                // when the vegetation coverage changes
                if (veg_fract != cirr_var->prev_Cv) {
                    cirr_var->offset = 0;
                }
                if (cirr_var->offset < cirr_con->offset) {
                    cirr_var->offset++;
                }
                cirr_var->prev_Cv = veg_fract;
            }
        }

        if (veg_fract > 0) {
            for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
                cirr_var = &(irr_var[iCell][iIrr][iBand]);
                ccell_var = &(all_vars[iCell].cell[cirr_con->veg_index][iBand]);
                cveg_var =
                    &(all_vars[iCell].veg_var[cirr_con->veg_index][iBand]);
                area_fract = csoil_con->AreaFract[iBand];

                if (area_fract > 0) {
                    /**********************************************************************
                    * Initialize
                    **********************************************************************/
                    // Reduce Ksat
                    if (cirr_con->paddy) {
                        ccell_var->layer[0].Ksat = pow(csoil_con->Ksat[0],
                                                       plugin_param.Ksat_expt);
                    }

                    // Get moisture content and critical moisture content of every layer
                    total_moist = 0.0; // total moisture content
                    total_wcr = 0.0; // total critical moisture content
                    total_wfc = 0.0; //soil moisture at field capacity
                    for (k = 0; k < options.Nlayer; k++) {
                        moist[k] = 0.0;
                    }

                    for (k = 0; k < options.Nlayer; k++) {
                        for (l = 0; l < options.Nfrost; l++) {
                            moist[k] += (ccell_var->layer[k].moist -
                                         ccell_var->layer[k].ice[l]) *
                                        csoil_con->frost_fract[l];
                        }

                        if (cveg_var->root[k] > 0.) {  //if current percentage in each soil layer is greater than 0, aka, there is root in this layer
                            total_moist += moist[k];   // get the moist from the current layer
                            total_wcr += ccell_var->layer[k].Wcr;   
                            total_wfc += csoil_con->Wfc[k]; //soil moisture at field capacity
                        }
                    }

                    /**********************************************************************
                    * Requirement
                    **********************************************************************/
                    // Calculate water requirement to fill up to field capacity
                    if (cirr_con->paddy) {
                        // With ponding the moisture of the top layer should
                        // always be saturated
                        cirr_var->requirement =
                            (csoil_con->max_moist[0] + PADDY_FLOOD_HEIGHT) -
                            (moist[0] + cirr_var->leftover);
                    }
                    else {
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

                    // if it is paddy rice and the top layer is not saturated and there is need for irrigation
                    if (cirr_var->offset >= cirr_con->offset) {
                        if (cirr_con->paddy &&       
                            csoil_con->max_moist[0] -
                            (moist[0] + cirr_var->leftover) > 0 &&
                            cirr_var->requirement > 0) {
                            cirr_var->flag_req = true;
                        } 
                        // if it is not paddy rice and the total soil moisture is below the critical soil moisture point and there is need for irrigation
                        else if (total_wcr -
                                 (total_moist + cirr_var->leftover) > 0 &&
                                 cirr_var->requirement > 0) {
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
    extern option_struct       options;
    extern all_vars_struct    *all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct    **irr_con;
    extern irr_var_struct   ***irr_var;
    extern soil_con_struct    *soil_con;
    extern veg_con_struct    **veg_con;

    double                     moist[MAX_LAYERS];
    double                     total_moist;
    double                     total_wcr;
    double                     lower_moist;
    double                     lower_wcr;
    double                     lower_root;
    double                     veg_fract;
    double                     area_fract;

    size_t                     iIrr;
    size_t                     iBand;
    size_t                     k;
    size_t                     l;

    irr_con_struct            *cirr_con;
    irr_var_struct            *cirr_var;
    soil_con_struct           *csoil_con;
    veg_con_struct            *cveg_con;
    veg_var_struct            *cveg_var;
    cell_data_struct          *ccell_var;

    csoil_con = &(soil_con[iCell]);

    for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
        cirr_con = &(irr_con[iCell][iIrr]);
        cveg_con = &(veg_con[iCell][cirr_con->veg_index]);

        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            cirr_var = &(irr_var[iCell][iIrr][iBand]);

            // Reset values
            cirr_var->shortage = 0.0;
        }

        veg_fract = cveg_con->Cv;

        if (veg_fract > 0) {
            for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
                cirr_var = &(irr_var[iCell][iIrr][iBand]);
                ccell_var = &(all_vars[iCell].cell[cirr_con->veg_index][iBand]);
                cveg_var =
                    &(all_vars[iCell].veg_var[cirr_con->veg_index][iBand]);
                area_fract = csoil_con->AreaFract[iBand];

                if (area_fract > 0) {
                    /**********************************************************************
                    * Initialize
                    **********************************************************************/
                    // Get moisture content and critical moisture content of every layer
                    total_moist = 0.0;
                    total_wcr = 0.0;
                    lower_moist = 0.0;
                    lower_wcr = 0.0;
                    lower_root = 0.0;
                    for (k = 0; k < options.Nlayer; k++) {
                        moist[k] = 0.0;
                    }

                    for (k = 0; k < options.Nlayer; k++) {
                        for (l = 0; l < options.Nfrost; l++) {
                            moist[k] += (ccell_var->layer[k].moist -
                                         ccell_var->layer[k].ice[l]) *
                                        csoil_con->frost_fract[l];
                        }

                        if (k < options.Nlayer - 1) {
                            if (cveg_var->root[k] > 0.) {
                                total_moist += moist[k];
                                total_wcr += ccell_var->layer[k].Wcr;
                            }
                        }
                        else {
                            // Lowest layer
                            lower_moist += moist[k];
                            lower_wcr += ccell_var->layer[k].Wcr;
                            lower_root += cveg_var->root[k];
                        }
                    }

                    /**********************************************************************
                    * Shortage & Deficit
                    **********************************************************************/
                    // Calculate shortage - suboptimal evapotranspiration
                    // (based on VIC equations for evapotranspiration)
                    if (total_moist < total_wcr && cirr_var->flag_req) {
                        cirr_var->shortage = total_wcr - total_moist;
                    }

                    if (cirr_var->shortage < 0.0) {
                        cirr_var->shortage = 0.0;
                    }

                    /**********************************************************************
                    * Water stress
                    **********************************************************************/
                    // Recalculate water stress
                    if (options.SHARE_LAYER_MOIST &&
                        ((total_moist >= total_wcr &&
                          lower_moist >= lower_wcr && total_wcr > 0.) ||
                         (total_moist >= total_wcr &&
                          (1 - lower_root) >= 0.5) ||
                         (lower_moist >= lower_wcr && lower_root >= 0.5))) {
                        for (k = 0; k < options.Nlayer; k++) {
                            // Store water stress
                            ccell_var->layer[k].water_stress = 1.;
                        }
                    }
                    else {
                        for (k = 0; k < options.Nlayer; k++) {
                            // Store water stress
                            if (moist[k] >= ccell_var->layer[k].Wcr) {
                                ccell_var->layer[k].water_stress = 1.0;
                            }
                            else if (moist[k] >= csoil_con->Wpwp[k] &&
                                     moist[k] < ccell_var->layer[k].Wcr) {
                                ccell_var->layer[k].water_stress =
                                    (moist[k] - csoil_con->Wpwp[k]) /
                                    (ccell_var->layer[k].Wcr -
                                     csoil_con->Wpwp[k]);
                            }
                            else {
                                ccell_var->layer[k].water_stress = 0.0;
                            }
                        }
                    }

                    ccell_var->water_stress = 0;
                    for (k = 0; k < options.Nlayer; k++) {
                        ccell_var->water_stress +=
                            ccell_var->layer[k].water_stress *
                            cveg_var->root[k];
                    }
                }
            }
        }
    }
}
