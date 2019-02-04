#include <vic_driver_image.h>

void 
plugin_get_forcing_file_info(nameid_struct *ncforcing)
{
    extern global_param_struct global_param;

    double                     nc_times[2];
    double                     nc_time_origin;
    size_t                     start = 0;
    size_t                     count = 2;
    char                      *nc_unit_chars = NULL;
    char                      *calendar_char = NULL;
    unsigned short int         time_units;
    unsigned short int         calendar;
    dmy_struct                 nc_origin_dmy;
    dmy_struct                 nc_start_dmy;
    size_t                     force_step_per_day;
    unsigned short int         forceyear;
    unsigned short int         forcemonth;
    unsigned short int         forceday;
    unsigned short int         forcesecond;
    
    // read time info from netcdf file
    get_nc_field_double(ncforcing, "time", &start, &count, nc_times);
    get_nc_var_attr(ncforcing, "time", "units", &nc_unit_chars);
    get_nc_var_attr(ncforcing, "time", "calendar", &calendar_char);

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
    forceyear = nc_start_dmy.year;
    forcemonth = nc_start_dmy.month;
    forceday = nc_start_dmy.day;
    forcesecond = nc_start_dmy.dayseconds;

    if(forceyear != global_param.forceyear[0] ||
            forcemonth != global_param.forcemonth[0] ||
            forceday != global_param.forceday[0] ||
            forcesecond != global_param.forcesec[0]){
        log_err("Plugin forcing file time must match the forcing file time.  "
                "Forcing file time is set to %04hu-%02hu-%02hu : %hu "
                "[year-month-day : seconds] and the plugin forcing "
                "file time is set to  %04hu-%02hu-%02hu : %hu "
                "[year-month-day : seconds]",
                global_param.forceyear[0],
                global_param.forcemonth[0],
                global_param.forceday[0],
                global_param.forcesec[0],
                forceyear,
                forcemonth,
                forceday,
                forcesecond);
    }

    // calculate timestep in forcing file
    if (time_units == TIME_UNITS_DAYS) {
        force_step_per_day =
            (size_t) nearbyint(1. / (nc_times[1] - nc_times[0]));
    }
    else if (time_units == TIME_UNITS_HOURS) {
       force_step_per_day =
            (size_t) nearbyint(HOURS_PER_DAY / (nc_times[1] - nc_times[0]));
    }
    else if (time_units == TIME_UNITS_MINUTES) {
        force_step_per_day =
            (size_t) nearbyint(MIN_PER_DAY / (nc_times[1] - nc_times[0]));
    }
    else if (time_units == TIME_UNITS_SECONDS) {
        force_step_per_day =
            (size_t) nearbyint(SEC_PER_DAY / (nc_times[1] - nc_times[0]));
    }

    // check that this forcing file will work
    if (force_step_per_day !=
        global_param.snow_steps_per_day) {
        log_err("Plugin forcing file timestep must match the force model timestep.  "
                "Force model timesteps per day is set to %zu and the plugin forcing "
                "file timestep is set to %zu",
                global_param.snow_steps_per_day,
                force_step_per_day);
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
