/******************************************************************************
 * @section DESCRIPTION
 *
 * Register dam history and operation
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
* @brief   Calculate the operational year. This function is called from global_dam_register when a new year starts.
******************************************/
void
dam_register_operation_start(dam_var_struct *dam_var)
{
    extern dmy_struct *dmy;
    extern size_t      current;

    double             inflow[MONTHS_PER_YEAR];
    double             inflow_avg;
    double             inflow_cum;
    double             inflow_cum_max;
    size_t             month;
    size_t             month_max;

    size_t             i;
    size_t             years_running;

    years_running = (size_t)(dam_var->months_running / MONTHS_PER_YEAR);
    // limit to dam hist years [5].
    if (years_running > DAM_HIST_YEARS) {
        years_running = DAM_HIST_YEARS;
    }

    // Setup arrays
    for (i = 0; i < MONTHS_PER_YEAR; i++) {
        // calculate average montly inflow of the historical years.
        // Since this main routine only runs at the start of a new year. The fist element is December.
        inflow[i] = array_average(dam_var->history_inflow,
                                  years_running, 1, i, MONTHS_PER_YEAR - 1);
    }

    // reverse inflow array. Ascending order. First value is now Janurary and second February.
    double_flip(inflow, MONTHS_PER_YEAR);

    // Calculate yearly average
    inflow_avg = 0.0;
    for (i = 0; i < MONTHS_PER_YEAR; i++) {
        inflow_avg += inflow[i];
    }
    inflow_avg /= MONTHS_PER_YEAR;

    // Calculate operational month
    inflow_cum = 0.0;
    inflow_cum_max = 0.0;
    month_max = 0;
    // Loop 24 months in the revered inflow array (historical months -> current month).
    // If the monthly average inflow drops below yearly average the dam should operate next month
    for (i = 0; i < 2 * MONTHS_PER_YEAR; i++) {
        // index of month in loop from current month to next 24 months.
        // Since the main rountine only runs in January. The first element in loop is month 0.
        month = (dmy[current].month - 1 + i) % MONTHS_PER_YEAR;
        // accumulate inflow when monthly inflow larger than average
        //
        if (inflow[month] > inflow_avg) {
            inflow_cum += inflow[month];
            // compare to accumulated inflow for month_max
            if (inflow_cum > inflow_cum_max) {
                inflow_cum_max = inflow_cum;

                // Add a month since the start is when the inflow drops
                // below the average
                month_max = month + 1;
            }
        }
        // otherwise reset
        else {
            inflow_cum = 0.0;
        }
    }

    // Convert index to month number
    month_max++;
    if (month_max > MONTHS_PER_YEAR) {
        month_max -= MONTHS_PER_YEAR;
    }

    dam_var->op_month = month_max;
}

