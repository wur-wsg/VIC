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
* @brief   Get the heat capacities to calculate energy transfer
******************************************/
void
get_heat_capacities(size_t iCell, 
        size_t iBand, 
        double *snow_surf_capacity, 
        double *snow_pack_capacity, 
        double **node_capacity)
{
    extern parameters_struct param;
    extern option_struct options;
    extern veg_con_map_struct *veg_con_map;
    extern soil_con_struct *soil_con;
    extern all_vars_struct *all_vars;
    
    double snow_ice;
    double snow_surf_swq;
    double snow_pack_swq;
    
    snow_data_struct **snow;
    energy_bal_struct **energy;
    
    size_t iVeg;
    size_t iNode;
    
    snow = all_vars[iCell].snow;
    energy = all_vars[iCell].energy;
    
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        snow_ice = snow[iVeg][iBand].swq - snow[iVeg][iBand].surf_water - snow[iVeg][iBand].pack_water;
        if (snow_ice > param.SNOW_MAX_SURFACE_SWE) {
            snow_surf_swq = param.SNOW_MAX_SURFACE_SWE;
        }
        else {
            snow_surf_swq = snow_ice;
        }
        snow_pack_swq = snow_ice - snow_surf_swq;

        snow_surf_capacity[iVeg] = CONST_VCPICE_WQ * snow_pack_swq;
        snow_pack_capacity[iVeg] = CONST_VCPICE_WQ * snow_surf_swq;
        
        for(iNode = 0; iNode < options.Nnode; iNode++){
            node_capacity[iVeg][iNode] = energy[iVeg][iBand].Cs_node[iNode] * soil_con[iCell].dz_node[iNode];
        }
    }
}

/******************************************
* @brief   Get the current energy contents
******************************************/
void
get_energy_terms(size_t iCell,
        size_t iBand,
        double *snow_surf_capacity, 
        double *snow_pack_capacity, 
        double **node_capacity,
        double *orig_surf_tempEnergy, 
        double *orig_pack_tempEnergy, 
        double **orig_TEnergy)
{
    extern option_struct options;
    extern veg_con_map_struct *veg_con_map;
    extern all_vars_struct *all_vars;
    
    snow_data_struct **snow;
    energy_bal_struct **energy;
    
    size_t iVeg;
    size_t iNode;
    
    snow = all_vars[iCell].snow;
    energy = all_vars[iCell].energy;
    
    /* ENERGY-BALANCE */
    
    // set
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        orig_surf_tempEnergy[iVeg] = snow[iVeg][iBand].surf_temp * snow_surf_capacity[iVeg];
        orig_pack_tempEnergy[iVeg] = snow[iVeg][iBand].pack_temp * snow_pack_capacity[iVeg];
        for(iNode = 0; iNode < options.Nnode; iNode++){
            orig_TEnergy[iVeg][iNode] = energy[iVeg][iBand].T[iNode] * node_capacity[iVeg][iNode];
        }
    }
}

