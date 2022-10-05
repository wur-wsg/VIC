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
get_heat_capacities(size_t   iCell,
                    size_t   iBand,
                    double  *snow_surf_capacity,
                    double  *snow_pack_capacity,
                    double **node_capacity,
                    double  *Cv)
{
    extern parameters_struct   param;
    extern option_struct       options;
    extern veg_con_map_struct *veg_con_map;
    extern soil_con_struct    *soil_con;
    extern all_vars_struct    *all_vars;

    double                     snow_ice;
    double                     snow_surf_swq;
    double                     snow_pack_swq;

    snow_data_struct         **snow;
    energy_bal_struct        **energy;

    size_t                     iVeg;
    size_t                     iNode;

    snow = all_vars[iCell].snow;
    energy = all_vars[iCell].energy;

    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        snow_ice = snow[iVeg][iBand].swq - snow[iVeg][iBand].surf_water -
                   snow[iVeg][iBand].pack_water;
        if (snow_ice > param.SNOW_MAX_SURFACE_SWE) {
            snow_surf_swq = param.SNOW_MAX_SURFACE_SWE;
        }
        else {
            snow_surf_swq = snow_ice;
        }
        snow_pack_swq = snow_ice - snow_surf_swq;

        if (Cv[iVeg] > 0) {
            snow_surf_capacity[iVeg] = CONST_VCPICE_WQ * snow_surf_swq;
            snow_pack_capacity[iVeg] = CONST_VCPICE_WQ * snow_pack_swq;

            for (iNode = 0; iNode < options.Nnode; iNode++) {
                node_capacity[iVeg][iNode] =
                    energy[iVeg][iBand].Cs_node[iNode] *
                    soil_con[iCell].dz_node[iNode];
            }
        }
        else {
            snow_surf_capacity[iVeg] = 0.;
            snow_pack_capacity[iVeg] = 0.;

            for (iNode = 0; iNode < options.Nnode; iNode++) {
                node_capacity[iVeg][iNode] = 0.;
            }
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
    extern dmy_struct         *dmy;
    extern size_t              current;
    extern option_struct       options;
    extern force_data_struct  *force;
    extern veg_con_map_struct *veg_con_map;
    extern veg_con_struct    **veg_con;
    extern veg_lib_struct    **veg_lib;
    extern soil_con_struct    *soil_con;
    extern veg_con_struct    **veg_con;
    extern all_vars_struct    *all_vars;
    extern size_t              NR;

    double                     runoff;
    double                     moist[MAX_LAYERS];
    double                     resid_moist;
    double                     max_moist;
    layer_data_struct          layer[MAX_LAYERS];

    cell_data_struct         **cell;
    veg_var_struct           **veg;
    snow_data_struct         **snow;
    energy_bal_struct        **energy;

    size_t                     iVeg;
    size_t                     veg_class;
    size_t                     iLayer;

    cell = all_vars[iCell].cell;
    veg = all_vars[iCell].veg_var;
    snow = all_vars[iCell].snow;
    energy = all_vars[iCell].energy;

    // calculate
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        // initialize
        veg_class = veg_con[iCell][iVeg].veg_class;
        for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
            moist[iLayer] = cell[iVeg][iBand].layer[iLayer].moist;
            layer[iLayer] = cell[iVeg][iBand].layer[iLayer];
        }

        /* CELL_DATA_STRUCT */
        // Some snow variables are ignored since they depend on the age of the snow or are constant
        // cell[iVeg][iBand].layer[iLayer].phi; // Not used in VIC?

        // asat
        compute_runoff_and_asat(&soil_con[iCell], moist, 0,
                                &cell[iVeg][iBand].asat, &runoff);
        // zwt & zwt_lumped
        wrap_compute_zwt(&soil_con[iCell], &cell[iVeg][iBand]);
        // rootmoist & wetness
        cell[iVeg][iBand].rootmoist = 0;
        cell[iVeg][iBand].wetness = 0;
        for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
            max_moist = soil_con[iCell].porosity[iLayer] *
                        soil_con[iCell].depth[iLayer] * MM_PER_M;

            if (veg[iVeg][iBand].root[iLayer] > 0) {
                cell[iVeg][iBand].rootmoist +=
                    cell[iVeg][iBand].layer[iLayer].moist;
                cell[iVeg][iBand].wetness +=
                    (cell[iVeg][iBand].layer[iLayer].moist -
                     soil_con[iCell].Wpwp[iLayer]) /
                    (max_moist -
                     soil_con[iCell].Wpwp[iLayer]);
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
        for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
            cell[iVeg][iBand].layer[iLayer].Cs = layer[iLayer].Cs;
            cell[iVeg][iBand].layer[iLayer].kappa = layer[iLayer].kappa;
        }

        // eff_sat & phi
        for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
            max_moist = soil_con[iCell].porosity[iLayer] *
                        soil_con[iCell].depth[iLayer] * MM_PER_M;
            resid_moist = soil_con[iCell].resid_moist[iLayer] *
                          soil_con[iCell].depth[iLayer] * MM_PER_M;

            cell[iVeg][iBand].layer[iLayer].eff_sat =
                (cell[iVeg][iBand].layer[iLayer].moist - resid_moist) /
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
        snow[iVeg][iBand].snow = false;
        snow[iVeg][iBand].MELTING = false;
        if (snow[iVeg][iBand].swq > 0 ||
            (snow[iVeg][iBand].snow_canopy > 0. &&
             veg_lib[iCell][veg_class].overstory)) {
            snow[iVeg][iBand].snow = true;
        }
        if (snow[iVeg][iBand].swq > 0. && snow[iVeg][iBand].coldcontent >= 0 &&
            ((soil_con[iCell].lat >= 0 && (dmy[current].day_in_year > 60 &&     // ~ March 1
                                           dmy[current].day_in_year < 273)) ||     // ~ October 1
             (soil_con[iCell].lat < 0 && (dmy[current].day_in_year < 60 ||    // ~ March 1
                                          dmy[current].day_in_year > 273))    // ~ October 1
            )) {
            // Different from VIC melting flag calculation to remove date dependency
            snow[iVeg][iBand].MELTING = true;
        }
        if (snow[iVeg][iBand].swq > 0.) {
            if (snow[iVeg][iBand].last_snow == 0 ||
                snow[iVeg][iBand].density <= 0.) {
                snow[iVeg][iBand].density = new_snow_density(
                    force->air_temp[NR] + soil_con->Tfactor[iBand]);
            }
            snow[iVeg][iBand].depth = CONST_RHOFW * snow[iVeg][iBand].swq /
                                      snow[iVeg][iBand].density;
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
* @brief   Calculate derived energy states
******************************************/
void
calculate_derived_energy_states(size_t  iCell,
                                size_t  iBand,
                                double *snow_surf_capacity)
{
    extern option_struct       options;
    extern veg_con_map_struct *veg_con_map;
    extern veg_con_struct    **veg_con;
    extern soil_con_struct    *soil_con;
    extern veg_con_struct    **veg_con;
    extern all_vars_struct    *all_vars;

    double                     moist[MAX_LAYERS];
    double                     T[MAX_NODES];
    layer_data_struct          layer[MAX_LAYERS];

    cell_data_struct         **cell;
    snow_data_struct         **snow;
    energy_bal_struct        **energy;

    size_t                     iVeg;
    size_t                     iLayer;
    size_t                     iNode;

    cell = all_vars[iCell].cell;
    snow = all_vars[iCell].snow;
    energy = all_vars[iCell].energy;

    // calculate
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        // initialize
        for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
            moist[iLayer] = cell[iVeg][iBand].layer[iLayer].moist;
            layer[iLayer] = cell[iVeg][iBand].layer[iLayer];
        }
        for (iNode = 0; iNode < options.Nnode; iNode++) {
            T[iNode] = energy[iVeg][iBand].T[iNode];
        }

        /* LAYER_DATA_STRUCT & ENERGY_BALANCE_STRUCT*/
        // Some layer and energy variables are ignored since they depend on the simulation or are constant
        // energy[iVeg][iBand].unfrozen; // Not used in VIC?

        // T & ice & Nfrost & Nthaw & tdepth & fdepth & frozen
        calc_layer_average_thermal_props(&energy[iVeg][iBand], layer,
                                         &soil_con[iCell], options.Nnode, T);

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
        snow[iVeg][iBand].coldcontent = snow_surf_capacity[iVeg] *
                                        snow[iVeg][iBand].surf_temp;
    }
}

