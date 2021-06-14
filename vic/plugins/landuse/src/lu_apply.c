/******************************************************************************
 * @section DESCRIPTION
 *
 * Apply land-use vegetation fractions
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
* @brief   Distribute paddy balance variables
******************************************/
void
distribute_paddy_balance_terms(size_t  iCell,
                               size_t  iBand,
                               double *Cv_change,
                               double *Cv_old,
                               double *Cv_new,
                               double *Cv_change_tmp)
{
    extern option_struct       options;
    extern plugin_option_struct       plugin_options;
    extern soil_con_struct    *soil_con;
    extern veg_lib_struct    **veg_lib;
    extern veg_con_map_struct *veg_con_map;
    extern veg_con_struct    **veg_con;
    extern all_vars_struct    *all_vars;
    extern irr_con_map_struct    *irr_con_map;
    extern irr_con_struct    **irr_con;

    double                     Cv_avail;
    double                     red_frac;
    double                     add_frac;
    double                     inflow;

    // water-balance
    // veg_var
    double             Wdew;
    // snow
    double             surf_water;
    double             pack_water;
    double             snow_canopy;
    double             swq;
    // cell
    double             moist[MAX_LAYERS];
    double             ice[MAX_LAYERS][MAX_FROST_AREAS];

    veg_var_struct   **veg_var;
    cell_data_struct **cell;
    snow_data_struct **snow;

    size_t             iIrr;
    int                iVeg_bare;
    size_t            *iVegs;
    size_t             nVegs;
    size_t             i;
    size_t             iLayer;
    size_t             iFrost;
    size_t             veg_class;

    veg_var = all_vars[iCell].veg_var;
    cell = all_vars[iCell].cell;
    snow = all_vars[iCell].snow;

    /* WATER-BALANCE */
    
    iVegs = malloc(options.NVEGTYPES * sizeof(*iVegs));
    check_alloc_status(iVegs, "Memory allocation error");
    
    iVeg_bare = veg_con_map[iCell].vidx[plugin_options.Pbare - 1];
    nVegs = 0;
    for(iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++){
        if(irr_con[iCell][iIrr].paddy){
            if (Cv_change[irr_con[iCell][iIrr].veg_index] < -MINCOVERAGECHANGE || 
                    Cv_change[irr_con[iCell][iIrr].veg_index] > MINCOVERAGECHANGE) {
                iVegs[nVegs] = irr_con[iCell][iIrr].veg_index;
                nVegs++;
            }
        }
    }
    
    if(iVeg_bare == NODATA_VEG || nVegs == 0){
        if(nVegs != 0){
            log_err("Paddy vegetation type but no paddy bare vegetation type?");
        }
        free(iVegs);
        return;
    }

    iVegs[nVegs] = iVeg_bare;
    nVegs++;

    // Get available area to redistribute
    Cv_avail = 0.0;
    for (i = 0; i < nVegs; i++) {
        if (Cv_change[iVegs[i]] > MINCOVERAGECHANGE) {
            Cv_avail += Cv_change[iVegs[i]];
        }
    }

    // get
    Wdew = 0.0;
    pack_water = 0.0;
    surf_water = 0.0;
    swq = 0.0;
    snow_canopy = 0.0;
    for (iLayer = 0; iLayer < MAX_LAYERS; iLayer++) {
        moist[iLayer] = 0.0;
        for (iFrost = 0; iFrost < MAX_FROST_AREAS; iFrost++) {
            ice[iLayer][iFrost] = 0.0;
        }
    }

    for (i = 0; i < nVegs; i++) {
        if (Cv_change[iVegs[i]] < -MINCOVERAGECHANGE) {
            Wdew += veg_var[iVegs[i]][iBand].Wdew * -Cv_change[iVegs[i]];
            pack_water += snow[iVegs[i]][iBand].pack_water * -Cv_change[iVegs[i]];
            surf_water += snow[iVegs[i]][iBand].surf_water * -Cv_change[iVegs[i]];
            swq += snow[iVegs[i]][iBand].swq * -Cv_change[iVegs[i]];
            snow_canopy += snow[iVegs[i]][iBand].snow_canopy * -Cv_change[iVegs[i]];
            for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                moist[iLayer] += cell[iVegs[i]][iBand].layer[iLayer].moist *
                                 -Cv_change[iVegs[i]];
                for (iFrost = 0; iFrost < options.Nfrost; iFrost++) {
                    ice[iLayer][iFrost] +=
                        cell[iVegs[i]][iBand].layer[iLayer].ice[iFrost] *
                        -Cv_change[iVegs[i]];
                }
            }
        }
    }

    // set
    for (i = 0; i < nVegs; i++) {
        if (Cv_change[iVegs[i]] > MINCOVERAGECHANGE) {
            red_frac = Cv_old[iVegs[i]] / Cv_new[iVegs[i]];
            add_frac = (Cv_change[iVegs[i]] / Cv_avail) / Cv_new[iVegs[i]];

            veg_var[iVegs[i]][iBand].Wdew = veg_var[iVegs[i]][iBand].Wdew * red_frac +
                                        Wdew * add_frac;
            snow[iVegs[i]][iBand].pack_water = snow[iVegs[i]][iBand].pack_water *
                                           red_frac + pack_water * add_frac;
            snow[iVegs[i]][iBand].surf_water = snow[iVegs[i]][iBand].surf_water *
                                           red_frac + surf_water * add_frac;
            snow[iVegs[i]][iBand].swq = snow[iVegs[i]][iBand].swq * red_frac + swq *
                                    add_frac;
            snow[iVegs[i]][iBand].snow_canopy = snow[iVegs[i]][iBand].snow_canopy *
                                            red_frac + snow_canopy * add_frac;
            for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                cell[iVegs[i]][iBand].layer[iLayer].moist =
                    cell[iVegs[i]][iBand].layer[iLayer].moist * red_frac +
                    moist[iLayer] * add_frac;
                for (iFrost = 0; iFrost < options.Nfrost; iFrost++) {
                    cell[iVegs[i]][iBand].layer[iLayer].ice[iFrost] =
                        cell[iVegs[i]][iBand].layer[iLayer].ice[iFrost] * red_frac +
                        ice[iLayer][iFrost] * add_frac;
                }
            }

            veg_class = veg_con[iCell][iVegs[i]].veg_class;
            if (!veg_lib[iCell][veg_class].overstory ||
                veg_var[iVegs[i]][iBand].LAI <= 0. ||
                veg_class == options.NVEGTYPES) {
                // Remove intercepted canopy snow for vegetation without overstory or leaves
                snow[iVegs[i]][iBand].swq += snow[iVegs[i]][iBand].snow_canopy;
                snow[iVegs[i]][iBand].snow_canopy = 0.;
            }

            if (veg_var[iVegs[i]][iBand].LAI <= 0. ||
                veg_class == options.NVEGTYPES) {
                // Remove intercepted canopy water for vegetation without leaves
                cell[iVegs[i]][iBand].layer[0].moist += veg_var[iVegs[i]][iBand].Wdew;
                veg_var[iVegs[i]][iBand].Wdew = 0.;
            }
            
            // Gather excess soil moisture
            inflow = 0.0;
            for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                if (cell[iVegs[i]][iBand].layer[iLayer].moist >
                    soil_con[iCell].max_moist[iLayer]) {
                    inflow += cell[iVegs[i]][iBand].layer[iLayer].moist -
                              soil_con[iCell].max_moist[iLayer];
                }
            }
            
            if(inflow > 0) {
                // Redistribute excess soil moisture
                for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                    cell[iVegs[i]][iBand].layer[iLayer].moist += inflow;
                    inflow = 0.0;
                    if (cell[iVegs[i]][iBand].layer[iLayer].moist >
                        soil_con[iCell].max_moist[iLayer]) {
                        inflow = cell[iVegs[i]][iBand].layer[iLayer].moist -
                                 soil_con[iCell].max_moist[iLayer];
                    }
                    if(inflow == 0.){
                        break;
                    }
                }
                if (inflow != 0.) {
                    log_warn("Could not redistribute (part of) water balance [%.4f mm] since soil is full",
                             inflow);
                }
            }
        }
    }
    
    for (i = 0; i < nVegs; i++) {
        Cv_change_tmp[iVegs[i]] = 0.;
    }
    
    free(iVegs);
}

