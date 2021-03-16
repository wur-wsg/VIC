/******************************************************************************
 * @section DESCRIPTION
 *
 * Environmental requirements header file
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

#ifndef EFR_H
#define EFR_H

/******************************************************************************
 * @brief   Environmental requirements forcing
 *****************************************************************************/
typedef struct {
    double discharge;          /**< environmental river discharge [m3 s-1] */
    double baseflow;           /**< environmental baseflow [mm] */
    double **moist;             /**< environmental third-layer soil-moisture [mm] */
} efr_force_struct;

/******************************************************************************
 * @brief   Public structures
 *****************************************************************************/
efr_force_struct *efr_force;

/******************************************************************************
 * @brief   Functions
 *****************************************************************************/
bool efr_get_global_param(char *);
void efr_validate_global_param(void);

void efr_start(void);
void efr_alloc(void);
void efr_initialize_local_structures(void);

void efr_set_output_met_data_info(void);
void efr_history(int, unsigned int *);

void efr_forcing(void);
void efr_put_data(size_t);
void efr_finalize(void);

#endif