/******************************************
* @brief   Calculate total water
******************************************/
double
calculate_total_water(size_t  iCell,
                      size_t  iBand,
                      double *Cv,
                      double *Cv_change)
{
    extern option_struct       options;
    extern veg_con_map_struct *veg_con_map;
    extern all_vars_struct    *all_vars;

    double                     total;

    veg_var_struct           **veg_var;
    cell_data_struct         **cell;
    snow_data_struct         **snow;

    size_t                     iVeg;
    size_t                     iLayer;

    veg_var = all_vars[iCell].veg_var;
    cell = all_vars[iCell].cell;
    snow = all_vars[iCell].snow;

    total = 0.0;
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change[iVeg] < -MINCOVERAGECHANGE ||
            Cv_change[iVeg] > MINCOVERAGECHANGE) {
            total += veg_var[iVeg][iBand].Wdew * Cv[iVeg];
            total += snow[iVeg][iBand].pack_water * Cv[iVeg];
            total += snow[iVeg][iBand].surf_water * Cv[iVeg];
            total += snow[iVeg][iBand].swq * Cv[iVeg];
            total += snow[iVeg][iBand].snow_canopy * Cv[iVeg];
            for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                total += cell[iVeg][iBand].layer[iLayer].moist *
                         Cv[iVeg];
            }
        }
    }

    return(total);
}

