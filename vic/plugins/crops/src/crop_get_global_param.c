#include <vic_driver_image.h>
#include <plugin.h>

bool
crop_get_global_param(char *cmdstr)
{
    extern plugin_option_struct       plugin_options;
    extern plugin_filenames_struct    plugin_filenames;

    char                              optstr[MAXSTRING];
    char                              flgstr[MAXSTRING];

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
    else {
        return false;
    }

    return true;
}

void
crop_validate_global_param(void)
{
    extern plugin_global_param_struct plugin_global_param;
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
        log_err("WOFOST = TRUE but netcdf file is missing");
    }
    if (strcasecmp(plugin_filenames.wofost_text, MISSING_S) == 0) {
        log_err("WOFOST = TRUE but text file is missing");
    }
     // Forcing
    if (strcasecmp(plugin_filenames.f_path_pfx[FORCING_CO2], MISSING_S) == 0) {
        log_err("WOFOST = TRUE but forcing file is missing");
    }
}

bool
crop_get_parameters(char *cmdstr)
{
    extern plugin_parameters_struct   plugin_param;

    char                              optstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);
    
    UNUSED(plugin_param);

    return true;
}

void
crop_validate_parameters(void)
{
    extern plugin_parameters_struct    plugin_param;
    
    UNUSED(plugin_param);
}