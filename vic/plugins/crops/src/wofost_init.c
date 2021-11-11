#include <vic_driver_image.h>
#include <plugin.h>

void
wofost_set_data_text(char *list)
{
    extern option_struct        options;
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;
    extern crop_con_map_struct *crop_con_map;
    extern SimUnit           ***Grid;

    FILE                       *ifp;
    int                         Emergence;
    char                        line[MAXSTRING];
    char                        path[MAXSTRING];
    char                        cf[MAXSTRING], mf[MAXSTRING], start[MAXSTRING];

    size_t                      i;
    size_t                      k;
    size_t                      m;
    size_t                      l;
    int                         iCrop;
    SimUnit                    *iGrid;
    SimUnit                    *tmpGrid;

    if ((ifp = fopen(list, "r")) == NULL) {
        log_err("Can't open wofost text file, %s\n", list);
    }

    k = 0;
    while (fgets(line, MAXSTRING, ifp)) {
        if (line[0] == '*' || line[0] == ' ' || line[0] == '\n' ||
            line[0] == '\r') {
            continue;
        }

        tmpGrid = malloc(sizeof(*tmpGrid));
        check_alloc_status(tmpGrid, "Memory allocation error");
        tmpGrid->crp = malloc(sizeof(*(tmpGrid->crp)));
        check_alloc_status(tmpGrid->crp, "Memory allocation error");
        tmpGrid->ste = malloc(sizeof(*(tmpGrid->ste)));
        check_alloc_status(tmpGrid->ste, "Memory allocation error");
        tmpGrid->mng = malloc(sizeof(*(tmpGrid->mng)));
        check_alloc_status(tmpGrid->mng, "Memory allocation error");
        tmpGrid->soil = malloc(sizeof(*(tmpGrid->soil)));
        check_alloc_status(tmpGrid->soil, "Memory allocation error");
        tmpGrid->met = malloc(sizeof(*(tmpGrid->met)));
        check_alloc_status(tmpGrid->met, "Memory allocation error");
        tmpGrid->next = NULL;

        initialize_wofost_grid(tmpGrid);

        sscanf(line, "%s %s %s %s %d", path, cf, mf, start, &Emergence);

        if (k >= plugin_options.NCROPTYPES) {
            log_err("Crops in wofost text (%zu) file are more than "
                    "crops in wofost netcdf file (%d)",
                    k, plugin_options.NCROPTYPES);
        }

        GetSimInput(path, cf, mf, start, Emergence, k, tmpGrid);
        tmpGrid->met->crop_class = k;

        for (i = 0; i < local_domain.ncells_active; i++) {
            iCrop = crop_con_map[i].cidx[k];

            if (iCrop != NODATA_VEG) {
                tmpGrid->met->Latitude = local_domain.locations[i].latitude;
                tmpGrid->met->Longitude = local_domain.locations[i].longitude;

                for (l = 0; l < options.SNOW_BAND; l++) {
                    iGrid = Grid[i][l];
                    for (m = 0; m < (size_t)iCrop; m++) {
                        iGrid = iGrid->next;
                    }

                    copy_wofost_grid(tmpGrid, iGrid);
                }
            }
        }

        Clean(tmpGrid);
        k++;
    }

    if (k + 1 < plugin_options.NCROPTYPES) {
        log_err("Crops in wofost text file are less than "
                "crops in wofost netcdf file");
    }

    fclose(ifp);
}

