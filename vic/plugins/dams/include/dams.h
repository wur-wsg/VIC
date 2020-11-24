/******************************************************************************
 * @section DESCRIPTION
 *
 * Dam header file
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

#ifndef DAMS_H
#define DAMS_H

#include <vic_driver_image.h>

#define NODATA_DAM    -1            /**< dam nodata flag */
#define DAM_HIST_YEARS 5            /**< dam history length [years] */

/******************************************************************************
 * @brief   Dam Mapping
 *****************************************************************************/
typedef struct {
    size_t nd_types;                /**< total number of dam types */
    size_t nd_active;               /**< number of active dam types */
    int *didx;                      /**< array of indices for active dam types */
} dam_con_map_struct;

/******************************************************************************
 * @brief   Dam Constants
 *****************************************************************************/
typedef struct {
    size_t dam_class;               /**< dam class id number */

    unsigned short int year;        /**< build year [year] */
    double capacity;                /**< capacity [hm3] */
    double inflow_frac;             /**< fraction of runoff/discharge used as inflow [-] */

    size_t nservice;                /**< number of service cells */
    size_t *service;                /**< service cell id */
    double *service_frac;           /**< sercive cell demand fraction used for operation [-] */
} dam_con_struct;

/******************************************************************************
 * @brief   Dam Variables
 *****************************************************************************/
typedef struct {
    bool active;                    /**< active flag */

    double inflow;                  /**< inflow [hm3] */
    double demand;                  /**< water demand [hm3] */
    double efr;                     /**< environmental requirments [hm3] */
    double release;                 /**< release [hm3] */
    double storage;                 /**< storage [hm3] */

    double history_inflow[MONTHS_PER_YEAR * DAM_HIST_YEARS];    /**< historical inflow array [hm3] */
    double history_demand[MONTHS_PER_YEAR * DAM_HIST_YEARS];    /**< historical demand array [hm3] */
    double history_efr[MONTHS_PER_YEAR * DAM_HIST_YEARS];       /**< historical environmental requirment array [hm3] */
    double op_release[MONTHS_PER_YEAR];                         /**< calculated release [hm3] */
    double op_storage[MONTHS_PER_YEAR];                         /**< calculated storage [hm3] */

    double total_inflow;            /**< accumulated inflow [hm3] */
    double total_demand;            /**< accumulated demand [hm3] */
    double total_efr;               /**< accumulated environmental requirments [hm3] */
    size_t register_steps;          /**< number of accumulation steps */

    int op_month;                   /**< start of the operational year [month] */
    size_t months_running;          /**< number of months history is registered */
} dam_var_struct;

/******************************************************************************
 * @brief   Public structures
 *****************************************************************************/
dam_con_map_struct *global_dam_con_map;
dam_var_struct    **global_dam_var;
dam_con_struct    **global_dam_con;
dam_con_map_struct *local_dam_con_map;
dam_var_struct    **local_dam_var;
dam_con_struct    **local_dam_con;

/******************************************************************************
 * @brief   Functions
 *****************************************************************************/
bool dam_get_global_param(char *);
void dam_validate_global_param(void);
bool dam_get_parameters(char *);
void dam_validate_parameters(void);
void dam_start(void);

void dam_alloc(void);
void dam_initialize_local_structures(void);

void dam_init(void);
void dam_generate_default_state(void);
void dam_compute_derived_state_vars(void);

void dam_set_output_met_data_info(void);
void dam_initialize_nc_file(nc_file_struct *);
void dam_add_hist_dim(nc_file_struct *, stream_struct *);
void dam_set_nc_var_info(unsigned int, nc_file_struct *, nc_var_struct *);
void dam_set_nc_var_dimids(unsigned int, nc_file_struct *, nc_var_struct *);
void dam_history(unsigned int, unsigned int *);
void dam_put_data(size_t);


void local_dam_register(dam_con_struct *, dam_var_struct *, size_t);
void global_dam_register(dam_con_struct *, dam_var_struct *, size_t);
void dam_calc_opt_release(double *, double *, double *, double *, size_t);
void dam_corr_opt_release(double *, double *, size_t, double, double);
double dam_corr_release(double, double, double);
double dam_calc_k_factor(double, double);
double dam_calc_c_factor(double *, double, size_t, size_t *);
void dam_calc_opt_storage(double *, double *, double *, size_t, size_t*,
                          double);
double dam_area(double, double, double, double);
double dam_height(double, double);
void local_dam_run(size_t);
void global_dam_run(size_t);

void dam_finalize(void);

#endif