/******************************************
* @brief   Calculate total carbon
******************************************/
double
calculate_total_carbon(size_t  iCell,
                       size_t  iBand,
                       double *Cv,
                       double *Cv_change)
{
    extern veg_con_map_struct *veg_con_map;
    extern all_vars_struct    *all_vars;

    double                     total;

    veg_var_struct           **veg_var;
    cell_data_struct         **cell;

    size_t                     iVeg;

    veg_var = all_vars[iCell].veg_var;
    cell = all_vars[iCell].cell;

    total = 0.0;
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change[iVeg] < -MINCOVERAGECHANGE ||
            Cv_change[iVeg] > MINCOVERAGECHANGE) {
            total += veg_var[iVeg][iBand].AnnualNPP * Cv[iVeg];
            total += veg_var[iVeg][iBand].AnnualNPPPrev * Cv[iVeg];
            total += cell[iVeg][iBand].CLitter * Cv[iVeg];
            total += cell[iVeg][iBand].CInter * Cv[iVeg];
            total += cell[iVeg][iBand].CSlow * Cv[iVeg];
        }
    }

    return(total);
}

/******************************************
* @brief   Calculate total energy
******************************************/
double
calculate_total_energy(size_t   iCell,
                       double  *surf_tempEnergy,
                       double  *pack_tempEnergy,
                       double **TEnergy,
                       double  *Cv,
                       double  *Cv_change)
{
    extern option_struct       options;
    extern veg_con_map_struct *veg_con_map;

    double                     total;

    size_t                     iVeg;
    size_t                     iNode;

    total = 0.0;
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        if (Cv_change[iVeg] < -MINCOVERAGECHANGE ||
            Cv_change[iVeg] > MINCOVERAGECHANGE) {
            total += surf_tempEnergy[iVeg] * Cv[iVeg];
            total += pack_tempEnergy[iVeg] * Cv[iVeg];
            for (iNode = 0; iNode < options.Nnode; iNode++) {
                total += TEnergy[iVeg][iNode] * Cv[iVeg];
            }
        }
    }

    return(total);
}

/******************************************
* @brief   Calculate total energy
******************************************/
double
calculate_total_irrigation(size_t  iCell,
                           size_t  iBand,
                           double *Cv,
                           double *Cv_change)
{
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern domain_struct              local_domain;
    extern irr_con_map_struct        *irr_con_map;
    extern irr_con_struct           **irr_con;
    extern irr_var_struct          ***irr_var;

    double                            total;

    irr_var_struct                  **irr;

    size_t                            iVeg;
    size_t                            iIrr;
    size_t                            iStep;
    size_t                            rout_steps_per_dt;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    irr = irr_var[iCell];

    total = 0.0;
    for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
        iVeg = irr_con[iCell][iIrr].veg_index;
        if (Cv_change[iVeg] < -MINCOVERAGECHANGE ||
            Cv_change[iVeg] > MINCOVERAGECHANGE) {
            total += irr[iIrr][iBand].leftover * Cv[iVeg];
        }
    }
    for (iStep = 0;
         iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
         iStep++) {
        total += rout_var[iCell].dt_discharge[iStep] * global_param.dt /
                 local_domain.locations[iCell].area * MM_PER_M;
    }

    return(total);
}

/******************************************
* @brief   Get the current energy contents
******************************************/
void
get_energy_terms(size_t   iCell,
                 size_t   iBand,
                 double  *snow_surf_capacity,
                 double  *snow_pack_capacity,
                 double **node_capacity,
                 double  *surf_tempEnergy,
                 double  *pack_tempEnergy,
                 double **TEnergy)
{
    extern option_struct       options;
    extern veg_con_map_struct *veg_con_map;
    extern all_vars_struct    *all_vars;

    snow_data_struct         **snow;
    energy_bal_struct        **energy;

    size_t                     iVeg;
    size_t                     iNode;

    snow = all_vars[iCell].snow;
    energy = all_vars[iCell].energy;

    /* ENERGY-BALANCE */

    // set
    for (iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++) {
        surf_tempEnergy[iVeg] = snow[iVeg][iBand].surf_temp *
                                snow_surf_capacity[iVeg];
        pack_tempEnergy[iVeg] = snow[iVeg][iBand].pack_temp *
                                snow_pack_capacity[iVeg];
        for (iNode = 0; iNode < options.Nnode; iNode++) {
            TEnergy[iVeg][iNode] = energy[iVeg][iBand].T[iNode] *
                                   node_capacity[iVeg][iNode];
        }
    }
}
