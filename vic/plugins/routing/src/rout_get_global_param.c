/******************************************************************************
 * @section DESCRIPTION
 *
 * Routing configuration functions
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
* @brief   Get routing configuration options
******************************************/
bool
rout_get_global_param(char *cmdstr)
{
    extern plugin_option_struct       plugin_options;
    extern plugin_filenames_struct    plugin_filenames;
    extern plugin_global_param_struct plugin_global_param;

    char                              optstr[MAXSTRING];
    char                              flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("ROUT_STEPS_PER_DAY", optstr) == 0) {
        sscanf(cmdstr, "%*s %zu", &plugin_global_param.rout_steps_per_day);
    }
    else if (strcasecmp("DECOMPOSITION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        if (strcasecmp("RANDOM", flgstr) == 0) {
            plugin_options.DECOMPOSITION = RANDOM_DECOMPOSITION;
        }
        else if (strcasecmp("BASIN", flgstr) == 0) {
            plugin_options.DECOMPOSITION = BASIN_DECOMPOSITION;
        }
        else if (strcasecmp("FILE", flgstr) == 0) {
            plugin_options.DECOMPOSITION = FILE_DECOMPOSITION;
        }
    }
    else if (strcasecmp("ROUTING", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.ROUTING = str_to_bool(flgstr);
    }
    else if (strcasecmp("ROUTING_FORCE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.FORCE_ROUTING = str_to_bool(flgstr);
    }
    else if (strcasecmp("ROUTING_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.routing.nc_filename);
    }
    else if (strcasecmp("DECOMPOSITION_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.decomposition.nc_filename);
    }
    else {
        return false;
    }

    return true;
}

/******************************************
* @brief   Validate routing configuration options
******************************************/
void
rout_validate_global_param(void)
{
    extern plugin_option_struct       plugin_options;
    extern plugin_filenames_struct    plugin_filenames;
    extern plugin_global_param_struct plugin_global_param;
    extern global_param_struct        global_param;

    // Validate routing time step
    if (plugin_global_param.rout_steps_per_day == 0) {
        log_err("Routing time steps per day has not been defined.  Make sure "
                "that the global file defines ROUT_STEPS_PER_DAY.");
    }
    else if (plugin_global_param.rout_steps_per_day <
             MIN_SUBDAILY_STEPS_PER_DAY) {
        log_err("The specified number of routing steps per day (%zu) < "
                "the minimum number of subdaily steps per day (%d).  Make "
                "sure that the global file defines RUNOFF_STEPS_PER_DAY of at "
                "least (%d).", plugin_global_param.rout_steps_per_day,
                MIN_SUBDAILY_STEPS_PER_DAY,
                MIN_SUBDAILY_STEPS_PER_DAY);
    }
    else if (plugin_global_param.rout_steps_per_day > HOURS_PER_DAY &&
             plugin_global_param.rout_steps_per_day % HOURS_PER_DAY != 0) {
        log_err("The specified number of routing steps per day (%zu) is > "
                "24 and is not evenly divided by 24.",
                plugin_global_param.rout_steps_per_day);
    }
    else if (plugin_global_param.rout_steps_per_day >
             MAX_SUBDAILY_STEPS_PER_DAY) {
        log_err("The specified number of routing steps per day (%zu) > the "
                "the maximum number of subdaily steps per day (%d).  Make "
                "sure that the global file defines RUNOFF_STEPS_PER_DAY of at "
                "most (%d).", plugin_global_param.rout_steps_per_day,
                MAX_SUBDAILY_STEPS_PER_DAY,
                MAX_SUBDAILY_STEPS_PER_DAY);
    }
    else if (plugin_global_param.rout_steps_per_day %
             global_param.model_steps_per_day != 0) {
        log_err("The specified number of routing timesteps (%zu) must be "
                "evenly divisible by the number of model timesteps per day "
                "(%zu)", plugin_global_param.rout_steps_per_day,
                global_param.model_steps_per_day);
    }
    else {
        plugin_global_param.rout_dt = SEC_PER_DAY /
                                      (double) plugin_global_param.
                                      rout_steps_per_day;
    }

    // Parameters
    if (strcasecmp(plugin_filenames.routing.nc_filename, MISSING_S) == 0) {
        log_err("ROUTING = TRUE but file is missing");
    }

    // Forcing
    if (plugin_options.FORCE_ROUTING) {
        if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_DISCHARGE],
                       MISSING_S) == 0) {
            log_err("FORCE_ROUTING = TRUE but file is missing");
        }
    }

    // Decomposition
    if (plugin_options.DECOMPOSITION == FILE_DECOMPOSITION) {
        if (strcasecmp(plugin_filenames.decomposition.nc_filename,
                       MISSING_S) == 0) {
            log_err("DECOMPOSITION = FILE but file is missing");
        }
    }
}
