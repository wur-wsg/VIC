/******************************************************************************
 * @section DESCRIPTION
 *
 * Dam output functions
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

/******************************************
* @brief   Set output metadata
******************************************/
void
dam_set_output_met_data_info(void)
{
    extern plugin_option_struct plugin_options;
    extern metadata_struct      out_metadata[];

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_INFLOW].varname,
           "OUT_LDAM_INFLOW");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_INFLOW].long_name,
           "local_dam_inflow");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_INFLOW].standard_name,
           "local_dam_inflow");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_INFLOW].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_INFLOW].description,
           "total dam inflow");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_DEMAND].varname,
           "OUT_LDAM_DEMAND");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_DEMAND].long_name,
           "local_dam_demand");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_DEMAND].standard_name,
           "local_dam_demand");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_DEMAND].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_DEMAND].description,
           "total dam demand");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_EFR].varname, "OUT_LDAM_EFR");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_EFR].long_name,
           "local_dam_efr");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_EFR].standard_name,
           "local_dam_efr");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_EFR].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_EFR].description,
           "total dam environmental flow requirement");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_STORAGE].varname,
           "OUT_LDAM_STORAGE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_STORAGE].long_name,
           "local_dam_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_STORAGE].standard_name,
           "local_dam_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_STORAGE].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_STORAGE].description,
           "final dam storage");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_RELEASE].varname,
           "OUT_LDAM_RELEASE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_RELEASE].long_name,
           "local_dam_release");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_RELEASE].standard_name,
           "local_dam_release");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_RELEASE].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_RELEASE].description,
           "total dam release");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_INFLOW].varname,
           "OUT_LDAM_HIST_INFLOW");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_INFLOW].long_name,
           "local_dam_historical_inflow");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_INFLOW].standard_name,
           "local_dam_historical_inflow");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_INFLOW].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_INFLOW].description,
           "total dam historical inflow");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_DEMAND].varname,
           "OUT_LDAM_HIST_DEMAND");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_DEMAND].long_name,
           "local_dam_historical_demand");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_DEMAND].standard_name,
           "local_dam_historical_demand");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_DEMAND].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_DEMAND].description,
           "total dam historical demand");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_EFR].varname,
           "OUT_LDAM_HIST_EFR");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_EFR].long_name,
           "local_dam_historical_efr");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_EFR].standard_name,
           "local_dam_historical_efr");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_EFR].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_HIST_EFR].description,
           "total dam historical environmental flow requirement");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_OP_STORAGE].varname,
           "OUT_LDAM_OP_STORAGE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_OP_STORAGE].long_name,
           "local_dam_operational_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_OP_STORAGE].standard_name,
           "local_dam_operational_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_OP_STORAGE].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_OP_STORAGE].description,
           "final dam operational storage");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_OP_RELEASE].varname,
           "OUT_LDAM_OP_RELEASE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_OP_RELEASE].long_name,
           "local_dam_operational_release");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_OP_RELEASE].standard_name,
           "local_dam_operational_release");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_OP_RELEASE].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_LDAM_OP_RELEASE].description,
           "total dam operational release");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_INFLOW].varname,
           "OUT_GDAM_INFLOW");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_INFLOW].long_name,
           "global_dam_inflow");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_INFLOW].standard_name,
           "global_dam_inflow");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_INFLOW].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_INFLOW].description,
           "total dam inflow");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_DEMAND].varname,
           "OUT_GDAM_DEMAND");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_DEMAND].long_name,
           "global_dam_demand");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_DEMAND].standard_name,
           "global_dam_demand");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_DEMAND].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_DEMAND].description,
           "total dam demand");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_EFR].varname, "OUT_GDAM_EFR");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_EFR].long_name,
           "global_dam_efr");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_EFR].standard_name,
           "global_dam_efr");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_EFR].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_EFR].description,
           "total dam environmental flow requirement");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_STORAGE].varname,
           "OUT_GDAM_STORAGE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_STORAGE].long_name,
           "global_dam_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_STORAGE].standard_name,
           "global_dam_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_STORAGE].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_STORAGE].description,
           "final dam storage");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_RELEASE].varname,
           "OUT_GDAM_RELEASE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_RELEASE].long_name,
           "global_dam_release");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_RELEASE].standard_name,
           "global_dam_release");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_RELEASE].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_RELEASE].description,
           "total dam release");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_INFLOW].varname,
           "OUT_GDAM_HIST_INFLOW");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_INFLOW].long_name,
           "global_dam_historical_inflow");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_INFLOW].standard_name,
           "global_dam_historical_inflow");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_INFLOW].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_INFLOW].description,
           "total dam historical inflow");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_DEMAND].varname,
           "OUT_GDAM_HIST_DEMAND");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_DEMAND].long_name,
           "global_dam_historical_demand");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_DEMAND].standard_name,
           "global_dam_historical_demand");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_DEMAND].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_DEMAND].description,
           "total dam historical demand");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_EFR].varname,
           "OUT_GDAM_HIST_EFR");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_EFR].long_name,
           "global_dam_historical_efr");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_EFR].standard_name,
           "global_dam_historical_efr");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_EFR].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_HIST_EFR].description,
           "total dam historical environmental flow requirement");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_OP_STORAGE].varname,
           "OUT_GDAM_OP_STORAGE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_OP_STORAGE].long_name,
           "global_dam_operational_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_OP_STORAGE].standard_name,
           "global_dam_operational_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_OP_STORAGE].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_OP_STORAGE].description,
           "final dam operational storage");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_OP_RELEASE].varname,
           "OUT_GDAM_OP_RELEASE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_OP_RELEASE].long_name,
           "global_dam_operational_release");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_OP_RELEASE].standard_name,
           "global_dam_operational_release");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_OP_RELEASE].units, "hm3");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_GDAM_OP_RELEASE].description,
           "total dam operational release");

    out_metadata[N_OUTVAR_TYPES +
                 OUT_LDAM_INFLOW].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_LDAM_DEMAND].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_LDAM_EFR].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_LDAM_RELEASE].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_LDAM_STORAGE].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_LDAM_HIST_INFLOW].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_LDAM_HIST_DEMAND].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_LDAM_HIST_EFR].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_LDAM_OP_STORAGE].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_LDAM_OP_RELEASE].nelem = plugin_options.NDAMTYPES;

    out_metadata[N_OUTVAR_TYPES +
                 OUT_GDAM_INFLOW].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_GDAM_DEMAND].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_GDAM_EFR].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_GDAM_RELEASE].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_GDAM_STORAGE].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_GDAM_HIST_INFLOW].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_GDAM_HIST_DEMAND].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_GDAM_HIST_EFR].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_GDAM_OP_STORAGE].nelem = plugin_options.NDAMTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_GDAM_OP_RELEASE].nelem = plugin_options.NDAMTYPES;
}

