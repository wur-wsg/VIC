/******************************************************************************
 * @section DESCRIPTION
 *
 * Output and state supporting functions
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
* @brief   Initialize output metadata
******************************************/
void
plugin_set_output_met_data_info(void)
{
    extern plugin_option_struct plugin_options;
    extern metadata_struct      out_metadata[];

    size_t                      v;

    // Set missing and/or default values
    for (v = N_OUTVAR_TYPES; v < N_OUTVAR_TYPES + PLUGIN_N_OUTVAR_TYPES; v++) {
        // Set default string values
        snprintf(out_metadata[v].varname, MAXSTRING, "%s", MISSING_S);
        snprintf(out_metadata[v].long_name, MAXSTRING, "%s", MISSING_S);
        snprintf(out_metadata[v].standard_name, MAXSTRING, "%s", MISSING_S);
        snprintf(out_metadata[v].units, MAXSTRING, "%s", MISSING_S);
        snprintf(out_metadata[v].description, MAXSTRING, "%s", MISSING_S);
        // Set default number of elements
        out_metadata[v].nelem = 1;
    }

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_ROUTING_ERROR].varname,
             MAXSTRING,
             "%s", "OUT_ROUTING_ERROR");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_ROUTING_ERROR].long_name,
             MAXSTRING, "%s", "non_renewable_deficit");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_ROUTING_ERROR].standard_name,
             MAXSTRING, "%s", "non_renewable_deficit");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_ROUTING_ERROR].units, MAXSTRING,
             "%s", "mm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_ROUTING_ERROR].description,
             MAXSTRING, "%s", "non-renewable deficit");

    if (plugin_options.ROUTING) {
        rout_set_output_met_data_info();
    }
    if (plugin_options.FORCE_LANDUSE) {
        lu_set_output_met_data_info();
    }
    if (plugin_options.EFR) {
        efr_set_output_met_data_info();
    }
    if (plugin_options.DAMS) {
        dam_set_output_met_data_info();
    }
    if (plugin_options.WATERUSE) {
        wu_set_output_met_data_info();
    }
    if (plugin_options.IRRIGATION) {
        irr_set_output_met_data_info();
    }
    if (plugin_options.WOFOST) {
        crop_set_output_met_data_info();
    }
}

/******************************************
* @brief   Initialize output file dimension size & id
******************************************/
void
plugin_initialize_nc_file(nc_file_struct *nc_file)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.DAMS) {
        dam_initialize_nc_file(nc_file);
    }
    if (plugin_options.WATERUSE) {
        wu_initialize_nc_file(nc_file);
    }
    if (plugin_options.WOFOST) {
        crop_initialize_nc_file(nc_file);
    }
}

/******************************************
* @brief   Add output file dimensions
******************************************/
void
plugin_add_hist_dim(nc_file_struct *nc,
                    stream_struct  *stream)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.DAMS) {
        dam_add_hist_dim(nc, stream);
    }
    if (plugin_options.WATERUSE) {
        wu_add_hist_dim(nc, stream);
    }
    if (plugin_options.WOFOST) {
        crop_add_hist_dim(nc, stream);
    }
}

/******************************************
* @brief   Set output variable dimension sizes
******************************************/
void
plugin_set_nc_var_info(unsigned int       varid,
                       unsigned short int dtype,
                       nc_file_struct    *nc_hist_file,
                       nc_var_struct     *nc_var)
{
    extern plugin_option_struct plugin_options;

    /* Local variables */
    size_t                      i;

    // set datatype
    nc_var->nc_type = get_nc_dtype(dtype);

    for (i = 0; i < MAXDIMS; i++) {
        nc_var->nc_dimids[i] = -1;
        nc_var->nc_counts[i] = 0;
    }

    // Set the number of dimensions and the count sizes
    nc_var->nc_dims = 3;
    nc_var->nc_counts[1] = nc_hist_file->nj_size;
    nc_var->nc_counts[2] = nc_hist_file->ni_size;

    if (plugin_options.FORCE_LANDUSE) {
        lu_set_nc_var_info(varid, nc_hist_file, nc_var);
    }
    if (plugin_options.WATERUSE) {
        wu_set_nc_var_info(varid, nc_hist_file, nc_var);
    }
    if (plugin_options.WOFOST) {
        crop_set_nc_var_info(varid, nc_hist_file, nc_var);
    }
}

