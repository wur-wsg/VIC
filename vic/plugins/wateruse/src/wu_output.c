/******************************************************************************
 * @section DESCRIPTION
 *
 * Water-use output functions
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
wu_set_output_met_data_info(void)
{
    extern plugin_option_struct plugin_options;
    extern metadata_struct out_metadata[];

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_GW_SECT].varname, "OUT_AV_GW_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_GW_SECT].long_name, "available_groundwater_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_GW_SECT].standard_name, "available_groundwater_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_GW_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_GW_SECT].description, "available from groundwater for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_SURF_SECT].varname, "OUT_AV_SURF_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_SURF_SECT].long_name, "available_surface_water_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_SURF_SECT].standard_name, "available_surface_water_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_SURF_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_SURF_SECT].description, "available from surface water for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_DAM_SECT].varname, "OUT_AV_DAM_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_DAM_SECT].long_name, "available_dam_storage_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_DAM_SECT].standard_name, "available_dam_storage_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_DAM_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_DAM_SECT].description, "available from dam storage for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_REM_SECT].varname, "OUT_AV_REM_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_REM_SECT].long_name, "available_remote_water_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_REM_SECT].standard_name, "available_remote_water_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_REM_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_REM_SECT].description, "available from remote cell for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_GW_SECT].varname, "OUT_DE_GW_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_GW_SECT].long_name, "demand_groundwater_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_GW_SECT].standard_name, "demand_groundwater_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_GW_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_GW_SECT].description, "demand from groundwater for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_SURF_SECT].varname, "OUT_DE_SURF_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_SURF_SECT].long_name, "demand_surface_water_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_SURF_SECT].standard_name, "demand_surface_water_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_SURF_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_SURF_SECT].description, "demand from surface water for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_NREN_SECT].varname, "OUT_DE_NREN_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_NREN_SECT].long_name, "demand_nonrenewable_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_NREN_SECT].standard_name, "demand_nonrenewable_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_NREN_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_NREN_SECT].description, "demand from non-renewable surface water for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_REM_SECT].varname, "OUT_DE_REM_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_REM_SECT].long_name, "demand_remote_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_REM_SECT].standard_name, "demand_remote_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_REM_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_REM_SECT].description, "demand from remote cell for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_GW_SECT].varname, "OUT_WI_GW_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_GW_SECT].long_name, "withdrawn_groundwater_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_GW_SECT].standard_name, "withdrawn_groundwater_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_GW_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_GW_SECT].description, "withdrawn from groundwater for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_SURF_SECT].varname, "OUT_WI_SURF_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_SURF_SECT].long_name, "withdrawn_surface_water_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_SURF_SECT].standard_name, "withdrawn_surface_water_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_SURF_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_SURF_SECT].description, "withdrawn from surface water for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_DAM_SECT].varname, "OUT_WI_DAM_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_DAM_SECT].long_name, "withdrawn_dam_storage_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_DAM_SECT].standard_name, "withdrawn_dam_storage_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_DAM_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_DAM_SECT].description, "withdrawn from dam storage for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_REM_SECT].varname, "OUT_WI_REM_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_REM_SECT].long_name, "withdrawn_remote_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_REM_SECT].standard_name, "withdrawn_remote_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_REM_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_REM_SECT].description, "withdrawn from remote cell for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_NREN_SECT].varname, "OUT_WI_NREN_SECT");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_NREN_SECT].long_name, "withdrawn_non_renewable_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_NREN_SECT].standard_name, "withdrawn_non_renewable_sector");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_NREN_SECT].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_NREN_SECT].description, "withdrawn from non-renewable source for sector");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_GW].varname, "OUT_AV_GW");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_GW].long_name, "available_groundwater");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_GW].standard_name, "available_groundwater");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_GW].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_GW].description, "available from groundwater");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_SURF].varname, "OUT_AV_SURF");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_SURF].long_name, "available_surface_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_SURF].standard_name, "available_surface_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_SURF].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_SURF].description, "available from surface water");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_DAM].varname, "OUT_AV_DAM");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_DAM].long_name, "available_dam_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_DAM].standard_name, "available_dam_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_DAM].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_DAM].description, "available from dam storage");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_REM].varname, "OUT_AV_REM");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_REM].long_name, "available_remote_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_REM].standard_name, "available_remote_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_REM].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AV_REM].description, "available from remote cell");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_GW].varname, "OUT_DE_GW");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_GW].long_name, "demand_groundwater");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_GW].standard_name, "demand_groundwater");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_GW].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_GW].description, "demand from groundwater");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_SURF].varname, "OUT_DE_SURF");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_SURF].long_name, "demand_surface_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_SURF].standard_name, "demand_surface_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_SURF].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_SURF].description, "demand from surface water");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_NREN].varname, "OUT_DE_NREN");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_NREN].long_name, "demand_nonrenewable");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_NREN].standard_name, "demand_nonrenewable");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_NREN].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_NREN].description, "demand from non-renewable surface water");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_REM].varname, "OUT_DE_REM");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_REM].long_name, "demand_remote");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_REM].standard_name, "demand_remote");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_REM].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DE_REM].description, "demand from remote cell");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_GW].varname, "OUT_WI_GW");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_GW].long_name, "withdrawn_groundwater");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_GW].standard_name, "withdrawn_groundwater");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_GW].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_GW].description, "withdrawn from groundwater");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_SURF].varname, "OUT_WI_SURF");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_SURF].long_name, "withdrawn_surface_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_SURF].standard_name, "withdrawn_surface_water");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_SURF].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_SURF].description, "withdrawn from surface water");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_DAM].varname, "OUT_WI_DAM");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_DAM].long_name, "withdrawn_dam_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_DAM].standard_name, "withdrawn_dam_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_DAM].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_DAM].description, "withdrawn from dam storage");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_REM].varname, "OUT_WI_REM");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_REM].long_name, "withdrawn_remote");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_REM].standard_name, "withdrawn_remote");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_REM].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_REM].description, "withdrawn from remote cell");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_NREN].varname, "OUT_WI_NREN");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_NREN].long_name, "withdrawn_non_renewable");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_NREN].standard_name, "withdrawn_non_renewable");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_NREN].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WI_NREN].description, "withdrawn from non-renewable source");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN].varname, "OUT_WITHDRAWN");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN].long_name, "withdrawn");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN].standard_name, "withdrawn");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN].description, "total water withdrawn");
    
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE].varname, "OUT_AVAILABLE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE].long_name, "available");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE].standard_name, "available");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE].description, "total water available");
    
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND].varname, "OUT_DEMAND");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND].long_name, "demand");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND].standard_name, "demand");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DEMAND].description, "total water demand");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_RETURNED].varname, "OUT_RETURNED");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_RETURNED].long_name, "returned");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_RETURNED].standard_name, "returned");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_RETURNED].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_RETURNED].description, "water returned");

    out_metadata[N_OUTVAR_TYPES + OUT_AV_GW_SECT].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_AV_SURF_SECT].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_AV_DAM_SECT].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_AV_REM_SECT].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_DE_GW_SECT].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_DE_SURF_SECT].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_DE_NREN_SECT].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_DE_REM_SECT].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_WI_GW_SECT].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_WI_SURF_SECT].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_WI_DAM_SECT].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_WI_REM_SECT].nelem = plugin_options.NWUTYPES;
    out_metadata[N_OUTVAR_TYPES + OUT_WI_NREN_SECT].nelem = plugin_options.NWUTYPES;

    out_metadata[N_OUTVAR_TYPES + OUT_AV_GW].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_AV_SURF].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_AV_DAM].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_AV_REM].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_DE_GW].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_DE_SURF].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_DE_NREN].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_DE_REM].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_WI_GW].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_WI_SURF].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_WI_DAM].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_WI_REM].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_WI_NREN].nelem = 1;
    
    out_metadata[N_OUTVAR_TYPES + OUT_AVAILABLE].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_DEMAND].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_WITHDRAWN].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_RETURNED].nelem = 1;
}

/******************************************
* @brief   Initialize output dimensions
******************************************/
void
wu_initialize_nc_file(nc_file_struct  *nc_state_file)
{
    extern plugin_option_struct plugin_options;
    
    nc_state_file->wu_dimid = MISSING;
    nc_state_file->wu_size = plugin_options.NWUTYPES;
}

