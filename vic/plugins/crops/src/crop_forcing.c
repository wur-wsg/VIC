#include <vic_driver_image.h>
#include <plugin.h>

void
crop_forcing(void)
{
    extern domain_struct           local_domain;
    extern global_param_struct     global_param;
    extern domain_struct           global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern crop_force_struct      *crop_force;
    extern size_t                  NF;
    extern size_t                  NR;

    double                        *dvar;

    size_t                         d3count[3];
    size_t                         d3start[3];

    size_t                         i;
    size_t                         j;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    // Get forcing data
    for (j = 0; j < NF; j++) {
        d3start[0] = global_param.forceskip[0] +
                     global_param.forceoffset[0] + j - NF;

        get_scatter_nc_field_double(&(plugin_filenames.forcing[FORCING_CO2]),
                                    plugin_filenames.f_varname[FORCING_CO2], d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            crop_force[i].CO2[j] = dvar[i];
        }
    }

    // Average forcing data
    for (i = 0; i < local_domain.ncells_active; i++) {
        crop_force[i].CO2[NR] = average(crop_force[i].CO2, NF);
    }

    free(dvar);
}
