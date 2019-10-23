/******************************************************************************
 * @section DESCRIPTION
 *
 * This routine converts data units, and stores finalized values in an array
 * for later output to the output files.
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

#include <vic_driver_shared_all.h>

#include "vic_driver_shared_image.h"

/******************************************************************************
 * @brief    This routine converts data units, and stores finalized values in
 *           an array for later output to the output files.
 *****************************************************************************/
void
put_data_lake_only(all_vars_struct   *all_vars,
                   force_data_struct *force,
                   soil_con_struct   *soil_con,
                   veg_con_struct    *veg_con,
                   veg_lib_struct    *veg_lib,
                   lake_con_struct   *lake_con,
                   double           **out_data,
                   save_data_struct  *save_data,
                   timer_struct      *timer)
{
    extern global_param_struct global_param;
    extern option_struct       options;
    extern parameters_struct   param;

    size_t                     veg;
    size_t                     index;
    size_t                     lake;
    int                        ilake;
    size_t                     band;
    unsigned short int         sband;
    size_t                     Nbands;
    bool                       overstory;
    bool                       HasVeg;
    bool                       IsWet;
    bool                      *AboveTreeLine;
    double                    *AreaFract;
    double                    *depth;
    double                    *frost_fract;
    double                     frost_slope;
    double                     Cv;
    double                     Clake;
    double                     cv_baresoil;
    double                     cv_veg;
    double                     cv_overstory;
    double                     cv_snow;
    double                     inflow;
    double                     outflow;
    double                     storage;
    double                    *TreeAdjustFactor;
    double                     ThisAreaFract;
    double                     ThisTreeAdjust;
    size_t                     i;
    double                     dt_sec;

    cell_data_struct         **cell;
    energy_bal_struct        **energy;
    lake_var_struct          *lake_var;
    snow_data_struct         **snow;
    veg_var_struct           **veg_var;

    cell = all_vars->cell;
    energy = all_vars->energy;
    lake_var = all_vars->lake_var;
    snow = all_vars->snow;
    veg_var = all_vars->veg_var;

    AboveTreeLine = soil_con->AboveTreeLine;
    AreaFract = soil_con->AreaFract;
    depth = soil_con->depth;
    frost_fract = soil_con->frost_fract;
    frost_slope = soil_con->frost_slope;
    dt_sec = global_param.dt;

    // Compute treeline adjustment factors
    TreeAdjustFactor = calloc(options.SNOW_BAND, sizeof(*TreeAdjustFactor));
    check_alloc_status(TreeAdjustFactor, "Memory allocation error.");
    for (band = 0; band < options.SNOW_BAND; band++) {
        if (AboveTreeLine[band]) {
            Cv = 0;
            for (veg = 0; veg < veg_con[0].vegetat_type_num; veg++) {
                if (veg_lib[veg_con[veg].veg_class].overstory) {
                    if (options.LAKES && veg_con[veg].LAKE) {
                        ilake = veg_con[veg].lake_idx;
                        
                        if (band == lake_con[ilake].elev_idx) {
                            // Fraction of tile that is flooded
                            Clake = lake_var[ilake].sarea / lake_con[ilake].basin[0];
                            Cv += veg_con[veg].Cv * (1 - Clake);
                        }
                    }
                    else {
                        Cv += veg_con[veg].Cv;
                    }
                }
            }
            TreeAdjustFactor[band] = 1. / (1. - Cv);
        }
        else {
            TreeAdjustFactor[band] = 1.;
        }
        if (TreeAdjustFactor[band] != 1) {
            log_warn("Tree adjust factor for band %zu is equal to %f.",
                     band, TreeAdjustFactor[band]);
        }
    }

    cv_baresoil = 0;
    cv_veg = 0;
    cv_overstory = 0;
    cv_snow = 0;

    // Initialize output data to zero
    zero_output_list(out_data);

    for (veg = 0; veg < veg_con[0].vegetat_type_num; veg ++) {
        if (options.LAKES && veg_con[veg].LAKE) {
            ilake = veg_con[veg].lake_idx;
            lake = lake_con[ilake].lake_class;
            
            // Set output versions of input forcings
            out_data[OUT_AIR_TEMP][lake] = force->air_temp[NR];
            out_data[OUT_DENSITY][lake] = force->density[NR];
            out_data[OUT_LWDOWN][lake] = force->longwave[NR];
            out_data[OUT_PREC][lake] = force->out_prec;  // mm over grid cell
            out_data[OUT_PRESSURE][lake] = force->pressure[NR] / PA_PER_KPA;
            out_data[OUT_QAIR][lake] = CONST_EPS * force->vp[NR] /
                                    force->pressure[NR];
            out_data[OUT_RAINF][lake] = force->out_rain;   // mm over grid cell
            out_data[OUT_REL_HUMID][lake] = FRACT_TO_PERCENT * force->vp[NR] /
                                         (force->vp[NR] + force->vpd[NR]);
            if (options.LAKES && options.FORCE_LAKES && lake_con->Cl[0] > 0) {
                out_data[OUT_LAKE_CHAN_IN][lake] = force->channel_in[NR];  // mm over grid cell
            }
            else {
                out_data[OUT_LAKE_CHAN_IN][lake] = 0;
            }
            out_data[OUT_SWDOWN][lake] = force->shortwave[NR];
            out_data[OUT_SNOWF][lake] = force->out_snow;   // mm over grid cell
            out_data[OUT_VP][lake] = force->vp[NR] / PA_PER_KPA;
            out_data[OUT_VPD][lake] = force->vpd[NR] / PA_PER_KPA;
            out_data[OUT_WIND][lake] = force->wind[NR];
            if (options.CARBON) {
                out_data[OUT_CATM][lake] = force->Catm[NR] / PPM_to_MIXRATIO;
                out_data[OUT_FDIR][lake] = force->fdir[NR];
                out_data[OUT_PAR][lake] = force->par[NR];
            }
            else {
                out_data[OUT_CATM][lake] = MISSING;
                out_data[OUT_FDIR][lake] = MISSING;
                out_data[OUT_PAR][lake] = MISSING;
            }
        }
    }

    /****************************************
       Store Output for all Vegetation Types (except lakes)
    ****************************************/
    for (veg = 0; veg <= veg_con[0].vegetat_type_num; veg++) {
        Cv = veg_con[veg].Cv;
        Clake = 0;
        sband = 0;
        Nbands = options.SNOW_BAND;
        IsWet = false;

        if (veg < veg_con[0].vegetat_type_num) {
            HasVeg = true;
        }
        else {
            HasVeg = false;
        }

        if (Cv > 0) {
            // Check if this is lake/wetland tile
            if (options.LAKES && veg_con[veg].LAKE) {
                ilake = veg_con[veg].lake_idx;
                lake = lake_con[ilake].lake_class;

                Clake = lake_var[ilake].sarea / lake_con[ilake].basin[0];
                sband = lake_con[ilake].elev_idx;
                Nbands = sband + 1;
                IsWet = true;

                overstory = veg_lib[veg_con[veg].veg_class].overstory;

                /*********************************
                   Store Output for all Bands
                *********************************/
                for (band = sband; band < Nbands; band++) {
                    ThisAreaFract = AreaFract[band];
                    ThisTreeAdjust = TreeAdjustFactor[band];
                    if (IsWet) {
                        Cv = 1;
                        ThisAreaFract = 1;
                        ThisTreeAdjust = 1;
                        Clake = 1;
                    }

                    if (ThisAreaFract > 0. &&
                        (veg == veg_con[0].vegetat_type_num ||
                         (!AboveTreeLine[band] ||
                          (AboveTreeLine[band] &&
                           !overstory)))) {
                        /** compute running totals of various landcovers **/
                        if (HasVeg) {
                            cv_veg += Cv * ThisAreaFract * ThisTreeAdjust;
                        }
                        else {
                            cv_baresoil += Cv * ThisAreaFract * ThisTreeAdjust;
                        }
                        if (overstory) {
                            cv_overstory += Cv * ThisAreaFract * ThisTreeAdjust;
                        }
                        if (snow[veg][band].swq > 0.0) {
                            cv_snow += Cv * ThisAreaFract * ThisTreeAdjust;
                        }

                        /*********************************
                           Record Water Balance Terms
                        *********************************/
                        collect_wb_terms_lake_only(cell[veg][band],
                                         veg_var[veg][band],
                                         snow[veg][band],
                                         Cv,
                                         ThisAreaFract,
                                         ThisTreeAdjust,
                                         HasVeg,
                                         (1 - Clake),
                                         overstory,
                                         frost_fract,
                                         out_data,
                                         lake);

                        /**********************************
                           Record Energy Balance Terms
                        **********************************/
                        collect_eb_terms_lake_only(energy[veg][band],
                                         snow[veg][band],
                                         cell[veg][band],
                                         Cv,
                                         ThisAreaFract,
                                         ThisTreeAdjust,
                                         HasVeg,
                                         0,
                                         (1 - Clake),
                                         overstory,
                                         band,
                                         frost_fract,
                                         frost_slope,
                                         out_data,
                                         lake);

                        // Store Wetland-Specific Variables
                        if (IsWet) {
                            // Wetland soil temperatures
                            for (i = 0; i < options.Nnode; i++) {
                                out_data[OUT_SOIL_TNODE_WL][i] =
                                    energy[veg][band].T[i];
                            }
                        }

                        /**********************************
                           Record Lake Variables
                        **********************************/
                        if (IsWet) {
                            // Override some variables of soil under lake with those of wetland
                            // This is for those variables whose lake values shouldn't be included
                            // in grid cell average
                            // Note: doing this for eb terms will lead to reporting of eb errors
                            // this should be fixed when we implement full thermal solution beneath lake
                            for (i = 0; i < MAX_FRONTS; i++) {
                                lake_var[ilake].energy.fdepth[i] =
                                    energy[veg][band].fdepth[i];
                                lake_var[ilake].energy.tdepth[i] =
                                    energy[veg][band].fdepth[i];
                            }
                            for (i = 0; i < options.Nnode; i++) {
                                lake_var[ilake].energy.ice[i] = energy[veg][band].ice[i];
                                lake_var[ilake].energy.T[i] = energy[veg][band].T[i];
                            }
                            lake_var[ilake].soil.pot_evap =
                                cell[veg][band].pot_evap;
                            lake_var[ilake].soil.rootmoist = cell[veg][band].rootmoist;
                            lake_var[ilake].energy.deltaH = energy[veg][band].deltaH;
                            lake_var[ilake].energy.fusion = energy[veg][band].fusion;
                            lake_var[ilake].energy.grnd_flux = energy[veg][band].grnd_flux;


                            /*********************************
                               Record Water Balance Terms
                            *********************************/
                            collect_wb_terms_lake_only(lake_var[ilake].soil,
                                             veg_var[0][0],
                                             lake_var[ilake].snow,
                                             Cv,
                                             ThisAreaFract,
                                             ThisTreeAdjust,
                                             0,
                                             Clake,
                                             overstory,
                                             frost_fract,
                                             out_data,
                                             lake);

                            /**********************************
                               Record Energy Balance Terms
                            **********************************/
                            collect_eb_terms_lake_only(lake_var[ilake].energy,
                                             lake_var[ilake].snow,
                                             lake_var[ilake].soil,
                                             Cv,
                                             ThisAreaFract,
                                             ThisTreeAdjust,
                                             0,
                                             1,
                                             Clake,
                                             overstory,
                                             band,
                                             frost_fract,
                                             frost_slope,
                                             out_data,
                                             lake);

                            // Store Lake-Specific Variables

                            // Lake ice
                            if (lake_var[ilake].new_ice_area > 0.0) {
                                out_data[OUT_LAKE_ICE][lake] =
                                    (lake_var[ilake].ice_water_eq /
                                     lake_var[ilake].new_ice_area) * CONST_RHOICE /
                                    CONST_RHOFW;
                                out_data[OUT_LAKE_ICE_TEMP][lake] =
                                    lake_var[ilake].tempi;
                                out_data[OUT_LAKE_ICE_HEIGHT][lake] =
                                    lake_var[ilake].hice;
                                out_data[OUT_LAKE_SWE][lake] = lake_var[ilake].swe /
                                                            lake_var[ilake].areai;       // m over lake ice
                                out_data[OUT_LAKE_SWE_V][lake] = lake_var[ilake].swe;  // m3
                            }
                            else {
                                out_data[OUT_LAKE_ICE][lake] = 0.0;
                                out_data[OUT_LAKE_ICE_TEMP][lake] = 0.0;
                                out_data[OUT_LAKE_ICE_HEIGHT][lake] = 0.0;
                                out_data[OUT_LAKE_SWE][lake] = 0.0;
                                out_data[OUT_LAKE_SWE_V][lake] = 0.0;
                            }
                            out_data[OUT_LAKE_DSWE_V][lake] = lake_var[ilake].swe -
                                                           lake_var[ilake].swe_save;       // m3
                            // same as OUT_LAKE_MOIST
                            out_data[OUT_LAKE_DSWE][lake] =
                                (lake_var[ilake].swe - lake_var[ilake].swe_save) * MM_PER_M /
                                soil_con->cell_area;

                            // Lake dimensions
                            out_data[OUT_LAKE_AREA_FRAC][lake] = Cv * Clake;
                            out_data[OUT_LAKE_DEPTH][lake] = lake_var[ilake].ldepth;
                            out_data[OUT_LAKE_SURF_AREA][lake] = lake_var[ilake].sarea;
                            if (out_data[OUT_LAKE_SURF_AREA][lake] > 0) {
                                out_data[OUT_LAKE_ICE_FRACT][lake] =
                                    lake_var[ilake].new_ice_area /
                                    out_data[OUT_LAKE_SURF_AREA][lake];
                            }
                            else {
                                out_data[OUT_LAKE_ICE_FRACT][lake] = 0.;
                            }
                            out_data[OUT_LAKE_VOLUME][lake] = lake_var[ilake].volume;
                            out_data[OUT_LAKE_DSTOR_V][lake] = lake_var[ilake].volume -
                                                            lake_var[ilake].
                                                            volume_save;
                            // mm over gridcell
                            out_data[OUT_LAKE_DSTOR][lake] =
                                (lake_var[ilake].volume - lake_var[ilake].volume_save) *
                                MM_PER_M /
                                soil_con->cell_area;

                            // Other lake characteristics
                            out_data[OUT_LAKE_SURF_TEMP][lake] = lake_var[ilake].temp[0];
                            for (index = 0; index < lake_var[ilake].activenod; index++) {
                                out_data[OUT_LAKE_LAYER_TEMP][index * options.NLAKETYPES  + lake] = 
                                        lake_var[ilake].temp[index];
                            }
                            if (out_data[OUT_LAKE_SURF_AREA][lake] > 0) {
                                // mm over gridcell
                                out_data[OUT_LAKE_MOIST][lake] =
                                    (lake_var[ilake].volume / soil_con->cell_area) *
                                    MM_PER_M;
                                // same as OUT_LAKE_MOIST
                                out_data[OUT_SURFSTOR][lake] =
                                    (lake_var[ilake].volume / soil_con->cell_area) *
                                    MM_PER_M;
                            }
                            else {
                                out_data[OUT_LAKE_MOIST][lake] = 0;
                                out_data[OUT_SURFSTOR][lake] = 0;
                            }

                            // Lake moisture fluxes
                            out_data[OUT_LAKE_BF_IN_V][lake] =
                                lake_var[ilake].baseflow_in;  // m3
                            out_data[OUT_LAKE_BF_OUT_V][lake] =
                                lake_var[ilake].baseflow_out;  // m3
                            out_data[OUT_LAKE_CHAN_IN_V][lake] =
                                lake_var[ilake].channel_in;  // m3
                            out_data[OUT_LAKE_CHAN_OUT_V][lake] =
                                lake_var[ilake].runoff_out;  // m3
                            out_data[OUT_LAKE_EVAP_V][lake] = lake_var[ilake].evapw;  // m3
                            out_data[OUT_LAKE_PREC_V][lake] = lake_var[ilake].prec;  // m3
                            out_data[OUT_LAKE_RCHRG_V][lake] = lake_var[ilake].recharge;  // m3
                            out_data[OUT_LAKE_RO_IN_V][lake] = lake_var[ilake].runoff_in;  // m3
                            out_data[OUT_LAKE_VAPFLX_V][lake] =
                                lake_var[ilake].vapor_flux;  // m3
                            out_data[OUT_LAKE_BF_IN][lake] =
                                lake_var[ilake].baseflow_in * MM_PER_M /
                                soil_con->cell_area;  // mm over gridcell
                            out_data[OUT_LAKE_BF_OUT][lake] =
                                lake_var[ilake].baseflow_out * MM_PER_M /
                                soil_con->cell_area;  // mm over gridcell
                            out_data[OUT_LAKE_CHAN_OUT][lake] =
                                lake_var[ilake].runoff_out * MM_PER_M /
                                soil_con->cell_area;  // mm over gridcell
                            // mm over gridcell
                            out_data[OUT_LAKE_EVAP][lake] = lake_var[ilake].evapw *
                                                         MM_PER_M /
                                                         soil_con->cell_area;
                            // mm over gridcell
                            out_data[OUT_LAKE_RCHRG][lake] = lake_var[ilake].recharge *
                                                          MM_PER_M /
                                                          soil_con->cell_area;
                            // mm over gridcell
                            out_data[OUT_LAKE_RO_IN][lake] = lake_var[ilake].runoff_in *
                                                          MM_PER_M /
                                                          soil_con->cell_area;
                            out_data[OUT_LAKE_VAPFLX][lake] =
                                lake_var[ilake].vapor_flux * MM_PER_M /
                                soil_con->cell_area;  // mm over gridcell
                            } // End if options.LAKES etc.
                    } // End if ThisAreaFract etc.
                } // End loop over bands
            } // End if options.LAKES etc.
        } // End if Cv > 0
    } // End loop over veg


    /*****************************************
       Finish aggregation of special-case variables
    *****************************************/

    for (veg = 0; veg < veg_con[0].vegetat_type_num; veg ++) {
        if (options.LAKES && veg_con[veg].LAKE) {
            ilake = veg_con[veg].lake_idx;
            lake = lake_con[ilake].lake_class;
            
            // Normalize quantities that aren't present over entire grid cell
            if (cv_baresoil > 0) {
                out_data[OUT_BARESOILT][lake] /= cv_baresoil;
            }
            if (cv_veg > 0) {
                out_data[OUT_VEGT][lake] /= cv_veg;
            }
            if (cv_overstory > 0) {
                out_data[OUT_AERO_COND2][lake] /= cv_overstory;
            }
            if (cv_snow > 0) {
                out_data[OUT_SALBEDO][lake] /= cv_snow;
                out_data[OUT_SNOW_SURF_TEMP][lake] /= cv_snow;
                out_data[OUT_SNOW_PACK_TEMP][lake] /= cv_snow;
            }

            // Radiative temperature
            out_data[OUT_RAD_TEMP][lake] = pow(out_data[OUT_RAD_TEMP][lake], 0.25);

            // Aerodynamic conductance and resistance
            if (out_data[OUT_AERO_COND1][lake] > DBL_EPSILON) {
                out_data[OUT_AERO_RESIST1][lake] = 1 / out_data[OUT_AERO_COND1][lake];
            }
            else {
                out_data[OUT_AERO_RESIST1][lake] = param.HUGE_RESIST;
            }
            if (out_data[OUT_AERO_COND2][lake] > DBL_EPSILON) {
                out_data[OUT_AERO_RESIST2][lake] = 1 / out_data[OUT_AERO_COND2][lake];
            }
            else {
                out_data[OUT_AERO_RESIST2][lake] = param.HUGE_RESIST;
            }
            if (out_data[OUT_AERO_COND][lake] > DBL_EPSILON) {
                out_data[OUT_AERO_RESIST][lake] = 1 / out_data[OUT_AERO_COND][lake];
            }
            else {
                out_data[OUT_AERO_RESIST][lake] = param.HUGE_RESIST;
            }

            /*****************************************
               Compute derived variables
            *****************************************/
            // Water balance terms
            out_data[OUT_DELSOILMOIST][lake] = 0;
            for (index = 0; index < options.Nlayer; index++) {
                out_data[OUT_SOIL_MOIST][index * options.NLAKETYPES + lake] =
                    out_data[OUT_SOIL_LIQ][index * options.NLAKETYPES + lake] +
                    out_data[OUT_SOIL_ICE][index * options.NLAKETYPES + lake];
                out_data[OUT_DELSOILMOIST][lake] +=
                    out_data[OUT_SOIL_MOIST][index * options.NLAKETYPES + lake];

                out_data[OUT_SMLIQFRAC][index * options.NLAKETYPES + lake] = out_data[OUT_SOIL_LIQ][index * options.NLAKETYPES + lake] /
                                                 out_data[OUT_SOIL_MOIST][index * options.NLAKETYPES + lake];
                out_data[OUT_SMFROZFRAC][index * options.NLAKETYPES + lake] = 1 - out_data[OUT_SMLIQFRAC][index * options.NLAKETYPES + lake];
                out_data[OUT_SOIL_LIQ_FRAC][index * options.NLAKETYPES + lake] = out_data[OUT_SOIL_LIQ][index * options.NLAKETYPES + lake] /
                                                     (depth[index] *
                                                      MM_PER_M);
                out_data[OUT_SOIL_ICE_FRAC][index * options.NLAKETYPES + lake] = out_data[OUT_SOIL_ICE][index * options.NLAKETYPES + lake] /
                                                     (depth[index] *
                                                      MM_PER_M);
            }
            out_data[OUT_DELSOILMOIST][lake] -= save_data->total_soil_moist;
            out_data[OUT_DELSWE][lake] = out_data[OUT_SWE][lake] +
                                      out_data[OUT_SNOW_CANOPY][lake] -
                                      save_data->swe;
            out_data[OUT_DELINTERCEPT][lake] = out_data[OUT_WDEW][lake] - save_data->wdew;
            out_data[OUT_DELSURFSTOR][lake] = out_data[OUT_SURFSTOR][lake] -
                                           save_data->surfstor;

            // Energy terms
            out_data[OUT_REFREEZE][lake] =
                (out_data[OUT_RFRZ_ENERGY][lake] / CONST_LATICE) * dt_sec;
            out_data[OUT_R_NET][lake] = out_data[OUT_SWNET][lake] + out_data[OUT_LWNET][lake];

            // Save current moisture state for use in next time step
            save_data->total_soil_moist = 0;
            for (index = 0; index < options.Nlayer; index++) {
                save_data->total_soil_moist += out_data[OUT_SOIL_MOIST][index * options.NLAKETYPES + lake];
            }
            save_data->surfstor = out_data[OUT_SURFSTOR][lake];
            save_data->swe = out_data[OUT_SWE][lake] + out_data[OUT_SNOW_CANOPY][lake];
            save_data->wdew = out_data[OUT_WDEW][lake];

            // Carbon Terms
            if (options.CARBON) {
                out_data[OUT_RHET][lake] *= dt_sec / SEC_PER_DAY;  // convert to gC/m2d
                out_data[OUT_NEE][lake] = out_data[OUT_NPP][lake] - out_data[OUT_RHET][lake];
            }

            /********************
               Check Water Balance
            ********************/
            inflow = out_data[OUT_PREC][lake] + out_data[OUT_LAKE_CHAN_IN][lake];  // mm over grid cell
            outflow = out_data[OUT_EVAP][lake] + out_data[OUT_RUNOFF][lake] +
                      out_data[OUT_BASEFLOW][lake];  // mm over grid cell
            storage = 0.;
            for (index = 0; index < options.Nlayer; index++) {
                storage += out_data[OUT_SOIL_LIQ][index * options.NLAKETYPES + lake] +
                           out_data[OUT_SOIL_ICE][index * options.NLAKETYPES + lake];
            }
            storage += out_data[OUT_SWE][lake] + out_data[OUT_SNOW_CANOPY][lake] +
                       out_data[OUT_WDEW][lake] + out_data[OUT_SURFSTOR][lake];
            out_data[OUT_WATER_ERROR][lake] = \
                calc_water_balance_error(inflow,
                                         outflow,
                                         storage,
                                         save_data->
                                         total_moist_storage);

            // Store total storage for next timestep
            save_data->total_moist_storage = storage;

            /********************
               Check Energy Balance
            ********************/
            if (options.FULL_ENERGY) {
                out_data[OUT_ENERGY_ERROR][lake] = \
                    calc_energy_balance_error(out_data[OUT_SWNET][lake] +
                                              out_data[OUT_LWNET][lake],
                                              out_data[OUT_LATENT][lake] +
                                              out_data[OUT_LATENT_SUB][lake],
                                              out_data[OUT_SENSIBLE][lake] +
                                              out_data[OUT_ADV_SENS][lake],
                                              out_data[OUT_GRND_FLUX][lake] +
                                              out_data[OUT_DELTAH][lake] +
                                              out_data[OUT_FUSION][lake],
                                              out_data[OUT_ADVECTION][lake] -
                                              out_data[OUT_DELTACC][lake] +
                                              out_data[OUT_SNOW_FLUX][lake] +
                                              out_data[OUT_RFRZ_ENERGY][lake]);
            }
            else {
                out_data[OUT_ENERGY_ERROR][lake] = MISSING;
            }

            // vic_run run time
            out_data[OUT_TIME_VICRUN_WALL][lake] = timer->delta_wall;
            out_data[OUT_TIME_VICRUN_CPU][lake] = timer->delta_cpu;
        }
    }

    free((char *) (TreeAdjustFactor));
}

