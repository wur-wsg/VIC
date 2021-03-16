/******************************************************************************
 * @section DESCRIPTION
 *
 * Force environmental flow requirements
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
* @brief   Calculate derived forcing variables
******************************************/
void
efr_derived_forcing(void)
{
    extern domain_struct       local_domain;
    extern global_param_struct global_param;
    extern option_struct       options;
    extern efr_force_struct   *efr_force;
    extern all_vars_struct    *all_vars;
    extern veg_con_map_struct *veg_con_map;
    extern soil_con_struct    *soil_con;
    extern veg_con_struct    **veg_con;

    double                     frac;
    double                     calculated_baseflow;
    double                     moist;
    double                     liq;
    double                     ice;
    double                     rel_liq;
    double                     res_moist;
    double                     max_moist;
    double                     dsmax;
    double                     bflow;
    double                     veg_frac;
    double                     area_frac;

    size_t                     i;
    size_t                     j;
    size_t                     k;
    size_t                     l;
    size_t                     f;

    l = options.Nlayer - 1;
    for (i = 0; i < local_domain.ncells_active; i++) {
        res_moist = soil_con[i].resid_moist[l] * soil_con[i].depth[l] *
                    MM_PER_M;
        max_moist = soil_con[i].max_moist[l];
        dsmax = soil_con[i].Dsmax / global_param.model_steps_per_day;

        for (frac = 1.0; frac >= 0.0; frac -= 0.01) {
            calculated_baseflow = 0.0;

            for (j = 0; j < veg_con_map[i].nv_active; j++) {
                veg_frac = veg_con[i][j].Cv;
                for (k = 0; k < options.SNOW_BAND; k++) {
                    area_frac = soil_con[i].AreaFract[k];

                    // Get moisture
                    moist = all_vars[i].cell[j][k].layer[l].moist;
                    ice = 0.0;
                    for (f = 0; f < options.Nfrost; f++) {
                        ice += all_vars[i].cell[j][k].layer[l].ice[f] *
                               soil_con[i].frost_fract[f];
                    }
                    liq = moist - ice;

                    // Based on VIC baseflow formulation
                    rel_liq = (liq * frac - res_moist) /
                              (max_moist - res_moist);
                    bflow = rel_liq * dsmax * soil_con[i].Ds / soil_con[i].Ws;
                    if (rel_liq > soil_con->Ws) {
                        bflow += dsmax * (1 - soil_con->Ds / soil_con->Ws) *
                                 pow((rel_liq - soil_con->Ws) /
                                     (1 - soil_con->Ws), soil_con->c);
                    }

                    if (bflow < 0) {
                        bflow = 0.0;
                    }

                    calculated_baseflow += bflow * veg_frac * area_frac;
                }
            }

            if (calculated_baseflow < efr_force[i].baseflow) {
                frac = min(1, frac + 0.01);
                break;
            }
        }

        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            for (k = 0; k < options.SNOW_BAND; k++) {
                // Get moisture
                moist = all_vars[i].cell[j][k].layer[l].moist;
                ice = 0.0;
                for (f = 0; f < options.Nfrost; f++) {
                    ice += all_vars[i].cell[j][k].layer[l].ice[f] *
                           soil_con[i].frost_fract[f];
                }
                liq = moist - ice;

                efr_force[i].moist[j][k] = liq * frac + ice;
            }
        }
    }
}

/******************************************
* @brief   Force environmental flow requirements
******************************************/
void
efr_forcing(void)
{
    extern domain_struct              local_domain;
    extern plugin_global_param_struct plugin_global_param;
    extern domain_struct              global_domain;
    extern plugin_filenames_struct    plugin_filenames;
    extern efr_force_struct          *efr_force;

    double                           *dvar;

    size_t                            d3count[3];
    size_t                            d3start[3];

    size_t                            i;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    // Get forcing data
    d3start[0] = plugin_global_param.forceskip[FORCING_EFR_DISCHARGE] +
                 plugin_global_param.forceoffset[FORCING_EFR_DISCHARGE];

    if (plugin_global_param.forcerun[FORCING_EFR_DISCHARGE]) {
        if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_EFR_DISCHARGE],
                       MISSING_S) != 0) {
            get_scatter_nc_field_double(&(plugin_filenames.forcing[
                                              FORCING_EFR_DISCHARGE]),
                                        plugin_filenames.f_varname[
                                            FORCING_EFR_DISCHARGE],
                                        d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                efr_force[i].discharge = dvar[i];
            }
        }
    }

    d3start[0] = plugin_global_param.forceskip[FORCING_EFR_BASEFLOW] +
                 plugin_global_param.forceoffset[FORCING_EFR_BASEFLOW];

    if (plugin_global_param.forcerun[FORCING_EFR_BASEFLOW]) {
        if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_EFR_BASEFLOW],
                       MISSING_S) != 0) {
            get_scatter_nc_field_double(&(plugin_filenames.forcing[
                                              FORCING_EFR_BASEFLOW]),
                                        plugin_filenames.f_varname[
                                            FORCING_EFR_BASEFLOW],
                                        d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                efr_force[i].baseflow = dvar[i];
            }
        }
    }

    efr_derived_forcing();

    free(dvar);
}