/******************************************
* @brief   Set output dimensions
******************************************/
void
wu_add_hist_dim(nc_file_struct *nc,
                stream_struct  *stream)
{
    int status;

    status = nc_def_dim(nc->nc_id, "wu_class", nc->wu_size, &(nc->wu_dimid));
    check_nc_status(status, "Error defining wu_class bounds dimension in %s",
                    stream->filename);
}

/******************************************
* @brief   Set output variable dimension sizes
******************************************/
void
wu_set_nc_var_info(unsigned int    varid,
                   nc_file_struct *nc_hist_file,
                   nc_var_struct  *nc_var)
{
    
    // Set the number of dimensions and dimids for each state variable
    switch(varid){
        case N_OUTVAR_TYPES + OUT_AV_GW_SECT:
        case N_OUTVAR_TYPES + OUT_AV_SURF_SECT:
        case N_OUTVAR_TYPES + OUT_AV_DAM_SECT:
        case N_OUTVAR_TYPES + OUT_AV_REM_SECT:
        case N_OUTVAR_TYPES + OUT_DE_GW_SECT:
        case N_OUTVAR_TYPES + OUT_DE_SURF_SECT:
        case N_OUTVAR_TYPES + OUT_DE_NREN_SECT:
        case N_OUTVAR_TYPES + OUT_DE_REM_SECT:
        case N_OUTVAR_TYPES + OUT_WI_GW_SECT:
        case N_OUTVAR_TYPES + OUT_WI_SURF_SECT:
        case N_OUTVAR_TYPES + OUT_WI_DAM_SECT:
        case N_OUTVAR_TYPES + OUT_WI_REM_SECT:
        case N_OUTVAR_TYPES + OUT_WI_NREN_SECT:
        nc_var->nc_dims = 4;
        nc_var->nc_counts[1] = nc_hist_file->wu_size;
        nc_var->nc_counts[2] = nc_hist_file->nj_size;
        nc_var->nc_counts[3] = nc_hist_file->ni_size;
        break;
        case N_OUTVAR_TYPES + OUT_AV_GW:
        case N_OUTVAR_TYPES + OUT_AV_SURF:
        case N_OUTVAR_TYPES + OUT_AV_DAM:
        case N_OUTVAR_TYPES + OUT_AV_REM:
        case N_OUTVAR_TYPES + OUT_DE_GW:
        case N_OUTVAR_TYPES + OUT_DE_SURF:
        case N_OUTVAR_TYPES + OUT_DE_NREN:
        case N_OUTVAR_TYPES + OUT_DE_REM:
        case N_OUTVAR_TYPES + OUT_WI_GW:
        case N_OUTVAR_TYPES + OUT_WI_SURF:
        case N_OUTVAR_TYPES + OUT_WI_DAM:
        case N_OUTVAR_TYPES + OUT_WI_REM:
        case N_OUTVAR_TYPES + OUT_WI_NREN:
        case N_OUTVAR_TYPES + OUT_AVAILABLE:
        case N_OUTVAR_TYPES + OUT_DEMAND:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN:
        case N_OUTVAR_TYPES + OUT_RETURNED:
        nc_var->nc_dims = 3;
        nc_var->nc_counts[1] = nc_hist_file->nj_size;
        nc_var->nc_counts[2] = nc_hist_file->ni_size;
        break;
    }
}

