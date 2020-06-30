/******************************************************************************
 * @section DESCRIPTION
 *
 * Irrigation header file
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

#ifndef IRRIGATION_H
#define IRRIGATION_H

/******************************************************************************
 * @brief   Irrigation Mapping
 *****************************************************************************/
typedef struct {
    size_t ni_types;                /**< total number of irrigation types */
    size_t ni_active;               /**< number of active irrigation types */

    int *vidx;                      /**< array of indices for active irrigation types */
    int *iidx;                      /**< array of indices for active vegetation types */
} irr_con_map_struct;

/******************************************************************************
 * @brief   Irrigation Constants
 *****************************************************************************/
typedef struct {
    int irr_class;                  /**< irrigation class id number */
    int veg_class;                  /**< vegetation class id number */
    int veg_index;                  /**< vegetation index number */

    bool paddy;                     /**< paddy irrigation flag */
    double groundwater_fraction;    /**< irrigation groundwater fraction [-] */
    double irrigation_efficiency;   /**< irrigation efficiency [mm mm-1] */
} irr_con_struct;

/******************************************************************************
 * @brief   Irrigation Variable
 *****************************************************************************/
typedef struct {
    double received;                /**< received irrigation water [mm] */
    double leftover;                /**< leftover irrigation water [mm] */
    double applied;                 /**< applied irrigation water [mm] */

    double shortage;                /**< shortage (below critical soil moisture point) [mm] */
    double requirement;             /**< requirement (between field capacity and critical soil moisture point) [mm] */
    bool flag_req;                  /**< requires irrigation flag */
} irr_var_struct;

/******************************************************************************
 * @brief   Public structures
 *****************************************************************************/
irr_var_struct   ***irr_var;
irr_con_struct    **irr_con;
irr_con_map_struct *irr_con_map;

/******************************************************************************
 * @brief   Functions
 *****************************************************************************/
bool irr_get_global_param(char *);
void irr_validate_global_param(void);
bool irr_get_parameters(char *);
void irr_validate_parameters(void);

void irr_mpi_map_decomp_domain(size_t, size_t, int **, int **, size_t **);

void irr_start(void);
void irr_late_alloc(void);
void irr_initialize_local_structures(void);
void irr_init(void);

void irr_set_output_met_data_info(void);
void irr_history(int, unsigned int *);
void irr_put_data(size_t);
void irr_run_requirement(size_t);
void irr_run_shortage(size_t);
void irr_set_demand(size_t);
void irr_get_withdrawn(size_t);
void irr_finalize(void);

#endif