/******************************************
* @brief   Distribute water balance variables
******************************************/
void
distribute_water_balance_terms(size_t  iCell,
                               size_t  iBand,
                               double *Cv_change,
                               double *Cv_old,
                               double *Cv_new)
{
    extern option_struct       options;
    extern soil_con_struct    *soil_con;
    extern veg_lib_struct    **veg_lib;
    extern veg_con_map_struct *veg_con_map;
    extern veg_con_struct    **veg_con;
    extern all_vars_struct    *all_vars;

    double                    *Cv_change_tmp;
    double                     Cv_avail;
    double                     red_frac;
    double                     add_frac;
    double                     before_moist;
    double                     after_moist;
    double                     inflow;

    // water-balance
    // veg_var
    double             Wdew;
    // snow
    double             surf_water;
    double             pack_water;
    double             snow_canopy;
    double             swq;
    // cell
    double             moist[MAX_LAYERS];
    double             ice[MAX_LAYERS][MAX_FROST_AREAS];

    veg_var_struct   **veg_var;
    cell_data_struct **cell;
    snow_data_struct **snow;

    size_t             iVeg;
    size_t             iLayer;
    size_t             iFrost;
    size_t             veg_class;

    veg_var = all_vars[iCell].veg_var;
    cell = all_vars[iCell].cell;
    snow = all_vars[iCell].snow;

    Cv_change_tmp = malloc(options.NVEGTYPES * sizeof(*Cv_change_tmp));
    check_alloc_status(Cv_change, "Memory allocation error");
    
    /* WATER-BALANCE */

    before_moist = calculate_total_water(iCell, iBand, Cv_old, Cv_change);
    
    // Do paddy changes
    for(iVeg = 0; iVeg < options.NVEGTYPES; iVeg++){
        Cv_change_tmp[iVeg] = Cv_change[iVeg];
    }
    if(plugin_options.IRRIGATION && 
            plugin_options.Pbare != options.NVEGTYPES){
        // Paddy
        distribute_paddy_balance_terms(iCell, iBand, 
                Cv_change, Cv_old, Cv_new, Cv_change_tmp);
    }

    // Get available area to redistribute
    Cv_avail = 0.0;
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change_tmp[iVeg] > MINCOVERAGECHANGE) {
            Cv_avail += Cv_change_tmp[iVeg];
        }
    }

    // get
    Wdew = 0.0;
    pack_water = 0.0;
    surf_water = 0.0;
    swq = 0.0;
    snow_canopy = 0.0;
    for (iLayer = 0; iLayer < MAX_LAYERS; iLayer++) {
        moist[iLayer] = 0.0;
        for (iFrost = 0; iFrost < MAX_FROST_AREAS; iFrost++) {
            ice[iLayer][iFrost] = 0.0;
        }
    }

    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change_tmp[iVeg] < -MINCOVERAGECHANGE) {
            Wdew += veg_var[iVeg][iBand].Wdew * -Cv_change_tmp[iVeg];
            pack_water += snow[iVeg][iBand].pack_water * -Cv_change_tmp[iVeg];
            surf_water += snow[iVeg][iBand].surf_water * -Cv_change_tmp[iVeg];
            swq += snow[iVeg][iBand].swq * -Cv_change_tmp[iVeg];
            snow_canopy += snow[iVeg][iBand].snow_canopy * -Cv_change_tmp[iVeg];
            for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                moist[iLayer] += cell[iVeg][iBand].layer[iLayer].moist *
                                 -Cv_change_tmp[iVeg];
                for (iFrost = 0; iFrost < options.Nfrost; iFrost++) {
                    ice[iLayer][iFrost] +=
                        cell[iVeg][iBand].layer[iLayer].ice[iFrost] *
                        -Cv_change_tmp[iVeg];
                }
            }
        }
    }

    // set
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change_tmp[iVeg] > MINCOVERAGECHANGE) {
            red_frac = Cv_old[iVeg] / Cv_new[iVeg];
            add_frac = (Cv_change_tmp[iVeg] / Cv_avail) / Cv_new[iVeg];

            veg_var[iVeg][iBand].Wdew = veg_var[iVeg][iBand].Wdew * red_frac +
                                        Wdew * add_frac;
            snow[iVeg][iBand].pack_water = snow[iVeg][iBand].pack_water *
                                           red_frac + pack_water * add_frac;
            snow[iVeg][iBand].surf_water = snow[iVeg][iBand].surf_water *
                                           red_frac + surf_water * add_frac;
            snow[iVeg][iBand].swq = snow[iVeg][iBand].swq * red_frac + swq *
                                    add_frac;
            snow[iVeg][iBand].snow_canopy = snow[iVeg][iBand].snow_canopy *
                                            red_frac + snow_canopy * add_frac;
            for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                cell[iVeg][iBand].layer[iLayer].moist =
                    cell[iVeg][iBand].layer[iLayer].moist * red_frac +
                    moist[iLayer] * add_frac;
                for (iFrost = 0; iFrost < options.Nfrost; iFrost++) {
                    cell[iVeg][iBand].layer[iLayer].ice[iFrost] =
                        cell[iVeg][iBand].layer[iLayer].ice[iFrost] * red_frac +
                        ice[iLayer][iFrost] * add_frac;
                }
            }

            veg_class = veg_con[iCell][iVeg].veg_class;
            if (!veg_lib[iCell][veg_class].overstory ||
                veg_var[iVeg][iBand].LAI <= 0. ||
                veg_class == options.NVEGTYPES) {
                // Remove intercepted canopy snow for vegetation without overstory or leaves
                snow[iVeg][iBand].swq += snow[iVeg][iBand].snow_canopy;
                snow[iVeg][iBand].snow_canopy = 0.;
            }

            if (veg_var[iVeg][iBand].LAI <= 0. ||
                veg_class == options.NVEGTYPES) {
                // Remove intercepted canopy water for vegetation without leaves
                cell[iVeg][iBand].layer[0].moist += veg_var[iVeg][iBand].Wdew;
                veg_var[iVeg][iBand].Wdew = 0.;
            }
            
            // Gather excess soil moisture
            inflow = 0.0;
            for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                if (cell[iVeg][iBand].layer[iLayer].moist >
                    soil_con[iCell].max_moist[iLayer]) {
                    inflow += cell[iVeg][iBand].layer[iLayer].moist -
                              soil_con[iCell].max_moist[iLayer];
                }
            }
            
            if(inflow > 0) {
                // Redistribute excess soil moisture
                for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                    cell[iVeg][iBand].layer[iLayer].moist += inflow;
                    inflow = 0.0;
                    if (cell[iVeg][iBand].layer[iLayer].moist >
                        soil_con[iCell].max_moist[iLayer]) {
                        inflow = cell[iVeg][iBand].layer[iLayer].moist -
                                 soil_con[iCell].max_moist[iLayer];
                    }
                    if(inflow == 0.){
                        break;
                    }
                }
                if (inflow != 0.) {
                    log_warn("Could not redistribute (part of) water balance [%.4f mm] since soil is full",
                             inflow * Cv_new[iVeg]);
                }
            }
        }
    }

    after_moist = calculate_total_water(iCell, iBand, Cv_new, Cv_change);

    // Check water balance
    if (abs(before_moist - after_moist) > DBL_EPSILON) {
        for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
            fprintf(LOG_DEST, "\niBand %zu iVeg %zu\n"
                    "\t\tAfter:\n"
                    "Cv\t\t[%.8f]\t[%.8f]\t[%.16f]\n"
                    "Wdew\t[%.4f mm]\n"
                    "pack_water\t[%.4f mm]\n"
                    "surf_water\t[%.4f mm]\n"
                    "swq\t[%.4f mm]\n"
                    "snow_canopy\t[%.4f mm]\n",
                    iBand, iVeg,
                    Cv_old[iVeg], Cv_new[iVeg], Cv_change_tmp[iVeg],
                    veg_var[iVeg][iBand].Wdew,
                    snow[iVeg][iBand].pack_water,
                    snow[iVeg][iBand].surf_water,
                    snow[iVeg][iBand].swq,
                    snow[iVeg][iBand].snow_canopy);
            for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                fprintf(LOG_DEST, "moist %zu\t[%.4f mm]\n",
                        iLayer, cell[iVeg][iBand].layer[iLayer].moist);
            }
        }
        fprintf(LOG_DEST, "\n\t\tTotals:\n"
                "Wdew\t[%.4f mm]\n"
                "pack_water\t[%.4f mm]\n"
                "surf_water\t[%.4f mm]\n"
                "swq\t[%.4f mm]\n"
                "snow_canopy\t[%.4f mm]\n",
                Wdew,
                pack_water,
                surf_water,
                swq,
                snow_canopy);
        for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
            fprintf(LOG_DEST, "moist %zu\t[%.4f mm]\n",
                    iLayer, moist[iLayer]);
        }
    }
    
    free(Cv_change_tmp);
}

