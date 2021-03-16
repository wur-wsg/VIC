/******************************************************************************
 * @section DESCRIPTION
 *
 * Irrigation output functions
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
irr_set_output_met_data_info(void)
{
    extern metadata_struct out_metadata[];

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_SHORTAGE].varname, MAXSTRING,
             "%s", "OUT_SHORTAGE");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_SHORTAGE].long_name, MAXSTRING,
             "%s", "irrigation_shortage");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_SHORTAGE].standard_name,
             MAXSTRING, "%s", "irrigation shortage");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_SHORTAGE].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_SHORTAGE].description,
             MAXSTRING, "%s", "average irrigation shortage");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_REQUIREMENT].varname, MAXSTRING,
             "%s", "OUT_REQUIREMENT");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_REQUIREMENT].long_name,
             MAXSTRING, "%s", "irrigation_requirement");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_REQUIREMENT].standard_name,
             MAXSTRING, "%s", "irrigation requirement");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_REQUIREMENT].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_REQUIREMENT].description,
             MAXSTRING, "%s", "average irrigation requirement");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_RECEIVED].varname, MAXSTRING,
             "%s", "OUT_RECEIVED");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_RECEIVED].long_name,
             MAXSTRING, "%s", "irrigation_received");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_RECEIVED].standard_name,
             MAXSTRING, "%s", "irrigation received");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_RECEIVED].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_RECEIVED].description,
             MAXSTRING, "%s", "total irrigation water received");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_LEFTOVER].varname, MAXSTRING,
             "%s", "OUT_LEFTOVER");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_LEFTOVER].long_name,
             MAXSTRING, "%s", "irrigation_leftover");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_LEFTOVER].standard_name,
             MAXSTRING, "%s", "irrigation leftover");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_LEFTOVER].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_LEFTOVER].description,
             MAXSTRING, "%s", "average irrigation leftover water");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_APPLIED].varname, MAXSTRING,
             "%s", "OUT_APPLIED");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_APPLIED].long_name,
             MAXSTRING, "%s", "irrigation_applied");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_APPLIED].standard_name,
             MAXSTRING, "%s", "irrigation applied");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_APPLIED].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_APPLIED].description,
             MAXSTRING, "%s", "total irrigation water applied");

    out_metadata[N_OUTVAR_TYPES + OUT_SHORTAGE].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_REQUIREMENT].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_RECEIVED].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_LEFTOVER].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_APPLIED].nelem = 1;
}

/******************************************
* @brief   Set output aggregation types
******************************************/
void
irr_history(int           varid,
            unsigned int *agg_type)
{
    switch (varid) {
    case  N_OUTVAR_TYPES + OUT_REQUIREMENT:
    case  N_OUTVAR_TYPES + OUT_SHORTAGE:
        (*agg_type) = AGG_TYPE_AVG;
        break;
    case N_OUTVAR_TYPES + OUT_RECEIVED:
    case N_OUTVAR_TYPES + OUT_APPLIED:
        (*agg_type) = AGG_TYPE_SUM;
        break;
    case  N_OUTVAR_TYPES + OUT_LEFTOVER:
        (*agg_type) = AGG_TYPE_END;
        break;
    }
}

/******************************************
* @brief   Write output
******************************************/
void
irr_put_data(size_t iCell)
{
    extern option_struct       options;
    extern irr_var_struct   ***irr_var;
    extern irr_con_map_struct *irr_con_map;
    extern soil_con_struct    *soil_con;
    extern veg_con_struct    **veg_con;
    extern double           ***out_data;

    size_t                     i;
    size_t                     j;
    double                     veg_fract;
    double                     area_fract;
    int                        veg_index;

    for (i = 0; i < irr_con_map[iCell].ni_active; i++) {
        veg_index = irr_con_map[iCell].vidx[i];

        if (veg_index != NODATA_VEG) {
            veg_fract = veg_con[iCell][veg_index].Cv;

            if (veg_fract > 0) {
                for (j = 0; j < options.SNOW_BAND; j++) {
                    area_fract = soil_con[iCell].AreaFract[j];

                    if (area_fract > 0) {
                        out_data[iCell][N_OUTVAR_TYPES +
                                        OUT_REQUIREMENT][0] +=
                            irr_var[iCell][i][j].requirement *
                            veg_fract *
                            area_fract;
                        out_data[iCell][N_OUTVAR_TYPES +
                                        OUT_SHORTAGE][0] +=
                            irr_var[iCell][i][j].shortage *
                            veg_fract *
                            area_fract;
                        out_data[iCell][N_OUTVAR_TYPES +
                                        OUT_RECEIVED][0] +=
                            irr_var[iCell][i][j].received *
                            veg_fract *
                            area_fract;
                        out_data[iCell][N_OUTVAR_TYPES +
                                        OUT_LEFTOVER][0] +=
                            irr_var[iCell][i][j].leftover *
                            veg_fract *
                            area_fract;
                        out_data[iCell][N_OUTVAR_TYPES +
                                        OUT_APPLIED][0] +=
                            irr_var[iCell][i][j].applied *
                            veg_fract *
                            area_fract;
                    }
                }
            }
        }
    }
}