/******************************************
* @brief   Distribute water balance variables
******************************************/
void
distribute_water_balance_terms(size_t iCell, 
        size_t iBand, 
        double *Cv_change, 
        double *Cv_old, 
        double *Cv_new)
{
    extern option_struct options;
    extern veg_con_map_struct *veg_con_map;
    extern all_vars_struct *all_vars;
    
    double Cv_avail;
    double red_frac;
    double add_frac;
    double before_moist;
    double after_moist;
    
    // water-balance
    // veg_var
    double Wdew;
    // snow
    double surf_water;
    double pack_water;
    double snow_canopy;
    double swq;
    // cell
    double moist[MAX_LAYERS];
    double ice[MAX_LAYERS][MAX_FROST_AREAS];
    
    veg_var_struct **veg_var;
    cell_data_struct **cell;
    snow_data_struct **snow;
    
    size_t iVeg;
    size_t iLayer;
    size_t iFrost;
    
    veg_var = all_vars[iCell].veg_var;
    cell = all_vars[iCell].cell;
    snow = all_vars[iCell].snow;
                
    // Get available area to redistribute
    Cv_avail = 0.0;
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        if (Cv_change[iVeg] > 0) {
            Cv_avail += Cv_change[iVeg];
        }
    }
    
    /* WATER-BALANCE */
    
    // initialize
    Wdew = 0.0;
    pack_water = 0.0;
    surf_water = 0.0;
    swq = 0.0;
    snow_canopy = 0.0;
    for(iLayer = 0; iLayer < MAX_LAYERS; iLayer++){
        moist[iLayer] = 0.0;
        for(iFrost = 0; iFrost < MAX_FROST_AREAS; iFrost++){
            ice[iLayer][iFrost] = 0.0;
        }
    }
    
    // get
    before_moist = 0.0;
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        if (Cv_change[iVeg] < 0) {
            Wdew += veg_var[iVeg][iBand].Wdew * -Cv_change[iVeg];
            pack_water += snow[iVeg][iBand].pack_water * -Cv_change[iVeg];
            surf_water += snow[iVeg][iBand].surf_water * -Cv_change[iVeg];
            swq += snow[iVeg][iBand].swq * -Cv_change[iVeg];
            snow_canopy += snow[iVeg][iBand].snow_canopy * -Cv_change[iVeg];
            for(iLayer = 0; iLayer < options.Nlayer; iLayer++){
                moist[iLayer] += cell[iVeg][iBand].layer[iLayer].moist * -Cv_change[iVeg];
                for(iFrost = 0; iFrost < options.Nfrost; iFrost++){
                    ice[iLayer][iFrost] += cell[iVeg][iBand].layer[iLayer].ice[iFrost] * -Cv_change[iVeg];
                }
            }
        }
        
        before_moist += veg_var[iVeg][iBand].Wdew * Cv_old[iVeg];
        before_moist += snow[iVeg][iBand].pack_water * Cv_old[iVeg];
        before_moist += snow[iVeg][iBand].surf_water * Cv_old[iVeg];
        before_moist += snow[iVeg][iBand].swq * Cv_old[iVeg];
        before_moist += snow[iVeg][iBand].snow_canopy * Cv_old[iVeg];
        for(iLayer = 0; iLayer < options.Nlayer; iLayer++){
            before_moist += cell[iVeg][iBand].layer[iLayer].moist * Cv_old[iVeg];
        }
    }
    
    // set
    after_moist = 0.0;
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        if (Cv_change[iVeg] > 0) {
            red_frac = Cv_old[iVeg] / Cv_new[iVeg];
            add_frac = (Cv_change[iVeg] / Cv_avail) / Cv_new[iVeg];

            veg_var[iVeg][iBand].Wdew = veg_var[iVeg][iBand].Wdew * red_frac + Wdew * add_frac;
            snow[iVeg][iBand].pack_water = snow[iVeg][iBand].pack_water * red_frac + pack_water * add_frac;
            snow[iVeg][iBand].surf_water = snow[iVeg][iBand].surf_water * red_frac + surf_water * add_frac;
            snow[iVeg][iBand].swq = snow[iVeg][iBand].swq * red_frac + swq * add_frac;
            snow[iVeg][iBand].snow_canopy = snow[iVeg][iBand].snow_canopy * red_frac + snow_canopy * add_frac;
            for(iLayer = 0; iLayer < options.Nlayer; iLayer++){
                cell[iVeg][iBand].layer[iLayer].moist = cell[iVeg][iBand].layer[iLayer].moist * red_frac + moist[iLayer] * add_frac;
                for(iFrost = 0; iFrost < options.Nfrost; iFrost++){
                    cell[iVeg][iBand].layer[iLayer].ice[iFrost] = cell[iVeg][iBand].layer[iLayer].ice[iFrost] * red_frac + ice[iLayer][iFrost] * add_frac;
                }
            }
        }
        
        after_moist += veg_var[iVeg][iBand].Wdew * Cv_new[iVeg];
        after_moist += snow[iVeg][iBand].pack_water * Cv_new[iVeg];
        after_moist += snow[iVeg][iBand].surf_water * Cv_new[iVeg];
        after_moist += snow[iVeg][iBand].swq * Cv_new[iVeg];
        after_moist += snow[iVeg][iBand].snow_canopy * Cv_new[iVeg];
        for(iLayer = 0; iLayer < options.Nlayer; iLayer++){
            after_moist += cell[iVeg][iBand].layer[iLayer].moist * Cv_new[iVeg];
        }
    }
    
    // Check water balance
    if(abs(before_moist - after_moist) > DBL_EPSILON){
        log_err("\nWater balance error for cell %zu:\n"
                "Initial water content [%.4f mm]\tFinal water content [%.4f mm]",
                iCell,
                before_moist,
                after_moist);
    }
}

