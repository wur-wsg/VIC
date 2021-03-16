/******************************************************************************
 * @section DESCRIPTION
 *
 * Function to average array values with repetitions, lengths, offsets and skips
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

/******************************************************************************
 * @brief   Function to average array values with repetitions, lengths, offsets and skips
 *****************************************************************************/
double
array_average(double *array,
              size_t  repetitions,
              size_t  length,
              size_t  offset,
              size_t  skip)
{
    size_t i;
    size_t j;

    double average = 0.0;

    if (repetitions == 0 || length == 0) {
        return average;
    }

    for (i = 0; i < repetitions; i++) {
        for (j = 0; j < length; j++) {
            average += array[offset + i * (length + skip) + j];
        }
    }
    average /= (repetitions * length);

    return average;
}
