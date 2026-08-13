/******************************************************************************
 * @section DESCRIPTION
 *
 * Read atmospheric forcing data.
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#include <vic_driver_image.h>
#include <plugin.h>

/******************************************************************************
 * @brief    Plausibility bounds for vegetation-history forcing.
 * @details  Values outside these bounds (and non-finite values such as an
 *           unmasked _FillValue) are treated as invalid on active tiles.  LAI
 *           is bounded generously; the intent is to catch fill values on the
 *           order of 1e36, not to police the science.
 *****************************************************************************/
#define VEGHIST_LAI_MAX 20.0

/******************************************************************************
 * @brief    Maximum number of per-tile invalid veg-history warnings to log.
 * @details  A global 5-minute domain has millions of tiles, so an unbounded
 *           warning stream would bury the rest of the log.  The running total
 *           is reported once at the end of the simulation regardless.
 *****************************************************************************/
#define VEGHIST_INVALID_WARN_LOG_LIMIT 20ULL

static unsigned long long veghist_invalid_warn_count = 0;
static unsigned long long veghist_invalid_total_count = 0;

/******************************************************************************
 * @brief    Is this vegetation-history forcing value usable?
 *****************************************************************************/
static bool
veghist_value_is_valid(double             value,
                       unsigned short int type)
{
    if (!isfinite(value)) {
        return false;
    }

    switch (type) {
    case LAI:
        return (value >= 0.0 && value <= VEGHIST_LAI_MAX);
    case FCANOPY:
    case ALBEDO:
        return (value >= 0.0 && value <= 1.0);
    default:
        return true;
    }
}

/******************************************************************************
 * @brief    Warn (rate-limited) about an invalid veg-history value.
 * @details  The caller leaves veg_hist at the parameter climatology that
 *           vic_force() already wrote, so the affected tile falls back to
 *           climatology for this time step.
 *****************************************************************************/
static void
warn_invalid_veghist(unsigned short int type,
                     size_t             iCell,
                     size_t             veg_class,
                     size_t             time_index,
                     double             value)
{
    extern domain_struct    local_domain;
    extern param_set_struct param_set;

    veghist_invalid_total_count++;

    if (veghist_invalid_warn_count < VEGHIST_INVALID_WARN_LOG_LIMIT) {
        log_warn(
            "Invalid vegetation-history forcing for %s: io_idx=%zu, "
            "veg_class=%zu, time index=%zu, value=%.6g; falling back to the "
            "parameter climatology for this time step",
            param_set.TYPE[type].varname,
            local_domain.locations[iCell].io_idx, veg_class, time_index,
            value);
        veghist_invalid_warn_count++;

        if (veghist_invalid_warn_count == VEGHIST_INVALID_WARN_LOG_LIMIT) {
            log_warn("Invalid vegetation-history forcing warnings reached "
                     "limit (%llu); suppressing further per-tile warnings.  A "
                     "running total is reported at the end of the simulation",
                     (unsigned long long) VEGHIST_INVALID_WARN_LOG_LIMIT);
        }
    }
}

/******************************************************************************
 * @brief    Record index of the monthly veg-history forcing for this date.
 * @details  Monthly files are validated at startup to hold exactly twelve
 *           records, January through December of the file's year, so the
 *           record index is simply the zero-based calendar month.  Keeping
 *           this a pure function of the current date (rather than an
 *           accumulating counter) is what makes a run restarted mid-month
 *           reproduce a continuous run exactly.
 *****************************************************************************/
static size_t
monthly_force_index(dmy_struct *dmy_current)
{
    return (size_t) (dmy_current->month - 1);
}

/******************************************************************************
 * @brief    Store one vegetation-history value in veg_hist.
 *****************************************************************************/
static void
set_veghist_value(unsigned short int type,
                  size_t             iCell,
                  int                vidx,
                  size_t             j,
                  double             value)
{
    extern veg_hist_struct **veg_hist;

    switch (type) {
    case LAI:
        veg_hist[iCell][vidx].LAI[j] = value;
        break;
    case FCANOPY:
        veg_hist[iCell][vidx].fcanopy[j] = value;
        break;
    case ALBEDO:
        veg_hist[iCell][vidx].albedo[j] = value;
        break;
    default:
        log_err("set_veghist_value called for forcing type %hu, which is not a "
                "vegetation-history variable", type);
    }
}