/******************************************
* @brief   Distribute carbon balance variables
******************************************/
void
distribute_carbon_balance_terms(size_t iCell, 
        size_t iBand, 
        double *Cv_change, 
        double *Cv_old, 
        double *Cv_new)
{
    extern veg_con_map_struct *veg_con_map;
    extern all_vars_struct *all_vars;
    
    double Cv_avail;
    double red_frac;
    double add_frac;
    
    // carbon-balance
    // veg_var
    double AnnualNPP;
    double AnnualNPPPrev;
    // cell
    double CLitter;
    double CInter;
    double CSlow;
    
    veg_var_struct **veg_var;
    cell_data_struct **cell;
    
    size_t iVeg;
    
    veg_var = all_vars[iCell].veg_var;
    cell = all_vars[iCell].cell;
                
    // Get available area to redistribute
    Cv_avail = 0.0;
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        if (Cv_change[iVeg] > 0) {
            Cv_avail += Cv_change[iVeg];
        }
    }
    
    /* CARBON-BALANCE */
    
    // initialize
    AnnualNPP = 0.0;
    AnnualNPPPrev = 0.0;
    CLitter = 0.0;
    CInter = 0.0;
    CSlow = 0.0;
    
    // get
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        if (Cv_change[iVeg] < 0) {
            AnnualNPP += veg_var[iVeg][iBand].AnnualNPP * -Cv_change[iVeg];
            AnnualNPPPrev += veg_var[iVeg][iBand].AnnualNPPPrev * -Cv_change[iVeg];
            CLitter += cell[iVeg][iBand].CLitter * -Cv_change[iVeg];
            CInter += cell[iVeg][iBand].CInter * -Cv_change[iVeg];
            CSlow += cell[iVeg][iBand].CSlow * -Cv_change[iVeg];
        }
    }
    
    // set
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        if (Cv_change[iVeg] > 0) {
            red_frac = Cv_old[iVeg] / Cv_new[iVeg];
            add_frac = (Cv_change[iVeg] / Cv_avail) / Cv_new[iVeg];

            veg_var[iVeg][iBand].AnnualNPP = veg_var[iVeg][iBand].AnnualNPP * red_frac + AnnualNPP * add_frac;
            veg_var[iVeg][iBand].AnnualNPPPrev = veg_var[iVeg][iBand].AnnualNPPPrev * red_frac + AnnualNPPPrev * add_frac;
            cell[iVeg][iBand].CLitter = cell[iVeg][iBand].CLitter * red_frac + CLitter * add_frac;
            cell[iVeg][iBand].CInter = cell[iVeg][iBand].CInter * red_frac + CInter * add_frac;
            cell[iVeg][iBand].CSlow = cell[iVeg][iBand].CSlow * red_frac + CSlow * add_frac;
        }
    }
}

