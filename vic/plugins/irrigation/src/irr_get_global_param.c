/******************************************************************************
 * @section DESCRIPTION
 *
 * Irrigation configuration functions
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
* @brief   Get irrigation configuration options
******************************************/
bool
irr_get_global_param(char *cmdstr)
{
    extern plugin_option_struct       plugin_options;
    extern plugin_filenames_struct    plugin_filenames;

    char                              optstr[MAXSTRING];
    char                              flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("IRRIGATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.IRRIGATION = str_to_bool(flgstr);
    }
    else if (strcasecmp("POTENTIAL_IRRIGATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.POTENTIAL_IRRIGATION = str_to_bool(flgstr);
    }
    else if (strcasecmp("IRRIGATION_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.irrigation.nc_filename);
    }
    else {
        return false;
    }

    return true;
}

/******************************************
* @brief   Validate irrigation configuration options
******************************************/
void
irr_validate_global_param(void)
{
    extern option_struct options;
    extern plugin_filenames_struct    plugin_filenames;
    
    // Parameters
    if (strcasecmp(plugin_filenames.irrigation.nc_filename, MISSING_S) == 0) {
        log_err("IRRIGATION = TRUE but file is missing");
    }
    if(!options.SHARE_LAYER_MOIST){
        log_err("IRRIGATION not yet implemented without SHARE_LAYER_MOIST");
    }
}

/******************************************
* @brief   Get irrigation parameters
******************************************/
bool
irr_get_parameters(char *cmdstr)
{
    extern plugin_parameters_struct   plugin_param;

    char                              optstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("Wfc_fract", optstr) == 0) {
        sscanf(cmdstr, "%*s %lf", &plugin_param.Wfc_fract);
    }
    else if (strcasecmp("Ksat_expt", optstr) == 0) {
        sscanf(cmdstr, "%*s %lf", &plugin_param.Ksat_expt);
    }
    else {
        return false;
    }

    return true;
}

/******************************************
* @brief   Validate irrigation parameters
******************************************/
void
irr_validate_parameters(void)
{
    extern plugin_parameters_struct    plugin_param;
    
    if (!(plugin_param.Wfc_fract >= 0 && plugin_param.Wfc_fract <= 1)) {
        log_err("Wfc_fract must be defined on the interval [0,1] (-)");
    }
    if (!(plugin_param.Ksat_expt >= 0)) {
        log_err("Ksat_expt must be defined on the interval [0,inf] (-)");
    }
}