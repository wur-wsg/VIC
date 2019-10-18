/******************************************************************************
 * @section DESCRIPTION
 *
 * Setup netCDF output variables.
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

#include <vic_driver_shared_image.h>

/******************************************************************************
 * @brief    Setup netCDF output variables.
 *****************************************************************************/
void
set_nc_var_info_lake_only(unsigned int       varid,
                unsigned short int dtype,
                nc_file_struct    *nc_hist_file,
                nc_var_struct     *nc_var)
{
    size_t i;

    // set datatype
    nc_var->nc_type = get_nc_dtype(dtype);

    for (i = 0; i < MAXDIMS; i++) {
        nc_var->nc_dimids[i] = -1;
        nc_var->nc_counts[i] = 0;
    }

    // Set the number of dimensions and the count sizes
    switch (varid) {
    case OUT_FDEPTH:
    case OUT_TDEPTH:
        nc_var->nc_dims = 3;
        nc_var->nc_counts[1] = nc_hist_file->front_size;
        nc_var->nc_counts[2] = nc_hist_file->lake_size;
        break;
    case OUT_SMLIQFRAC:
    case OUT_SMFROZFRAC:
    case OUT_SOIL_ICE:
    case OUT_SOIL_LIQ:
    case OUT_SOIL_ICE_FRAC:
    case OUT_SOIL_LIQ_FRAC:
    case OUT_SOIL_MOIST:
    case OUT_SOIL_TEMP:
        nc_var->nc_dims = 3;
        nc_var->nc_counts[1] = nc_hist_file->layer_size;
        nc_var->nc_counts[2] = nc_hist_file->lake_size;
        break;
    case OUT_SOIL_TNODE:
    case OUT_SOIL_TNODE_WL:
    case OUT_SOILT_FBFLAG:
        nc_var->nc_dims = 3;
        nc_var->nc_counts[1] = nc_hist_file->node_size;
        nc_var->nc_counts[2] = nc_hist_file->lake_size;
        break;
    case OUT_ADV_SENS_BAND:
    case OUT_ADVECTION_BAND:
    case OUT_ALBEDO_BAND:
    case OUT_DELTACC_BAND:
    case OUT_GRND_FLUX_BAND:
    case OUT_IN_LONG_BAND:
    case OUT_LATENT_BAND:
    case OUT_LATENT_SUB_BAND:
    case OUT_MELT_ENERGY_BAND:
    case OUT_LWNET_BAND:
    case OUT_SWNET_BAND:
    case OUT_RFRZ_ENERGY_BAND:
    case OUT_SENSIBLE_BAND:
    case OUT_SNOW_CANOPY_BAND:
    case OUT_SNOW_COVER_BAND:
    case OUT_SNOW_DEPTH_BAND:
    case OUT_SNOW_FLUX_BAND:
    case OUT_SNOW_MELT_BAND:
    case OUT_SNOW_PACKT_BAND:
    case OUT_SNOW_SURFT_BAND:
    case OUT_SWE_BAND:
        nc_var->nc_dims = 3;
        nc_var->nc_counts[1] = nc_hist_file->band_size;
        nc_var->nc_counts[2] = nc_hist_file->lake_size;
        break;
    default:
        nc_var->nc_dims = 2;
        nc_var->nc_counts[1] = nc_hist_file->lake_size;
    }
}

/******************************************************************************
 * @brief    Set netcdf dim ids.
 *****************************************************************************/
void
set_nc_var_dimids_lake_only(unsigned int    varid,
                  nc_file_struct *nc_hist_file,
                  nc_var_struct  *nc_var)
{
    size_t i;

    for (i = 0; i < MAXDIMS; i++) {
        nc_var->nc_dimids[i] = -1;
    }

    // Set the non-default ones
    switch (varid) {
    case OUT_FDEPTH:
    case OUT_TDEPTH:
        nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
        nc_var->nc_dimids[1] = nc_hist_file->front_dimid;
        nc_var->nc_dimids[2] = nc_hist_file->lake_dimid;
        break;
    case OUT_SMLIQFRAC:
    case OUT_SMFROZFRAC:
    case OUT_SOIL_ICE:
    case OUT_SOIL_LIQ:
    case OUT_SOIL_ICE_FRAC:
    case OUT_SOIL_LIQ_FRAC:
    case OUT_SOIL_MOIST:
    case OUT_SOIL_TEMP:
        nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
        nc_var->nc_dimids[1] = nc_hist_file->layer_dimid;
        nc_var->nc_dimids[2] = nc_hist_file->lake_dimid;
        break;
    case OUT_SOIL_TNODE:
    case OUT_SOIL_TNODE_WL:
    case OUT_SOILT_FBFLAG:
        nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
        nc_var->nc_dimids[1] = nc_hist_file->node_dimid;
        nc_var->nc_dimids[2] = nc_hist_file->lake_dimid;
        break;
    case OUT_ADV_SENS_BAND:
    case OUT_ADVECTION_BAND:
    case OUT_ALBEDO_BAND:
    case OUT_DELTACC_BAND:
    case OUT_GRND_FLUX_BAND:
    case OUT_IN_LONG_BAND:
    case OUT_LATENT_BAND:
    case OUT_LATENT_SUB_BAND:
    case OUT_MELT_ENERGY_BAND:
    case OUT_LWNET_BAND:
    case OUT_SWNET_BAND:
    case OUT_RFRZ_ENERGY_BAND:
    case OUT_SENSIBLE_BAND:
    case OUT_SNOW_CANOPY_BAND:
    case OUT_SNOW_COVER_BAND:
    case OUT_SNOW_DEPTH_BAND:
    case OUT_SNOW_FLUX_BAND:
    case OUT_SNOW_MELT_BAND:
    case OUT_SNOW_PACKT_BAND:
    case OUT_SNOW_SURFT_BAND:
    case OUT_SWE_BAND:
        nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
        nc_var->nc_dimids[1] = nc_hist_file->band_dimid;
        nc_var->nc_dimids[2] = nc_hist_file->lake_dimid;
        break;
    default:
        nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
        nc_var->nc_dimids[1] = nc_hist_file->lake_dimid;
    }
}
