#include <vic_driver_image.h>
#include <plugin.h>

bool
crop_get_global_param(char *cmdstr)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;

    char                           optstr[MAXSTRING];
    char                           flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("WOFOST_STEPS_PER_DAY", optstr) == 0) {
        sscanf(cmdstr, "%*s %zu", &plugin_global_param.wofost_steps_per_day);
    }
    else if (strcasecmp("WOFOST", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WOFOST = str_to_bool(flgstr);
    }
    else if (strcasecmp("CROP_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.crop.nc_filename);
    }
    else if (strcasecmp("WOFOST_TEXT_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.wofost_text);
    }
    else if (strcasecmp("WOFOST_POTENTIAL_IRRIGATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WOFOST_PIRR = str_to_bool(flgstr);
    }
    else if (strcasecmp("WOFOST_POTENTIAL_FERTILIZER", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WOFOST_PFERT = str_to_bool(flgstr);
    }
    else if (strcasecmp("WOFOST_DIST_SEASON", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WOFOST_DIST_SEASON = str_to_bool(flgstr);
    }
    else if (strcasecmp("WOFOST_DIST_TSUM", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WOFOST_DIST_TSUM = str_to_bool(flgstr);
    }
    else if (strcasecmp("WOFOST_FORCE_TSUM", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WOFOST_FORCE_TSUM = str_to_bool(flgstr);
    }
    else if (strcasecmp("WOFOST_DIST_FERTILIZER", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WOFOST_DIST_FERT = str_to_bool(flgstr);
    }
    else if (strcasecmp("WOFOST_DIST_MINERALIZATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WOFOST_DIST_MIN = str_to_bool(flgstr);
    }
    else if (strcasecmp("WOFOST_CALC_MINERALIZATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WOFOST_CALC_MIN = str_to_bool(flgstr);
    }
    else if (strcasecmp("WOFOST_CONTINUE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WOFOST_CONTINUE = str_to_bool(flgstr);
    }
    else if (strcasecmp("WOFOST_FORCE_TSUM", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WOFOST_FORCE_TSUM = str_to_bool(flgstr);
    }
    else if (strcasecmp("WOFOST_FORCE_FERTILIZER", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.WOFOST_FORCE_FERT = str_to_bool(flgstr);
    }
    else {
        return false;
    }

    return true;
}

void
crop_validate_global_param(void)
{
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct       plugin_options;
    extern plugin_filenames_struct    plugin_filenames;

    // Validate wofost time step
    if (plugin_global_param.wofost_steps_per_day == 0) {
        log_err("Wofost time steps per day has not been defined.  Make sure "
                "that the global file defines WOFOST_STEPS_PER_DAY.");
    }
    else if (plugin_global_param.wofost_steps_per_day != 1) {
        log_err("The specified number of wofost steps per day (%zu) != "
                "the number of subdaily steps per day required (%d).  Make "
                "sure that the global file defines WOFOST_STEPS_PER_DAY of (%d).",
                plugin_global_param.wofost_steps_per_day, 1, 1);
    }
    else {
        plugin_global_param.wofost_dt = SEC_PER_DAY /
                                        (double) plugin_global_param.
                                        wofost_steps_per_day;
    }

    // Parameters
    if (strcasecmp(plugin_filenames.crop.nc_filename, MISSING_S) == 0) {
        log_err("WOFOST = TRUE but crop netcdf file is missing");
    }
    if (strcasecmp(plugin_filenames.wofost_text, MISSING_S) == 0) {
        log_err("WOFOST = TRUE but text file is missing");
    }
    // Forcing
    if (!plugin_options.FORCE_CO2) {
        log_err("WOFOST = TRUE but FORCE_CO2 = FALSE");
    }
    if (plugin_options.WOFOST_FORCE_TSUM) {
        if (!plugin_options.WOFOST_DIST_TSUM) {
            log_err("WOFOST_FORCE_TSUM = TRUE but WOFOST_DIST_TSUM = FALSE");
        }
        if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_TSUM_1],
                       MISSING_S) == 0) {
            log_err("WOFOST_FORCE_TSUM = TRUE but tsum1 forcing file is missing");
        }
        if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_TSUM_2],
                       MISSING_S) == 0) {
            log_err("WOFOST_FORCE_TSUM = TRUE but tsum2 forcing file is missing");
        }
    }
    if (plugin_options.WOFOST_FORCE_FERT) {
        if (!plugin_options.WOFOST_DIST_FERT) {
            log_err("WOFOST_FORCE_FERT = TRUE but WOFOST_DIST_FERT = FALSE");
        }
        if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_FERT_DVS],
                       MISSING_S) == 0) {
            log_err("WOFOST_FORCE_FERT = TRUE but DVS forcing file is missing");
        }
        if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_FERT_N],
                       MISSING_S) == 0) {
            log_err("WOFOST_FORCE_FERT = TRUE but N forcing file is missing");
        }
        if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_FERT_P],
                       MISSING_S) == 0) {
            log_err("WOFOST_FORCE_FERT = TRUE but P forcing file is missing");
        }
        if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_FERT_K],
                       MISSING_S) == 0) {
            log_err("WOFOST_FORCE_FERT = TRUE but K forcing file is missing");
        }
    }
    if (plugin_options.WOFOST_CALC_MIN) {
        if (!plugin_options.WOFOST_DIST_MIN) {
            log_err("WOFOST_CALC_MIN = TRUE but WOFOST_DIST_MIN = FALSE");
        }
    }
}
