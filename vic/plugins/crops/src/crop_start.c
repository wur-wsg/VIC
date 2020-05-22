#include <vic_driver_image.h>
#include <plugin.h>

/******************************************************************************
 * @brief    Read the number of crop type per grid cell from file
 *****************************************************************************/
void
add_ncrop_to_global_domain(nameid_struct *nc_nameid,
                           domain_struct *global_domain)
{
    size_t d2count[2];
    size_t d2start[2];
    size_t i;
    int   *ivar = NULL;

    ivar = malloc(global_domain->ncells_total * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain->n_ny;
    d2count[1] = global_domain->n_nx;
    get_nc_field_int(nc_nameid, "Ncrop", d2start, d2count, ivar);

    for (i = 0; i < global_domain->ncells_total; i++) {
        global_domain->locations[i].ncrop = (size_t) ivar[i];
    }

    free(ivar);
}

void
crop_start(void)
{
    extern plugin_option_struct    plugin_options;
    extern global_param_struct     global_param;
    extern domain_struct           global_domain;
    extern plugin_filenames_struct plugin_filenames;

    size_t                         fert_idx[4] = {FORCING_FERT_DVS, FORCING_FERT_N, FORCING_FERT_P, FORCING_FERT_K};
    size_t                         dim_len;
    int                            status;
    
    size_t                         i;

    // Check domain & get dimensions
    status = nc_open(plugin_filenames.crop.nc_filename, NC_NOWRITE,
                     &(plugin_filenames.crop.nc_id));
    check_nc_status(status, "Error opening %s",
                    plugin_filenames.crop.nc_filename);

    // add the number of crop type to the location info in the
    // global domain struct. This just makes life easier
    add_ncrop_to_global_domain(&(plugin_filenames.crop), &global_domain);
        
    plugin_options.NCROPTYPES = get_nc_dimension(&(plugin_filenames.crop),
                                                "crop_class");
    if (plugin_options.WOFOST_DIST_FERT) {
        plugin_options.NFERTTIMES = get_nc_dimension(&(plugin_filenames.crop),
                                                "fertilizer_times");
    }
    if (plugin_options.WOFOST_FORCE_FERT) {
        for (i = 0; i < 4; i++) {
            // Get information from the forcing file(s)
            // Open first-year forcing files and get info
            snprintf(plugin_filenames.forcing[fert_idx[i]].nc_filename, MAXSTRING, "%s%4d.nc",
                     plugin_filenames.f_path_pfx[fert_idx[i]], global_param.startyear);
            status = nc_open(plugin_filenames.forcing[fert_idx[i]].nc_filename, NC_NOWRITE,
                             &(plugin_filenames.forcing[fert_idx[i]].nc_id));
            check_nc_status(status, "Error opening %s",
                            plugin_filenames.forcing[fert_idx[i]].nc_filename);

            dim_len = get_nc_dimension(&(plugin_filenames.forcing[fert_idx[i]]),
                                                        "crop_class");
            if (dim_len != plugin_options.NCROPTYPES) {
                log_err("Fertilizer forcing crop_class length is not equal to NCROPTYPES");
            }
            dim_len = get_nc_dimension(&(plugin_filenames.forcing[fert_idx[i]]),
                                                    "fertilizer_times");
            if (dim_len != plugin_options.NFERTTIMES) {
                log_err("Fertilizer forcing fertilizer_times length is not equal to NFERTTIMES");
            }

            // Close first-year forcing files
            status = nc_close(plugin_filenames.forcing[fert_idx[i]].nc_id);
            check_nc_status(status, "Error closing %s",
                            plugin_filenames.forcing[fert_idx[i]].nc_filename);
        }
    }
    
    compare_ncdomain_with_global_domain(&plugin_filenames.crop);

    status = nc_close(plugin_filenames.crop.nc_id);
    check_nc_status(status, "Error closing %s",
                    plugin_filenames.crop.nc_filename);
}