/******************************************
* @brief   Distribute carbon balance variables
******************************************/
void
distribute_carbon_balance_terms(size_t  iCell,
                                size_t  iBand,
                                double *Cv_change,
                                double *Cv_old,
                                double *Cv_new)
{
    extern veg_con_map_struct *veg_con_map;
    extern all_vars_struct    *all_vars;

    double                     Cv_avail;
    double                     red_frac;
    double                     add_frac;
    double                     before_carbon;
    double                     after_carbon;

    // carbon-balance
    // veg_var
    double             AnnualNPP;
    double             AnnualNPPPrev;
    // cell
    double             CLitter;
    double             CInter;
    double             CSlow;

    veg_var_struct   **veg_var;
    cell_data_struct **cell;

    size_t             iVeg;

    veg_var = all_vars[iCell].veg_var;
    cell = all_vars[iCell].cell;

    /* CARBON-BALANCE */

    before_carbon = calculate_total_carbon(iCell, iBand, Cv_old, Cv_change);

    // Get available area to redistribute
    Cv_avail = 0.0;
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change[iVeg] > MINCOVERAGECHANGE) {
            Cv_avail += Cv_change[iVeg];
        }
    }

    // get
    AnnualNPP = 0.0;
    AnnualNPPPrev = 0.0;
    CLitter = 0.0;
    CInter = 0.0;
    CSlow = 0.0;
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change[iVeg] < -MINCOVERAGECHANGE) {
            AnnualNPP += veg_var[iVeg][iBand].AnnualNPP * -Cv_change[iVeg];
            AnnualNPPPrev += veg_var[iVeg][iBand].AnnualNPPPrev *
                             -Cv_change[iVeg];
            CLitter += cell[iVeg][iBand].CLitter * -Cv_change[iVeg];
            CInter += cell[iVeg][iBand].CInter * -Cv_change[iVeg];
            CSlow += cell[iVeg][iBand].CSlow * -Cv_change[iVeg];
        }
    }

    // set
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change[iVeg] > MINCOVERAGECHANGE) {
            red_frac = Cv_old[iVeg] / Cv_new[iVeg];
            add_frac = (Cv_change[iVeg] / Cv_avail) / Cv_new[iVeg];

            veg_var[iVeg][iBand].AnnualNPP = veg_var[iVeg][iBand].AnnualNPP *
                                             red_frac + AnnualNPP * add_frac;
            veg_var[iVeg][iBand].AnnualNPPPrev =
                veg_var[iVeg][iBand].AnnualNPPPrev * red_frac + AnnualNPPPrev *
                add_frac;
            cell[iVeg][iBand].CLitter = cell[iVeg][iBand].CLitter * red_frac +
                                        CLitter * add_frac;
            cell[iVeg][iBand].CInter = cell[iVeg][iBand].CInter * red_frac +
                                       CInter * add_frac;
            cell[iVeg][iBand].CSlow = cell[iVeg][iBand].CSlow * red_frac +
                                      CSlow * add_frac;
        }
    }

    after_carbon = calculate_total_carbon(iCell, iBand, Cv_new, Cv_change);

    // Check water balance
    if (abs(before_carbon - after_carbon) > DBL_EPSILON) {
        for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
            fprintf(LOG_DEST, "\niBand %zu iVeg %zu\n"
                    "\t\tAfter:\n"
                    "Cv\t\t[%.8f]\t[%.8f]\t[%.16f]\n"
                    "AnnualNPP\t[%.4f mm]\n"
                    "AnnualNPPPrev\t[%.4f mm]\n"
                    "CLitter\t[%.4f mm]\n"
                    "CInter\t[%.4f mm]\n"
                    "CSlow\t[%.4f mm]\n",
                    iBand, iVeg,
                    Cv_old[iVeg], Cv_new[iVeg], Cv_change[iVeg],
                    veg_var[iVeg][iBand].AnnualNPP,
                    veg_var[iVeg][iBand].AnnualNPPPrev,
                    cell[iVeg][iBand].CLitter,
                    cell[iVeg][iBand].CInter,
                    cell[iVeg][iBand].CSlow);
        }
        fprintf(LOG_DEST, "\n\t\tTotals:\n"
                "AnnualNPP\t[%.4f mm]\n"
                "AnnualNPPPrev\t[%.4f mm]\n"
                "CLitter\t[%.4f mm]\n"
                "CInter\t[%.4f mm]\n"
                "CSlow\t[%.4f mm]\n",
                AnnualNPP,
                AnnualNPPPrev,
                CLitter,
                CInter,
                CSlow);
    }
}