/******************************************
* @brief   Calculate derived water states
******************************************/
void
calculate_derived_water_states(size_t iCell, 
        size_t iBand)
{
    extern dmy_struct *dmy;
    extern size_t current;
    extern option_struct options;
    extern veg_con_map_struct *veg_con_map;
    extern veg_con_struct **veg_con;
    extern veg_lib_struct *veg_lib;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern all_vars_struct *all_vars;
    
    double runoff;
    double moist[MAX_LAYERS];
    double resid_moist;
    double max_moist;
    layer_data_struct layer[MAX_LAYERS];
    
    cell_data_struct **cell;
    snow_data_struct **snow;
    energy_bal_struct **energy;
    
    size_t iVeg;
    size_t veg_class;
    size_t iLayer;
    
    cell = all_vars[iCell].cell;
    snow = all_vars[iCell].snow;
    energy = all_vars[iCell].energy;
    
    // calculate
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        
        // initialize
        veg_class = veg_con[iCell][iVeg].veg_class;
        for(iLayer = 0; iLayer < options.Nlayer; iLayer++) {
            moist[iLayer] = cell[iVeg][iBand].layer[iLayer].moist;
            layer[iLayer] = cell[iVeg][iBand].layer[iLayer];
        }
        
        /* CELL_DATA_STRUCT */
        // Some snow variables are ignored since they depend on the age of the snow or are constant
        // cell[iVeg][iBand].layer[iLayer].phi; // Not used in VIC?
        
        // asat
        compute_runoff_and_asat(&soil_con[iCell], moist, 0, &cell[iVeg][iBand].asat, &runoff);
        // zwt & zwt_lumped
        wrap_compute_zwt(&soil_con[iCell], &cell[iVeg][iBand]);
        // rootmoist & wetness
        cell[iVeg][iBand].rootmoist = 0;
        cell[iVeg][iBand].wetness = 0;
        for(iLayer = 0; iLayer < options.Nlayer; iLayer++){
            max_moist = soil_con[iCell].porosity[iLayer] * soil_con[iCell].depth[iLayer] * MM_PER_M;
            
            if (veg_con[iCell][iVeg].root[iLayer] > 0) {
                cell[iVeg][iBand].rootmoist += cell[iVeg][iBand].layer[iLayer].moist;
                cell[iVeg][iBand].wetness += (cell[iVeg][iBand].layer[iLayer].moist - soil_con[iCell].Wpwp[iLayer]) /
                                             (max_moist - soil_con[iCell].Wpwp[iLayer]);
            }
            cell[iVeg][iBand].wetness /= options.Nlayer;
        }
        
        /* LAYER_DATA_STRUCT */
        // Cs & kappa
        compute_soil_layer_thermal_properties(layer, soil_con[iCell].depth,
                                              soil_con[iCell].bulk_dens_min,
                                              soil_con[iCell].soil_dens_min,
                                              soil_con[iCell].quartz,
                                              soil_con[iCell].bulk_density,
                                              soil_con[iCell].soil_density,
                                              soil_con[iCell].organic,
                                              soil_con[iCell].frost_fract,
                                              options.Nlayer);
        for(iLayer = 0; iLayer < options.Nlayer; iLayer++){
            cell[iVeg][iBand].layer[iLayer].Cs = layer[iLayer].Cs;
            cell[iVeg][iBand].layer[iLayer].kappa = layer[iLayer].kappa;
        }
        
        // eff_sat & phi
        for(iLayer = 0; iLayer < options.Nlayer; iLayer++){
            max_moist = soil_con[iCell].porosity[iLayer] * soil_con[iCell].depth[iLayer] * MM_PER_M;
            resid_moist = soil_con[iCell].resid_moist[iLayer] * soil_con[iCell].depth[iLayer] * MM_PER_M;
            
            cell[iVeg][iBand].layer[iLayer].eff_sat = (cell[iVeg][iBand].layer[iLayer].moist - resid_moist) /
                                                      (max_moist - resid_moist);
        }
        
        /* SNOW_DATA_STRUCT */
        // Some snow variables are ignored since they depend on the age of the snow or are constant
        // snow[iVeg][iBand].albedo;
        // snow[iVeg][iBand].canopy_albedo;
        // snow[iVeg][iBand].density;
        // snow[iVeg][iBand].snow_distrib_slope;
        // snow[iVeg][iBand].coverage;
        // snow[iVeg][iBand].max_snow_depth;
        // snow[iVeg][iBand].snow_distrib_slope;
        // snow[iVeg][iBand].tmp_int_storage;
        
        // depth & snow & MELTING
        snow[iVeg][iBand].depth = CONST_RHOFW * snow[iVeg][iBand].swq / snow[iVeg][iBand].density;
        snow[iVeg][iBand].snow = false;
        snow[iVeg][iBand].MELTING = false;
        if (snow[iVeg][iBand].swq > 0 || (snow[iVeg][iBand].snow_canopy > 0. && veg_lib[veg_class].overstory)) {
            snow[iVeg][iBand].snow = true;
        }
        if (snow[iVeg][iBand].swq > 0. && snow[iVeg][iBand].coldcontent >= 0 && 
                ((soil_con[iCell].lat >= 0 && (dmy[current].day_in_year > 60 && // ~ March 1
                                               dmy[current].day_in_year < 273)) || // ~ October 1
                (soil_con[iCell].lat < 0 && (dmy[current].day_in_year < 60 || // ~ March 1
                                             dmy[current].day_in_year > 273)) // ~ October 1
                )) {
            // Different from VIC melting flag calculation to remove date dependency
            snow[iVeg][iBand].MELTING = true;
        }
        
        /* ENERGY_BALANCE_STRUCT */
        // Some energy variables are ignored since they depend on the simulation are constant
        // energy[iVeg][iBand].T_fbflag;
        // energy[iVeg][iBand].T_fbcount;
        // energy[iVeg][iBand].T1_index;
        // energy[iVeg][iBand].Tcanopy;
        // energy[iVeg][iBand].Tcanopy_fbflag;
        // energy[iVeg][iBand].Tcanopy_fbcount;
        // energy[iVeg][iBand].Tfoliage;
        // energy[iVeg][iBand].Tfoliage_fbflag;
        // energy[iVeg][iBand].Tfoliage_fbcount;
        // energy[iVeg][iBand].Tsurf_fbflag;
        // energy[iVeg][iBand].Tsurf_fbcount;
        // energy[iVeg][iBand].unfrozen; // Not used in VIC?
        // energy[iVeg][iBand].AlbedoLake;
        // energy[iVeg][iBand].AlbedoOver;
        // energy[iVeg][iBand].AlbedoUnder;
        
        // moist & ice & Cs_node & kappa_node
        distribute_node_moisture_properties(energy[iVeg][iBand].moist,
                                            energy[iVeg][iBand].ice,
                                            energy[iVeg][iBand].kappa_node,
                                            energy[iVeg][iBand].Cs_node,
                                            soil_con[iCell].Zsum_node,
                                            energy[iVeg][iBand].T,
                                            soil_con[iCell].max_moist_node,
                                            soil_con[iCell].expt_node,
                                            soil_con[iCell].bubble_node,
                                            moist, soil_con[iCell].depth,
                                            soil_con[iCell].soil_dens_min,
                                            soil_con[iCell].bulk_dens_min,
                                            soil_con[iCell].quartz,
                                            soil_con[iCell].soil_density,
                                            soil_con[iCell].bulk_density,
                                            soil_con[iCell].organic,
                                            options.Nnode,
                                            options.Nlayer,
                                            soil_con[iCell].FS_ACTIVE);
        
        // Cs & kappa
        energy[iVeg][iBand].Cs[0] = cell[iVeg][iBand].layer[0].Cs;
        energy[iVeg][iBand].Cs[2] = cell[iVeg][iBand].layer[1].Cs;
        energy[iVeg][iBand].kappa[0] = cell[iVeg][iBand].layer[0].kappa;
        energy[iVeg][iBand].kappa[2] = cell[iVeg][iBand].layer[1].kappa;
    }
}

