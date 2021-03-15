/******************************************************************************
 * @section DESCRIPTION
 *
 * Function to convolute a single quantity over an array (used in routing)
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
 * @brief   Function to convolute a single quantity over an array
 *****************************************************************************/
void
convolute(double  quantity,
          double *function,
          double *output,
          size_t  length,
          size_t  offset)
{
    size_t i;

    for (i = 0; i < length; i++) {
        output[offset + i + 1] += quantity * function[i];
    }
}

/******************************************************************************
 * @brief   Function to convolute two arrays
 *****************************************************************************/
void
convolve(const double Signal[] /* SignalLen */,
         size_t       SignalLen,
         const double Kernel[] /* KernelLen */,
         size_t       KernelLen,
         double       Result[] /* SignalLen + KernelLen - 1 */)
{
    size_t n, kmin, kmax, k;

    for (n = 0; n < SignalLen + KernelLen - 1; n++) {
        Result[n] = 0;

        kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
        kmax = (n < SignalLen - 1) ? n : SignalLen - 1;

        for (k = kmin; k <= kmax; k++) {
            Result[n] += Signal[k] * Kernel[n - k];
        }
    }
}
