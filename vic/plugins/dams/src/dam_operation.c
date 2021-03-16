/******************************************************************************
 * @section DESCRIPTION
 *
 * Dam operation functions
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

#include <vic_driver_image.h>
#include <plugin.h>

/******************************************
* @brief   Calculate optimal release based on history
******************************************/
void
dam_calc_opt_release(double *inflow,
                     double *demand,
                     double *efr,
                     double *release,
                     size_t  length)
{
    // Calculates dam release similar to Hanasaki et al. 2006
    // Adjusted to account for varying environmental flow requirements
    // Make sure all inputs have the same units & length

    double inflow_avg;
    double demand_avg;

    size_t i;

    // Average and remove environmental flow requirements
    inflow_avg = 0.0;
    demand_avg = 0.0;
    for (i = 0; i < length; i++) {
        inflow_avg += inflow[i] - efr[i];
        demand_avg += demand[i];
    }
    inflow_avg /= length;
    demand_avg /= length;

    if (inflow_avg < 0) {
        inflow_avg = 0;
    }

    if (demand_avg <= 0) {
        // No water demand, reduce annual release fluctuations
        for (i = 0; i < length; i++) {
            release[i] = inflow_avg;
        }
    }
    else {
        if (demand_avg > inflow_avg) {
            // High water demand, supply only partially
            for (i = 0; i < length; i++) {
                release[i] = inflow_avg * (demand[i] / demand_avg);
            }
        }
        else {
            // Low water demand, supply all
            for (i = 0; i < length; i++) {
                release[i] = inflow_avg - demand_avg + demand[i];
            }
        }
    }

    // Add environmental flow requirements
    for (i = 0; i < length; i++) {
        release[i] += efr[i];
    }
}

/******************************************
* @brief   Correct optimal release
******************************************/
void
dam_corr_opt_release(double *inflow,
                     double *release,
                     size_t  length,
                     double  k,
                     double  c)
{
    // Calculates dam release according to Hanasaki et al. 2006
    // Dam release is adjusted based on relative dam capacity
    // Make sure all inputs have the same units & length

    size_t i;

    if (c >= 0.5) {
        // Large dam capacity, release all
        for (i = 0; i < length; i++) {
            release[i] *= k;
        }
    }
    else {
        // Small dam capacity, release fraction
        for (i = 0; i < length; i++) {
            release[i] = pow(c / 0.5, 2) * k * release[i] +
                         (1 - pow(c / 0.5, 2)) * inflow[i];
        }
    }
}

/******************************************
* @brief   Calculate k factor
******************************************/
double
dam_calc_k_factor(double capacity,
                  double cur_storage)
{
    // Calculates dam Kr factor according to Hanasaki et al. 2006
    // This factor handles inter-annual variability in storage
    // Make sure all inputs have the same units & length

    extern plugin_parameters_struct plugin_param;

    return(cur_storage / (capacity * plugin_param.DAM_ALPHA));
}

/******************************************
* @brief   Calculate c factor
******************************************/
double
dam_calc_c_factor(double *inflow,
                  double  capacity,
                  size_t  length,
                  size_t *steps)
{
    // Calculates dam c factor according to Hanasaki et al. 2006
    // This factor handles the relative capacity of the dam
    // Make sure all inputs have the same units & length

    double inflow_tot;

    size_t i;

    inflow_tot = 0.0;
    for (i = 0; i < length; i++) {
        inflow_tot += inflow[i] * steps[i];
    }

    return(capacity / inflow_tot);
}

/******************************************
* @brief   Calculate optimal storage based on optimal release
******************************************/
void
dam_calc_opt_storage(double *inflow,
                     double *release,
                     double *storage,
                     size_t  length,
                     size_t *steps,
                     double  cur_storage)
{
    double difference;

    size_t i;

    for (i = 0; i < length; i++) {
        if (i == 0) {
            storage[i] = cur_storage;
        }
        else {
            difference = (inflow[i - 1] - release[i - 1]) * steps[i - 1];
            storage[i] = storage[i - 1] + difference;
        }
    }
}

/******************************************
* @brief   Calculate generic reservoir water area
******************************************/
double
dam_area(double volume,
         double max_volume,
         double max_area,
         double max_height)
{
    // Calculate surface area based on Kaveh et al 2013

    double N = (2 * max_volume) / (max_height * max_area);
    double area = max_area * pow((volume / max_volume), ((2 - N) / N));

    if (area > max_area) {
        area = max_area;
    }

    return area;
}

/******************************************
* @brief   Calculate generic reservoir water height
******************************************/
double
dam_height(double area,
           double max_height)
{
    // Calculate dam height based on Liebe et al 2005

    double height = sqrt(area) * (1 / 19.45);

    if (height > max_height) {
        height = max_height;
    }

    return height;
}

/******************************************
* @brief   Correct dam release (to prevent overflow)
******************************************/
double
dam_corr_release(double release,
                 double cur_storage,
                 double capacity)
{
    // Calculates release correction similar to van Beek et al. 2011
    // This factor reduces release based on deviations from optimal storage

    extern global_param_struct      global_param;
    extern plugin_parameters_struct plugin_param;

    double                          difference;
    double                          frac;
    double                          corrected;

    difference = 0;
    if (cur_storage > capacity * plugin_param.DAM_ALPHA) {
        difference = cur_storage - capacity * plugin_param.DAM_ALPHA;
    }
    else if (cur_storage < capacity - capacity * plugin_param.DAM_ALPHA) {
        difference = cur_storage -
                     (capacity - capacity * plugin_param.DAM_ALPHA);
    }

    frac = 1;
    if (difference != 0) {
        frac = max(1 - cur_storage / capacity, cur_storage / capacity);
        frac = (frac - plugin_param.DAM_ALPHA) / (1 - plugin_param.DAM_ALPHA);
        frac = pow(frac, plugin_param.DAM_BETA);
    }

    corrected = release + difference * frac /
                (plugin_param.DAM_GAMMA * global_param.model_steps_per_day);

    return(max(0, corrected));
}
