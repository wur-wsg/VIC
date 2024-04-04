/******************************************************************************
 * @section DESCRIPTION
 *
 * Run water-use
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
* @brief   Get (cell) demand from sectors
******************************************/
void
calculate_demand_nonrenew(size_t  iCell,
                          double *demand_nonrenew)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct      **wu_var;
    extern wu_con_map_struct   *wu_con_map;

    size_t                      i;
    int                         iSector;

    for (i = 0; i < plugin_options.NWUTYPES; i++) {
        if (i == WU_ENERGY) {
            continue; // now the envery sector hasn't been calculated seperatly yet
        }

        iSector = wu_con_map[iCell].sidx[i];
        if (iSector == NODATA_WU) {
            continue;
        }

        // non-renewable
        wu_var[iCell][iSector].demand_nonrenew =
            (wu_var[iCell][iSector].demand_surf +
             wu_var[iCell][iSector].demand_gw) -
            (wu_var[iCell][iSector].withdrawn_surf +
             wu_var[iCell][iSector].withdrawn_gw +
             wu_var[iCell][iSector].withdrawn_dam +
             wu_var[iCell][iSector].withdrawn_tremote);
        if (wu_var[iCell][iSector].demand_nonrenew < 0.) {
            wu_var[iCell][iSector].demand_nonrenew = 0.;
        }
        (*demand_nonrenew) += wu_var[iCell][iSector].demand_nonrenew;
    }
}

/******************************************
* @brief   Get (cell) availability
******************************************/
void
calculate_availability_nonrenew(size_t  iCell,
                                double *available_nonrenew)
{
    extern plugin_parameters_struct plugin_param;
    extern rout_var_struct         *rout_var;

    (*available_nonrenew) = plugin_param.NREN_LIM -
                            rout_var[iCell].nonrenew_deficit;

    if ((*available_nonrenew) < 0) {
        (*available_nonrenew) = 0;
    }
}

/******************************************
* @brief   Divide available resources over sectors
******************************************/
void
calculate_division_nonrenew(size_t iCell,
                            double available_nonrenew,
                            double demand_nonrenew)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct      **wu_var;
    extern wu_con_map_struct   *wu_con_map;

    double                      available_nonrenew_tmp;

    size_t                      i;
    int                         iSector;

    available_nonrenew_tmp = available_nonrenew;

    for (i = 0; i < plugin_options.NWUTYPES; i++) {
        iSector = wu_con_map[iCell].sidx[i];
        if (iSector == NODATA_WU) {
            continue;
        }

        // non-renewable
        if (demand_nonrenew > 0) {
            if (wu_var[iCell][iSector].demand_nonrenew >=
                available_nonrenew_tmp) {
                wu_var[iCell][iSector].available_nonrenew =
                    available_nonrenew_tmp;

                available_nonrenew_tmp = 0.0;
            }
            else {
                wu_var[iCell][iSector].available_nonrenew =
                    wu_var[iCell][iSector].demand_nonrenew;

                available_nonrenew_tmp -=
                    wu_var[iCell][iSector].demand_nonrenew;
            }
        }
    }
}

