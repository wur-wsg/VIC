/******************************************************************************
 * @section DESCRIPTION
 *
 * Dam configuration functions
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
* @brief   Get dam configuration options
******************************************/
bool
dam_get_global_param(char *cmdstr)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;

    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("DAMS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.DAMS = str_to_bool(flgstr);
    }
    else if (strcasecmp("DAMS_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.dams.nc_filename);
    }
    else {
        return false;
    }

    return true;
}

/******************************************
* @brief   Validate dam configuration options
******************************************/
void
dam_validate_global_param(void)
{
    extern plugin_filenames_struct plugin_filenames;
    
    // Parameters
    if (strcasecmp(plugin_filenames.dams.nc_filename, MISSING_S) == 0) {
        log_err("DAMS = TRUE but file is missing");
    }
}

/******************************************
* @brief   Get dam parameters
******************************************/
bool
dam_get_parameters(char *cmdstr)
{
    extern plugin_parameters_struct    plugin_param;

    char                    optstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("DAM_ALPHA", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &plugin_param.DAM_ALPHA);
    } else if (strcasecmp("DAM_BETA", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &plugin_param.DAM_BETA);
    } else if (strcasecmp("DAM_GAMMA", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &plugin_param.DAM_GAMMA);
    }
    else {
        return false;
    }

    return true;
}

/******************************************
* @brief   Validate dam parameters
******************************************/
void
dam_validate_parameters(void)
{
    extern plugin_parameters_struct    plugin_param;
    
    if (!(plugin_param.DAM_ALPHA >= 0 && plugin_param.DAM_ALPHA <= 1)) {
        log_err("DAM_ALPHA must be defined on the interval [0,1] (-)");
    }
    if (!(plugin_param.DAM_BETA >= 0)) {
        log_err("DAM_BETA must be defined on the interval [0,Inf) (-)");
    }
}