/******************************************
* @brief   Initialize output dimensions
******************************************/
void
dam_initialize_nc_file(nc_file_struct *nc_file)
{
    extern plugin_option_struct plugin_options;

    nc_file->dam_dimid = MISSING;
    nc_file->dam_size = plugin_options.NDAMTYPES;
}

/******************************************
* @brief   Set output dimensions
******************************************/
void
dam_add_hist_dim(nc_file_struct *nc,
                 stream_struct  *stream)
{
    int status;

    status = nc_def_dim(nc->nc_id, "dam_class", nc->dam_size,
                        &(nc->dam_dimid));
    check_nc_status(status, "Error defining dam class dimension in %s",
                    stream->filename);
}

/******************************************
* @brief   Set output variable dimension sizes
******************************************/
void
dam_set_nc_var_info(unsigned int    varid,
                    nc_file_struct *nc_hist_file,
                    nc_var_struct  *nc_var)
{
    switch (varid) {
    case N_OUTVAR_TYPES + OUT_LDAM_INFLOW:
    case N_OUTVAR_TYPES + OUT_LDAM_DEMAND:
    case N_OUTVAR_TYPES + OUT_LDAM_EFR:
    case N_OUTVAR_TYPES + OUT_LDAM_RELEASE:
    case N_OUTVAR_TYPES + OUT_LDAM_STORAGE:
    case N_OUTVAR_TYPES + OUT_LDAM_HIST_INFLOW:
    case N_OUTVAR_TYPES + OUT_LDAM_HIST_DEMAND:
    case N_OUTVAR_TYPES + OUT_LDAM_HIST_EFR:
    case N_OUTVAR_TYPES + OUT_LDAM_OP_RELEASE:
    case N_OUTVAR_TYPES + OUT_LDAM_OP_STORAGE:
    case N_OUTVAR_TYPES + OUT_GDAM_INFLOW:
    case N_OUTVAR_TYPES + OUT_GDAM_DEMAND:
    case N_OUTVAR_TYPES + OUT_GDAM_EFR:
    case N_OUTVAR_TYPES + OUT_GDAM_RELEASE:
    case N_OUTVAR_TYPES + OUT_GDAM_STORAGE:
    case N_OUTVAR_TYPES + OUT_GDAM_HIST_INFLOW:
    case N_OUTVAR_TYPES + OUT_GDAM_HIST_DEMAND:
    case N_OUTVAR_TYPES + OUT_GDAM_HIST_EFR:
    case N_OUTVAR_TYPES + OUT_GDAM_OP_RELEASE:
    case N_OUTVAR_TYPES + OUT_GDAM_OP_STORAGE:
        nc_var->nc_dims = 4;
        nc_var->nc_counts[1] = nc_hist_file->dam_size;
        nc_var->nc_counts[2] = nc_hist_file->nj_size;
        nc_var->nc_counts[3] = nc_hist_file->ni_size;
    }
}