/******************************************
* @brief   Distribute energy balance variables
******************************************/
void
distribute_energy_balance_terms(size_t   iCell,
                                size_t   iBand,
                                double  *Cv_change,
                                double  *Cv_old,
                                double  *Cv_new,
                                double  *snow_surf_capacity,
                                double  *snow_pack_capacity,
                                double **node_capacity,
                                double  *orig_surf_tempEnergy,
                                double  *orig_pack_tempEnergy,
                                double **orig_TEnergy,
                                double  *new_surf_tempEnergy,
                                double  *new_pack_tempEnergy,
                                double **new_TEnergy)
{
    extern option_struct       options;
    extern veg_con_map_struct *veg_con_map;
    extern all_vars_struct    *all_vars;

    double                     Cv_avail;
    double                     red_frac;
    double                     add_frac;
    double                     before_energy;
    double                     after_energy;

    // energy-balance
    // snow
    double              surf_tempEnergy;
    double              pack_tempEnergy;
    // energy
    double              TEnergy[MAX_NODES];

    snow_data_struct  **snow;
    energy_bal_struct **energy;

    size_t              iVeg;
    size_t              iNode;

    snow = all_vars[iCell].snow;
    energy = all_vars[iCell].energy;

    /* ENERGY-BALANCE */

    before_energy = calculate_total_energy(iCell, 
            orig_surf_tempEnergy, orig_pack_tempEnergy, orig_TEnergy, Cv_old, Cv_change);

    // Get available area to redistribute
    Cv_avail = 0.0;
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change[iVeg] > MINCOVERAGECHANGE) {
            Cv_avail += Cv_change[iVeg];
        }
    }

    // get
    pack_tempEnergy = 0.0;
    surf_tempEnergy = 0.0;
    for (iNode = 0; iNode < options.Nnode; iNode++) {
        TEnergy[iNode] = 0.0;
    }
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change[iVeg] < -MINCOVERAGECHANGE) {
            surf_tempEnergy += orig_surf_tempEnergy[iVeg] * -Cv_change[iVeg];
            pack_tempEnergy += orig_pack_tempEnergy[iVeg] * -Cv_change[iVeg];
            for (iNode = 0; iNode < options.Nnode; iNode++) {
                TEnergy[iNode] += orig_TEnergy[iVeg][iNode] * -Cv_change[iVeg];
            }
        }
    }

    // set
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change[iVeg] > MINCOVERAGECHANGE) {
            red_frac = Cv_old[iVeg] / Cv_new[iVeg];
            add_frac = (Cv_change[iVeg] / Cv_avail) / Cv_new[iVeg];

            new_pack_tempEnergy[iVeg] = orig_pack_tempEnergy[iVeg] * red_frac +
                                        pack_tempEnergy * add_frac;
            new_surf_tempEnergy[iVeg] = orig_surf_tempEnergy[iVeg] * red_frac +
                                        surf_tempEnergy * add_frac;
            for (iNode = 0; iNode < options.Nnode; iNode++) {
                new_TEnergy[iVeg][iNode] = orig_TEnergy[iVeg][iNode] *
                                           red_frac + TEnergy[iNode] * add_frac;
            }
        }
        else {
            new_pack_tempEnergy[iVeg] = orig_pack_tempEnergy[iVeg];
            new_surf_tempEnergy[iVeg] = orig_surf_tempEnergy[iVeg];
            for (iNode = 0; iNode < options.Nnode; iNode++) {
                new_TEnergy[iVeg][iNode] = orig_TEnergy[iVeg][iNode];
            }
        }
    }
    
    after_energy = calculate_total_energy(iCell, 
            new_surf_tempEnergy, new_pack_tempEnergy, new_TEnergy, Cv_new, Cv_change);

    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (snow_pack_capacity[iVeg] > 0) {
            snow[iVeg][iBand].pack_temp = new_pack_tempEnergy[iVeg] /
                                          snow_pack_capacity[iVeg];
        }
        else {
            if (new_pack_tempEnergy[iVeg] > 0) {
                new_surf_tempEnergy[iVeg] += new_pack_tempEnergy[iVeg];
            }
        }

        if (snow_surf_capacity[iVeg] > 0) {
            snow[iVeg][iBand].surf_temp = new_surf_tempEnergy[iVeg] /
                                          snow_surf_capacity[iVeg];
        }
        else {
            if (new_surf_tempEnergy[iVeg] > 0) {
                new_TEnergy[iVeg][0] += new_surf_tempEnergy[iVeg];
            }
        }

        for (iNode = 0; iNode < options.Nnode; iNode++) {
            if (node_capacity[iVeg][iNode] > 0) {
                energy[iVeg][iBand].T[iNode] = new_TEnergy[iVeg][iNode] /
                                               node_capacity[iVeg][iNode];
            }
        }
    }

    // Check energy balance
    if (abs(before_energy - after_energy) > DBL_EPSILON) {
        for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
            fprintf(LOG_DEST, "\niBand %zu iVeg %zu\n"
                    "\t\tBefore\tAfter:\n"
                    "Cv\t\t[%.8f]\t[%.8f]\t[%.16f]\n"
                    "surf_tempEnergy\t[%.4f J m-2]\t[%.4f J m-2]\n"
                    "pack_tempEnergy\t[%.4f J m-2]\t[%.4f J m-2]\n",
                    iBand, iVeg,
                    Cv_old[iVeg], Cv_new[iVeg], Cv_change[iVeg],
                    orig_surf_tempEnergy[iVeg], new_surf_tempEnergy[iVeg],
                    orig_pack_tempEnergy[iVeg], new_pack_tempEnergy[iVeg]);
            for (iNode = 0; iNode < options.Nnode; iNode++) {
                fprintf(LOG_DEST, "TEnergy %zu\t[%.4f J m-2]\t[%.4f J m-2]\n",
                        iNode, orig_TEnergy[iVeg][iNode],
                        new_TEnergy[iVeg][iNode]);
            }
            fprintf(LOG_DEST, "snow_surf_cap\t[%.4f J m-3 K-1]\n"
                    "snow_pack_cap\t[%.4f J m-3 K-1]\n",
                    snow_surf_capacity[iVeg],
                    snow_pack_capacity[iVeg]);
            for (iNode = 0; iNode < options.Nnode; iNode++) {
                fprintf(LOG_DEST, "node_cap %zu\t[%.4f J m-3 K-1]\n",
                        iNode,
                        node_capacity[iVeg][iNode]);
            }
        }
        fprintf(LOG_DEST, "\n\t\tTotals:\n"
                "surf_tempEnergy\t[%.4f J m-2]\n"
                "pack_tempEnergy\t[%.4f J m-2]\n",
                surf_tempEnergy,
                pack_tempEnergy);
        for (iNode = 0; iNode < options.Nnode; iNode++) {
            fprintf(LOG_DEST, "TEnergy %zu\t[%.4f J m-2]\n",
                    iNode, TEnergy[iNode]);
        }
    }
}