/******************************************************************************
 * @brief    This routine collects water balance terms.
 *****************************************************************************/
void
collect_wb_terms_lake_only(cell_data_struct cell,
                 veg_var_struct   veg_var,
                 snow_data_struct snow,
                 double           Cv,
                 double           AreaFract,
                 double           TreeAdjustFactor,
                 bool             HasVeg,
                 double           lakefactor,
                 bool             overstory,
                 double          *frost_fract,
                 double         **out_data,
                 size_t           lake)
{
    extern option_struct     options;
    extern parameters_struct param;

    double                   AreaFactor;
    double                   tmp_evap;
    double                   tmp_cond1;
    double                   tmp_cond2;
    double                   tmp_moist;
    double                   tmp_ice;
    size_t                   index;
    size_t                   frost_area;

    AreaFactor = Cv * AreaFract * TreeAdjustFactor * lakefactor;

    /** record evaporation components **/
    tmp_evap = 0.0;
    for (index = 0; index < options.Nlayer; index++) {
        tmp_evap += cell.layer[index].evap;
        out_data[OUT_EVAP_BARE][lake] += cell.layer[index].esoil *
                                      AreaFactor;
        if (HasVeg) {
            out_data[OUT_TRANSP_VEG][lake] += cell.layer[index].transp *
                                           AreaFactor;
        }
    }
    tmp_evap += snow.vapor_flux * MM_PER_M;
    out_data[OUT_SUB_SNOW][lake] += snow.vapor_flux * MM_PER_M * AreaFactor;
    out_data[OUT_SUB_SURFACE][lake] += snow.surface_flux * MM_PER_M *
                                    AreaFactor;
    out_data[OUT_SUB_BLOWING][lake] += snow.blowing_flux * MM_PER_M *
                                    AreaFactor;
    if (HasVeg) {
        tmp_evap += snow.canopy_vapor_flux * MM_PER_M;
        out_data[OUT_SUB_CANOP][lake] += snow.canopy_vapor_flux * MM_PER_M *
                                      AreaFactor;
    }
    if (HasVeg) {
        tmp_evap += veg_var.canopyevap;
        out_data[OUT_EVAP_CANOP][lake] += veg_var.canopyevap * AreaFactor;
    }
    out_data[OUT_EVAP][lake] += tmp_evap * AreaFactor;  // mm over gridcell

    /** record potential evap **/
    out_data[OUT_PET][lake] += cell.pot_evap * AreaFactor;

    /** record saturated area fraction **/
    out_data[OUT_ASAT][lake] += cell.asat * AreaFactor;

    /** record runoff **/
    out_data[OUT_RUNOFF][lake] += cell.runoff * AreaFactor;

    /** record baseflow **/
    out_data[OUT_BASEFLOW][lake] += cell.baseflow * AreaFactor;

    /** record inflow **/
    out_data[OUT_INFLOW][lake] += (cell.inflow) * AreaFactor;

    /** record canopy interception **/
    if (HasVeg) {
        out_data[OUT_WDEW][lake] += veg_var.Wdew * AreaFactor;
    }

    /** record LAI **/
    out_data[OUT_LAI][lake] += veg_var.LAI * AreaFactor;

    /** record fcanopy **/
    out_data[OUT_FCANOPY][lake] += veg_var.fcanopy * AreaFactor;

    /** record aerodynamic conductance and resistance **/
    if (cell.aero_resist[0] > DBL_EPSILON) {
        tmp_cond1 = (1 / cell.aero_resist[lake]) * AreaFactor;
    }
    else {
        tmp_cond1 = param.HUGE_RESIST;
    }
    out_data[OUT_AERO_COND1][lake] += tmp_cond1;
    if (overstory) {
        if (cell.aero_resist[1] > DBL_EPSILON) {
            tmp_cond2 = (1 / cell.aero_resist[1]) * AreaFactor;
        }
        else {
            tmp_cond2 = param.HUGE_RESIST;
        }
        out_data[OUT_AERO_COND2][lake] += tmp_cond2;
        out_data[OUT_AERO_COND][lake] += tmp_cond2;
    }
    else {
        out_data[OUT_AERO_COND][lake] += tmp_cond1;
    }

    /** record layer moistures **/
    for (index = 0; index < options.Nlayer; index++) {
        tmp_moist = cell.layer[index].moist;
        tmp_ice = 0;
        for (frost_area = 0; frost_area < options.Nfrost; frost_area++) {
            tmp_ice +=
                (cell.layer[index].ice[frost_area] * frost_fract[frost_area]);
        }
        tmp_moist -= tmp_ice;

        out_data[OUT_SOIL_LIQ][index * options.NLAKETYPES + lake] += tmp_moist * AreaFactor;
        out_data[OUT_SOIL_ICE][index * options.NLAKETYPES + lake] += tmp_ice * AreaFactor;
    }
    out_data[OUT_SOIL_WET][lake] += cell.wetness * AreaFactor;
    out_data[OUT_ROOTMOIST][lake] += cell.rootmoist * AreaFactor;

    /** record water table position **/
    out_data[OUT_ZWT][lake] += cell.zwt * AreaFactor;
    out_data[OUT_ZWT_LUMPED][lake] += cell.zwt_lumped * AreaFactor;

    /** record layer temperatures **/
    for (index = 0; index < options.Nlayer; index++) {
        out_data[OUT_SOIL_TEMP][index * options.NLAKETYPES + lake] += cell.layer[index].T * AreaFactor;
    }

    /*****************************
       Record Snow Pack Variables
    *****************************/

    /** record snow water equivalence **/
    out_data[OUT_SWE][lake] += snow.swq * AreaFactor * MM_PER_M;

    /** record snowpack depth **/
    out_data[OUT_SNOW_DEPTH][lake] += snow.depth * AreaFactor * CM_PER_M;

    /** record snowpack albedo, temperature **/
    if (snow.swq > 0.0) {
        out_data[OUT_SALBEDO][lake] += snow.albedo * AreaFactor;
        out_data[OUT_SNOW_SURF_TEMP][lake] += snow.surf_temp * AreaFactor;
        out_data[OUT_SNOW_PACK_TEMP][lake] += snow.pack_temp * AreaFactor;
    }

    /** record canopy intercepted snow **/
    if (HasVeg) {
        out_data[OUT_SNOW_CANOPY][lake] += (snow.snow_canopy) * AreaFactor *
                                        MM_PER_M;
    }

    /** record snowpack melt **/
    out_data[OUT_SNOW_MELT][lake] += snow.melt * AreaFactor;

    /** record snow cover fraction **/
    out_data[OUT_SNOW_COVER][lake] += snow.coverage * AreaFactor;

    /*****************************
       Record Carbon Cycling Variables
    *****************************/
    if (options.CARBON) {
        out_data[OUT_APAR][lake] += veg_var.aPAR * AreaFactor;
        out_data[OUT_GPP][lake] += veg_var.GPP * CONST_MWC / MOLE_PER_KMOLE *
                                CONST_CDAY *
                                AreaFactor;
        out_data[OUT_RAUT][lake] += veg_var.Raut * CONST_MWC /
                                 MOLE_PER_KMOLE * CONST_CDAY *
                                 AreaFactor;
        out_data[OUT_NPP][lake] += veg_var.NPP * CONST_MWC / MOLE_PER_KMOLE *
                                CONST_CDAY *
                                AreaFactor;
        out_data[OUT_LITTERFALL][lake] += veg_var.Litterfall * AreaFactor;
        out_data[OUT_RHET][lake] += cell.RhTot * AreaFactor;
        out_data[OUT_CLITTER][lake] += cell.CLitter * AreaFactor;
        out_data[OUT_CINTER][lake] += cell.CInter * AreaFactor;
        out_data[OUT_CSLOW][lake] += cell.CSlow * AreaFactor;
    }
}