/******************************************
* @brief   Set output variable dimension ids
******************************************/
void
dam_set_nc_var_dimids(unsigned int    varid,
                      nc_file_struct *nc_hist_file,
                      nc_var_struct  *nc_var)
{
    switch (varid) {
    case N_OUTVAR_TYPES + OUT_LDAM_INFLOW:
    case N_OUTVAR_TYPES + OUT_LDAM_DEMAND:
    case N_OUTVAR_TYPES + OUT_LDAM_EFR:
    case N_OUTVAR_TYPES + OUT_LDAM_RELEASE:
    case N_OUTVAR_TYPES + OUT_LDAM_STORAGE:
    case N_OUTVAR_TYPES + OUT_LDAM_HIST_INFLOW:
    case N_OUTVAR_TYPES + OUT_LDAM_HIST_DEMAND:
    case N_OUTVAR_TYPES + OUT_LDAM_HIST_EFR:
    case N_OUTVAR_TYPES + OUT_LDAM_OP_RELEASE:
    case N_OUTVAR_TYPES + OUT_LDAM_OP_STORAGE:
    case N_OUTVAR_TYPES + OUT_GDAM_INFLOW:
    case N_OUTVAR_TYPES + OUT_GDAM_DEMAND:
    case N_OUTVAR_TYPES + OUT_GDAM_EFR:
    case N_OUTVAR_TYPES + OUT_GDAM_RELEASE:
    case N_OUTVAR_TYPES + OUT_GDAM_STORAGE:
    case N_OUTVAR_TYPES + OUT_GDAM_HIST_INFLOW:
    case N_OUTVAR_TYPES + OUT_GDAM_HIST_DEMAND:
    case N_OUTVAR_TYPES + OUT_GDAM_HIST_EFR:
    case N_OUTVAR_TYPES + OUT_GDAM_OP_RELEASE:
    case N_OUTVAR_TYPES + OUT_GDAM_OP_STORAGE:
        nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
        nc_var->nc_dimids[1] = nc_hist_file->dam_dimid;
        nc_var->nc_dimids[2] = nc_hist_file->nj_dimid;
        nc_var->nc_dimids[3] = nc_hist_file->ni_dimid;
    }
}

/******************************************
* @brief   Set output aggregation types
******************************************/
void
dam_history(unsigned int  varid,
            unsigned int *agg_type)
{
    switch (varid) {
    case N_OUTVAR_TYPES + OUT_LDAM_STORAGE:
    case N_OUTVAR_TYPES + OUT_GDAM_STORAGE:
    case N_OUTVAR_TYPES + OUT_GDAM_OP_STORAGE:
    case N_OUTVAR_TYPES + OUT_LDAM_OP_STORAGE:
        (*agg_type) = AGG_TYPE_END;
        break;
    case N_OUTVAR_TYPES + OUT_LDAM_HIST_INFLOW:
    case N_OUTVAR_TYPES + OUT_LDAM_HIST_DEMAND:
    case N_OUTVAR_TYPES + OUT_LDAM_HIST_EFR:
    case N_OUTVAR_TYPES + OUT_LDAM_OP_RELEASE:
    case N_OUTVAR_TYPES + OUT_GDAM_HIST_INFLOW:
    case N_OUTVAR_TYPES + OUT_GDAM_HIST_DEMAND:
    case N_OUTVAR_TYPES + OUT_GDAM_HIST_EFR:
    case N_OUTVAR_TYPES + OUT_GDAM_OP_RELEASE:
    case N_OUTVAR_TYPES + OUT_LDAM_INFLOW:
    case N_OUTVAR_TYPES + OUT_LDAM_DEMAND:
    case N_OUTVAR_TYPES + OUT_LDAM_EFR:
    case N_OUTVAR_TYPES + OUT_LDAM_RELEASE:
    case N_OUTVAR_TYPES + OUT_GDAM_INFLOW:
    case N_OUTVAR_TYPES + OUT_GDAM_DEMAND:
    case N_OUTVAR_TYPES + OUT_GDAM_EFR:
    case N_OUTVAR_TYPES + OUT_GDAM_RELEASE:
        (*agg_type) = AGG_TYPE_SUM;
        break;
    }
}