/******************************************************************************
 * @brief    Read one vegetation-history forcing variable into veg_hist.
 * @details  Handles both STEP frequency (one record per snow sub-step, the
 *           historical behaviour) and MONTH frequency (one record per calendar
 *           month, shared by every sub-step of every day in that month).  For
 *           MONTH the record is read once and broadcast to all NF sub-steps,
 *           which both keeps the value constant within the month and cuts the
 *           number of netCDF reads by a factor of NF.
 *****************************************************************************/
static void
read_veghist_forcing(unsigned short int type,
                     dmy_struct        *dmy_current,
                     double            *dvar)
{
    extern size_t              NF;
    extern global_param_struct global_param;
    extern domain_struct       global_domain;
    extern domain_struct       local_domain;
    extern filenames_struct    filenames;
    extern option_struct       options;
    extern param_set_struct    param_set;
    extern veg_con_map_struct *veg_con_map;

    size_t                     d4start[4];
    size_t                     d4count[4];
    size_t                     i;
    size_t                     j;
    size_t                     v;
    size_t                     rec;
    size_t                     nrec;
    int                        vidx;
    bool                       monthly;

    monthly = (global_param.forcefreq[type] == FORCE_FREQ_MONTH);

    // only the time and veg_class slices change; the rest is constant
    d4start[2] = 0;
    d4start[3] = 0;
    d4count[0] = 1;
    d4count[1] = 1;
    d4count[2] = global_domain.n_ny;
    d4count[3] = global_domain.n_nx;

    // A monthly record covers the whole model step, so it is read once and
    // applied to every sub-step instead of once per sub-step.
    nrec = monthly ? 1 : NF;

    for (rec = 0; rec < nrec; rec++) {
        if (monthly) {
            d4start[0] = monthly_force_index(dmy_current);
        }
        else {
            d4start[0] = global_param.forceskip[type] +
                         global_param.forceoffset[type] + rec;
        }

        for (v = 0; v < options.NVEGTYPES; v++) {
            d4start[1] = v;
            get_scatter_nc_field_double(&(filenames.forcing[type]),
                                        param_set.TYPE[type].varname,
                                        d4start, d4count, dvar);

            for (i = 0; i < local_domain.ncells_active; i++) {
                vidx = veg_con_map[i].vidx[v];
                if (vidx == NODATA_VEG) {
                    continue;
                }

                if (!veghist_value_is_valid(dvar[i], type)) {
                    // Leave the parameter climatology that vic_force() already
                    // wrote into veg_hist for this tile and time step.
                    warn_invalid_veghist(type, i, v, d4start[0], dvar[i]);
                    continue;
                }

                if (monthly) {
                    for (j = 0; j < NF; j++) {
                        set_veghist_value(type, i, vidx, j, dvar[i]);
                    }
                }
                else {
                    set_veghist_value(type, i, vidx, rec, dvar[i]);
                }
            }
        }
    }
}

/******************************************************************************
 * @brief    Read atmospheric forcing data.
 *****************************************************************************/
