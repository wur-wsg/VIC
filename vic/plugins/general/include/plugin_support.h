/******************************************************************************
 * @section DESCRIPTION
 *
 * Plugin support header file
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

#ifndef PLUGIN_SUPPORT_H
#define PLUGIN_SUPPORT_H

/******************************************************************************
 * @brief   Functions
 *****************************************************************************/
void convolute(double, double *, double *, size_t, size_t);
void convolve(const double[], size_t, const double[], size_t, double[]);
void cshift(double *, int, int, int, int);
void size_t_sort(size_t *, size_t *, size_t, bool);
void size_t_sort2(size_t *, int *, size_t, bool);
void double_flip(double *, size_t);
void size_t_swap(size_t, size_t, size_t *);
void int_swap(size_t, size_t, int *);
void double_swap(size_t, size_t, double *);
void copy_dmy(dmy_struct *, dmy_struct *);
void initialize_dmy(dmy_struct *);
double between_dmy(dmy_struct, dmy_struct, dmy_struct);
double between_jday(double, double, double);
unsigned short int days_per_month(unsigned short int, unsigned short int,
                                  unsigned short int);
double array_average(double *, size_t, size_t, size_t, size_t);
void dmy_doy(double, unsigned short int, unsigned short int, dmy_struct *);

#endif /* PLUGIN_SUPPORT_H */
