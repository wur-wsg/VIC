#include <vic_driver_image.h>
#include <plugin.h>

void
rout_check_time(void)
{
    extern plugin_option_struct       plugin_options;
    extern plugin_filenames_struct    plugin_filenames;
    extern plugin_global_param_struct plugin_global_param;

    int                              *seconds;
    size_t                            i;

    size_t                            d1start[1];
    size_t                            d1count[1];

    d1start[0] = 0;
    d1count[0] = plugin_options.UH_LENGTH;

    seconds = malloc(plugin_options.UH_LENGTH * sizeof(*seconds));
    check_alloc_status(seconds, "Memory allocation error");

    // get time
    get_nc_field_int(&plugin_filenames.routing, "time",
                     d1start, d1count, seconds);

    for (i = 0; i < plugin_options.UH_LENGTH; i++) {
        if (seconds[i] != i * plugin_global_param.rout_dt) {
            log_err("ROUT_STEPS_PER_DAY and time in routing parameter "
                    "file do not match");
        }
    }

    free(seconds);
}

void
rout_start(void)
{
    extern plugin_option_struct    plugin_options;
    extern plugin_filenames_struct plugin_filenames;
    extern global_param_struct     global_param;

    int                            status;

    // Check domain & get dimensions
    status = nc_open(plugin_filenames.routing.nc_filename, NC_NOWRITE,
                     &(plugin_filenames.routing.nc_id));
    check_nc_status(status, "Error opening %s",
                    plugin_filenames.routing.nc_filename);

    plugin_options.UH_LENGTH = get_nc_dimension(&(plugin_filenames.routing),
                                                "time");
    compare_ncdomain_with_global_domain(&plugin_filenames.routing);
    rout_check_time();

    status = nc_close(plugin_filenames.routing.nc_id);
    check_nc_status(status, "Error closing %s",
                    plugin_filenames.routing.nc_filename);

    if (plugin_options.FORCE_ROUTING) {
        // Check the forcing
        snprintf(plugin_filenames.forcing[FORCING_DISCHARGE].nc_filename, MAXSTRING,
                 "%s%4d.nc",
                 plugin_filenames.f_path_pfx[FORCING_DISCHARGE], global_param.startyear);
        status = nc_open(plugin_filenames.forcing[FORCING_DISCHARGE].nc_filename,
                         NC_NOWRITE,
                         &(plugin_filenames.forcing[FORCING_DISCHARGE].nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.forcing[FORCING_DISCHARGE].nc_filename);

        plugin_get_forcing_file_info(&plugin_filenames.forcing[FORCING_DISCHARGE]);
        compare_ncdomain_with_global_domain(&plugin_filenames.forcing[FORCING_DISCHARGE]);

        status = nc_close(plugin_filenames.forcing[FORCING_DISCHARGE].nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.forcing[FORCING_DISCHARGE].nc_filename);
    }
}
