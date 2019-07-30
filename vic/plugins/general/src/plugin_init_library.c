/******************************************************************************
 * @section DESCRIPTION
 *
 * Functions used to initialize plugin structures
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
* @brief   Initialize options
******************************************/
void
plugin_initialize_options(plugin_option_struct *plugin_options)
{
    plugin_options->DECOMPOSITION = RANDOM_DECOMPOSITION;
    plugin_options->ROUTING = false;
    plugin_options->UH_LENGTH = 0;
    plugin_options->FORCE_ROUTING = false;
}

/******************************************
* @brief   Initialize global params
******************************************/
void
plugin_initialize_global(plugin_global_param_struct *plugin_global_param)
{
    size_t i;

    plugin_global_param->rout_steps_per_day = 0;
    plugin_global_param->rout_dt = 0;
    for (i = 0; i < PLUGIN_N_FORCING_TYPES; i++) {
        plugin_global_param->force_steps_per_year[i] = 0;
        plugin_global_param->force_dt[i] = 0;
        plugin_global_param->forceyear[i] = 0;
        plugin_global_param->forcemonth[i] = 1;
        plugin_global_param->forceday[i] = 1;
        plugin_global_param->forcesec[i] = 0;
        plugin_global_param->forceskip[i] = 0;
        plugin_global_param->forcerun[i] = true;
        plugin_global_param->forceoffset[i] = 0;
        plugin_global_param->forcefreq[i] = 0;
    }
}

/******************************************
* @brief   Initialize parameters
******************************************/
void
plugin_initialize_parameters(plugin_parameters_struct *plugin_param)
{
    /* Unused variables */
    UNUSED(plugin_param);
}

/******************************************
* @brief   Initialize filenames
******************************************/
void
plugin_initialize_filenames(plugin_filenames_struct *plugin_filenames)
{
    size_t i;

    snprintf(plugin_filenames->routing.nc_filename, MAXSTRING, "%s", MISSING_S);
    snprintf(plugin_filenames->decomposition.nc_filename, MAXSTRING, "%s",
             MISSING_S);

    for (i = 0; i < PLUGIN_N_FORCING_TYPES; i++) {
        snprintf(plugin_filenames->forcing[i].nc_filename, MAXSTRING, "%s",
                 MISSING_S);
        snprintf(plugin_filenames->f_path_pfx[i], MAXSTRING, "%s", MISSING_S);
        snprintf(plugin_filenames->f_varname[i], MAXSTRING, "%s", MISSING_S);
    }
}

/******************************************
* @brief   Initialize plugin structures
******************************************/
void
plugin_initialize_global_structures(void)
{
    extern plugin_option_struct       plugin_options;
    extern plugin_filenames_struct    plugin_filenames;
    extern plugin_parameters_struct   plugin_param;
    extern plugin_global_param_struct plugin_global_param;

    plugin_initialize_options(&plugin_options);
    plugin_initialize_global(&plugin_global_param);
    plugin_initialize_parameters(&plugin_param);
    plugin_initialize_filenames(&plugin_filenames);
}