/******************************************
* @brief   Set output variable dimension ids
******************************************/
void
wu_set_nc_var_dimids(unsigned int    varid,
                     nc_file_struct *nc_hist_file,
                     nc_var_struct  *nc_var)
{
    switch(varid){
        case N_OUTVAR_TYPES + OUT_AV_GW_SECT:
        case N_OUTVAR_TYPES + OUT_AV_SURF_SECT:
        case N_OUTVAR_TYPES + OUT_AV_DAM_SECT:
        case N_OUTVAR_TYPES + OUT_AV_REM_SECT:
        case N_OUTVAR_TYPES + OUT_DE_GW_SECT:
        case N_OUTVAR_TYPES + OUT_DE_SURF_SECT:
        case N_OUTVAR_TYPES + OUT_DE_NREN_SECT:
        case N_OUTVAR_TYPES + OUT_DE_REM_SECT:
        case N_OUTVAR_TYPES + OUT_WI_GW_SECT:
        case N_OUTVAR_TYPES + OUT_WI_SURF_SECT:
        case N_OUTVAR_TYPES + OUT_WI_DAM_SECT:
        case N_OUTVAR_TYPES + OUT_WI_REM_SECT:
        case N_OUTVAR_TYPES + OUT_WI_NREN_SECT:
        nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
        nc_var->nc_dimids[1] = nc_hist_file->wu_dimid;
        nc_var->nc_dimids[2] = nc_hist_file->nj_dimid;
        nc_var->nc_dimids[3] = nc_hist_file->ni_dimid;
        break;
        case N_OUTVAR_TYPES + OUT_AV_GW:
        case N_OUTVAR_TYPES + OUT_AV_SURF:
        case N_OUTVAR_TYPES + OUT_AV_DAM:
        case N_OUTVAR_TYPES + OUT_AV_REM:
        case N_OUTVAR_TYPES + OUT_DE_GW:
        case N_OUTVAR_TYPES + OUT_DE_SURF:
        case N_OUTVAR_TYPES + OUT_DE_NREN:
        case N_OUTVAR_TYPES + OUT_DE_REM:
        case N_OUTVAR_TYPES + OUT_WI_GW:
        case N_OUTVAR_TYPES + OUT_WI_SURF:
        case N_OUTVAR_TYPES + OUT_WI_DAM:
        case N_OUTVAR_TYPES + OUT_WI_REM:
        case N_OUTVAR_TYPES + OUT_WI_NREN:
        case N_OUTVAR_TYPES + OUT_AVAILABLE:
        case N_OUTVAR_TYPES + OUT_DEMAND:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN:
        case N_OUTVAR_TYPES + OUT_RETURNED:
        nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
        nc_var->nc_dimids[1] = nc_hist_file->nj_dimid;
        nc_var->nc_dimids[2] = nc_hist_file->ni_dimid;
        break;
    }
}

