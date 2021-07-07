#include <vic_driver_image.h>
#include <plugin.h>

void
crop_forcing(void)
{
    extern domain_struct              local_domain;
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_option_struct       plugin_options;
    extern domain_struct              global_domain;
    extern plugin_filenames_struct    plugin_filenames;
    extern crop_con_map_struct       *crop_con_map;
    extern crop_force_struct         *crop_force;

    double                           *dvar;

    size_t                            d5count[5];
    size_t                            d5start[5];

    size_t                            i;
    size_t                            j;
    size_t                            k;
    int                               iCrop;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    d5start[0] = 0;
    d5start[1] = 0;
    d5start[2] = 0;
    d5start[3] = 0;
    d5start[4] = 0;
    d5count[0] = 1;
    d5count[1] = 1;
    d5count[2] = 1;
    d5count[3] = global_domain.n_ny;
    d5count[4] = global_domain.n_nx;

    // Get fertilizer forcing data
    if (plugin_options.WOFOST_FORCE_FERT) {
        // Get DVS_point
        if (plugin_global_param.forcerun[FORCING_FERT_DVS]) {
            d5start[0] = plugin_global_param.forceskip[FORCING_FERT_DVS] +
                         plugin_global_param.forceoffset[FORCING_FERT_DVS];
            for (k = 0; k < plugin_options.NFERTTIMES; k++) {
                d5start[1] = k;
                for (j = 0; j < plugin_options.NCROPTYPES; j++) {
                    d5start[2] = j;
                    get_scatter_nc_field_double(&(plugin_filenames.forcing[
                                                      FORCING_FERT_DVS]),
                                                plugin_filenames.f_varname[
                                                    FORCING_FERT_DVS], d5start,
                                                d5count, dvar);

                    for (i = 0; i < local_domain.ncells_active; i++) {
                        iCrop = crop_con_map[i].cidx[j];
                        if (iCrop != NODATA_VEG) {
                            crop_force[i].DVS_point[iCrop][k] = dvar[i];
                        }
                    }
                }
            }
        }

        // Get N_amount
        if (plugin_global_param.forcerun[FORCING_FERT_N]) {
            d5start[0] = plugin_global_param.forceskip[FORCING_FERT_N] +
                         plugin_global_param.forceoffset[FORCING_FERT_N];
            for (k = 0; k < plugin_options.NFERTTIMES; k++) {
                d5start[1] = k;
                for (j = 0; j < plugin_options.NCROPTYPES; j++) {
                    d5start[2] = j;
                    get_scatter_nc_field_double(&(plugin_filenames.forcing[
                                                      FORCING_FERT_N]),
                                                plugin_filenames.f_varname[
                                                    FORCING_FERT_N], d5start,
                                                d5count, dvar);

                    for (i = 0; i < local_domain.ncells_active; i++) {
                        iCrop = crop_con_map[i].cidx[j];
                        if (iCrop != NODATA_VEG) {
                            crop_force[i].N_amount[iCrop][k] = dvar[i];
                        }
                    }
                }
            }
        }

        // Get P_amount
        if (plugin_global_param.forcerun[FORCING_FERT_P]) {
            d5start[0] = plugin_global_param.forceskip[FORCING_FERT_P] +
                         plugin_global_param.forceoffset[FORCING_FERT_P];
            for (k = 0; k < plugin_options.NFERTTIMES; k++) {
                d5start[1] = k;
                for (j = 0; j < plugin_options.NCROPTYPES; j++) {
                    d5start[2] = j;
                    get_scatter_nc_field_double(&(plugin_filenames.forcing[
                                                      FORCING_FERT_P]),
                                                plugin_filenames.f_varname[
                                                    FORCING_FERT_P], d5start,
                                                d5count, dvar);

                    for (i = 0; i < local_domain.ncells_active; i++) {
                        iCrop = crop_con_map[i].cidx[j];
                        if (iCrop != NODATA_VEG) {
                            crop_force[i].P_amount[iCrop][k] = dvar[i];
                        }
                    }
                }
            }
        }

        // Get K_amount
        if (plugin_global_param.forcerun[FORCING_FERT_K]) {
            d5start[0] = plugin_global_param.forceskip[FORCING_FERT_K] +
                         plugin_global_param.forceoffset[FORCING_FERT_K];
            for (k = 0; k < plugin_options.NFERTTIMES; k++) {
                d5start[1] = k;
                for (j = 0; j < plugin_options.NCROPTYPES; j++) {
                    d5start[2] = j;
                    get_scatter_nc_field_double(&(plugin_filenames.forcing[
                                                      FORCING_FERT_K]),
                                                plugin_filenames.f_varname[
                                                    FORCING_FERT_K], d5start,
                                                d5count, dvar);

                    for (i = 0; i < local_domain.ncells_active; i++) {
                        iCrop = crop_con_map[i].cidx[j];
                        if (iCrop != NODATA_VEG) {
                            crop_force[i].K_amount[iCrop][k] = dvar[i];
                        }
                    }
                }
            }
        }
    }

    free(dvar);
}