/******************************************
* @brief   Write output
******************************************/
void
dam_put_data(size_t iCell)
{
    extern plugin_option_struct plugin_options;
    extern double            ***out_data;
    extern dam_con_map_struct  *local_dam_con_map;
    extern dam_var_struct     **local_dam_var;
    extern dam_con_map_struct  *global_dam_con_map;
    extern dam_var_struct     **global_dam_var;

    size_t                      i;
    size_t                      years_running;
    double                      inflow;
    double                      demand;
    double                      efr;

    for (i = 0; i < plugin_options.NDAMTYPES; i++) {
        if (local_dam_con_map[iCell].didx[i] != NODATA_DAM) {
            years_running =
                (size_t)(local_dam_var[iCell][i].months_running /
                         MONTHS_PER_YEAR);
            if (years_running > DAM_HIST_YEARS) {
                years_running = DAM_HIST_YEARS;
            }

            inflow = array_average(local_dam_var[iCell][i].history_inflow,
                                   years_running, 1, MONTHS_PER_YEAR - 1,
                                   MONTHS_PER_YEAR - 1);
            demand = array_average(local_dam_var[iCell][i].history_demand,
                                   years_running, 1, MONTHS_PER_YEAR - 1,
                                   MONTHS_PER_YEAR - 1);
            efr = array_average(local_dam_var[iCell][i].history_efr,
                                years_running, 1, MONTHS_PER_YEAR - 1,
                                MONTHS_PER_YEAR - 1);

            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_LDAM_INFLOW][i] =
                local_dam_var[iCell][i].inflow;
            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_LDAM_DEMAND][i] =
                local_dam_var[iCell][i].demand;
            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_LDAM_EFR][i] = local_dam_var[iCell][i].efr;

            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_LDAM_STORAGE][i] =
                local_dam_var[iCell][i].storage;
            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_LDAM_RELEASE][i] =
                local_dam_var[iCell][i].release;

            out_data[iCell][N_OUTVAR_TYPES + OUT_LDAM_HIST_INFLOW][i] = inflow;
            out_data[iCell][N_OUTVAR_TYPES + OUT_LDAM_HIST_DEMAND][i] = demand;
            out_data[iCell][N_OUTVAR_TYPES + OUT_LDAM_HIST_EFR][i] = efr;

            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_LDAM_OP_RELEASE][i] =
                local_dam_var[iCell][i].op_release[0];
            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_LDAM_OP_STORAGE][i] =
                local_dam_var[iCell][i].op_storage[0];
        }

        if (global_dam_con_map[iCell].didx[i] != NODATA_DAM) {
            years_running =
                (size_t)(global_dam_var[iCell][i].months_running /
                         MONTHS_PER_YEAR);
            if (years_running > DAM_HIST_YEARS) {
                years_running = DAM_HIST_YEARS;
            }

            inflow = array_average(global_dam_var[iCell][i].history_inflow,
                                   years_running, 1, MONTHS_PER_YEAR - 1,
                                   MONTHS_PER_YEAR - 1);
            demand = array_average(global_dam_var[iCell][i].history_demand,
                                   years_running, 1, MONTHS_PER_YEAR - 1,
                                   MONTHS_PER_YEAR - 1);
            efr = array_average(global_dam_var[iCell][i].history_efr,
                                years_running, 1, MONTHS_PER_YEAR - 1,
                                MONTHS_PER_YEAR - 1);

            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_GDAM_INFLOW][i] =
                global_dam_var[iCell][i].inflow;
            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_GDAM_DEMAND][i] =
                global_dam_var[iCell][i].demand;
            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_GDAM_EFR][i] = global_dam_var[iCell][i].efr;

            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_GDAM_STORAGE][i] =
                global_dam_var[iCell][i].storage;
            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_GDAM_RELEASE][i] =
                global_dam_var[iCell][i].release;

            out_data[iCell][N_OUTVAR_TYPES + OUT_GDAM_HIST_INFLOW][i] = inflow;
            out_data[iCell][N_OUTVAR_TYPES + OUT_GDAM_HIST_DEMAND][i] = demand;
            out_data[iCell][N_OUTVAR_TYPES + OUT_GDAM_HIST_EFR][i] = efr;

            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_GDAM_OP_RELEASE][i] =
                global_dam_var[iCell][i].op_release[0];
            out_data[iCell][N_OUTVAR_TYPES +
                            OUT_GDAM_OP_STORAGE][i] =
                global_dam_var[iCell][i].op_storage[0];
        }
    }
}
