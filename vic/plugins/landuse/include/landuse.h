/******************************************************************************
 * @section DESCRIPTION
 *
 * Land-use header file
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

#ifndef LANDUSE_H
#define LANDUSE_H

/***** Define minimum values for model parameters *****/
#define MINCOVERAGECHANGE    0.00001  /**< Minimum vegetation coverage change with which model can work (-) */

/******************************************************************************
 * @brief   Landuse forcing
 *****************************************************************************/
typedef struct {
    double *Cv; /**< vegetation coverage per vegatation type [-] */
    double *veg_class; /**< vegetation class */
} lu_force_struct;

/******************************************************************************
 * @brief   Public structures
 *****************************************************************************/
lu_force_struct *lu_force;

/******************************************************************************
 * @brief   Functions
 *****************************************************************************/
bool lu_get_global_param(char *);
void lu_validate_global_param(void);
void lu_start(void);
void lu_alloc(void);
void lu_initialize_local_structures(void);
void lu_forcing(void);
void lu_apply(void);

void get_heat_capacities(size_t, size_t, double *, double *, double **, 
        double *);
void calculate_derived_water_states(size_t, size_t);
void calculate_derived_energy_states(size_t, size_t, double *);
double calculate_total_water(size_t, size_t, double *, double *);
double calculate_total_carbon(size_t, size_t, double *, double *);
double calculate_total_energy(size_t, double *, double *, double **, double *, double *);
void get_energy_terms(size_t, size_t, double *, double *, double **, double *,
                 double *, double **);

void lu_set_output_met_data_info(void);
void lu_set_nc_var_info(unsigned int, nc_file_struct *, nc_var_struct *);
void lu_set_nc_var_dimids(unsigned int, nc_file_struct *, nc_var_struct *);
void lu_history(int, unsigned int *);
void lu_put_data(size_t);

void lu_put_data(size_t);
void lu_finalize(void);

#endif