/******************************************
* @brief   Calculate the optimal release and storage (based on history)
******************************************/
void
dam_register_operation(dam_con_struct *dam_con,
                       dam_var_struct *dam_var)
{
    extern dmy_struct         *dmy;
    extern size_t              current;
    extern global_param_struct global_param;

    double                     inflow[MONTHS_PER_YEAR];
    double                     demand[MONTHS_PER_YEAR];
    double                     efr[MONTHS_PER_YEAR];
    double                     release[MONTHS_PER_YEAR];
    double                     storage[MONTHS_PER_YEAR];
    size_t                     steps_per_month[MONTHS_PER_YEAR];
    double                     d_steps_per_month[MONTHS_PER_YEAR];
    double                     k;
    double                     c;

    size_t                     i;
    size_t                     years_running;

    years_running = (size_t)(dam_var->months_running / MONTHS_PER_YEAR);
    if (years_running > DAM_HIST_YEARS) {
        years_running = DAM_HIST_YEARS;
    }
    // calculates the simulation step for each month in current year starting from January.
    for (i = 0; i < MONTHS_PER_YEAR; i++) {
        steps_per_month[i] =
            days_per_month(i + 1, dmy[current].year, global_param.calendar) *
            global_param.model_steps_per_day;
    }

    // Calculates the average of each calendar month from the past 5 years monthly historical data.
    // First element in history is the past month. Descending months in array.
    for (i = 0; i < MONTHS_PER_YEAR; i++) {
        inflow[i] = array_average(dam_var->history_inflow,
                                  years_running, 1, i, MONTHS_PER_YEAR - 1);
        demand[i] = array_average(dam_var->history_demand,
                                  years_running, 1, i, MONTHS_PER_YEAR - 1);
        efr[i] = array_average(dam_var->history_efr,
                               years_running, 1, i, MONTHS_PER_YEAR - 1);
    }

    // Reverse arrays. First element is current month. Ascending months in array.
    double_flip(inflow, MONTHS_PER_YEAR);
    double_flip(demand, MONTHS_PER_YEAR);
    double_flip(efr, MONTHS_PER_YEAR);

    // cast to doubles. prep for cshift
    for (i = 0; i < MONTHS_PER_YEAR; i++) {
        d_steps_per_month[i] = (double) steps_per_month[i];
    }

    // shift steps_per_month so it start at the current month.
    for (i = 1; i < dmy[current].month; i++) {
        cshift(d_steps_per_month, MONTHS_PER_YEAR, 1, 0, 1);
    }

    // cast back to integers
    for (i = 0; i < MONTHS_PER_YEAR; i++) {
        steps_per_month[i] = (size_t) d_steps_per_month[i];
    }

    // Calculate dam release
    dam_calc_opt_release(inflow, demand, efr, release, MONTHS_PER_YEAR);
    k = dam_calc_k_factor(dam_con->capacity, dam_var->storage);
    c = dam_calc_c_factor(inflow, dam_con->capacity, MONTHS_PER_YEAR,
                          steps_per_month);
    dam_corr_opt_release(inflow, release, MONTHS_PER_YEAR, k, c);

    for (i = 0; i < MONTHS_PER_YEAR; i++) {
        dam_var->op_release[i] = release[i];
    }

    // Calculate dam volume
    dam_calc_opt_storage(inflow, release, storage, MONTHS_PER_YEAR,
                         steps_per_month, dam_var->storage);
    for (i = 0; i < MONTHS_PER_YEAR; i++) {
        dam_var->op_storage[i] = storage[i];
    }
}

/******************************************
* @brief   Register history
******************************************/
void
dam_register_history(dam_var_struct *dam_var)
{
    dam_var->months_running++;

    // Shift array
    cshift(dam_var->history_demand, 1, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, -1); // shift backwards
    cshift(dam_var->history_inflow, 1, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, -1);
    cshift(dam_var->history_efr, 1, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, -1);
    cshift(dam_var->op_release, MONTHS_PER_YEAR, 1, 0, 1); // shift forwards
    cshift(dam_var->op_storage, MONTHS_PER_YEAR, 1, 0, 1);

    // Store last monthly average -> so if dmy.month = 5 history_xxx[0] -> month 4.
    dam_var->history_inflow[0] = dam_var->total_inflow /
                                 dam_var->register_steps;
    dam_var->history_demand[0] = dam_var->total_demand /
                                 dam_var->register_steps;
    dam_var->history_efr[0] = dam_var->total_efr / dam_var->register_steps;

    dam_var->total_inflow = 0.0;
    dam_var->total_demand = 0.0;
    dam_var->total_efr = 0.0;
    dam_var->register_steps = 0;
}

/******************************************
* @brief   Register demand
******************************************/
void
dam_register_demand(dam_con_struct *dam_con,
                    dam_var_struct *dam_var,
                    size_t          iCell)
{
    extern domain_struct      local_domain;
    extern wu_var_struct    **wu_var;
    extern wu_con_map_struct *wu_con_map;

    size_t                    demand_cell;
    double                    demand_fraction;

    size_t                    iService;
    size_t                    iSector;

    dam_var->demand = 0.0;

    for (iService = 0; iService < dam_con->nservice; iService++) {
        demand_cell = dam_con->service[iService];
        demand_fraction = dam_con->service_frac[iService];

        for (iSector = 0;
             iSector < wu_con_map[demand_cell].ns_active;
             iSector++) {
            dam_var->demand += wu_var[demand_cell][iSector].demand_surf *
                               local_domain.locations[iCell].area *
                               demand_fraction / MM_PER_M / M3_PER_HM3;
        }
    }
}