/******************************************
* @brief   Calculate (cell) withdrawals and return flows from sectors
******************************************/
void
calculate_use_nonrenew(size_t  iCell,
                       double *withdrawn_nonrenew,
                       double *returned)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct      **wu_var;
    extern wu_force_struct    **wu_force;
    extern wu_con_map_struct   *wu_con_map;
    extern option_struct        options;

    double                      frac;

    size_t                      i;
    int                         iSector;

    for (i = 0; i < plugin_options.NWUTYPES; i++) {
        iSector = wu_con_map[iCell].sidx[i];
        if (iSector == NODATA_WU) {
            continue;
        }

        // non-renewable
        if (wu_var[iCell][iSector].available_nonrenew > 0) {
            frac = wu_var[iCell][iSector].demand_nonrenew /
                   wu_var[iCell][iSector].available_nonrenew;
            frac = min(frac, 1);

            wu_var[iCell][iSector].withdrawn_nonrenew =
                wu_var[iCell][iSector].available_nonrenew * frac;
        }
        else {
            wu_var[iCell][iSector].withdrawn_nonrenew = 0.0;
        }
        
        wu_var[iCell][iSector].returned += wu_var[iCell][iSector].withdrawn_nonrenew *
            (1 - wu_force[iCell][iSector].consumption_frac);
        wu_var[iCell][iSector].consumed += wu_var[iCell][iSector].withdrawn_nonrenew *
            wu_force[iCell][iSector].consumption_frac;
        (*returned) +=
            wu_var[iCell][iSector].withdrawn_nonrenew *
            (1 - wu_force[iCell][iSector].consumption_frac);
        (*withdrawn_nonrenew) += wu_var[iCell][iSector].withdrawn_nonrenew;
        
        
        

       
    }
}

/******************************************
* @brief   Handle withdrawals and return flows
******************************************/
void
calculate_hydrology_nonrenew(size_t iCell,
                             double withdrawn_nonrenew,
                             double returned)
{
    extern global_param_struct global_param;
    extern domain_struct       local_domain;
    extern rout_var_struct    *rout_var;
    extern option_struct       options;

    double                     available_stream_tmp;
    double                     returned_stream_tmp;
    double                     available_nonrenew_tmp;
    double                     returned_nonrenew_tmp;
    double                     discharge_dt;

    size_t                     rout_steps_per_dt;
    size_t                     i;
    size_t                     iStep;
    int                        iSector;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    // non-renewable
    if (withdrawn_nonrenew > 0 && options.GWM == false) {
        rout_var[iCell].nonrenew_deficit += withdrawn_nonrenew; // TODO：deficit is positive value？

        if (rout_var[iCell].nonrenew_deficit < 0) {
            rout_var[iCell].nonrenew_deficit = 0;
        }
    }
    

    // non-renewable returns
    if (returned > 0.&& options.GWM == false) {
        for (i = 0; i < plugin_options.NWUTYPES; i++) {
            iSector = wu_con_map[iCell].sidx[i];
            if (iSector == NODATA_WU) {
                continue;
            }

            // get available nonrenewable requirements
            available_nonrenew_tmp = rout_var[iCell].nonrenew_deficit; 
           
            returned_nonrenew_tmp =
                wu_var[iCell][iSector].withdrawn_nonrenew *
                (1 - wu_force[iCell][iSector].consumption_frac);  
            // add returned resources to nonrenewable
            returned_nonrenew_tmp = min(available_nonrenew_tmp,
                                        returned_nonrenew_tmp); 
            if (available_nonrenew_tmp > 0) {
                rout_var[iCell].nonrenew_deficit -= returned_nonrenew_tmp;

                if (rout_var[iCell].nonrenew_deficit < 0) {
                    rout_var[iCell].nonrenew_deficit = 0;
                }
            }

            // decrease returned
            returned -= returned_nonrenew_tmp;
            if (returned < 0) {
                returned = 0;
            }
        }
    }

    // surface returns
    if (returned > 0.) {
        // surface
        available_stream_tmp = 0.;
        for (iStep = rout_steps_per_dt;
             iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
             iStep++) {
            available_stream_tmp += rout_var[iCell].dt_discharge[iStep];
        }

        returned_stream_tmp = returned /
                              MM_PER_M * local_domain.locations[iCell].area /
                              global_param.dt;

        for (iStep = rout_steps_per_dt;
             iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
             iStep++) {
            discharge_dt = rout_var[iCell].dt_discharge[iStep];

            // Stream returns
            if (available_stream_tmp > 0) {
                // Scale returns proportionally to stream availability
                rout_var[iCell].dt_discharge[iStep] +=
                    returned_stream_tmp *
                    (discharge_dt / available_stream_tmp);
            }
            else {
                // Scale returns proportionally to stream length
                rout_var[iCell].dt_discharge[iStep] +=
                    returned_stream_tmp / (plugin_options.UH_LENGTH - 1);
            }

            if (rout_var[iCell].dt_discharge[iStep] < 0) {
                rout_var[iCell].dt_discharge[iStep] = 0.;
            }
        }

        // Recalculate discharge and stream moisture
        rout_var[iCell].discharge = 0.;
        rout_var[iCell].stream = 0.;
        for (iStep = 0;
             iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
             iStep++) {
            if (iStep < rout_steps_per_dt) {
                rout_var[iCell].discharge +=
                    rout_var[iCell].dt_discharge[iStep];
            }
            else {
                rout_var[iCell].stream += rout_var[iCell].dt_discharge[iStep];
            }
        }
    }
}

