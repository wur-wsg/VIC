/******************************************************************************
 * @section DESCRIPTION
 *
 * Water-use configuration functions
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
* @brief   Get water-use configuration options
******************************************/
bool
wu_get_global_param(char *cmdstr)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;

    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("WATERUSE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WATERUSE = str_to_bool(flgstr);
    }
    else if (strcasecmp("WATERUSE_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.wateruse.nc_filename);
    }
    else if (strcasecmp("FORCE_PUMPING_CAPACITY", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.FORCE_PUMP_CAP = str_to_bool(flgstr);
    }
    else if (strcasecmp("COMPENSATION_WITHDRAWAL", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.COMP_WITH = str_to_bool(flgstr);
    }
    else if (strcasecmp("REMOTE_WITHDRAWAL", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.REMOTE_WITH = str_to_bool(flgstr);
    }
    else if (strcasecmp("NONRENEWABLE_WITHDRAWAL", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.NONRENEW_WITH = str_to_bool(flgstr);
    }
    else if (strcasecmp("NONRENEWABLE_RUNOFF", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.NONRENEW_RUNOFF = str_to_bool(flgstr);
    }
    else {
        return false;
    }

    return true;
}

/******************************************
* @brief   Validate water-use configuration options
******************************************/
void
wu_validate_global_param(void)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;
    
    // Options
    if(!plugin_options.ROUTING){
        log_err("WATERUSE = TRUE but ROUTING = FALSE");
    }
    if(plugin_options.DECOMPOSITION == RANDOM_DECOMPOSITION){
        log_err("WATERUSE = TRUE but DECOMPOSITION = RANDOM");
    }
   
    // Parameters
    if (strcasecmp(plugin_filenames.wateruse.nc_filename, MISSING_S) == 0) {
        log_err("WATERUSE = TRUE but file is missing");
    }
    
    // Forcing
    if(plugin_options.FORCE_PUMP_CAP) {
        if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_PUMPING_CAP], MISSING_S) == 0) {
            log_err("FORCE_PUMP_CAP = TRUE but file is missing");
        }
    }
    if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_IRR_DEMAND], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_IRR_GROUNDWATER], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_IRR_CONSUMPTION], MISSING_S) != 0) {
        if(plugin_options.IRRIGATION){
            log_err("WATERUSE = TRUE and IRRIGATION = TRUE but forcing file provided");
        }
        plugin_options.WU_INPUT[WU_IRRIGATION] = WU_FROM_FILE;
    }
    if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_MUN_DEMAND], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_MUN_GROUNDWATER], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_MUN_CONSUMPTION], MISSING_S) != 0) {
        plugin_options.WU_INPUT[WU_MUNICIPAL] = WU_FROM_FILE;
    }
    if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_LIV_DEMAND], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_LIV_GROUNDWATER], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_LIV_CONSUMPTION], MISSING_S) != 0) {
        plugin_options.WU_INPUT[WU_LIVESTOCK] = WU_FROM_FILE;
    }
    if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_ENE_DEMAND], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_ENE_GROUNDWATER], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_ENE_CONSUMPTION], MISSING_S) != 0) {
        plugin_options.WU_INPUT[WU_ENERGY] = WU_FROM_FILE;
    }
    if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_MAN_DEMAND], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_MAN_GROUNDWATER], MISSING_S) != 0 &&
            strcasecmp(plugin_filenames.f_path_pfx[FORCING_MAN_CONSUMPTION], MISSING_S) != 0) {
        plugin_options.WU_INPUT[WU_MANUFACTURING] = WU_FROM_FILE;
    }
    if(plugin_options.IRRIGATION){
        plugin_options.WU_INPUT[WU_IRRIGATION] = WU_CALCULATE;
    }
}

/******************************************
* @brief   Get water-use parameters
******************************************/
bool
wu_get_parameters(char *cmdstr)
{
    extern plugin_parameters_struct    plugin_param;

    char                    optstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("NONRENEWABLE_LIMIT", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &plugin_param.NREN_LIM);
    }
    else {
        return false;
    }

    return true;
}

/******************************************
* @brief   Validate water-use parameters
******************************************/
void
wu_validate_parameters(void)
{
    extern plugin_parameters_struct    plugin_param;
    
    if (!(plugin_param.NREN_LIM >= 0)) {
        log_err("NREN_LIM must be defined on the interval [0,Inf) (mm)");
    }
}
