/******************************************************************************
 * @section DESCRIPTION
 *
 * Functions for swapping, sorting and flipping arrays
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

#include <stddef.h>
#include <stdbool.h>

/******************************************************************************
 * @brief   Swap size_t in array
 *****************************************************************************/
void
size_t_swap(size_t  i,
            size_t  j,
            size_t *array)
{
    size_t hold;

    hold = array[i];
    array[i] = array[j];
    array[j] = hold;
}

/******************************************************************************
 * @brief   Swap integer in array
 *****************************************************************************/
void
int_swap(size_t i,
         size_t j,
         int   *array)
{
    int hold;

    hold = array[i];
    array[i] = array[j];
    array[j] = hold;
}

/******************************************************************************
 * @brief   Swap double precision in array
 *****************************************************************************/
void
double_swap(size_t  i,
            size_t  j,
            double *array)
{
    double hold;

    hold = array[i];
    array[i] = array[j];
    array[j] = hold;
}

/******************************************************************************
 * @brief   Sort size_t array based on cost
 *****************************************************************************/
void
size_t_sort(size_t *array,
            size_t *cost,
            size_t  Nelements,
            bool    acending)
{
    size_t i, j;
    size_t tmp_cost[Nelements];

    for (i = 0; i < Nelements; i++) {
        tmp_cost[i] = cost[i];
    }

    if (acending) {
        for (i = 0; i < Nelements - 1; i++) {
            for (j = 0; j < Nelements - i - 1; j++) {
                if (tmp_cost[j] > tmp_cost[j + 1]) {
                    size_t_swap(j, j + 1, array);
                    size_t_swap(j, j + 1, tmp_cost);
                }
            }
        }
    }
    else {
        for (i = 0; i < Nelements - 1; i++) {
            for (j = 0; j < Nelements - i - 1; j++) {
                if (tmp_cost[j] < tmp_cost[j + 1]) {
                    size_t_swap(j, j + 1, array);
                    size_t_swap(j, j + 1, tmp_cost);
                }
            }
        }
    }
}

/******************************************************************************
 * @brief   Sort size_t array based on cost (integer)
 *****************************************************************************/
void
size_t_sort2(size_t *array,
             int    *cost,
             size_t  Nelements,
             bool    acending)
{
    size_t i, j;
    int    tmp_cost[Nelements];

    for (i = 0; i < Nelements; i++) {
        tmp_cost[i] = cost[i];
    }

    if (acending) {
        for (i = 0; i < Nelements - 1; i++) {
            for (j = 0; j < Nelements - i - 1; j++) {
                if (tmp_cost[j] > tmp_cost[j + 1]) {
                    size_t_swap(j, j + 1, array);
                    int_swap(j, j + 1, tmp_cost);
                }
            }
        }
    }
    else {
        for (i = 0; i < Nelements - 1; i++) {
            for (j = 0; j < Nelements - i - 1; j++) {
                if (tmp_cost[j] < tmp_cost[j + 1]) {
                    size_t_swap(j, j + 1, array);
                    int_swap(j, j + 1, tmp_cost);
                }
            }
        }
    }
}

/******************************************************************************
 * @brief   Flip double array
 *****************************************************************************/
void
double_flip(double *array,
            size_t  Nelements)
{
    size_t i, end;

    end = Nelements - 1;
    for (i = 0; i < Nelements / 2; i++) {
        double_swap(i, end, array);
        end--;
    }
}
