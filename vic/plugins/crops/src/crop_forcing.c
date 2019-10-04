#include <vic_driver_image.h>
#include <plugin.h>

void
crop_forcing(void)
{
    extern domain_struct              local_domain;
    extern plugin_global_param_struct plugin_global_param;
    extern domain_struct              global_domain;
    extern plugin_filenames_struct    plugin_filenames;
    extern crop_force_struct         *crop_force;

    double                           *dvar;

    size_t                            d3count[3];
    size_t                            d3start[3];

    size_t                            i;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    // Get forcing data
    d3start[0] = plugin_global_param.forceskip[FORCING_CO2] +
                 plugin_global_param.forceoffset[FORCING_CO2];

    if (plugin_global_param.forcerun[FORCING_CO2]) {
        get_scatter_nc_field_double(&(plugin_filenames.forcing[FORCING_CO2]),
                                    plugin_filenames.f_varname[FORCING_CO2], d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            crop_force[i].CO2 = dvar[i];
        }
    }

    free(dvar);
}