/******************************************
* @brief   Register environmental requirements
******************************************/
void
dam_register_efr(dam_con_struct *dam_con,
                 dam_var_struct *dam_var,
                 size_t          iCell)
{
    extern global_param_struct global_param;
    extern efr_force_struct   *efr_force;

    dam_var->efr = efr_force[iCell].discharge * global_param.dt *
                   dam_con->inflow_frac / M3_PER_HM3;
}

/******************************************
* @brief   Register global inflow (discharge)
******************************************/
void
global_dam_register_inflow(dam_con_struct *dam_con,
                           dam_var_struct *dam_var,
                           size_t          iCell)
{
    extern global_param_struct global_param;
    extern rout_var_struct    *rout_var;

    dam_var->inflow = rout_var[iCell].discharge * global_param.dt *
                      dam_con->inflow_frac / M3_PER_HM3;
}

/******************************************
* @brief   Register local inflow (runoff)
******************************************/
void
local_dam_register_inflow(dam_con_struct *dam_con,
                          dam_var_struct *dam_var,
                          size_t          iCell)
{
    extern domain_struct local_domain;
    extern double     ***out_data;

    dam_var->inflow = out_data[iCell][OUT_RUNOFF][0] *
                      local_domain.locations[iCell].area *
                      dam_con->inflow_frac / M3_PER_HM3 / MM_PER_M;
}

/******************************************
* @brief   Register local dam
******************************************/
void
local_dam_register(dam_con_struct *dam_con,
                   dam_var_struct *dam_var,
                   size_t          iCell)
{
    extern dmy_struct *dmy;
    extern size_t      current;

    if (current > 0) {
        if (dmy[current].month != dmy[current - 1].month) {
            // Set dam active
            if (dmy[current].year >= dam_con->year) {
                dam_var->active = true;
            }
            dam_var->months_running++;
        }
    }

    local_dam_register_inflow(dam_con, dam_var, iCell);
}

/******************************************
* @brief   Register global dam
******************************************/
void
global_dam_register(dam_con_struct *dam_con,
                    dam_var_struct *dam_var,
                    size_t          iCell)
{
    extern plugin_option_struct plugin_options;
    extern option_struct        options;
    extern global_param_struct  global_param;
    extern dmy_struct          *dmy;
    extern size_t               current;

    dmy_struct                  prev_dmy;
    dmy_struct                  init_dmy;
    double                      offset;
    double                      init_time;

    if (current > 0) {
        prev_dmy = dmy[current - 1];
    }
    else if (options.INIT_STATE) {
        offset = global_param.dt / (double) SEC_PER_DAY;
        init_dmy.year = global_param.inityear;
        init_dmy.month = global_param.initmonth;
        init_dmy.day = global_param.initday;
        init_dmy.dayseconds = global_param.initsec;

        init_time = date2num(global_param.time_origin_num, &init_dmy, 0,
                             global_param.calendar, TIME_UNITS_DAYS);
        // This should be the last timestamp from simulation which produced the restart file, but assumes identical temporal resolution.
        num2date(global_param.time_origin_num, (init_time - offset), 0,
                 global_param.calendar, TIME_UNITS_DAYS, &prev_dmy);
    }

    if (current > 0 || (current == 0 && options.INIT_STATE)) {
        if (dmy[current].month != prev_dmy.month) {
            dam_register_history(dam_var);
            // set the operational month of the new year.
            if (dmy[current].year != prev_dmy.year) {
                dam_register_operation_start(dam_var);
            }
            if (dmy[current].month == dam_var->op_month) {
                // Set dam active when current year equal or larger than construction year
                if (dmy[current].year >= dam_con->year) {
                    dam_var->active = true;
                }
                // Calculate the optimal release and storage. Only calculated once a year when the dam starts to operate.
                dam_register_operation(dam_con, dam_var);
            }
        }
    }

    global_dam_register_inflow(dam_con, dam_var, iCell);

    if (plugin_options.WATERUSE) {
        dam_register_demand(dam_con, dam_var, iCell);
    }
    if (plugin_options.EFR) {
        dam_register_efr(dam_con, dam_var, iCell);
    }

    dam_var->total_inflow += dam_var->inflow;
    dam_var->total_demand += dam_var->demand;
    dam_var->total_efr += dam_var->efr;
    dam_var->register_steps++;
}
