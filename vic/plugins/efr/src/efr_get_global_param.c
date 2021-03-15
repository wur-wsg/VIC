/******************************************************************************
 * @section DESCRIPTION
 *
 * Environmental requirement configuration functions
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
* @brief   Get environmental requirement configuration options
******************************************/
bool
efr_get_global_param(char *cmdstr)
{
    extern plugin_option_struct plugin_options;

    char                        optstr[MAXSTRING];
    char                        flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("EFR", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.EFR = str_to_bool(flgstr);
    }
    else {
        return false;
    }

    return true;
}

/******************************************
* @brief   Validate environmental requirment configuration options
******************************************/
void
efr_validate_global_param(void)
{
    extern plugin_filenames_struct plugin_filenames;

    // Forcing
    if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_EFR_DISCHARGE],
                   MISSING_S) == 0 ||
        strcasecmp(plugin_filenames.f_path_pfx[FORCING_EFR_BASEFLOW],
                   MISSING_S) == 0) {
        log_warn(
            "EFR = TRUE but some forcing files are missing, is this intentional?");
    }
}