/******************************************
* @brief   Distribute irrigation balance variables
******************************************/
void
distribute_irrigation_balance_terms(size_t  iCell,
                                    size_t  iBand,
                                    double *Cv_change,
                                    double *Cv_old,
                                    double *Cv_new)
{
    extern global_param_struct        global_param;
    extern plugin_global_param_struct        plugin_global_param;
    extern plugin_save_data_struct *plugin_save_data;
    extern domain_struct              local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct    **irr_con;
    extern irr_var_struct   ***irr_var;

    double                     Cv_avail;
    double                     before_moist;
    double                     after_moist;

    double             leftover;
    double             leftover_discharge_tmp;

    irr_var_struct   **irr;

    size_t             iVeg;
    size_t             iIrr;
    size_t             iStep;
    size_t                            rout_steps_per_dt;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;
    
    irr = irr_var[iCell];

    /* WATER-BALANCE */
    
    before_moist = calculate_total_irrigation(iCell, iBand, Cv_old, Cv_change);

    // Get available area to redistribute
    Cv_avail = 0.0;
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change[iVeg] > MINCOVERAGECHANGE) {
            Cv_avail += Cv_change[iVeg];
        }
    }

    // get
    leftover = 0.0;
    for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
        iVeg = irr_con[iCell][iIrr].veg_index;
        if (Cv_change[iVeg] < -MINCOVERAGECHANGE || Cv_change[iVeg] > MINCOVERAGECHANGE) {
            leftover += irr[iIrr][iBand].leftover * Cv_old[iVeg];
        }
    }


    // set
    for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
        iVeg = irr_con[iCell][iIrr].veg_index;
        if (Cv_change[iVeg] < -MINCOVERAGECHANGE || Cv_change[iVeg] > MINCOVERAGECHANGE) {
            irr[iIrr][iBand].leftover = 0.0;
        }
    }
    
    leftover_discharge_tmp = leftover / MM_PER_M * local_domain.locations[iCell].area / global_param.dt;
    for (iStep = rout_steps_per_dt; iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1; iStep++) {
        // Scale leftover proportionally to length
        rout_var[iCell].dt_discharge[iStep] += leftover_discharge_tmp / (plugin_options.UH_LENGTH - 1);
        if (rout_var[iCell].dt_discharge[iStep] < 0) {
            rout_var[iCell].dt_discharge[iStep] = 0.;
        }
    }

    // Recalculate discharge and stream moisture
    rout_var[iCell].discharge = 0.;
    rout_var[iCell].stream = 0.;
    for (iStep = 0; iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1; iStep++) {
        if (iStep < rout_steps_per_dt) {
            rout_var[iCell].discharge += rout_var[iCell].dt_discharge[iStep];
        }
        else {
            rout_var[iCell].stream += rout_var[iCell].dt_discharge[iStep];
        }
    }
        
    after_moist = calculate_total_irrigation(iCell, iBand, Cv_new, Cv_change);

    plugin_save_data[iCell].total_moist_storage += leftover;
    
    // Check water balance
    if (abs(before_moist - after_moist) > DBL_EPSILON) {
        for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
            iVeg = irr_con[iCell][iIrr].veg_index;
            
            fprintf(LOG_DEST, "\niBand %zu iVeg %zu\n"
                    "\t\tAfter:\n"
                    "Cv\t\t[%.8f]\t[%.8f]\t[%.16f]\n"
                    "leftover\t[%.4f mm]\n",
                    iBand, iVeg,
                    Cv_old[iVeg], Cv_new[iVeg], Cv_change[iVeg],
                    irr[iVeg][iIrr].leftover);
        }
        fprintf(LOG_DEST, "\n\t\tTotals:\n"
                "leftover\t[%.4f mm]\n",
                leftover);
        log_err("\nIrrigation balance error for cell %zu:\n"
                "Initial water content [%.4f mm]\tFinal water content [%.4f mm]",
                iCell,
                before_moist,
                after_moist);
    }
}

