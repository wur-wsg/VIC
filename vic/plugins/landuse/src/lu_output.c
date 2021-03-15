/******************************************************************************
 * @section DESCRIPTION
 *
 * Land-use output functions
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
lu_set_output_met_data_info(void)
{
    extern option_struct   options;
    extern metadata_struct out_metadata[];

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CV].varname, MAXSTRING,
             "%s", "OUT_CV");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CV].long_name, MAXSTRING,
             "%s", "coverage");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CV].standard_name,
             MAXSTRING, "%s", "coverage");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CV].units, MAXSTRING,
             "%s", "-");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CV].description,
             MAXSTRING, "%s", "vegetation coverage per vegetation type");

    out_metadata[N_OUTVAR_TYPES + OUT_CV].nelem = options.NVEGTYPES;
}

/******************************************
* @brief   Set output variable dimension sizes
******************************************/
void
lu_set_nc_var_info(unsigned int    varid,
                   nc_file_struct *nc_hist_file,
                   nc_var_struct  *nc_var)
{
    switch (varid) {
    case N_OUTVAR_TYPES + OUT_CV:
        nc_var->nc_dims = 4;
        nc_var->nc_counts[1] = nc_hist_file->veg_size;
        nc_var->nc_counts[2] = nc_hist_file->nj_size;
        nc_var->nc_counts[3] = nc_hist_file->ni_size;
    }
}

/******************************************
* @brief   Set output variable dimension ids
******************************************/
void
lu_set_nc_var_dimids(unsigned int    varid,
                     nc_file_struct *nc_hist_file,
                     nc_var_struct  *nc_var)
{
    switch (varid) {
    case N_OUTVAR_TYPES + OUT_CV:
        nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
        nc_var->nc_dimids[1] = nc_hist_file->veg_dimid;
        nc_var->nc_dimids[2] = nc_hist_file->nj_dimid;
        nc_var->nc_dimids[3] = nc_hist_file->ni_dimid;
    }
}

/******************************************
* @brief   Set output aggregation types
******************************************/
void
lu_history(int           varid,
           unsigned int *agg_type)
{
    switch (varid) {
    case  N_OUTVAR_TYPES + OUT_CV:
        (*agg_type) = AGG_TYPE_AVG;
        break;
    }
}

/******************************************
* @brief   Write output
******************************************/
void
lu_put_data(size_t iCell)
{
    extern veg_con_map_struct *veg_con_map;
    extern double           ***out_data;

    size_t                     i;

    for (i = 0; i < veg_con_map[iCell].nv_types; i++) {
        out_data[iCell][N_OUTVAR_TYPES +
                        OUT_CV][i] = veg_con_map[iCell].Cv[i];
    }
}
