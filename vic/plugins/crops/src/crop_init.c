#include <vic_driver_image.h>
#include <plugin.h>

void
crop_set_mapping(void)
{
    extern option_struct           options;
    extern domain_struct           local_domain;
    extern domain_struct           global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct    plugin_options;
    extern crop_con_map_struct    *crop_con_map;
    extern veg_con_map_struct     *veg_con_map;

    int                           *ivar;
    double                        *dvar;
    double                         Cc_sum;
    char                           locstr[MAXSTRING];

    size_t                         i;
    size_t                         j;
    size_t                         k;
    size_t                         l;
    int                            veg_class;

    size_t                         d3count[3];
    size_t                         d3start[3];
    size_t                         d4count[4];
    size_t                         d4start[4];

    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    d4start[0] = 0;
    d4start[1] = 0;
    d4start[2] = 0;
    d4start[3] = 0;
    d4count[0] = 1;
    d4count[1] = 1;
    d4count[2] = global_domain.n_ny;
    d4count[3] = global_domain.n_nx;

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    // Set vegetation classes
    for (j = 0; j < plugin_options.NCROPTYPES; j++) {
        d3start[0] = j;

        get_scatter_nc_field_int(&plugin_filenames.crop,
                                 "crop_veg_class", d3start, d3count, ivar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (ivar[i] > 0) {
                if ((size_t)ivar[i] > options.NVEGTYPES) {
                    sprint_location(locstr, &(local_domain.locations[i]));
                    log_err("Crop %zu veg class [%d] is larger than number of "
                            "vegetation classes [%zu].\n%s",
                            j, ivar[i], options.NVEGTYPES, locstr);
                }
                crop_con_map[i].veg_class[j] = ivar[i] - 1;
            }
            else {
                crop_con_map[i].veg_class[j] = NODATA_VEG;
            }
        }
    }

    // Set crop coverage
    for (k = 0; k < MONTHS_PER_YEAR; k++) {
        d4start[0] = k;
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            d4start[1] = j;

            get_scatter_nc_field_double(&plugin_filenames.crop,
                                        "Cc", d4start, d4count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                crop_con_map[i].Cc[j][k] = dvar[i];
            }
        }
    }

    // Do the mapping
    for (i = 0; i < local_domain.ncells_active; i++) {
        k = 0;
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            Cc_sum = 0;
            for (l = 0; l < MONTHS_PER_YEAR; l++) {
                Cc_sum += crop_con_map[i].Cc[j][l];
            }
            if (Cc_sum > 0) {
                crop_con_map[i].cidx[j] = k;
                veg_class = crop_con_map[i].veg_class[j];
                if (veg_class == NODATA_VEG ||
                    veg_con_map[i].vidx[veg_class] == NODATA_VEG) {
                    sprint_location(locstr, &(local_domain.locations[i]));
                    log_err("Crop %zu coverage sum %.4f is > 0 for "
                            "empty vegetation tile.\n%s",
                            j, Cc_sum, locstr);
                }
                k++;
            }
            else {
                crop_con_map[i].cidx[j] = NODATA_VEG;
            }
        }

        // check the number of nonzero crop tiles
        if (k > local_domain.locations[i].ncrop) {
            sprint_location(locstr, &(local_domain.locations[i]));
            log_err("Number of crop tiles with nonzero area (%zu) > ncrop "
                    "(%zu).\n%s", k, local_domain.locations[i].ncrop,
                    locstr);
        }
        else if (k < local_domain.locations[i].ncrop) {
            sprint_location(locstr, &(local_domain.locations[i]));
            log_err("Number of crop tiles with nonzero area (%zu) < ncrop "
                    "(%zu).\n%s", k, local_domain.locations[i].ncrop,
                    locstr);
        }
    }

    free(dvar);
    free(ivar);
}

void
crop_set_tfactor(void)
{
    extern domain_struct           local_domain;
    extern domain_struct           global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern soil_con_struct        *soil_con;
    double                        *dvar;

    size_t                         i;

    size_t                         d2count[2];
    size_t                         d2start[2];

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    // Set t factor
    get_scatter_nc_field_double(&plugin_filenames.crop,
                                "Tfactor", d2start, d2count, dvar);

    for (i = 0; i < local_domain.ncells_active; i++) {
        soil_con[i].Tfactor[0] = dvar[i];
    }

    free(dvar);
}

void
crop_init(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern int                     mpi_rank;

    int                            status;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.crop.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.crop.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.crop.nc_filename);
    }

    crop_set_mapping();
    crop_set_tfactor();
    wofost_set_data_text(plugin_filenames.wofost_text);
    wofost_set_data();
    wofost_check_data();

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.crop.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.crop.nc_filename);
    }
}