/******************************************
* @brief   Apply land-use vegetation fractions
******************************************/
void
lu_apply(void)
{
    extern domain_struct       local_domain;
    extern option_struct       options;
    extern plugin_option_struct       plugin_options;
    extern veg_con_map_struct *veg_con_map;
    extern veg_con_struct    **veg_con;
    extern lu_force_struct    *lu_force;
    extern soil_con_struct    *soil_con;

    char                       locstr[MAXSTRING];
    double                     Cv_sum;
    double                    *Cv_old;
    double                    *Cv_new;
    double                    *Cv_change;

    double                    *snow_surf_capacity;
    double                    *snow_pack_capacity;
    double                   **node_capacity;
    double                    *orig_surf_tempEnergy;
    double                    *orig_pack_tempEnergy;
    double                   **orig_TEnergy;
    double                    *new_surf_tempEnergy;
    double                    *new_pack_tempEnergy;
    double                   **new_TEnergy;
    
    double before_water;
    double before_carbon;
    double before_energy;
    double after_water;
    double after_carbon;
    double after_energy;
    double max_water;
    double max_carbon;
    double max_energy;

    size_t                     iCell;
    size_t                     iVeg;
    size_t                     iBand;
    size_t                     veg_class;

    Cv_old = malloc(options.NVEGTYPES * sizeof(*Cv_old));
    check_alloc_status(Cv_old, "Memory allocation error");
    Cv_new = malloc(options.NVEGTYPES * sizeof(*Cv_new));
    check_alloc_status(Cv_new, "Memory allocation error");
    Cv_change = malloc(options.NVEGTYPES * sizeof(*Cv_change));
    check_alloc_status(Cv_change, "Memory allocation error");

    snow_surf_capacity =
        malloc(options.NVEGTYPES * sizeof(*snow_surf_capacity));
    check_alloc_status(snow_surf_capacity, "Memory allocation error");
    snow_pack_capacity =
        malloc(options.NVEGTYPES * sizeof(*snow_pack_capacity));
    check_alloc_status(snow_pack_capacity, "Memory allocation error");
    node_capacity = malloc(options.NVEGTYPES * sizeof(*node_capacity));
    check_alloc_status(node_capacity, "Memory allocation error");
    orig_surf_tempEnergy =
        malloc(options.NVEGTYPES * sizeof(*orig_surf_tempEnergy));
    check_alloc_status(orig_surf_tempEnergy, "Memory allocation error");
    orig_pack_tempEnergy =
        malloc(options.NVEGTYPES * sizeof(*orig_pack_tempEnergy));
    check_alloc_status(orig_pack_tempEnergy, "Memory allocation error");
    orig_TEnergy = malloc(options.NVEGTYPES * sizeof(*orig_TEnergy));
    check_alloc_status(orig_TEnergy, "Memory allocation error");
    new_surf_tempEnergy = malloc(
        options.NVEGTYPES * sizeof(*new_surf_tempEnergy));
    check_alloc_status(new_surf_tempEnergy, "Memory allocation error");
    new_pack_tempEnergy = malloc(
        options.NVEGTYPES * sizeof(*new_pack_tempEnergy));
    check_alloc_status(new_pack_tempEnergy, "Memory allocation error");
    new_TEnergy = malloc(options.NVEGTYPES * sizeof(*new_TEnergy));
    check_alloc_status(new_TEnergy, "Memory allocation error");
    for (iVeg = 0; iVeg < options.NVEGTYPES; iVeg++) {
        node_capacity[iVeg] = malloc(
            options.Nnode * sizeof(*node_capacity[iVeg]));
        check_alloc_status(node_capacity[iVeg], "Memory allocation error");
        orig_TEnergy[iVeg] =
            malloc(options.Nnode * sizeof(*orig_TEnergy[iVeg]));
        check_alloc_status(orig_TEnergy[iVeg], "Memory allocation error");
        new_TEnergy[iVeg] = malloc(options.Nnode * sizeof(*new_TEnergy[iVeg]));
        check_alloc_status(new_TEnergy[iVeg], "Memory allocation error");
    }

    for (iCell = 0; iCell < local_domain.ncells_active; iCell++) {
        // Initialize
        Cv_sum = 0.0;

        // Calculate
        for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
            Cv_sum += lu_force[iCell].Cv[iVeg];
            Cv_old[iVeg] = veg_con[iCell][iVeg].Cv;
        }

        // Set
        for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
            veg_class = lu_force[iCell].veg_class[iVeg];

            veg_con_map[iCell].Cv[veg_class] = lu_force[iCell].Cv[iVeg] /
                                               Cv_sum;
            veg_con[iCell][iVeg].Cv = lu_force[iCell].Cv[iVeg] / Cv_sum;
        }

        // Check
        if (Cv_sum <= 0.0) {
            sprint_location(locstr, &(local_domain.locations[iCell]));
            log_err("Sum of veg tile area fractions !=  1.0 (%.16f) at grid "
                    "cell %zd. Cannot adjust fractions ...\n%s", Cv_sum, iCell,
                    locstr);
        }
        if (!assert_close_double(Cv_sum, 1., 0., AREA_SUM_ERROR_THRESH)) {
            sprint_location(locstr, &(local_domain.locations[iCell]));
            log_warn("Sum of veg tile area fractions !=  1.0 (%.16f) at grid "
                     "cell %zd. Adjusting fractions ...\n%s", Cv_sum, iCell,
                     locstr);
        }

        for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
            Cv_new[iVeg] = veg_con[iCell][iVeg].Cv;
            Cv_change[iVeg] = Cv_new[iVeg] - Cv_old[iVeg];
        }

        // Adjust
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            if (soil_con[iCell].AreaFract[iBand] > 0) {
                
                // Initialize energy
                calculate_derived_water_states(iCell, iBand);
                get_heat_capacities(iCell, iBand,
                                    snow_surf_capacity, snow_pack_capacity,
                                    node_capacity,
                                    Cv_old);
                get_energy_terms(iCell, iBand,
                                 snow_surf_capacity, snow_pack_capacity,
                                 node_capacity,
                                 orig_surf_tempEnergy, orig_pack_tempEnergy,
                                 orig_TEnergy);
                
                // Gather initial states
                before_water = calculate_total_water(iCell, iBand, Cv_old, Cv_change);
                before_carbon = calculate_total_carbon(iCell, iBand, Cv_old, Cv_change);
                before_energy = calculate_total_energy(iCell, 
                                    orig_surf_tempEnergy, orig_pack_tempEnergy,
                                    orig_TEnergy, Cv_old, Cv_change);

                // Water
                distribute_water_balance_terms(iCell, iBand,
                                               Cv_change, Cv_old, Cv_new);
                calculate_derived_water_states(iCell, iBand);

                // Carbon
                distribute_carbon_balance_terms(iCell, iBand,
                                                Cv_change, Cv_old, Cv_new);

                // Energy
                get_heat_capacities(iCell, iBand,
                                    snow_surf_capacity, snow_pack_capacity,
                                    node_capacity,
                                    Cv_new);
                distribute_energy_balance_terms(iCell, iBand,
                                                Cv_change, Cv_old, Cv_new,
                                                snow_surf_capacity,
                                                snow_pack_capacity,
                                                node_capacity,
                                                orig_surf_tempEnergy,
                                                orig_pack_tempEnergy,
                                                orig_TEnergy,
                                                new_surf_tempEnergy,
                                                new_pack_tempEnergy,
                                                new_TEnergy);
                get_energy_terms(iCell, iBand,
                                 snow_surf_capacity, snow_pack_capacity,
                                 node_capacity,
                                 new_surf_tempEnergy, new_pack_tempEnergy,
                                 new_TEnergy);
                calculate_derived_energy_states(iCell, iBand,
                                                snow_surf_capacity);
                
                // Gather final states
                after_water = calculate_total_water(iCell, iBand, Cv_new, Cv_change);
                after_carbon = calculate_total_carbon(iCell, iBand, Cv_new, Cv_change);
                after_energy = calculate_total_energy(iCell, 
                                    new_surf_tempEnergy, new_pack_tempEnergy,
                                    new_TEnergy, Cv_new, Cv_change);
                
                max_water = max(abs(before_water), abs(after_water));
                max_carbon = max(abs(before_carbon), abs(after_carbon));
                max_energy = max(abs(before_energy), abs(after_energy));
                max_water = max(max_water, DBL_EPSILON);
                max_carbon = max(max_carbon, DBL_EPSILON);
                max_energy = max(max_energy, DBL_EPSILON);
                if (abs(before_water - after_water) > 
                        max_water * veg_con_map[iCell].nv_active * 
                        MINCOVERAGECHANGE) {
                    log_err("\nWater balance error for cell %zu:\n"
                            "Initial water content [%.4f mm]\tFinal water content [%.4f mm]",
                            iCell, before_water, after_water);
                }
                if (abs(before_carbon - after_carbon) > 
                        max_carbon * veg_con_map[iCell].nv_active * 
                        MINCOVERAGECHANGE) {
                    log_err("\nCarbon balance error for cell %zu:\n"
                            "Initial carbon content [%.4f mm]\tFinal carbon content [%.4f mm]",
                            iCell, before_carbon, after_carbon);
                }
                if (abs(before_energy - after_energy) > 
                        max_energy * veg_con_map[iCell].nv_active * 
                        MINCOVERAGECHANGE) {
                    log_err("\nEnergy balance error for cell %zu:\n"
                            "Initial energy content [%.4f mm]\tFinal energy content [%.4f mm]",
                            iCell, before_energy, after_energy);
                }
                
                if(plugin_options.IRRIGATION && plugin_options.ROUTING) {
                    // Irrigation
                    distribute_irrigation_balance_terms(iCell, iBand,
                                                   Cv_change, Cv_old, Cv_new);
                }
            }
        }
    }

    free(Cv_change);
    free(Cv_old);
    free(Cv_new);

    free(snow_surf_capacity);
    free(snow_pack_capacity);
    free(orig_surf_tempEnergy);
    free(orig_pack_tempEnergy);
    free(new_surf_tempEnergy);
    free(new_pack_tempEnergy);
    for (iVeg = 0; iVeg < options.NVEGTYPES; iVeg++) {
        free(node_capacity[iVeg]);
        free(orig_TEnergy[iVeg]);
        free(new_TEnergy[iVeg]);
    }
    free(node_capacity);
    free(orig_TEnergy);
    free(new_TEnergy);
}