void
vic_force(void)
{
    extern size_t              NF;
    extern size_t              NR;
    extern size_t              current;
    extern int                 mpi_rank;
    extern force_data_struct  *force;
    extern dmy_struct         *dmy;
    extern domain_struct       global_domain;
    extern domain_struct       local_domain;
    extern filenames_struct    filenames;
    extern global_param_struct global_param;
    extern option_struct       options;
    extern soil_con_struct    *soil_con;
    extern veg_con_map_struct *veg_con_map;
    extern veg_con_struct    **veg_con;
    extern veg_hist_struct   **veg_hist;
    extern parameters_struct   param;
    extern param_set_struct    param_set;

    double                    *t_offset = NULL;
    double                    *dvar = NULL;
    size_t                     i;
    size_t                     j;
    size_t                     v;
    size_t                     f;
    size_t                     band;
    int                        vidx;
    int                        status;
    size_t                     d3count[3];
    size_t                     d3start[3];
    double                    *Tfactor;

    // allocate memory for variables to be read
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    for (f = 0; f < N_FORCING_TYPES; f++) {
        if (strcmp(filenames.f_path_pfx[f], "MISSING") == 0) {
            continue;
        }

        // global_param.forceoffset resets every year since the met file restarts
        // every year
        // global_param.forceskip should also reset to 0 after the first year
        if (current > 0 && (dmy[current].year != dmy[current - 1].year)) {
            global_param.forceoffset[f] = 0;
            global_param.forceskip[f] = 0;
            // close the forcing file for the previous year and open the forcing
            // file for the current new year
            // (forcing file for the first year should already be open in
            // get_global_param)
            if (mpi_rank == VIC_MPI_ROOT) {
                // close previous forcing file
                status = nc_close(filenames.forcing[f].nc_id);
                check_nc_status(status, "Error closing %s",
                                filenames.forcing[f].nc_filename);
                // open new forcing file
                snprintf(filenames.forcing[f].nc_filename, MAXSTRING,
                         "%s%4d.nc",
                         filenames.f_path_pfx[f], dmy[current].year);
                status = nc_open(filenames.forcing[f].nc_filename, NC_NOWRITE,
                                 &(filenames.forcing[f].nc_id));
                check_nc_status(status, "Error opening %s",
                                filenames.forcing[f].nc_filename);
            }
        }
    }

    // only the time slice changes for the met file reads. The rest is constant
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    // Air temperature: tas
    for (j = 0; j < NF; j++) {
        d3start[0] = global_param.forceskip[AIR_TEMP] +
                     global_param.forceoffset[AIR_TEMP] +
                     j;
        get_scatter_nc_field_double(&(filenames.forcing[AIR_TEMP]),
                                    param_set.TYPE[AIR_TEMP].varname,
                                    d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            force[i].air_temp[j] = (double) dvar[i];
        }
    }

    // Precipitation: prcp
    for (j = 0; j < NF; j++) {
        d3start[0] = global_param.forceskip[PREC] +
                     global_param.forceoffset[PREC] +
                     j;
        get_scatter_nc_field_double(&(filenames.forcing[PREC]),
                                    param_set.TYPE[PREC].varname,
                                    d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            force[i].prec[j] = (double) dvar[i];
        }
    }

    // Downward solar radiation: dswrf
    for (j = 0; j < NF; j++) {
        d3start[0] = global_param.forceskip[SWDOWN] +
                     global_param.forceoffset[SWDOWN] +
                     j;
        get_scatter_nc_field_double(&(filenames.forcing[SWDOWN]),
                                    param_set.TYPE[SWDOWN].varname,
                                    d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            force[i].shortwave[j] = (double) dvar[i];
        }
    }

    // Downward longwave radiation: dlwrf
    for (j = 0; j < NF; j++) {
        d3start[0] = global_param.forceskip[LWDOWN] +
                     global_param.forceoffset[LWDOWN] +
                     j;
        get_scatter_nc_field_double(&(filenames.forcing[LWDOWN]),
                                    param_set.TYPE[LWDOWN].varname,
                                    d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            force[i].longwave[j] = (double) dvar[i];
        }
    }

    // Wind speed: wind
    for (j = 0; j < NF; j++) {
        d3start[0] = global_param.forceskip[WIND] +
                     global_param.forceoffset[WIND] +
                     j;
        get_scatter_nc_field_double(&(filenames.forcing[WIND]),
                                    param_set.TYPE[WIND].varname,
                                    d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            force[i].wind[j] = (double) dvar[i];
        }
    }

    // vapor pressure: vp
    for (j = 0; j < NF; j++) {
        d3start[0] = global_param.forceskip[VP] + global_param.forceoffset[VP] +
                     j;
        get_scatter_nc_field_double(&(filenames.forcing[VP]),
                                    param_set.TYPE[VP].varname,
                                    d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            force[i].vp[j] = (double) dvar[i];
        }
    }

    // Pressure: pressure
    for (j = 0; j < NF; j++) {
        d3start[0] = global_param.forceskip[PRESSURE] +
                     global_param.forceoffset[PRESSURE] +
                     j;
        get_scatter_nc_field_double(&(filenames.forcing[PRESSURE]),
                                    param_set.TYPE[PRESSURE].varname,
                                    d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            force[i].pressure[j] = (double) dvar[i];
        }
    }
    // Optional inputs
    if (options.LAKES) {
        // Channel inflow to lake
        d3start[0] = global_param.forceskip[CHANNEL_IN] +
                     global_param.forceoffset[CHANNEL_IN] +
                     j;
        get_scatter_nc_field_double(&(filenames.forcing[CHANNEL_IN]),
                                    param_set.TYPE[CHANNEL_IN].varname,
                                    d3start, d3count, dvar);
        for (j = 0; j < NF; j++) {
            for (i = 0; i < local_domain.ncells_active; i++) {
                force[i].channel_in[j] = (double) dvar[i];
            }
        }
    }
    if (options.CARBON) {
        // Atmospheric CO2 mixing ratio
        for (j = 0; j < NF; j++) {
            d3start[0] = global_param.forceskip[CATM] +
                         global_param.forceoffset[CATM] + j;
            get_scatter_nc_field_double(&(filenames.forcing[CATM]),
                                        param_set.TYPE[CATM].varname,
                                        d3start, d3count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                force[i].Catm[j] = (double) dvar[i];
            }
        }
        // Cosine of solar zenith angle
        for (j = 0; j < NF; j++) {
            for (i = 0; i < local_domain.ncells_active; i++) {
                force[i].coszen[j] = compute_coszen(
                    local_domain.locations[i].latitude,
                    local_domain.locations[i].longitude,
                    soil_con[i].time_zone_lng, dmy[current].day_in_year,
                    dmy[current].dayseconds);
            }
        }
        // Fraction of shortwave that is direct
        for (j = 0; j < NF; j++) {
            d3start[0] = global_param.forceskip[FDIR] +
                         global_param.forceoffset[FDIR] + j;
            get_scatter_nc_field_double(&(filenames.forcing[FDIR]),
                                        param_set.TYPE[FDIR].varname,
                                        d3start, d3count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                force[i].fdir[j] = (double) dvar[i];
            }
        }
        // Photosynthetically active radiation
        for (j = 0; j < NF; j++) {
            d3start[0] = global_param.forceskip[PAR] +
                         global_param.forceoffset[PAR] + j;
            get_scatter_nc_field_double(&(filenames.forcing[PAR]),
                                        param_set.TYPE[PAR].varname,
                                        d3start, d3count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                force[i].par[j] = (double) dvar[i];
            }
        }
    }
    else {
        // Atmospheric CO2 mixing ratio
        for (j = 0; j < NF; j++) {
            for (i = 0; i < local_domain.ncells_active; i++) {
                force[i].Catm[j] = (double) param.CANOPY_CO2REF *
                                   PPM_to_MIXRATIO;
            }
        }
    }

    // Initialize the veg_hist structure with the current climatological
    // vegetation parameters.  This may be overwritten with the historical
    // forcing time series.
    for (i = 0; i < local_domain.ncells_active; i++) {
        for (v = 0; v < options.NVEGTYPES; v++) {
            vidx = veg_con_map[i].vidx[v];
            if (vidx != NODATA_VEG) {
                for (j = 0; j < NF; j++) {
                    veg_hist[i][vidx].albedo[j] =
                        veg_con[i][vidx].albedo[dmy[current].month - 1];
                    veg_hist[i][vidx].displacement[j] =
                        veg_con[i][vidx].displacement[dmy[current].month - 1];
                    veg_hist[i][vidx].fcanopy[j] =
                        veg_con[i][vidx].fcanopy[dmy[current].month - 1];
                    veg_hist[i][vidx].LAI[j] =
                        veg_con[i][vidx].LAI[dmy[current].month - 1];
                    veg_hist[i][vidx].roughness[j] =
                        veg_con[i][vidx].roughness[dmy[current].month - 1];
                }
            }
        }
    }

    // Read veg_hist file
    // Leaf Area Index: LAI
    if (options.LAI_SRC == FROM_VEGHIST) {
        read_veghist_forcing(LAI, &(dmy[current]), dvar);
    }

    // Partial veg cover fraction: fcanopy
    if (options.FCAN_SRC == FROM_VEGHIST) {
        read_veghist_forcing(FCANOPY, &(dmy[current]), dvar);
    }

    // Albedo: albedo
    if (options.ALB_SRC == FROM_VEGHIST) {
        read_veghist_forcing(ALBEDO, &(dmy[current]), dvar);
    }

    for (f = 0; f < N_FORCING_TYPES; f++) {
        if (strcmp(filenames.f_path_pfx[f], "MISSING") == 0) {
            continue;
        }

        if (mpi_rank == VIC_MPI_ROOT) {
            // Close forcing file if it is the last time step
            if (current == global_param.nrecs - 1) {
                status = nc_close(filenames.forcing[f].nc_id);
                check_nc_status(status, "Error closing %s",
                                filenames.forcing[f].nc_filename);
            }
        }

        // Update the offset counter.  Monthly forcing does not use the offset
        // counter at all: its record index is derived directly from the
        // current date (see monthly_force_index), so that a restart mid-month
        // lands on the same record as a continuous run.
        if (global_param.forcefreq[f] != FORCE_FREQ_MONTH) {
            global_param.forceoffset[f] += NF;
        }
    }

    // Report the running total of invalid veg-history values once, at the end
    // of the simulation, so that rate-limited warnings cannot hide the scale
    // of a bad forcing dataset.
    if (current == global_param.nrecs - 1 && veghist_invalid_total_count > 0) {
        log_warn("Vegetation-history forcing contained %llu invalid values on "
                 "active tiles over the whole simulation; each fell back to "
                 "the parameter climatology for its time step",
                 veghist_invalid_total_count);
    }


    // allocate memory for t_offset
    t_offset = malloc(local_domain.ncells_active * sizeof(*t_offset));
    check_alloc_status(t_offset, "Memory allocation error.");

    for (i = 0; i < local_domain.ncells_active; i++) {
        if (options.SNOW_BAND > 1) {
            Tfactor = soil_con[i].Tfactor;
            t_offset[i] = Tfactor[0];
            for (band = 1; band < options.SNOW_BAND; band++) {
                if (Tfactor[band] < t_offset[i]) {
                    t_offset[i] = Tfactor[band];
                }
            }
        }
        else {
            t_offset[i] = 0;
        }
    }
    // Convert forcings into what we need and calculate missing ones
    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < NF; j++) {
            // pressure in Pa
            force[i].pressure[j] *= PA_PER_KPA;
            // vapor pressure in Pa
            force[i].vp[j] *= PA_PER_KPA;
            // vapor pressure deficit in Pa
            force[i].vpd[j] = svp(force[i].air_temp[j]) - force[i].vp[j];
            if (force[i].vpd[j] < 0) {
                force[i].vpd[j] = 0;
                force[i].vp[j] = svp(force[i].air_temp[j]);
            }
            // air density in kg/m3
            force[i].density[j] = air_density(force[i].air_temp[j],
                                              force[i].pressure[j]);
            // snow flag
            force[i].snowflag[j] = will_it_snow(&(force[i].air_temp[j]),
                                                t_offset[i],
                                                param.SNOW_MAX_SNOW_TEMP,
                                                &(force[i].prec[j]), 1);
        }
        // Check on fcanopy
        for (v = 0; v < options.NVEGTYPES; v++) {
            vidx = veg_con_map[i].vidx[v];
            if (vidx != NODATA_VEG) {
                for (j = 0; j < NF; j++) {
                    if ((veg_hist[i][vidx].fcanopy[j] < MIN_FCANOPY) &&
                        ((current == 0) ||
                         (options.FCAN_SRC == FROM_VEGHIST))) {
                        // Only issue this warning once if not using veg hist fractions
                        log_warn(
                            "cell %zu, veg` %d substep %zu fcanopy %f < minimum of %f; setting = %f", i, vidx, j,
                            veg_hist[i][vidx].fcanopy[j], MIN_FCANOPY,
                            MIN_FCANOPY);
                        veg_hist[i][vidx].fcanopy[j] = MIN_FCANOPY;
                    }
                }
            }
        }
    }


    // Put average value in NR field
    for (i = 0; i < local_domain.ncells_active; i++) {
        force[i].air_temp[NR] = average(force[i].air_temp, NF);
        // For precipitation put total
        force[i].prec[NR] = average(force[i].prec, NF) * NF;
        force[i].shortwave[NR] = average(force[i].shortwave, NF);
        force[i].longwave[NR] = average(force[i].longwave, NF);
        force[i].pressure[NR] = average(force[i].pressure, NF);
        force[i].wind[NR] = average(force[i].wind, NF);
        force[i].vp[NR] = average(force[i].vp, NF);
        force[i].vpd[NR] = (svp(force[i].air_temp[NR]) - force[i].vp[NR]);
        force[i].density[NR] = air_density(force[i].air_temp[NR],
                                           force[i].pressure[NR]);
        force[i].snowflag[NR] = will_it_snow(force[i].air_temp, t_offset[i],
                                             param.SNOW_MAX_SNOW_TEMP,
                                             force[i].prec, NF);

        for (v = 0; v < options.NVEGTYPES; v++) {
            vidx = veg_con_map[i].vidx[v];
            if (vidx != NODATA_VEG) {
                // not the correct way to calculate average albedo in general,
                // but leave for now (it's correct if albedo is constant over
                // the model step)
                veg_hist[i][vidx].albedo[NR] = average(veg_hist[i][vidx].albedo,
                                                       NF);
                veg_hist[i][vidx].displacement[NR] = average(
                    veg_hist[i][vidx].displacement, NF);
                veg_hist[i][vidx].fcanopy[NR] = average(
                    veg_hist[i][vidx].fcanopy, NF);
                veg_hist[i][vidx].LAI[NR] = average(veg_hist[i][vidx].LAI, NF);
                veg_hist[i][vidx].roughness[NR] = average(
                    veg_hist[i][vidx].roughness, NF);
            }
        }

        // Optional inputs
        if (options.LAKES) {
            force[i].channel_in[NR] = average(force[i].channel_in, NF) * NF;
        }
        if (options.CARBON) {
            force[i].Catm[NR] = average(force[i].Catm, NF);
            force[i].fdir[NR] = average(force[i].fdir, NF);
            force[i].par[NR] = average(force[i].par, NF);
            // for coszen, use value at noon
            force[i].coszen[NR] = compute_coszen(
                local_domain.locations[i].latitude,
                local_domain.locations[i].longitude, soil_con[i].time_zone_lng,
                dmy[current].day_in_year, SEC_PER_DAY / 2);
        }
        else {
            force[i].Catm[NR] = average(force[i].Catm, NF);
        }
    }


    // cleanup
    free(dvar);
    free(t_offset);

    plugin_force();
}