/******************************************
* @brief   Check the water-use water balance
******************************************/
void
check_water_use_balance_nonrenew(size_t iCell,
                                 double available_nonrenew,
                                 double demand_nonrenew,
                                 double withdrawn_nonrenew)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct      **wu_var;
    extern wu_con_map_struct   *wu_con_map;

    size_t                      i;
    int                         iSector;

    for (i = 0; i < plugin_options.NWUTYPES; i++) {
        iSector = wu_con_map[iCell].sidx[i];
        if (iSector == NODATA_WU) {
            continue;
        }

        if (wu_var[iCell][iSector].withdrawn_nonrenew -
            wu_var[iCell][iSector].demand_nonrenew > WU_BALANCE_ERROR_THRESH ||
            wu_var[iCell][iSector].withdrawn_nonrenew -
            wu_var[iCell][iSector].available_nonrenew >
            WU_BALANCE_ERROR_THRESH) {
            log_err("Water-use non-renewable water balance error for sector %zu:\n"
                    "non-renewable:\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n",
                    i,
                    wu_var[iCell][iSector].withdrawn_nonrenew,
                    wu_var[iCell][iSector].demand_nonrenew,
                    wu_var[iCell][iSector].available_nonrenew);
        }
    }
    if (withdrawn_nonrenew - demand_nonrenew >
        WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES ||
        withdrawn_nonrenew - available_nonrenew >
        WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES) {
        log_err("Water-use non-renewable water balance error for cell %zu:\n"
                "non-renewable:\twithdrawn [%.4f]\tdemand [%.4f]\t\t\tavailable [%.4f]\n",
                iCell,
                withdrawn_nonrenew,
                demand_nonrenew,
                available_nonrenew);
    }
}

/******************************************
* @brief   Run water-use module
******************************************/
void
wu_nonrenew(size_t iCell)
{
    double available_nonrenew;
    double demand_nonrenew;
    double withdrawn_nonrenew;
    double returned;

    /******************************************
       Init
    ******************************************/
    available_nonrenew = 0.;
    demand_nonrenew = 0.;
    withdrawn_nonrenew = 0.;
    returned = 0.;

    /******************************************
       Demand
    ******************************************/
    calculate_demand_nonrenew(iCell, &demand_nonrenew);

    /******************************************
       Availability
    ******************************************/
    calculate_availability_nonrenew(iCell,
                                    &available_nonrenew);

    /******************************************
       Divide
    ******************************************/
    calculate_division_nonrenew(iCell,
                                available_nonrenew, demand_nonrenew);

    /******************************************
       Withdrawals & Consumption
    ******************************************/
    calculate_use_nonrenew(iCell, &withdrawn_nonrenew, &returned);

    /******************************************
       Return
    ******************************************/
    calculate_hydrology_nonrenew(iCell, withdrawn_nonrenew, returned);

    /******************************************
       Check balance
    ******************************************/
    check_water_use_balance_nonrenew(iCell, available_nonrenew, demand_nonrenew,
                                     withdrawn_nonrenew);
}