/******************************************
* @brief   Distribute energy balance variables
******************************************/
void
distribute_energy_balance_terms(size_t iCell, 
        size_t iBand, 
        double *Cv_change, 
        double *Cv_old, 
        double *Cv_new,
        double *snow_surf_capacity, 
        double *snow_pack_capacity, 
        double **node_capacity,
        double *orig_surf_tempEnergy, 
        double *orig_pack_tempEnergy, 
        double **orig_TEnergy)
{
    extern option_struct options;
    extern veg_con_map_struct *veg_con_map;
    extern all_vars_struct *all_vars;
    
    double Cv_avail;
    double red_frac;
    double add_frac;
    double before_energy;
    double after_energy;
    
    // energy-balance
    // snow
    double surf_tempEnergy;
    double pack_tempEnergy;
    // energy
    double TEnergy[MAX_NODES];
    
    snow_data_struct **snow;
    energy_bal_struct **energy;
    
    size_t iVeg;
    size_t iNode;
    
    snow = all_vars[iCell].snow;
    energy = all_vars[iCell].energy;
    
    // Get available area to redistribute
    Cv_avail = 0.0;
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        if (Cv_change[iVeg] > 0) {
            Cv_avail += Cv_change[iVeg];
        }
    }
    
    /* ENERGY-BALANCE */
    
    // initialize
    pack_tempEnergy = 0.0;
    surf_tempEnergy = 0.0;
    for(iNode = 0; iNode < options.Nnode; iNode++){
        TEnergy[iNode] = 0.0;
    }
    
    // get
    before_energy = 0.0;
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        if (Cv_change[iVeg] < 0) {
            surf_tempEnergy += orig_surf_tempEnergy[iVeg] * -Cv_change[iVeg];
            pack_tempEnergy += orig_pack_tempEnergy[iVeg] * -Cv_change[iVeg];
            for(iNode = 0; iNode < options.Nnode; iNode++){
                TEnergy[iNode] += orig_TEnergy[iVeg][iNode] * -Cv_change[iVeg];
            }
        }
        
        before_energy += orig_surf_tempEnergy[iVeg] * Cv_old[iVeg];
        before_energy += orig_pack_tempEnergy[iVeg] * Cv_old[iVeg];
        for(iNode = 0; iNode < options.Nnode; iNode++){
            before_energy += orig_TEnergy[iVeg][iNode] * Cv_old[iVeg];
        }
    }
    
    // set
    after_energy = 0.0;
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        if (Cv_change[iVeg] > 0) {
            red_frac = Cv_old[iVeg] / Cv_new[iVeg];
            add_frac = (Cv_change[iVeg] / Cv_avail) / Cv_new[iVeg];
            
            if (snow_surf_capacity[iVeg] > 0){
                snow[iVeg][iBand].surf_temp = (orig_surf_tempEnergy[iVeg] * red_frac + surf_tempEnergy * add_frac) / snow_surf_capacity[iVeg];
            }
            if (snow_pack_capacity[iVeg] > 0){
                snow[iVeg][iBand].pack_temp = (orig_pack_tempEnergy[iVeg] * red_frac + pack_tempEnergy * add_frac) / snow_pack_capacity[iVeg];
            }
            for(iNode = 0; iNode < options.Nnode; iNode++){
                if(node_capacity[iVeg][iNode] > 0){
                    energy[iVeg][iBand].T[iNode] = (orig_TEnergy[iVeg][iNode] * red_frac + TEnergy[iNode] * add_frac) / node_capacity[iVeg][iNode];
                }
            }
        }
        
        after_energy += snow[iVeg][iBand].surf_temp * snow_surf_capacity[iVeg] * Cv_new[iVeg];
        after_energy += snow[iVeg][iBand].pack_temp * snow_pack_capacity[iVeg] * Cv_new[iVeg];
        for(iNode = 0; iNode < options.Nnode; iNode++){
            after_energy += energy[iVeg][iBand].T[iNode] * node_capacity[iVeg][iNode] * Cv_new[iVeg];
        }
    }
    
    // Check energy balance
    if(abs(before_energy - after_energy) > DBL_EPSILON){
        log_err("\nEnergy balance error for cell %zu:\n"
                "Initial energy content [%.4f J]\tFinal energy content [%.4f J]",
                iCell,
                before_energy,
                after_energy);
    }
}

