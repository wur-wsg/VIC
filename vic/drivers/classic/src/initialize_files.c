/******************************************************************************
 * @section DESCRIPTION
 *
 * This subroutine initalizes all filefilenames before they are called by
 * the model.
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

#include <vic_driver_classic.h>

/******************************************************************************
 * @brief    Initialize all filenames before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_filenames()
{
    extern filenames_struct filenames;

    size_t                  i;

    snprintf(filenames.init_state, MAXSTRING, "%s", "MISSING");
    snprintf(filenames.statefile, MAXSTRING, "%s", "MISSING");
    snprintf(filenames.constants, MAXSTRING, "%s", "MISSING");
    snprintf(filenames.soil, MAXSTRING, "%s", "MISSING");
    snprintf(filenames.veg, MAXSTRING, "%s", "MISSING");
    snprintf(filenames.veglib, MAXSTRING, "%s", "MISSING");
    snprintf(filenames.snowband, MAXSTRING, "%s", "MISSING");
    snprintf(filenames.lakeparam, MAXSTRING, "%s", "MISSING");
    snprintf(filenames.result_dir, MAXSTRING, "%s", "MISSING");
    snprintf(filenames.log_path, MAXSTRING, "%s", "MISSING");
    for (i = 0; i < 2; i++) {
        snprintf(filenames.f_path_pfx[i], MAXSTRING, "%s", "MISSING");
    }
}

/******************************************************************************
 * @brief    Initialize all file pointers
 *****************************************************************************/
void
initialize_fileps()
{
    extern filep_struct filep;

    size_t              i;

    filep.globalparam = NULL;
    filep.constants = NULL;
    filep.init_state = NULL;
    filep.lakeparam = NULL;
    filep.snowband = NULL;
    filep.soilparam = NULL;
    filep.statefile = NULL;
    filep.veglib = NULL;
    filep.vegparam = NULL;
    filep.logfile = NULL;
    for (i = 0; i < 2; i++) {
        filep.forcing[i] = NULL;
    }
}
