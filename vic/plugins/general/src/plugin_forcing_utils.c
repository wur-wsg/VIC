/******************************************************************************
 * @section DESCRIPTION
 *
 * Functions used for plugin forcing
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
* @brief   Get forcing types from configuration file
******************************************/
bool
plugin_force_get_global_param(char *cmdstr)
{
    char optstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("PLUGIN_FORCE_TYPE", optstr) == 0) {
        plugin_set_force_type(cmdstr);
        return true;
    }

    return false;
}

/******************************************
* @brief   Validate forcing types from configuration file
******************************************/
void
plugin_force_validate_global_param(void)
{
    extern global_param_struct     global_param;
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;

    int                            status;

    size_t                         i;

    for (i = 0; i < PLUGIN_N_FORCING_TYPES; i++) {
        // Validate forcing files and variables
        if (strcmp(plugin_filenames.f_path_pfx[i], MISSING_S) == 0) {
            continue;
        }

        // Get information from the forcing file(s)
        // Open first-year forcing files and get info
        snprintf(plugin_filenames.forcing[i].nc_filename, MAXSTRING, "%s%4d.nc",
                 plugin_filenames.f_path_pfx[i], global_param.startyear);
        status = nc_open(plugin_filenames.forcing[i].nc_filename, NC_NOWRITE,
                         &(plugin_filenames.forcing[i].nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.forcing[i].nc_filename);

        plugin_get_forcing_file_info(i);

        // Close first-year forcing files
        status = nc_close(plugin_filenames.forcing[i].nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.forcing[i].nc_filename);
    }

    if (strcmp(plugin_filenames.f_path_pfx[FORCING_CO2], MISSING_S) != 0) {
        plugin_options.FORCE_CO2 = true;
    }
}

/******************************************
* @brief   Check plugin forcing skips
******************************************/
void
plugin_force_start(void)
{
    extern plugin_filenames_struct plugin_filenames;

    size_t                         i;

    for (i = 0; i < PLUGIN_N_FORCING_TYPES; i++) {
        // Validate forcing files and variables
        if (strcmp(plugin_filenames.f_path_pfx[i], MISSING_S) == 0) {
            continue;
        }

        plugin_get_forcing_file_skip(i);
    }
}

/******************************************
* @brief   Check plugin forcing files
******************************************/
void
plugin_force_init(void)
{
    extern global_param_struct     global_param;
    extern plugin_filenames_struct plugin_filenames;
    extern int                     mpi_rank;

    int                            status;

    size_t                         i;

    for (i = 0; i < PLUGIN_N_FORCING_TYPES; i++) {
        // Validate forcing files and variables
        if (strcmp(plugin_filenames.f_path_pfx[i], MISSING_S) == 0) {
            continue;
        }

        if (mpi_rank == VIC_MPI_ROOT) {
            // Get information from the forcing file(s)
            // Open first-year forcing files and get info
            snprintf(plugin_filenames.forcing[i].nc_filename, MAXSTRING,
                     "%s%4d.nc",
                     plugin_filenames.f_path_pfx[i], global_param.startyear);
            status = nc_open(plugin_filenames.forcing[i].nc_filename,
                             NC_NOWRITE,
                             &(plugin_filenames.forcing[i].nc_id));
            check_nc_status(status, "Error opening %s",
                            plugin_filenames.forcing[i].nc_filename);

            compare_ncdomain_with_global_domain(&plugin_filenames.forcing[i]);

            // Close first-year forcing files
            status = nc_close(plugin_filenames.forcing[i].nc_id);
            check_nc_status(status, "Error closing %s",
                            plugin_filenames.forcing[i].nc_filename);
        }
    }
}

/******************************************
* @brief   Open plugin forcing files
******************************************/
void
plugin_start_forcing(void)
{
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern size_t                     current;
    extern dmy_struct                *dmy;
    extern plugin_filenames_struct    plugin_filenames;
    extern int                        mpi_rank;

    int                               status;
    size_t                            f;

    for (f = 0; f < PLUGIN_N_FORCING_TYPES; f++) {
        if (strcasecmp(plugin_filenames.f_path_pfx[f], MISSING_S) == 0) {
            continue;
        }

        // Open forcing file if it is the first time step
        if (current == 0) {
            if (mpi_rank == VIC_MPI_ROOT) {
                // open new forcing file
                sprintf(plugin_filenames.forcing[f].nc_filename, "%s%4d.nc",
                        plugin_filenames.f_path_pfx[f], dmy[current].year);
                status =
                    nc_open(plugin_filenames.forcing[f].nc_filename, NC_NOWRITE,
                            &(plugin_filenames.forcing[f].nc_id));
                check_nc_status(status, "Error opening %s",
                                plugin_filenames.forcing[f].nc_filename);
            }
            plugin_global_param.forcerun[f] = true;
        }
        else {
            // Open forcing file if it is a new year
            if (dmy[current].year != dmy[current - 1].year) {
                // global_param.forceoffset resets every year since the met file restarts
                // every year
                // global_param.forceoffset should reset to 0 after the first year
                // global_param.forceskip should also reset to 0 after the first year
                plugin_global_param.forceoffset[f] = 0;
                plugin_global_param.forceskip[f] = 0;

                if (mpi_rank == VIC_MPI_ROOT) {
                    // close previous forcing file
                    status = nc_close(plugin_filenames.forcing[f].nc_id);
                    check_nc_status(status, "Error closing %s",
                                    plugin_filenames.forcing[f].nc_filename);

                    // open new forcing file
                    sprintf(plugin_filenames.forcing[f].nc_filename, "%s%4d.nc",
                            plugin_filenames.f_path_pfx[f], dmy[current].year);
                    status =
                        nc_open(plugin_filenames.forcing[f].nc_filename,
                                NC_NOWRITE,
                                &(plugin_filenames.forcing[f].nc_id));
                    check_nc_status(status, "Error opening %s",
                                    plugin_filenames.forcing[f].nc_filename);
                }
            }
            else {
                // global_param.forceoffset should increase during the year
                if (plugin_global_param.forcefreq[f] == FORCE_STEP) {
                    plugin_global_param.forceoffset[f] += 1;
                }
                else if (plugin_global_param.forcefreq[f] == FORCE_DAY) {
                    if (dmy[current].day != dmy[current - 1].day) {
                        plugin_global_param.forceoffset[f] += 1;
                    }
                }
                else if (plugin_global_param.forcefreq[f] == FORCE_MONTH) {
                    if (dmy[current].month != dmy[current - 1].month) {
                        plugin_global_param.forceoffset[f] += 1;
                    }
                }
                else if (plugin_global_param.forcefreq[f] == FORCE_YEAR) {
                    if (dmy[current].year != dmy[current - 1].year) {
                        plugin_global_param.forceoffset[f] += 1;
                    }
                }
            }

            // Update the run flag
            plugin_global_param.forcerun[f] = false;
            if (plugin_global_param.forcefreq[f] == FORCE_STEP) {
                plugin_global_param.forcerun[f] = true;
            }
            else if (plugin_global_param.forcefreq[f] == FORCE_DAY) {
                if (dmy[current].day != dmy[current - 1].day) {
                    plugin_global_param.forcerun[f] = true;
                }
            }
            else if (plugin_global_param.forcefreq[f] == FORCE_MONTH) {
                if (dmy[current].month != dmy[current - 1].month) {
                    plugin_global_param.forcerun[f] = true;
                }
            }
            else if (plugin_global_param.forcefreq[f] == FORCE_YEAR) {
                if (dmy[current].year != dmy[current - 1].year) {
                    plugin_global_param.forcerun[f] = true;
                }
            }
        }
    }
}

/******************************************
* @brief   Close plugin forcing files (and update counter)
******************************************/
void
plugin_end_forcing(void)
{
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern size_t                     current;
    extern dmy_struct                *dmy;
    extern plugin_filenames_struct    plugin_filenames;
    extern int                        mpi_rank;

    int                               status;
    size_t                            f;

    for (f = 0; f < PLUGIN_N_FORCING_TYPES; f++) {
        if (strcasecmp(plugin_filenames.f_path_pfx[f], MISSING_S) == 0) {
            continue;
        }

        // Close forcing file if it is the last time step
        if (current == global_param.nrecs - 1) {
            if (mpi_rank == VIC_MPI_ROOT) {
                // close previous forcing file
                status = nc_close(plugin_filenames.forcing[f].nc_id);
                check_nc_status(status, "Error closing %s",
                                plugin_filenames.forcing[f].nc_filename);
            }
        }
    }
}