/******************************************
* @brief   Calculate derived energy states
******************************************/
void
calculate_derived_energy_states(size_t iCell, 
        size_t iBand,
        double *snow_surf_capacity)
{
    extern option_struct options;
    extern veg_con_map_struct *veg_con_map;
    extern veg_con_struct **veg_con;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern all_vars_struct *all_vars;
    
    double moist[MAX_LAYERS];
    double T[MAX_NODES];
    layer_data_struct layer[MAX_LAYERS];
    
    cell_data_struct **cell;
    snow_data_struct **snow;
    energy_bal_struct **energy;
    
    size_t iVeg;
    size_t iLayer;
    size_t iNode;
    
    cell = all_vars[iCell].cell;
    snow = all_vars[iCell].snow;
    energy = all_vars[iCell].energy;
    
    // calculate
    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
        
        // initialize
        for(iLayer = 0; iLayer < options.Nlayer; iLayer++) {
            moist[iLayer] = cell[iVeg][iBand].layer[iLayer].moist;
            layer[iLayer] = cell[iVeg][iBand].layer[iLayer];
        }
        for(iNode = 0; iNode < options.Nnode; iNode++) {
            T[iNode] = energy[iVeg][iBand].T[iNode];
        }
        
        /* LAYER_DATA_STRUCT & ENERGY_BALANCE_STRUCT*/
        // Some layer and energy variables are ignored since they depend on the simulation or are constant
        // energy[iVeg][iBand].unfrozen; // Not used in VIC?
        
        // T & ice & Nfrost & Nthaw & tdepth & fdepth & frozen
        calc_layer_average_thermal_props(&energy[iVeg][iBand], layer, &soil_con[iCell], options.Nnode, T);
        
        // moist & ice & Cs_node & kappa_node (only used to recompute node ice content)
        distribute_node_moisture_properties(energy[iVeg][iBand].moist,
                                            energy[iVeg][iBand].ice,
                                            energy[iVeg][iBand].kappa_node,
                                            energy[iVeg][iBand].Cs_node,
                                            soil_con[iCell].Zsum_node,
                                            energy[iVeg][iBand].T,
                                            soil_con[iCell].max_moist_node,
                                            soil_con[iCell].expt_node,
                                            soil_con[iCell].bubble_node,
                                            moist, soil_con[iCell].depth,
                                            soil_con[iCell].soil_dens_min,
                                            soil_con[iCell].bulk_dens_min,
                                            soil_con[iCell].quartz,
                                            soil_con[iCell].soil_density,
                                            soil_con[iCell].bulk_density,
                                            soil_con[iCell].organic,
                                            options.Nnode,
                                            options.Nlayer,
                                            soil_con[iCell].FS_ACTIVE);
        
        /* SNOW_DATA_STRUCT */
        snow[iVeg][iBand].coldcontent = snow_surf_capacity[iVeg] * snow[iVeg][iBand].surf_temp;
    }
}

