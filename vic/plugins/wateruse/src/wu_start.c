/******************************************************************************
 * @section DESCRIPTION
 *
 * Water-use start functions
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
wu_start(void)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;

    int                            status;

    if (plugin_options.REMOTE_WITH) {
        // Check domain & get dimensions
        status = nc_open(plugin_filenames.wateruse.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.wateruse.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.wateruse.nc_filename);

        plugin_options.NWURECEIVING =
            get_nc_dimension(&(plugin_filenames.wateruse),
                             "wu_receiving");

        compare_ncdomain_with_global_domain(&plugin_filenames.wateruse);

        status = nc_close(plugin_filenames.wateruse.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.wateruse.nc_filename);
    }
    else {
        plugin_options.NWURECEIVING = 0;
    }
}