/******************************************************************************
 * @brief    Determine timestep and start year, month, day, and seconds of forcing files
 *****************************************************************************/
void
get_forcing_file_info(param_set_struct *param_set,
                      size_t            file_num)
{
    extern global_param_struct global_param;
    extern filenames_struct    filenames;

    double                     nc_times[MONTHS_PER_YEAR];
    double                     nc_time_origin;
    size_t                     start = 0;
    size_t                     count = 2;
    size_t                     ntimes;
    size_t                     m;
    char                      *nc_unit_chars = NULL;
    char                      *calendar_char = NULL;
    unsigned short int         time_units;
    unsigned short int         calendar;
    dmy_struct                 nc_origin_dmy;
    dmy_struct                 nc_start_dmy;
    dmy_struct                 nc_rec_dmy;
    bool                       monthly;

    monthly = (global_param.forcefreq[file_num] == FORCE_FREQ_MONTH);

    if (monthly) {
        // A monthly file must hold exactly one full calendar year, so that the
        // record index is the zero-based calendar month with no negotiation.
        ntimes = get_nc_dimension(&(filenames.forcing[file_num]), "time");
        if (ntimes != MONTHS_PER_YEAR) {
            log_err("Monthly forcing file %s has %zu time records; exactly %d "
                    "(January through December) are required",
                    filenames.forcing[file_num].nc_filename, ntimes,
                    MONTHS_PER_YEAR);
        }
        count = MONTHS_PER_YEAR;
    }

    // read time info from netcdf file
    get_nc_field_double(&(filenames.forcing[file_num]), "time", &start, &count,
                        nc_times);
    get_nc_var_attr(&(filenames.forcing[file_num]), "time", "units",
                    &nc_unit_chars);
    get_nc_var_attr(&(filenames.forcing[file_num]), "time", "calendar",
                    &calendar_char);

    // parse the calendar string and check to make sure it matches the global clock
    calendar = str_to_calendar(calendar_char);

    // parse the time units
    parse_nc_time_units(nc_unit_chars, &time_units, &nc_origin_dmy);

    // Get date/time of the first entry in the forcing file.
    nc_time_origin =
        date2num(0., &nc_origin_dmy, 0., calendar, TIME_UNITS_DAYS);
    num2date(nc_time_origin, nc_times[0], 0., calendar, time_units,
             &nc_start_dmy);

    // Assign file start date/time
    global_param.forceyear[file_num] = nc_start_dmy.year;
    global_param.forcemonth[file_num] = nc_start_dmy.month;
    global_param.forceday[file_num] = nc_start_dmy.day;
    global_param.forcesec[file_num] = nc_start_dmy.dayseconds;

    if (monthly) {
        // Validate the contract: twelve records, January through December of a
        // single year, in order.  Checking each record's decoded month at once
        // covers duplicated, missing and out-of-order records, and does so
        // without assuming any particular month length (the decoding is done
        // by the calendar-aware num2date).
        for (m = 0; m < MONTHS_PER_YEAR; m++) {
            num2date(nc_time_origin, nc_times[m], 0., calendar, time_units,
                     &nc_rec_dmy);
            if (nc_rec_dmy.year != nc_start_dmy.year ||
                nc_rec_dmy.month != (unsigned short int) (m + 1)) {
                log_err("Monthly forcing file %s must contain the twelve "
                        "months of year %hu in order; record %zu decodes to "
                        "%04hu-%02hu.  Duplicated, missing or out-of-order "
                        "records are not allowed",
                        filenames.forcing[file_num].nc_filename,
                        nc_start_dmy.year, m, nc_rec_dmy.year,
                        nc_rec_dmy.month);
            }
        }

        // Monthly files have no meaningful "steps per day".  Leaving this at
        // zero also makes make_dmy() skip its forceskip calculation, which is
        // only valid for fixed-length forcing intervals; the monthly record
        // index is derived from the date instead (see monthly_force_index).
        param_set->force_steps_per_day[file_num] = 0;
    }
    else {
        // calculate timestep in forcing file
        if (time_units == TIME_UNITS_DAYS) {
            param_set->force_steps_per_day[file_num] =
                (size_t) nearbyint(1. / (nc_times[1] - nc_times[0]));
        }
        else if (time_units == TIME_UNITS_HOURS) {
            param_set->force_steps_per_day[file_num] =
                (size_t) nearbyint(HOURS_PER_DAY / (nc_times[1] - nc_times[0]));
        }
        else if (time_units == TIME_UNITS_MINUTES) {
            param_set->force_steps_per_day[file_num] =
                (size_t) nearbyint(MIN_PER_DAY / (nc_times[1] - nc_times[0]));
        }
        else if (time_units == TIME_UNITS_SECONDS) {
            param_set->force_steps_per_day[file_num] =
                (size_t) nearbyint(SEC_PER_DAY / (nc_times[1] - nc_times[0]));
        }

        // check that this forcing file will work
        if (param_set->force_steps_per_day[file_num] !=
            global_param.snow_steps_per_day) {
            log_err("Forcing file timestep must match the snow model timestep. "
                    " Snow model timesteps per day is set to %zu and the "
                    "forcing file timestep is set to %zu",
                    global_param.snow_steps_per_day,
                    param_set->force_steps_per_day[file_num])
        }
    }
    if (calendar != global_param.calendar) {
        log_err("Calendar in forcing file (%s) does not match the calendar of "
                "VIC's clock", calendar_char);
    }

    // Free attribute character arrays
    free(nc_unit_chars);
    free(calendar_char);
}
