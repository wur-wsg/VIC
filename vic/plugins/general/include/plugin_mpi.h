/******************************************************************************
 * @section DESCRIPTION
 *
 * Plugin MPI header file
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

#ifndef PLUGIN_MPI_H
#define PLUGIN_MPI_H

#include <vic_mpi.h>

/******************************************************************************
 * @brief   Public structures
 *****************************************************************************/
MPI_Datatype plugin_mpi_global_struct_type;
MPI_Datatype plugin_mpi_filenames_struct_type;
MPI_Datatype plugin_mpi_option_struct_type;
MPI_Datatype plugin_mpi_param_struct_type;

/******************************************************************************
 * @brief   Functions
 *****************************************************************************/
void gather_double(double *, double *);
void gather_double_2d(double **, double **, int);
void gather_size_t(size_t *, size_t *);
void gather_size_t_2d(size_t **, size_t **, int);
void gather_int(int *, int *);
void gather_int_2d(int **, int **, int);
void scatter_double(double *, double *);
void scatter_double_2d(double **, double **, int);
void scatter_size_t(size_t *, size_t *);
void scatter_size_t_2d(size_t **, size_t **, int);
void scatter_int(int *, int *);
void scatter_int_2d(int **, int **, int);
int get_active_nc_field_double(nameid_struct *, char *, size_t *, size_t *,
                               double *);
int get_active_nc_field_int(nameid_struct *, char *, size_t *, size_t *, int *);

#endif /* PLUGIN_MPI_H */
