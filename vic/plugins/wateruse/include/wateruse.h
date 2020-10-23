/******************************************************************************
 * @section DESCRIPTION
 *
 * Water-use header file
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

#ifndef WATERUSE_H
#define WATERUSE_H

#include <vic_driver_image.h>

#define NODATA_WU              -1       /**< water-use nodata flag */
#define WU_BALANCE_ERROR_THRESH 1e-10   /**< water-use balance error threshold */

#define CONSUMP_ONLY false

/******************************************************************************
 * @brief   Water-use Input type
 *****************************************************************************/
enum {
    WU_FROM_FILE,                   /**< input comes from file */
    WU_CALCULATE,                   /**< input is calculated */
    WU_SKIP,                        /**< skip input */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    WU_NINPUT                       /**< used as a loop counter*/
};

/******************************************************************************
 * @brief   Water-use Mapping
 *****************************************************************************/
typedef struct {
    size_t ns_types;                /**< total number of sector types */
    size_t ns_active;               /**< number of active sector types */
    int *sidx;                      /**< array of indices for active sector types */
} wu_con_map_struct;

/******************************************************************************
 * @brief   Water-use Constants
 *****************************************************************************/
typedef struct {
    double pumping_capacity;        /**< pumping capacity [mm day-1] */
    size_t nreceiving;              /**< number of receiving cells */
    size_t *receiving;              /**< receiving cell id */
} wu_con_struct;

/******************************************************************************
 * @brief   Water-use Variables
 *****************************************************************************/
typedef struct {
    double available_gw;            /**< available groundwater resources [mm] */
    double available_surf;          /**< available surface water resources [mm] */
    double available_dam;           /**< available dam reservoir resources [mm] */
    double available_tremote;       /**< available remote resources (for this cell) [mm] */
    double available_remote;        /**< available remote resources (for other cells) [mm] */
    double available_nonrenew;      /**< available non-renewable resources [mm] */
    double demand_gw;               /**< demand for groundwater resources [mm] */
    double demand_surf;             /**< demand for surface water resources [mm] */
    double demand_tremote;          /**< demand for remote resources (for this cell) [mm] */
    double demand_remote;           /**< demand for remote resources (for other cells) [mm] */
    double demand_nonrenew;         /**< demand for non-renewable resources [mm] */
    double withdrawn_gw;            /**< withdrawn groundwater resources [mm] */
    double withdrawn_surf;          /**< withdrawn surface water resources [mm] */
    double withdrawn_dam;           /**< withdrawn dam reservoir resources [mm] */
    double withdrawn_tremote;       /**< withdrawn remote resources (for this cell) [mm] */
    double withdrawn_remote;        /**< withdrawn remote resources (for other cells) [mm] */
    double withdrawn_nonrenew;      /**< withdrawn non-renewable resources [mm] */
    double returned;                /**< returned water resources [mm] */
    double consumed;                /**< consumed water resources [mm] */
    
    double available_remote_tmp;    /**< available remote resources (temporary) [mm] */
    double demand_remote_tmp;       /**< demand for remote resources (temporary) [mm] */
    double withdrawn_remote_tmp;    /**< withdrawn remote resources (temporary) [mm] */
} wu_var_struct;

/******************************************************************************
 * @brief   Water-use Forcing
 *****************************************************************************/
typedef struct {
    double pumping_capacity;       /**< pumping capacity [mm day-1] */
    double demand;                 /**< water demand [mm] */
    double consumption_frac;       /**< water groundwater fraction [-] */
    double groundwater_frac;       /**< water consumption fraction [-] */
} wu_force_struct;

/******************************************************************************
 * @brief   Public structures
 *****************************************************************************/
wu_con_map_struct *wu_con_map;
wu_con_struct *wu_con;
wu_force_struct **wu_force;
wu_var_struct **wu_var;

/******************************************************************************
 * @brief   Functions
 *****************************************************************************/
bool wu_get_global_param(char *);
void wu_validate_global_param(void);
bool wu_get_parameters(char *);
void wu_validate_parameters(void);

void wu_start(void);

void wu_alloc(void);
void wu_initialize_local_structures(void);

void wu_init(void);

void wu_set_output_met_data_info(void);
void wu_initialize_nc_file(nc_file_struct *);
void wu_add_hist_dim(nc_file_struct *, stream_struct *);
void wu_set_nc_var_info(unsigned int, nc_file_struct *, nc_var_struct *);
void wu_set_nc_var_dimids(unsigned int, nc_file_struct *, nc_var_struct *);
void wu_history(unsigned int, unsigned int *);
void wu_put_data(size_t);

void wu_update_step_vars(size_t);
void wu_forcing(void);
void wu_run(size_t);
void wu_remote(size_t);
void wu_nonrenew(size_t);

void wu_finalize(void);

#endif /* WATERUSE_H */

