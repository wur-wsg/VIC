#include <vic_driver_image.h>
#include <routing.h>

#include "plugin.h"

void
rout_forcing(void)
{
    extern domain_struct           local_domain;
    extern plugin_global_param_struct     plugin_global_param;
    extern domain_struct           global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern rout_force_struct      *rout_force;
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
        d3start[0] = plugin_global_param.forceskip[FORCING_DISCHARGE] +
                     plugin_global_param.forceoffset[FORCING_DISCHARGE] + j;

        if(plugin_global_param.forcerun[FORCING_DISCHARGE]){
            get_scatter_nc_field_double(&(plugin_filenames.forcing[FORCING_DISCHARGE]),
                                        plugin_filenames.f_varname[FORCING_DISCHARGE], d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                rout_force[i].discharge[j] = dvar[i];
            }
        }
    }

    // Average forcing data
    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_force[i].discharge[NR] = average(rout_force[i].discharge, NF);
    }

    free(dvar);
}
