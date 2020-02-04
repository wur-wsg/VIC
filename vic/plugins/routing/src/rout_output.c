/******************************************************************************
 * @section DESCRIPTION
 *
 * Routing output functions
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
rout_set_output_met_data_info(void)
{
    extern metadata_struct out_metadata[];

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].varname, MAXSTRING,
             "%s", "OUT_DISCHARGE");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].long_name, MAXSTRING,
             "%s", "discharge");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].standard_name,
             MAXSTRING, "%s", "discharge");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].units, MAXSTRING,
             "%s", "m3/s");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].description,
             MAXSTRING, "%s", "discharge at the cell outflow point");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].varname, MAXSTRING,
             "%s", "OUT_STREAM_MOIST");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].long_name,
             MAXSTRING, "%s", "stream_storage");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].standard_name,
             MAXSTRING, "%s", "stream storage");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].description,
             MAXSTRING, "%s", "moisture storage in stream flow");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_NONREN_DEFICIT].varname, MAXSTRING,
             "%s", "OUT_NONREN_DEFICIT");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_NONREN_DEFICIT].long_name,
             MAXSTRING, "%s", "non_renewable_deficit");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_NONREN_DEFICIT].standard_name,
             MAXSTRING, "%s", "non_renewable_deficit");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_NONREN_DEFICIT].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_NONREN_DEFICIT].description,
             MAXSTRING, "%s", "non-renewable deficit");

    out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_NONREN_DEFICIT].nelem = 1;
}

/******************************************
* @brief   Set output aggregation types
******************************************/
void
rout_history(int           varid,
             unsigned int *agg_type)
{
    switch (varid) {
    case  N_OUTVAR_TYPES + OUT_STREAM_MOIST:
    case  N_OUTVAR_TYPES + OUT_NONREN_DEFICIT:
        (*agg_type) = AGG_TYPE_END;
        break;
    case  N_OUTVAR_TYPES + OUT_DISCHARGE:
        (*agg_type) = AGG_TYPE_AVG;
        break;
    }
}

/******************************************
* @brief   Write output
******************************************/
void
rout_put_data(size_t iCell)
{
    extern global_param_struct global_param;
    extern domain_struct       local_domain;
    extern rout_var_struct    *rout_var;
    extern double           ***out_data;

    out_data[iCell][N_OUTVAR_TYPES +
                    OUT_STREAM_MOIST][0] = rout_var[iCell].stream *
                                           global_param.dt /
                                           local_domain.locations[iCell].area *
                                           MM_PER_M;
    out_data[iCell][N_OUTVAR_TYPES +
                    OUT_NONREN_DEFICIT][0] = rout_var[iCell].nonrenew_deficit;
    out_data[iCell][N_OUTVAR_TYPES +
                    OUT_DISCHARGE][0] = rout_var[iCell].discharge;
}