void
wofost_set_data(void)
{
    extern option_struct           options;
    extern domain_struct           local_domain;
    extern domain_struct           global_domain;
    extern global_param_struct     global_param;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct    plugin_options;
    extern crop_con_map_struct    *crop_con_map;
    extern crop_con_struct       **crop_con;
    extern SimUnit              ***Grid;

    double                        *dvar;

    size_t                         i;
    size_t                         j;
    size_t                         k;
    size_t                         l;
    int                            iCrop;
    SimUnit                       *iGrid;

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

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    if (plugin_options.WOFOST_DIST_SEASON) {
        // Set plant day
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            d3start[0] = j;

            get_scatter_nc_field_double(&plugin_filenames.crop,
                                        "plant_day", d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iCrop = crop_con_map[i].cidx[j];
                if (iCrop != NODATA_VEG) {
                    for (k = 0; k < options.SNOW_BAND; k++) {
                        iGrid = Grid[i][k];
                        for (l = 0; l < (size_t)iCrop; l++) {
                            iGrid = iGrid->next;
                        }

                        // plant is set to a random non-leap year
                        dmy_doy(dvar[i], 1900, global_param.calendar,
                                &(iGrid->start));
                    }
                }
            }
        }

        // Set harvest day
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            d3start[0] = j;

            get_scatter_nc_field_double(&plugin_filenames.crop,
                                        "harvest_day", d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iCrop = crop_con_map[i].cidx[j];
                if (iCrop != NODATA_VEG) {
                    for (k = 0; k < options.SNOW_BAND; k++) {
                        iGrid = Grid[i][k];
                        for (l = 0; l < (size_t)iCrop; l++) {
                            iGrid = iGrid->next;
                        }

                        // harvest is set to a random non-leap year
                        dmy_doy(dvar[i], 1900, global_param.calendar,
                                &(iGrid->end));
                    }
                }
            }
        }
    }

    if (plugin_options.WOFOST_DIST_TSUM) {
        // Set TSUM1
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            d3start[0] = j;

            get_scatter_nc_field_double(&plugin_filenames.crop,
                                        "TSUM1", d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iCrop = crop_con_map[i].cidx[j];
                if (iCrop != NODATA_VEG) {
                    for (k = 0; k < options.SNOW_BAND; k++) {
                        iGrid = Grid[i][k];
                        for (l = 0; l < (size_t)iCrop; l++) {
                            iGrid = iGrid->next;
                        }

                        iGrid->crp->prm.TempSum1 = dvar[i];
                    }
                }
            }
        }

        // Set TSUM2
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            d3start[0] = j;

            get_scatter_nc_field_double(&plugin_filenames.crop,
                                        "TSUM2", d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iCrop = crop_con_map[i].cidx[j];
                if (iCrop != NODATA_VEG) {
                    for (k = 0; k < options.SNOW_BAND; k++) {
                        iGrid = Grid[i][k];
                        for (l = 0; l < (size_t)iCrop; l++) {
                            iGrid = iGrid->next;
                        }

                        iGrid->crp->prm.TempSum2 = dvar[i];
                    }
                }
            }
        }
    }

    if (plugin_options.WOFOST_DIST_FERT) {
        // Set DVS_point
        for (k = 0; k < plugin_options.NFERTTIMES; k++) {
            d4start[0] = k;
            for (j = 0; j < plugin_options.NCROPTYPES; j++) {
                d4start[1] = j;

                get_scatter_nc_field_double(&plugin_filenames.crop,
                                            "DVS_point", d4start, d4count,
                                            dvar);

                for (i = 0; i < local_domain.ncells_active; i++) {
                    iCrop = crop_con_map[i].cidx[j];
                    if (iCrop != NODATA_VEG) {
                        crop_con[i][iCrop].DVS_point[k] = dvar[i];
                    }
                }
            }
        }

        // Set N_amount
        for (k = 0; k < plugin_options.NFERTTIMES; k++) {
            d4start[0] = k;
            for (j = 0; j < plugin_options.NCROPTYPES; j++) {
                d4start[1] = j;

                get_scatter_nc_field_double(&plugin_filenames.crop,
                                            "N_amount", d4start, d4count, dvar);

                for (i = 0; i < local_domain.ncells_active; i++) {
                    iCrop = crop_con_map[i].cidx[j];
                    if (iCrop != NODATA_VEG) {
                        crop_con[i][iCrop].N_amount[k] = dvar[i];
                    }
                }
            }
        }

        // Set P_amount
        for (k = 0; k < plugin_options.NFERTTIMES; k++) {
            d4start[0] = k;
            for (j = 0; j < plugin_options.NCROPTYPES; j++) {
                d4start[1] = j;

                get_scatter_nc_field_double(&plugin_filenames.crop,
                                            "P_amount", d4start, d4count, dvar);

                for (i = 0; i < local_domain.ncells_active; i++) {
                    iCrop = crop_con_map[i].cidx[j];
                    if (iCrop != NODATA_VEG) {
                        crop_con[i][iCrop].P_amount[k] = dvar[i];
                    }
                }
            }
        }

        // Set K_amount
        for (k = 0; k < plugin_options.NFERTTIMES; k++) {
            d4start[0] = k;
            for (j = 0; j < plugin_options.NCROPTYPES; j++) {
                d4start[1] = j;

                get_scatter_nc_field_double(&plugin_filenames.crop,
                                            "K_amount", d4start, d4count, dvar);

                for (i = 0; i < local_domain.ncells_active; i++) {
                    iCrop = crop_con_map[i].cidx[j];
                    if (iCrop != NODATA_VEG) {
                        crop_con[i][iCrop].K_amount[k] = dvar[i];
                    }
                }
            }
        }
    }

    if (plugin_options.WOFOST_DIST_MIN) {
        // Set N_Mins
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            d3start[0] = j;

            get_scatter_nc_field_double(&plugin_filenames.crop,
                                        "N_mins", d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iCrop = crop_con_map[i].cidx[j];
                if (iCrop != NODATA_VEG) {
                    for (k = 0; k < options.SNOW_BAND; k++) {
                        iGrid = Grid[i][k];
                        for (l = 0; l < (size_t)iCrop; l++) {
                            iGrid = iGrid->next;
                        }

                        iGrid->mng->N_Mins = dvar[i];
                    }
                }
            }
        }

        // Set NRecoveryFrac
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            d3start[0] = j;

            get_scatter_nc_field_double(&plugin_filenames.crop,
                                        "N_recovery", d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iCrop = crop_con_map[i].cidx[j];
                if (iCrop != NODATA_VEG) {
                    for (k = 0; k < options.SNOW_BAND; k++) {
                        iGrid = Grid[i][k];
                        for (l = 0; l < (size_t)iCrop; l++) {
                            iGrid = iGrid->next;
                        }

                        iGrid->mng->NRecoveryFrac = dvar[i];
                    }
                }
            }
        }

        // Set P_Mins
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            d3start[0] = j;

            get_scatter_nc_field_double(&plugin_filenames.crop,
                                        "P_mins", d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iCrop = crop_con_map[i].cidx[j];
                if (iCrop != NODATA_VEG) {
                    for (k = 0; k < options.SNOW_BAND; k++) {
                        iGrid = Grid[i][k];
                        for (l = 0; l < (size_t)iCrop; l++) {
                            iGrid = iGrid->next;
                        }

                        iGrid->mng->P_Mins = dvar[i];
                    }
                }
            }
        }

        // Set PRecoveryFrac
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            d3start[0] = j;

            get_scatter_nc_field_double(&plugin_filenames.crop,
                                        "P_recovery", d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iCrop = crop_con_map[i].cidx[j];
                if (iCrop != NODATA_VEG) {
                    for (k = 0; k < options.SNOW_BAND; k++) {
                        iGrid = Grid[i][k];
                        for (l = 0; l < (size_t)iCrop; l++) {
                            iGrid = iGrid->next;
                        }

                        iGrid->mng->PRecoveryFrac = dvar[i];
                    }
                }
            }
        }

        // Set K_Mins
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            d3start[0] = j;

            get_scatter_nc_field_double(&plugin_filenames.crop,
                                        "K_mins", d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iCrop = crop_con_map[i].cidx[j];
                if (iCrop != NODATA_VEG) {
                    for (k = 0; k < options.SNOW_BAND; k++) {
                        iGrid = Grid[i][k];
                        for (l = 0; l < (size_t)iCrop; l++) {
                            iGrid = iGrid->next;
                        }

                        iGrid->mng->K_Mins = dvar[i];
                    }
                }
            }
        }

        // Set KRecoveryFrac
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            d3start[0] = j;

            get_scatter_nc_field_double(&plugin_filenames.crop,
                                        "K_recovery", d3start, d3count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                iCrop = crop_con_map[i].cidx[j];
                if (iCrop != NODATA_VEG) {
                    for (k = 0; k < options.SNOW_BAND; k++) {
                        iGrid = Grid[i][k];
                        for (l = 0; l < (size_t)iCrop; l++) {
                            iGrid = iGrid->next;
                        }

                        iGrid->mng->KRecoveryFrac = dvar[i];
                    }
                }
            }
        }
    }

    // Set cycle length
    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < plugin_options.NCROPTYPES; j++) {
            iCrop = crop_con_map[i].cidx[j];
            if (iCrop != NODATA_VEG) {
                for (k = 0; k < options.SNOW_BAND; k++) {
                    iGrid = Grid[i][k];
                    for (l = 0; l < (size_t)iCrop; l++) {
                        iGrid = iGrid->next;
                    }

                    if (!plugin_options.WOFOST_CONTINUE) {
                        if (iGrid->start.day_in_year < iGrid->end.day_in_year) {
                            iGrid->met->CycleLength = iGrid->end.day_in_year -
                                                      iGrid->start.day_in_year;
                        }
                        else {
                            iGrid->met->CycleLength = DAYS_PER_YEAR -
                                                      iGrid->start.day_in_year +
                                                      iGrid->end.day_in_year;
                        }
                    }
                    else {
                        /* Restrict cycle length to about a year to avoid double-sowing */
                        iGrid->met->CycleLength = DAYS_PER_YEAR - 10;
                    }
                }
            }
        }
    }

    free(dvar);
}

