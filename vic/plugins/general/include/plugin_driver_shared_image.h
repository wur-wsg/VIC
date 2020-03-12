/******************************************************************************
 * @section DESCRIPTION
 *
 * Header file for plugin_driver_shared_image routines
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

#ifndef PLUGIN_DRIVER_SHARED_IMAGE_H
#define PLUGIN_DRIVER_SHARED_IMAGE_H

#include <vic_driver_shared_image.h>
#include <plugin_mpi.h>
#include <plugin_def.h>

/******************************************************************************
 * @brief   Plugin filenames
 *****************************************************************************/
typedef struct {
    nameid_struct routing;          /**< routing parameter file */
    nameid_struct decomposition;    /**< decomposition parameter file */
    nameid_struct dams;             /**< dams parameter file */
    nameid_struct wateruse;         /**< water-use parameter file */
    nameid_struct irrigation;       /**< irrigation parameter file */

    nameid_struct forcing[PLUGIN_N_FORCING_TYPES];      /**< forcing files */
    char f_path_pfx[PLUGIN_N_FORCING_TYPES][MAXSTRING]; /**< path and prefix for forcing files */
    char f_varname[PLUGIN_N_FORCING_TYPES][MAXSTRING];  /**< variable name for forcing files */
} plugin_filenames_struct;

/******************************************************************************
 * @brief   Plugin save data
 *****************************************************************************/
typedef struct {
    double total_moist_storage;     /**< total moisture storage [mm] */
} plugin_save_data_struct;

/******************************************************************************
 * @brief   Public structures
 *****************************************************************************/
plugin_filenames_struct plugin_filenames;
plugin_save_data_struct *plugin_save_data;

/******************************************************************************
 * @brief   Functions
 *****************************************************************************/
// output
void plugin_set_output_met_data_info(void);
void plugin_initialize_nc_file(nc_file_struct *nc_file);
void plugin_add_hist_dim(nc_file_struct *nc, stream_struct  *stream);
void plugin_set_nc_var_info(unsigned int varid, unsigned short int dtype,
                            nc_file_struct    *nc_hist_file,
                            nc_var_struct     *nc_var);
void plugin_set_nc_var_dimids(unsigned int varid, nc_file_struct *nc_hist_file,
                              nc_var_struct  *nc_var);
void plugin_get_default_outvar_aggtype(unsigned int varid, unsigned int *);
void plugin_store_error(size_t);
void plugin_put_data(void);

// states
void plugin_set_state_meta_data_info(void);
void plugin_set_nc_state_file_info(nc_file_struct *nc_state_file);
void plugin_add_state_dim(char *filename, nc_file_struct *nc_state_file);
void plugin_add_state_dim_var(char *filename, nc_file_struct *nc_state_file);
void plugin_add_state_dim_var_data(char           *filename,
                                   nc_file_struct *nc_state_file);
void plugin_set_nc_state_var_info(nc_file_struct *nc);
void plugin_store(nc_file_struct *);

#endif /* PLUGIN_DRIVER_SHARED_IMAGE_H */
