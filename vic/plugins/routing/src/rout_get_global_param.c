#include <vic_driver_image.h>
#include <plugin.h>

bool
rout_get_global_param(char *cmdstr)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_global_param_struct plugin_global_param;

    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("ROUT_STEPS_PER_DAY", optstr) == 0) {
        sscanf(cmdstr, "%*s %zu", &plugin_global_param.rout_steps_per_day);
    } else if (strcasecmp("ROUTING", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.ROUTING = str_to_bool(flgstr);
    } else if (strcasecmp("ROUTING_FORCE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        plugin_options.FORCE_ROUTING = str_to_bool(flgstr);
    }
    else if (strcasecmp("ROUTING_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.routing.nc_filename);
    }
    else if (strcasecmp("ROUTING_FORCING_FILE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", plugin_filenames.rf_path_pfx);
    }
    else {
        return false;
    }

    return true;
}

void
rout_validate_global_param(void)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_global_param_struct plugin_global_param;
    extern global_param_struct global_param;
    
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
                                 (double) plugin_global_param.rout_steps_per_day;
    }
    
    // Parameters
    if (strcasecmp(plugin_filenames.routing.nc_filename, MISSING_S) == 0) {
        log_err("ROUTING = TRUE but file is missing");
    }

    // Forcing
    if(plugin_options.FORCE_ROUTING){
        if (strcasecmp(plugin_filenames.rf_path_pfx, MISSING_S) == 0) {
            log_err("FORCE_ROUTING = TRUE but file is missing");
        }
    }
}