/******************************************
* @brief   Set output aggregation types
******************************************/
void
wu_history(unsigned int varid, unsigned int *agg_type)
{
    switch (varid) {
        case N_OUTVAR_TYPES + OUT_AV_GW_SECT:
        case N_OUTVAR_TYPES + OUT_AV_SURF_SECT:
        case N_OUTVAR_TYPES + OUT_AV_DAM_SECT:
        case N_OUTVAR_TYPES + OUT_AV_REM_SECT:
        case N_OUTVAR_TYPES + OUT_DE_GW_SECT:
        case N_OUTVAR_TYPES + OUT_DE_SURF_SECT:
        case N_OUTVAR_TYPES + OUT_DE_NREN_SECT:
        case N_OUTVAR_TYPES + OUT_DE_REM_SECT:
        case N_OUTVAR_TYPES + OUT_WI_GW_SECT:
        case N_OUTVAR_TYPES + OUT_WI_SURF_SECT:
        case N_OUTVAR_TYPES + OUT_WI_DAM_SECT:
        case N_OUTVAR_TYPES + OUT_WI_REM_SECT:
        case N_OUTVAR_TYPES + OUT_WI_NREN_SECT:
        case N_OUTVAR_TYPES + OUT_AV_GW:
        case N_OUTVAR_TYPES + OUT_AV_SURF:
        case N_OUTVAR_TYPES + OUT_AV_DAM:
        case N_OUTVAR_TYPES + OUT_AV_REM:
        case N_OUTVAR_TYPES + OUT_DE_GW:
        case N_OUTVAR_TYPES + OUT_DE_SURF:
        case N_OUTVAR_TYPES + OUT_DE_NREN:
        case N_OUTVAR_TYPES + OUT_DE_REM:
        case N_OUTVAR_TYPES + OUT_WI_GW:
        case N_OUTVAR_TYPES + OUT_WI_SURF:
        case N_OUTVAR_TYPES + OUT_WI_DAM:
        case N_OUTVAR_TYPES + OUT_WI_REM:
        case N_OUTVAR_TYPES + OUT_WI_NREN:
        case N_OUTVAR_TYPES + OUT_AVAILABLE:
        case N_OUTVAR_TYPES + OUT_DEMAND:
        case N_OUTVAR_TYPES + OUT_WITHDRAWN:
        case N_OUTVAR_TYPES + OUT_RETURNED:
        (*agg_type) = AGG_TYPE_SUM;
        break;
    }
}

