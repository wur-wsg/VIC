/******************************************************************************
 * @section DESCRIPTION
 *
 * Plugin get forcing file information (time, timestep and skip)
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
* @brief    Get and validate forcing file time and timestep
******************************************/
void
plugin_get_forcing_file_info(short unsigned int type)
{
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_filenames_struct    plugin_filenames;

    double                            nc_times[2];
    double                            nc_time_origin;
    size_t                            start = 0;
    size_t                            count = 2;
    char                             *nc_unit_chars = NULL;
    char                             *calendar_char = NULL;
    unsigned short int                time_units;
    unsigned short int                calendar;
    dmy_struct                        nc_origin_dmy;
    dmy_struct                        nc_start_dmy;
    size_t                            days_per_year;
    int                               status;
    int                               dim_id;
    size_t                            dim_len;

    // read time length from netcdf file
    status =
        nc_inq_dimid(plugin_filenames.forcing[type].nc_id, "time", &dim_id);
    check_nc_status(status, "Error getting dimension id for %s in %s",
                    "time", plugin_filenames.forcing[type].nc_filename);

    status = nc_inq_dimlen(plugin_filenames.forcing[type].nc_id, dim_id,
                           &dim_len);
    check_nc_status(status, "Error getting dimension length for %s in %s",
                    "time", plugin_filenames.forcing[type].nc_filename);

    if (dim_len == 1) {
        count = 1;
    }


    // read time info from netcdf file
    get_nc_field_double(&plugin_filenames.forcing[type], "time", &start, &count,
                        nc_times);
    get_nc_var_attr(&plugin_filenames.forcing[type], "time", "units",
                    &nc_unit_chars);
    get_nc_var_attr(&plugin_filenames.forcing[type], "time", "calendar",
                    &calendar_char);

    // parse the calendar string and check to make sure it matches the global clock
    calendar = str_to_calendar(calendar_char);

    // parse the time units
    parse_nc_time_units(nc_unit_chars, &time_units, &nc_origin_dmy);

    // Get date/time of the first entry in the forcing file.
    nc_time_origin =
        date2num(0., &nc_origin_dmy, 0., calendar, TIME_UNITS_DAYS);
    num2date(nc_time_origin, nc_times[0], 0., calendar, time_units,
             &nc_start_dmy);

    // Assign file start date/time
    plugin_global_param.forceyear[type] = nc_start_dmy.year;
    plugin_global_param.forcemonth[type] = nc_start_dmy.month;
    plugin_global_param.forceday[type] = nc_start_dmy.day;
    plugin_global_param.forcesec[type] = nc_start_dmy.dayseconds;
    days_per_year = DAYS_PER_YEAR +
                    leap_year(plugin_global_param.forceyear[type], calendar);

    if (plugin_global_param.forceyear[type] != global_param.forceyear[0] ||
        plugin_global_param.forcemonth[type] != global_param.forcemonth[0] ||
        plugin_global_param.forceday[type] != global_param.forceday[0] ||
        plugin_global_param.forcesec[type] != global_param.forcesec[0]) {
        log_err("Plugin forcing file time must match the forcing file time.  "
                "Forcing file time is set to %04hu-%02hu-%02hu : %hu "
                "[year-month-day : seconds] and the plugin forcing "
                "file time is set to  %04hu-%02hu-%02hu : %hu "
                "[year-month-day : seconds]",
                global_param.forceyear[0],
                global_param.forcemonth[0],
                global_param.forceday[0],
                global_param.forcesec[0],
                plugin_global_param.forceyear[type],
                plugin_global_param.forcemonth[type],
                plugin_global_param.forceday[type],
                plugin_global_param.forcesec[type]);
    }

    // calculate timestep in forcing file
    if (dim_len == 1) {
        plugin_global_param.force_steps_per_year[type] =
            (size_t) 1;
    }
    else if (time_units == TIME_UNITS_DAYS) {
        plugin_global_param.force_steps_per_year[type] =
            (size_t) nearbyint(days_per_year /
                               (nc_times[1] - nc_times[0]));
    }
    else if (time_units == TIME_UNITS_HOURS) {
        plugin_global_param.force_steps_per_year[type] =
            (size_t) nearbyint((days_per_year * HOURS_PER_DAY) /
                               (nc_times[1] - nc_times[0]));
    }
    else if (time_units == TIME_UNITS_MINUTES) {
        plugin_global_param.force_steps_per_year[type] =
            (size_t) nearbyint((days_per_year * MIN_PER_DAY) /
                               (nc_times[1] - nc_times[0]));
    }
    else if (time_units == TIME_UNITS_SECONDS) {
        plugin_global_param.force_steps_per_year[type] =
            (size_t) nearbyint((days_per_year * SEC_PER_DAY) /
                               (nc_times[1] - nc_times[0]));
    }

    plugin_global_param.force_dt[type] = (SEC_PER_DAY * days_per_year) /
                                         (double) plugin_global_param.
                                         force_steps_per_year[type];

    // check that this forcing file will work
    if (plugin_global_param.forcefreq[type] == FORCE_STEP) {
        if (plugin_global_param.force_steps_per_year[type] !=
            global_param.model_steps_per_day * days_per_year) {
            log_err(
                "Plugin forcing file timestep must match the force model timestep.  "
                "Force model timesteps per year is set to %zu and the plugin forcing "
                "file timestep is set to %zu (frequency = FORCE_STEP)",
                global_param.model_steps_per_day * days_per_year,
                plugin_global_param.force_steps_per_year[type]);
        }
    }
    else if (plugin_global_param.forcefreq[type] == FORCE_DAY) {
        if (plugin_global_param.force_steps_per_year[type] !=
            days_per_year) {
            log_err(
                "Plugin forcing file timestep must match the force model timestep.  "
                "Force model timesteps per year is set to %zu and the plugin forcing "
                "file timestep is set to %zu (frequency = FORCE_DAY)",
                days_per_year,
                plugin_global_param.force_steps_per_year[type]);
        }
    }
    else if (plugin_global_param.forcefreq[type] == FORCE_MONTH) {
        if (plugin_global_param.force_steps_per_year[type] !=
            MONTHS_PER_YEAR) {
            log_err(
                "Plugin forcing file timestep must match the force model timestep.  "
                "Force model timesteps per year is set to %d and the plugin forcing "
                "file timestep is set to %zu (frequency = FORCE_MONTH)",
                MONTHS_PER_YEAR,
                plugin_global_param.force_steps_per_year[type]);
        }
    }
    else if (plugin_global_param.forcefreq[type] == FORCE_YEAR) {
        if (plugin_global_param.force_steps_per_year[type] != 1) {
            log_err(
                "Plugin forcing file timestep must match the force model timestep.  "
                "Force model timesteps per year is set to %d and the plugin forcing "
                "file timestep is set to %zu (frequency = FORCE_YEAR)",
                1,
                plugin_global_param.force_steps_per_year[type]);
        }
    }

    if (calendar != global_param.calendar) {
        log_err("Calendar in water use forcing file (%s) "
                "does not match the calendar of "
                "VIC's clock", calendar_char);
    }

    // Free attribute character arrays
    free(nc_unit_chars);
    free(calendar_char);
}

