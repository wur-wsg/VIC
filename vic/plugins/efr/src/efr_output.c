/******************************************************************************
 * @section DESCRIPTION
 *
 * Environmental requirement output functions
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
* @brief   Set output metadata
******************************************/
void
efr_set_output_met_data_info(void)
{
    extern metadata_struct out_metadata[];

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_DISCHARGE].varname,
             MAXSTRING,
             "%s", "OUT_EFR_DISCHARGE");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_DISCHARGE].long_name,
             MAXSTRING,
             "%s", "efr_discharge");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_DISCHARGE].standard_name,
             MAXSTRING, "%s", "efr discharge");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_DISCHARGE].units, MAXSTRING,
             "%s", "m3/s");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_DISCHARGE].description,
             MAXSTRING, "%s", "required environmental discharge");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_BASEFLOW].varname, MAXSTRING,
             "%s", "OUT_EFR_BASEFLOW");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_BASEFLOW].long_name,
             MAXSTRING, "%s", "efr_baseflow");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_BASEFLOW].standard_name,
             MAXSTRING, "%s", "efr baseflow");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_BASEFLOW].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_BASEFLOW].description,
             MAXSTRING, "%s", "required environmental baseflow");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_MOIST].varname, MAXSTRING,
             "%s", "OUT_EFR_MOIST");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_MOIST].long_name,
             MAXSTRING, "%s", "efr_moist");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_MOIST].standard_name,
             MAXSTRING, "%s", "efr moist");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_MOIST].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_EFR_MOIST].description,
             MAXSTRING, "%s", "required environmental soil moisture");

    out_metadata[N_OUTVAR_TYPES + OUT_EFR_DISCHARGE].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_EFR_BASEFLOW].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_EFR_MOIST].nelem = 1;
}

/******************************************
* @brief   Set output aggregation types
******************************************/
void
efr_history(int           varid,
            unsigned int *agg_type)
{
    switch (varid) {
    case  N_OUTVAR_TYPES + OUT_EFR_MOIST:
        (*agg_type) = AGG_TYPE_END;
        break;
    case  N_OUTVAR_TYPES + OUT_EFR_BASEFLOW:
        (*agg_type) = AGG_TYPE_SUM;
        break;
    case  N_OUTVAR_TYPES + OUT_EFR_DISCHARGE:
        (*agg_type) = AGG_TYPE_AVG;
        break;
    }
}

/******************************************
* @brief   Write output
******************************************/
void
efr_put_data(size_t iCell)
{
    extern option_struct       options;
    extern efr_force_struct   *efr_force;
    extern soil_con_struct    *soil_con;
    extern veg_con_struct    **veg_con;
    extern veg_con_map_struct *veg_con_map;
    extern double           ***out_data;

    double                     veg_fract;
    double                     area_fract;

    size_t                     i;
    size_t                     j;

    out_data[iCell][N_OUTVAR_TYPES +
                    OUT_EFR_DISCHARGE][0] = efr_force[iCell].discharge;
    out_data[iCell][N_OUTVAR_TYPES +
                    OUT_EFR_BASEFLOW][0] = efr_force[iCell].baseflow;

    for (i = 0; i < veg_con_map[iCell].nv_active; i++) {
        veg_fract = veg_con[iCell][i].Cv;
        for (j = 0; j < options.SNOW_BAND; j++) {
            area_fract = soil_con[iCell].AreaFract[j];

            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_EFR_MOIST][0] += efr_force[iCell].moist[i][j] *
                                                 veg_fract * area_fract;
        }
    }
}
