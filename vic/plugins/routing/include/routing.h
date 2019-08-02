/******************************************************************************
 * @section DESCRIPTION
 *
 * Routing header file
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

#ifndef ROUTING_H
#define ROUTING_H

#define MAX_UPSTREAM 8          /**< maximum number of upstream cells */

/******************************************************************************
 * @brief   Basin structure
 *****************************************************************************/
typedef struct {
    int *basin_map;             /**< basin grid-map */
    size_t *sorted_basins;      /**< sorted basin ids */
    size_t Nbasin;              /**< number of basins */
    size_t *Ncells;             /**< number of basin cells per basin */
    size_t **catchment;         /**< basin cell ids per basin */
} basin_struct;

/******************************************************************************
 * @brief   Routing Constants
 *****************************************************************************/
typedef struct {
    size_t downstream;          /**< downstream cell id */
    size_t Nupstream;           /**< number of upstream cells */
    size_t *upstream;           /**< upstream cell id */
    double *inflow_uh;          /**< inflow unit-hydrograph */
    double *runoff_uh;          /**< runoff unit-hydrograph */
} rout_con_struct;

/******************************************************************************
 * @brief   Routing Variables
 *****************************************************************************/
typedef struct {
    double stream;              /**< river (in-cell) stream moisture [mm] */
    double discharge;           /**< river (outflow) discharge [m3 s-1] */
    double *dt_discharge;       /**< routing sub-step discharge [m3 s-1] */
} rout_var_struct;

/******************************************************************************
 * @brief   Routing forcing
 *****************************************************************************/
typedef struct {
    double *discharge;          /**< river (inflow) discharge [m3 s-1] */
} rout_force_struct;

/******************************************************************************
 * @brief   Public structures
 *****************************************************************************/
size_t            *routing_order;
rout_var_struct   *rout_var;
rout_con_struct   *rout_con;
rout_force_struct *rout_force;

/******************************************************************************
 * @brief   Functions
 *****************************************************************************/
bool rout_get_global_param(char *);
void rout_validate_global_param(void);

void rout_mpi_map_decomp_domain(size_t, size_t, int **, int **, size_t **);

void rout_start(void);
void rout_alloc(void);
void rout_initialize_local_structures(void);
void rout_init(void);

void rout_set_nc_state_file_info(nc_file_struct *);
void rout_add_state_dim(char *, nc_file_struct *);
void rout_add_state_dim_var(char *, nc_file_struct *);
void rout_add_state_dim_var_data(char *, nc_file_struct *);
void rout_set_nc_state_var_info(nc_file_struct *, size_t);
void rout_set_output_met_data_info(void);
void rout_set_state_meta_data_info(void);
void rout_store(nc_file_struct *);

void rout_check_init_state_file(void);
void rout_restore(void);
void rout_compute_derived_state_vars(void);

void rout_history(int, unsigned int *);
void rout_forcing(void);
void rout_basin_run(size_t);
void rout_random_run(void);
void rout_put_data(size_t);
void rout_finalize(void);
void rout_add_types(void);

size_t get_downstream_global(size_t, int);
size_t get_downstream_local(size_t, int, size_t);

#endif