/******************************************
* @brief    Get forcing file skip (based on simulation start)
******************************************/
void
plugin_get_forcing_file_skip(short unsigned int type)
{
    extern global_param_struct        global_param;
    extern plugin_global_param_struct plugin_global_param;

    dmy_struct                        start_dmy, force_dmy;
    double                            start_num, force_num;
    size_t                            days_per_year;

    start_dmy.dayseconds = global_param.startsec;
    start_dmy.year = global_param.startyear;
    start_dmy.day = global_param.startday;
    start_dmy.month = global_param.startmonth;

    start_num = date2num(global_param.time_origin_num, &start_dmy, 0.,
                         global_param.calendar, global_param.time_units);

    /** Determine number of forcing records to skip before model start time **/
    force_dmy.dayseconds = plugin_global_param.forcesec[type];
    force_dmy.year = plugin_global_param.forceyear[type];
    force_dmy.day = plugin_global_param.forceday[type];
    force_dmy.month = plugin_global_param.forcemonth[type];
    days_per_year = DAYS_PER_YEAR +
                    leap_year(plugin_global_param.forceyear[type],
                              global_param.calendar);

    force_num = date2num(global_param.time_origin_num, &force_dmy, 0.,
                         global_param.calendar, global_param.time_units);

    if (plugin_global_param.forcefreq[type] == FORCE_STEP || 
            plugin_global_param.forcefreq[type] == FORCE_DAY) {
        plugin_global_param.forceskip[type] =
            (unsigned int) round((start_num - force_num) *
                                 ((double)plugin_global_param.force_steps_per_year[type] /
                                  (double)days_per_year));
    } else if (plugin_global_param.forcefreq[type] == FORCE_MONTH) {
        plugin_global_param.forceskip[type] = start_dmy.month - force_dmy.month;
    } else if (plugin_global_param.forcefreq[type] == FORCE_YEAR) {
        plugin_global_param.forceskip[type] = 0;
    } else {
        log_err("Forcing timestep not implemented for forceskip calculation");
    }
}