/******************************************
* @brief   Apply land-use vegetation fractions
******************************************/
void
lu_apply(void)
{
    extern domain_struct              local_domain;
    extern option_struct options;
    extern veg_con_map_struct *veg_con_map;
    extern veg_con_struct **veg_con;
    extern lu_force_struct           *lu_force;
    extern soil_con_struct *soil_con;
    
    char locstr[MAXSTRING];
    double Cv_sum;
    double *Cv_old;
    double *Cv_new;
    double *Cv_change;
    
    double *snow_surf_capacity;
    double *snow_pack_capacity;
    double **node_capacity;
    double *orig_surf_tempEnergy;
    double *orig_pack_tempEnergy;
    double **orig_TEnergy;
    
    size_t iCell;
    size_t iVeg;
    size_t iBand;
    size_t veg_class;
        
    Cv_old = malloc(options.NVEGTYPES * sizeof(*Cv_old));
    check_alloc_status(Cv_old, "Memory allocation error");
    Cv_new = malloc(options.NVEGTYPES * sizeof(*Cv_new));
    check_alloc_status(Cv_new, "Memory allocation error");
    Cv_change = malloc(options.NVEGTYPES * sizeof(*Cv_change));
    check_alloc_status(Cv_change, "Memory allocation error");
    
    snow_surf_capacity = malloc(options.NVEGTYPES * sizeof(*snow_surf_capacity));
    check_alloc_status(snow_surf_capacity, "Memory allocation error");
    snow_pack_capacity = malloc(options.NVEGTYPES * sizeof(*snow_pack_capacity));
    check_alloc_status(snow_pack_capacity, "Memory allocation error");
    node_capacity = malloc(options.NVEGTYPES * sizeof(*node_capacity));
    check_alloc_status(node_capacity, "Memory allocation error");
    orig_surf_tempEnergy = malloc(options.NVEGTYPES * sizeof(*orig_surf_tempEnergy));
    check_alloc_status(orig_surf_tempEnergy, "Memory allocation error");
    orig_pack_tempEnergy = malloc(options.NVEGTYPES * sizeof(*orig_pack_tempEnergy));
    check_alloc_status(orig_pack_tempEnergy, "Memory allocation error");
    orig_TEnergy = malloc(options.NVEGTYPES * sizeof(*orig_TEnergy));
    check_alloc_status(orig_TEnergy, "Memory allocation error");
    for(iVeg = 0; iVeg < options.NVEGTYPES; iVeg++){
        node_capacity[iVeg] = malloc(options.Nnode * sizeof(*node_capacity[iVeg]));
        check_alloc_status(node_capacity[iVeg], "Memory allocation error");
        orig_TEnergy[iVeg] = malloc(options.Nnode * sizeof(*orig_TEnergy[iVeg]));
        check_alloc_status(orig_TEnergy[iVeg], "Memory allocation error");
    }
    
    for (iCell = 0; iCell < local_domain.ncells_active; iCell++) {
        // Initialize
        Cv_sum = 0.0;
        
        // Calculate
        for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
            Cv_sum += lu_force[iCell].Cv[iVeg];
            Cv_old[iVeg] = veg_con[iCell][iVeg].Cv;
        }
        
        // Set
        for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
            veg_class = lu_force[iCell].veg_class[iVeg];

            veg_con_map[iCell].Cv[veg_class] = lu_force[iCell].Cv[iVeg] / Cv_sum;
            veg_con[iCell][iVeg].Cv = lu_force[iCell].Cv[iVeg] / Cv_sum;
        }
        
        // Check
        if(Cv_sum <= 0.0){
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
        
        for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
            Cv_new[iVeg] = veg_con[iCell][iVeg].Cv;
            Cv_change[iVeg] = Cv_new[iVeg] - Cv_old[iVeg];
        }
        
        // Adjust
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            if(soil_con[iCell].AreaFract[iBand] > 0){
                
                // Initialize energy
                calculate_derived_water_states(iCell, iBand);
                get_heat_capacities(iCell, iBand, snow_surf_capacity, snow_pack_capacity, node_capacity);
                get_energy_terms(iCell, iBand, snow_surf_capacity, snow_pack_capacity, node_capacity, orig_surf_tempEnergy, orig_pack_tempEnergy, orig_TEnergy);
                
                // Water
                distribute_water_balance_terms(iCell, iBand, Cv_change, Cv_old, Cv_new);
                calculate_derived_water_states(iCell, iBand);
                
                // Carbon
                distribute_carbon_balance_terms(iCell, iBand, Cv_change, Cv_old, Cv_new);
                
                // Energy
                get_heat_capacities(iCell, iBand, snow_surf_capacity, snow_pack_capacity, node_capacity);
                distribute_energy_balance_terms(iCell, iBand, Cv_change, Cv_old, Cv_new, snow_surf_capacity, snow_pack_capacity, node_capacity, orig_surf_tempEnergy, orig_pack_tempEnergy, orig_TEnergy);
                calculate_derived_energy_states(iCell, iBand, snow_surf_capacity);
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
    for(iVeg = 0; iVeg < options.NVEGTYPES; iVeg++){
        free(node_capacity[iVeg]);
        free(orig_TEnergy[iVeg]);
    }
    free(node_capacity);
    free(orig_TEnergy);
}