void
wofost_check_data(void)
{
    extern option_struct options;
    extern domain_struct local_domain;
    extern SimUnit    ***Grid;

    size_t               i;
    size_t               k;
    SimUnit             *iGrid;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (k = 0; k < options.SNOW_BAND; k++) {
            iGrid = Grid[i][k];

            while (iGrid) {
                if (iGrid->crp->prm.TempSum1 <= 0) {
                    log_err("TempSum1 [%.2f] should be in range [0, inf)",
                            iGrid->crp->prm.TempSum1);
                }
                if (iGrid->crp->prm.TempSum2 <= 0) {
                    log_err("TempSum2 [%.2f] should be in range [0, inf)",
                            iGrid->crp->prm.TempSum2);
                }
                if (iGrid->mng->N_Mins < 0) {
                    log_err("N_Mins [%.2f] should be in range (0, inf)",
                            iGrid->mng->N_Mins);
                }
                if (iGrid->mng->P_Mins < 0) {
                    log_err("P_Mins [%.2f] should be in range (0, inf)",
                            iGrid->mng->P_Mins);
                }
                if (iGrid->mng->K_Mins < 0) {
                    log_err("K_Mins [%.2f] should be in range (0, inf)",
                            iGrid->mng->K_Mins);
                }
                if (iGrid->mng->NRecoveryFrac < 0 ||
                    iGrid->mng->NRecoveryFrac > 1) {
                    log_err("NRecoveryFrac [%.2f] should be in range (0, 1)",
                            iGrid->mng->NRecoveryFrac);
                }
                if (iGrid->mng->PRecoveryFrac < 0 ||
                    iGrid->mng->PRecoveryFrac > 1) {
                    log_err("PRecoveryFrac [%.2f] should be in range (0, 1)",
                            iGrid->mng->PRecoveryFrac);
                }
                if (iGrid->mng->KRecoveryFrac < 0 ||
                    iGrid->mng->KRecoveryFrac > 1) {
                    log_err("KRecoveryFrac [%.2f] should be in range (0, 1)",
                            iGrid->mng->KRecoveryFrac);
                }

                iGrid = iGrid->next;
            }
        }
    }
}
