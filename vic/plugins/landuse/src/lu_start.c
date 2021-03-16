/******************************************************************************
 * @section DESCRIPTION
 *
 * Land-use start functions
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
* @brief   Check input files
******************************************/
void
lu_start(void)
{
    extern option_struct           options;
    extern global_param_struct     global_param;
    extern plugin_filenames_struct plugin_filenames;

    int                            status;
    size_t                         NVEGTYPES;

    // Get information from the forcing file(s)
    // Open first-year forcing files and get info
    snprintf(plugin_filenames.forcing[FORCING_CV].nc_filename, MAXSTRING,
             "%s%4d.nc",
             plugin_filenames.f_path_pfx[FORCING_CV], global_param.startyear);
    status = nc_open(plugin_filenames.forcing[FORCING_CV].nc_filename,
                     NC_NOWRITE,
                     &(plugin_filenames.forcing[FORCING_CV].nc_id));
    check_nc_status(status, "Error opening %s",
                    plugin_filenames.forcing[FORCING_CV].nc_filename);

    NVEGTYPES = get_nc_dimension(&(plugin_filenames.forcing[FORCING_CV]),
                                 "veg_class");

    if (NVEGTYPES != options.NVEGTYPES) {
        log_err("NVEGTYPES in the land-use forcing file [%zu] "
                "is not equal to the parameter file [%zu]",
                NVEGTYPES,
                options.NVEGTYPES);
    }

    // Close first-year forcing files
    status = nc_close(plugin_filenames.forcing[FORCING_CV].nc_id);
    check_nc_status(status, "Error closing %s",
                    plugin_filenames.forcing[FORCING_CV].nc_filename);
}