/******************************************
* @brief   Write output
******************************************/
void
wu_put_data(size_t iCell)
{
    extern plugin_option_struct plugin_options;
    extern wu_con_map_struct *wu_con_map;
    extern wu_var_struct **wu_var;
    extern double ***out_data;
    
    size_t i;
    size_t j;
    size_t iCell2;
    int iSector;
    int iSector2;
    
    for(i = 0; i < plugin_options.NWUTYPES; i++){
        iSector = wu_con_map[iCell].sidx[i];
        if(iSector != NODATA_WU){
            out_data[iCell][N_OUTVAR_TYPES + OUT_AV_GW_SECT][i] = wu_var[iCell][iSector].available_gw;
            out_data[iCell][N_OUTVAR_TYPES + OUT_AV_SURF_SECT][i] = wu_var[iCell][iSector].available_surf;
            out_data[iCell][N_OUTVAR_TYPES + OUT_AV_DAM_SECT][i] = wu_var[iCell][iSector].available_dam;
            out_data[iCell][N_OUTVAR_TYPES + OUT_DE_GW_SECT][i] = wu_var[iCell][iSector].demand_gw;
            out_data[iCell][N_OUTVAR_TYPES + OUT_DE_SURF_SECT][i] = wu_var[iCell][iSector].demand_surf;
            out_data[iCell][N_OUTVAR_TYPES + OUT_DE_NREN_SECT][i] = wu_var[iCell][iSector].demand_nonrenew;
            out_data[iCell][N_OUTVAR_TYPES + OUT_WI_GW_SECT][i] = wu_var[iCell][iSector].withdrawn_gw;
            out_data[iCell][N_OUTVAR_TYPES + OUT_WI_SURF_SECT][i] = wu_var[iCell][iSector].withdrawn_surf;
            out_data[iCell][N_OUTVAR_TYPES + OUT_WI_DAM_SECT][i] = wu_var[iCell][iSector].withdrawn_dam;
            out_data[iCell][N_OUTVAR_TYPES + OUT_WI_NREN_SECT][i] = wu_var[iCell][iSector].withdrawn_nonrenew;
        
            out_data[iCell][N_OUTVAR_TYPES + OUT_RETURNED][0] += wu_var[iCell][iSector].returned;
            
            for(j = 0; j < wu_con[iCell].nreceiving; j++){
                iCell2 = wu_con[iCell].receiving[j];

                iSector2 = wu_con_map[iCell2].sidx[i];        
                if(iSector2 != NODATA_WU){
                    out_data[iCell][N_OUTVAR_TYPES + OUT_AV_REM_SECT][i] += wu_var[iCell2][iSector2].available_remote;
                    out_data[iCell][N_OUTVAR_TYPES + OUT_DE_REM_SECT][i] += wu_var[iCell2][iSector2].demand_remote;
                    out_data[iCell][N_OUTVAR_TYPES + OUT_WI_REM_SECT][i] += wu_var[iCell2][iSector2].withdrawn_remote;
                }
            }
        }
    }
            
    for(i = 0; i < plugin_options.NWUTYPES; i++){
        out_data[iCell][N_OUTVAR_TYPES + OUT_AV_GW][0] += out_data[iCell][N_OUTVAR_TYPES + OUT_AV_GW_SECT][i];
        out_data[iCell][N_OUTVAR_TYPES + OUT_AV_SURF][0] += out_data[iCell][N_OUTVAR_TYPES + OUT_AV_SURF_SECT][i];
        out_data[iCell][N_OUTVAR_TYPES + OUT_AV_DAM][0] += out_data[iCell][N_OUTVAR_TYPES + OUT_AV_DAM_SECT][i];
        out_data[iCell][N_OUTVAR_TYPES + OUT_AV_REM][0] += out_data[iCell][N_OUTVAR_TYPES + OUT_AV_REM_SECT][i];
        out_data[iCell][N_OUTVAR_TYPES + OUT_DE_GW][0] += out_data[iCell][N_OUTVAR_TYPES + OUT_DE_GW_SECT][i];
        out_data[iCell][N_OUTVAR_TYPES + OUT_DE_SURF][0] += out_data[iCell][N_OUTVAR_TYPES + OUT_DE_SURF_SECT][i];
        out_data[iCell][N_OUTVAR_TYPES + OUT_DE_REM][0] += out_data[iCell][N_OUTVAR_TYPES + OUT_DE_REM_SECT][i];
        out_data[iCell][N_OUTVAR_TYPES + OUT_WI_GW][0] += out_data[iCell][N_OUTVAR_TYPES + OUT_WI_GW_SECT][i];
        out_data[iCell][N_OUTVAR_TYPES + OUT_WI_SURF][0] += out_data[iCell][N_OUTVAR_TYPES + OUT_WI_SURF_SECT][i];
        out_data[iCell][N_OUTVAR_TYPES + OUT_WI_DAM][0] += out_data[iCell][N_OUTVAR_TYPES + OUT_WI_DAM_SECT][i];
        out_data[iCell][N_OUTVAR_TYPES + OUT_WI_REM][0] += out_data[iCell][N_OUTVAR_TYPES + OUT_WI_REM_SECT][i];
        out_data[iCell][N_OUTVAR_TYPES + OUT_WI_NREN][0] += out_data[iCell][N_OUTVAR_TYPES + OUT_WI_NREN_SECT][i];
    }
            
    out_data[iCell][N_OUTVAR_TYPES + OUT_AVAILABLE][0] = 
            out_data[iCell][N_OUTVAR_TYPES + OUT_AV_GW][0] +
            out_data[iCell][N_OUTVAR_TYPES + OUT_AV_SURF][0] +
            out_data[iCell][N_OUTVAR_TYPES + OUT_AV_DAM][0];
    out_data[iCell][N_OUTVAR_TYPES + OUT_DEMAND][0] = 
            out_data[iCell][N_OUTVAR_TYPES + OUT_DE_GW][0] +
            out_data[iCell][N_OUTVAR_TYPES + OUT_DE_SURF][0] +
            out_data[iCell][N_OUTVAR_TYPES + OUT_DE_REM][0];
    out_data[iCell][N_OUTVAR_TYPES + OUT_WITHDRAWN][0] = 
            out_data[iCell][N_OUTVAR_TYPES + OUT_WI_GW][0] +
            out_data[iCell][N_OUTVAR_TYPES + OUT_WI_SURF][0] +
            out_data[iCell][N_OUTVAR_TYPES + OUT_WI_DAM][0] +
            out_data[iCell][N_OUTVAR_TYPES + OUT_WI_REM][0] +
            out_data[iCell][N_OUTVAR_TYPES + OUT_WI_NREN][0];
}