/******************************************************************************
 * @brief    This routine collects energy balance terms.
 *****************************************************************************/
void
collect_eb_terms_lake_only(energy_bal_struct energy,
                 snow_data_struct  snow,
                 cell_data_struct  cell_wet,
                 double            Cv,
                 double            AreaFract,
                 double            TreeAdjustFactor,
                 bool              HasVeg,
                 bool              IsWet,
                 double            lakefactor,
                 bool              overstory,
                 int               band,
                 double           *frost_fract,
                 double            frost_slope,
                 double          **out_data,
                 size_t            lake)
{
    extern option_struct options;
    double               AreaFactor;
    double               tmp_fract;
    double               rad_temp;
    double               surf_temp;
    size_t               index;
    size_t               frost_area;

    AreaFactor = Cv * AreaFract * TreeAdjustFactor * lakefactor;

    /**********************************
       Record Frozen Soil Variables
    **********************************/

    /** record freezing and thawing front depths **/
    if (options.FROZEN_SOIL) {
        for (index = 0; index < MAX_FRONTS; index++) {
            if (energy.fdepth[index] != MISSING) {
                out_data[OUT_FDEPTH][index * options.NLAKETYPES + lake] += energy.fdepth[index] *
                                               AreaFactor * CM_PER_M;
            }
            if (energy.tdepth[index] != MISSING) {
                out_data[OUT_TDEPTH][index * options.NLAKETYPES + lake] += energy.tdepth[index] *
                                               AreaFactor * CM_PER_M;
            }
        }
    }

    tmp_fract = 0;
    for (frost_area = 0; frost_area < options.Nfrost; frost_area++) {
        if (cell_wet.layer[0].ice[frost_area]) {
            tmp_fract += frost_fract[frost_area];
        }
    }
    out_data[OUT_SURF_FROST_FRAC][lake] += tmp_fract * AreaFactor;

    tmp_fract = 0;
    if ((energy.T[0] + frost_slope / 2.) > 0) {
        if ((energy.T[0] - frost_slope / 2.) <= 0) {
            tmp_fract +=
                linear_interp(0, (energy.T[0] + frost_slope / 2.),
                              (energy.T[0] - frost_slope / 2.), 1,
                              0) * AreaFactor;
        }
    }
    else {
        tmp_fract += 1 * AreaFactor;
    }

    /**********************************
       Record Energy Balance Variables
    **********************************/

    /** record surface radiative temperature **/
    if (overstory && snow.snow && !(options.LAKES && IsWet)) {
        rad_temp = energy.Tfoliage + CONST_TKFRZ;
    }
    else {
        rad_temp = energy.Tsurf + CONST_TKFRZ;
    }

    /** record surface skin temperature **/
    surf_temp = energy.Tsurf;

    /** record landcover temperature **/
    if (!HasVeg) {
        // landcover is bare soil
        out_data[OUT_BARESOILT][lake] +=
            (rad_temp - CONST_TKFRZ) * AreaFactor;
    }
    else {
        // landcover is vegetation
        if (overstory && !snow.snow) {
            // here, rad_temp will be wrong since it will pick the understory temperature
            out_data[OUT_VEGT][lake] += energy.Tfoliage * AreaFactor;
        }
        else {
            out_data[OUT_VEGT][lake] += (rad_temp - CONST_TKFRZ) * AreaFactor;
        }
    }

    /** record mean surface temperature [C]  **/
    out_data[OUT_SURF_TEMP][lake] += surf_temp * AreaFactor;

    /** record thermal node temperatures **/
    for (index = 0; index < options.Nnode; index++) {
        out_data[OUT_SOIL_TNODE][index * options.NLAKETYPES + lake] += energy.T[index] * AreaFactor;
    }
    if (IsWet) {
        for (index = 0; index < options.Nnode; index++) {
            out_data[OUT_SOIL_TNODE_WL][index * options.NLAKETYPES + lake] = energy.T[index];
        }
    }

    /** record temperature flags  **/
    out_data[OUT_SURFT_FBFLAG][lake] += energy.Tsurf_fbflag * AreaFactor;
    for (index = 0; index < options.Nnode; index++) {
        out_data[OUT_SOILT_FBFLAG][index * options.NLAKETYPES + lake] += energy.T_fbflag[index] *
                                             AreaFactor;
    }
    out_data[OUT_SNOWT_FBFLAG][lake] += snow.surf_temp_fbflag * AreaFactor;
    out_data[OUT_TFOL_FBFLAG][lake] += energy.Tfoliage_fbflag * AreaFactor;
    out_data[OUT_TCAN_FBFLAG][lake] += energy.Tcanopy_fbflag * AreaFactor;

    /** record net shortwave radiation **/
    out_data[OUT_SWNET][lake] += energy.NetShortAtmos * AreaFactor;

    /** record net longwave radiation **/
    out_data[OUT_LWNET][lake] += energy.NetLongAtmos * AreaFactor;

    /** record incoming longwave radiation at ground surface (under veg) **/
    if (snow.snow && overstory) {
        out_data[OUT_IN_LONG][lake] += energy.LongOverIn * AreaFactor;
    }
    else {
        out_data[OUT_IN_LONG][lake] += energy.LongUnderIn * AreaFactor;
    }

    /** record albedo **/
    if (snow.snow && overstory) {
        out_data[OUT_ALBEDO][lake] += energy.AlbedoOver * AreaFactor;
    }
    else {
        out_data[OUT_ALBEDO][lake] += energy.AlbedoUnder * AreaFactor;
    }

    /** record latent heat flux **/
    out_data[OUT_LATENT][lake] -= energy.AtmosLatent * AreaFactor;

    /** record latent heat flux from sublimation **/
    out_data[OUT_LATENT_SUB][lake] -= energy.AtmosLatentSub * AreaFactor;

    /** record sensible heat flux **/
    out_data[OUT_SENSIBLE][lake] -= energy.AtmosSensible * AreaFactor;

    /** record ground heat flux (+ heat storage) **/
    out_data[OUT_GRND_FLUX][lake] -= energy.grnd_flux * AreaFactor;

    /** record heat storage **/
    out_data[OUT_DELTAH][lake] -= energy.deltaH * AreaFactor;

    /** record heat of fusion **/
    out_data[OUT_FUSION][lake] -= energy.fusion * AreaFactor;

    /** record radiative effective temperature [K],
        emissivities set = 1.0  **/
    out_data[OUT_RAD_TEMP][lake] +=
        ((rad_temp) * (rad_temp) * (rad_temp) * (rad_temp)) * AreaFactor;

    /** record snowpack cold content **/
    out_data[OUT_DELTACC][lake] += energy.deltaCC * AreaFactor;

    /** record snowpack advection **/
    if (snow.snow && overstory) {
        out_data[OUT_ADVECTION][lake] += energy.canopy_advection * AreaFactor;
    }
    out_data[OUT_ADVECTION][lake] += energy.advection * AreaFactor;

    /** record snow energy flux **/
    out_data[OUT_SNOW_FLUX][lake] += energy.snow_flux * AreaFactor;

    /** record refreeze energy **/
    if (snow.snow && overstory) {
        out_data[OUT_RFRZ_ENERGY][lake] += energy.canopy_refreeze *
                                        AreaFactor;
    }
    out_data[OUT_RFRZ_ENERGY][lake] += energy.refreeze_energy * AreaFactor;

    /** record melt energy **/
    out_data[OUT_MELT_ENERGY][lake] += energy.melt_energy * AreaFactor;

    /** record advected sensible heat energy **/
    if (!overstory) {
        out_data[OUT_ADV_SENS][lake] -= energy.advected_sensible * AreaFactor;
    }

    /**********************************
       Record Band-Specific Variables
    **********************************/

    /** record band snow water equivalent **/
    out_data[OUT_SWE_BAND][band * options.NLAKETYPES + lake] += snow.swq * Cv * lakefactor * MM_PER_M;

    /** record band snowpack depth **/
    out_data[OUT_SNOW_DEPTH_BAND][band * options.NLAKETYPES + lake] += snow.depth * Cv * lakefactor *
                                           CM_PER_M;

    /** record band canopy intercepted snow **/
    if (HasVeg) {
        out_data[OUT_SNOW_CANOPY_BAND][band * options.NLAKETYPES + lake] += (snow.snow_canopy) * Cv *
                                                lakefactor * MM_PER_M;
    }

    /** record band snow melt **/
    out_data[OUT_SNOW_MELT_BAND][band * options.NLAKETYPES + lake] += snow.melt * Cv * lakefactor;

    /** record band snow coverage **/
    out_data[OUT_SNOW_COVER_BAND][band * options.NLAKETYPES + lake] += snow.coverage * Cv * lakefactor;

    /** record band cold content **/
    out_data[OUT_DELTACC_BAND][band * options.NLAKETYPES + lake] += energy.deltaCC * Cv * lakefactor;

    /** record band advection **/
    out_data[OUT_ADVECTION_BAND][band * options.NLAKETYPES + lake] += energy.advection * Cv *
                                          lakefactor;

    /** record band snow flux **/
    out_data[OUT_SNOW_FLUX_BAND][band * options.NLAKETYPES + lake] += energy.snow_flux * Cv *
                                          lakefactor;

    /** record band refreeze energy **/
    out_data[OUT_RFRZ_ENERGY_BAND][band * options.NLAKETYPES + lake] += energy.refreeze_energy * Cv *
                                            lakefactor;

    /** record band melt energy **/
    out_data[OUT_MELT_ENERGY_BAND][band * options.NLAKETYPES + lake] += energy.melt_energy * Cv *
                                            lakefactor;

    /** record band advected sensble heat **/
    out_data[OUT_ADV_SENS_BAND][band * options.NLAKETYPES + lake] -= energy.advected_sensible * Cv *
                                         lakefactor;

    /** record surface layer temperature **/
    out_data[OUT_SNOW_SURFT_BAND][band * options.NLAKETYPES + lake] += snow.surf_temp * Cv *
                                           lakefactor;

    /** record pack layer temperature **/
    out_data[OUT_SNOW_PACKT_BAND][band * options.NLAKETYPES + lake] += snow.pack_temp * Cv *
                                           lakefactor;

    /** record latent heat of sublimation **/
    out_data[OUT_LATENT_SUB_BAND][band * options.NLAKETYPES + lake] += energy.latent_sub * Cv *
                                           lakefactor;

    /** record band net downwards shortwave radiation **/
    out_data[OUT_SWNET_BAND][band * options.NLAKETYPES + lake] += energy.NetShortAtmos * Cv *
                                      lakefactor;

    /** record band net downwards longwave radiation **/
    out_data[OUT_LWNET_BAND][band * options.NLAKETYPES + lake] += energy.NetLongAtmos * Cv *
                                      lakefactor;

    /** record band albedo **/
    if (snow.snow && overstory) {
        out_data[OUT_ALBEDO_BAND][band * options.NLAKETYPES + lake] += energy.AlbedoOver * Cv *
                                           lakefactor;
    }
    else {
        out_data[OUT_ALBEDO_BAND][band * options.NLAKETYPES + lake] += energy.AlbedoUnder * Cv *
                                           lakefactor;
    }

    /** record band net latent heat flux **/
    out_data[OUT_LATENT_BAND][band * options.NLAKETYPES + lake] -= energy.latent * Cv * lakefactor;

    /** record band net sensible heat flux **/
    out_data[OUT_SENSIBLE_BAND][band * options.NLAKETYPES + lake] -= energy.sensible * Cv * lakefactor;

    /** record band net ground heat flux **/
    out_data[OUT_GRND_FLUX_BAND][band * options.NLAKETYPES + lake] -= energy.grnd_flux * Cv *
                                          lakefactor;
}