/******************************************
* @brief   Set output variable dimension ids
******************************************/
void
plugin_set_nc_var_dimids(unsigned int    varid,
                         nc_file_struct *nc_hist_file,
                         nc_var_struct  *nc_var)
{
    extern plugin_option_struct plugin_options;

    /* Local variables */
    size_t                      i;

    for (i = 0; i < MAXDIMS; i++) {
        nc_var->nc_dimids[i] = -1;
    }

    // Set the non-default ones
    nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
    nc_var->nc_dimids[1] = nc_hist_file->nj_dimid;
    nc_var->nc_dimids[2] = nc_hist_file->ni_dimid;

    if (plugin_options.FORCE_LANDUSE) {
        lu_set_nc_var_dimids(varid, nc_hist_file, nc_var);
    }
    if (plugin_options.WATERUSE) {
        wu_set_nc_var_dimids(varid, nc_hist_file, nc_var);
    }
    if (plugin_options.WOFOST) {
        crop_set_nc_var_dimids(varid, nc_hist_file, nc_var);
    }
}

/******************************************
* @brief   Set output variable aggregation type
******************************************/
void
plugin_get_default_outvar_aggtype(unsigned int  varid,
                                  unsigned int *agg_type)
{
    extern plugin_option_struct plugin_options;

    switch (varid) {
    case  N_OUTVAR_TYPES + OUT_ROUTING_ERROR:
        (*agg_type) = AGG_TYPE_SUM;
        break;
    }

    if (plugin_options.ROUTING) {
        rout_history(varid, agg_type);
    }
    if (plugin_options.FORCE_LANDUSE) {
        lu_history(varid, agg_type);
    }
    if (plugin_options.EFR) {
        efr_history(varid, agg_type);
    }
    if (plugin_options.DAMS) {
        dam_history(varid, agg_type);
    }
    if (plugin_options.WATERUSE) {
        wu_history(varid, agg_type);
    }
    if (plugin_options.IRRIGATION) {
        irr_history(varid, agg_type);
    }
    if (plugin_options.WOFOST) {
        crop_history(varid, agg_type);
    }
}

/******************************************
* @brief   Initialize state metadata
******************************************/
void
plugin_set_state_meta_data_info(void)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.ROUTING) {
        rout_set_state_meta_data_info();
    }
}

/******************************************
* @brief   Initialize state file dimension size & id
******************************************/
void
plugin_set_nc_state_file_info(nc_file_struct *nc_state_file)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.ROUTING) {
        rout_set_nc_state_file_info(nc_state_file);
    }
}

/******************************************
* @brief   Add state file dimensions
******************************************/
void
plugin_add_state_dim(char           *filename,
                     nc_file_struct *nc_state_file)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.ROUTING) {
        rout_add_state_dim(filename, nc_state_file);
    }
}

/******************************************
* @brief   Add state file dimensions
******************************************/
void
plugin_add_state_dim_var(char           *filename,
                         nc_file_struct *nc_state_file)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.ROUTING) {
        rout_add_state_dim_var(filename, nc_state_file);
    }
}

/******************************************
* @brief   Add state file variables
******************************************/
void
plugin_add_state_dim_var_data(char           *filename,
                              nc_file_struct *nc_state_file)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.ROUTING) {
        rout_add_state_dim_var_data(filename, nc_state_file);
    }
}

/******************************************
* @brief   Set output variable dimension sizes and ids
******************************************/
void
plugin_set_nc_state_var_info(nc_file_struct *nc)
{
    extern plugin_option_struct plugin_options;

    size_t                      i;
    size_t                      j;

    for (i = N_STATE_VARS; i < (N_STATE_VARS + PLUGIN_N_STATE_VARS); i++) {
        nc->nc_vars[i].nc_varid = i;

        nc->nc_vars[i].nc_type = NC_DOUBLE;
        for (j = 0; j < MAXDIMS; j++) {
            nc->nc_vars[i].nc_dimids[j] = -1;
            nc->nc_vars[i].nc_counts[j] = 0;
        }

        // Set the number of dimensions and dimids for each state variable
        nc->nc_vars[i].nc_dims = 2;
        nc->nc_vars[i].nc_dimids[0] = nc->nj_dimid;
        nc->nc_vars[i].nc_dimids[1] = nc->ni_dimid;
        nc->nc_vars[i].nc_counts[0] = nc->nj_size;
        nc->nc_vars[i].nc_counts[1] = nc->ni_size;

        if (plugin_options.ROUTING) {
            rout_set_nc_state_var_info(nc, i);
        }

        if (nc->nc_vars[i].nc_dims > MAXDIMS) {
            log_err("Too many dimensions specified in variable %zu", i);
        }
    }
}
