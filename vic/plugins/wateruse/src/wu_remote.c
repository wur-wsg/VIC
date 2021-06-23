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
* @brief   Reset water-use from sectors
******************************************/
void
reset_wu_remote(size_t iCell)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct      **wu_var;
    extern wu_con_struct       *wu_con;
    extern wu_con_map_struct   *wu_con_map;

    size_t                      i;
    size_t                      j;
    int                         iSector;
    size_t                      iCell2;
    int                         iSector2;

    for (i = 0; i < plugin_options.NWUTYPES; i++) {
        iSector = wu_con_map[iCell].sidx[i];
        if (iSector == NODATA_WU) {
            continue;
        }

        // remote
        for (j = 0; j < wu_con[iCell].nreceiving; j++) {
            iCell2 = wu_con[iCell].receiving[j];

            iSector2 = wu_con_map[iCell2].sidx[i];
            if (iSector2 == NODATA_WU) {
                continue;
            }

            wu_var[iCell2][iSector2].demand_remote_tmp = 0.;
            wu_var[iCell2][iSector2].withdrawn_remote_tmp = 0.;
            wu_var[iCell2][iSector2].available_remote_tmp = 0.;
            wu_var[iCell2][iSector2].returned_remote_tmp = 0.;
        }
    }
}

/******************************************
* @brief   Get (cell) demand from sectors
******************************************/
void
calculate_demand_remote(size_t  iCell,
                        double *demand_remote)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct      **wu_var;
    extern wu_con_struct       *wu_con;
    extern wu_con_map_struct   *wu_con_map;

    size_t                      i;
    size_t                      j;
    int                         iSector;
    size_t                      iCell2;
    int                         iSector2;

    for (i = 0; i < plugin_options.NWUTYPES; i++) {
        iSector = wu_con_map[iCell].sidx[i];
        if (iSector == NODATA_WU) {
            continue;
        }

        // remote
        // for water-use receiving cells the demand is calculated based on
        // their demand minus their local withdrawals
        wu_var[iCell][iSector].demand_remote = 0.0;
        for (j = 0; j < wu_con[iCell].nreceiving; j++) {
            iCell2 = wu_con[iCell].receiving[j];

            iSector2 = wu_con_map[iCell2].sidx[i];
            if (iSector2 == NODATA_WU) {
                continue;
            }

            wu_var[iCell2][iSector2].demand_remote_tmp =
                (wu_var[iCell2][iSector2].demand_surf +
                 wu_var[iCell2][iSector2].demand_gw) -
                (wu_var[iCell2][iSector2].withdrawn_surf +
                 wu_var[iCell2][iSector2].withdrawn_gw +
                 wu_var[iCell2][iSector2].withdrawn_dam +
                 wu_var[iCell2][iSector2].withdrawn_tremote);
            if (wu_var[iCell2][iSector2].demand_remote_tmp < 0) {
                wu_var[iCell2][iSector2].demand_remote_tmp = 0.;
            }

            wu_var[iCell][iSector].demand_remote +=
                wu_var[iCell2][iSector2].demand_remote_tmp;
            demand_remote[i] += wu_var[iCell2][iSector2].demand_remote_tmp;
        }
    }
}

/******************************************
* @brief   Get (cell) availability
******************************************/
void
calculate_availability_remote(size_t  iCell,
                              double *available_remote)
{
    extern plugin_global_param_struct plugin_global_param;
    extern global_param_struct        global_param;
    extern domain_struct              local_domain;
    extern global_param_struct        global_param;

    double                            frac;

    size_t                            rout_steps_per_dt;
    size_t                            iStep;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    // surface water
    for (iStep = 0;
         iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
         iStep++) {
        (*available_remote) += rout_var[iCell].dt_discharge[iStep] *
                               global_param.dt /
                               local_domain.locations[iCell].area * MM_PER_M;
    }
    if (plugin_options.EFR) {
        frac = min(efr_force[iCell].discharge / rout_var[iCell].discharge, 1.0);
        (*available_remote) *= (1 - frac);
    }
    if ((*available_remote) < 0) {
        (*available_remote) = 0;
    }
}

/******************************************
* @brief   Divide available resources over sectors
******************************************/
void
calculate_division_remote(size_t iCell,
                          double available_remote,
                          double *demand_remote)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct      **wu_var;
    extern wu_con_struct       *wu_con;
    extern wu_con_map_struct   *wu_con_map;

    double                      demand_remote_tmp;
    double                      available_remote_tmp;
    double                      frac_remote_tmp;

    size_t                      i;
    size_t                      j;
    int                         iSector;
    size_t                      iCell2;
    int                         iSector2;

    available_remote_tmp = available_remote;

    for (i = 0; i < plugin_options.NWUTYPES; i++) {
        iSector = wu_con_map[iCell].sidx[i];
        if (iSector == NODATA_WU) {
            continue;
        }
        
        demand_remote_tmp = demand_remote[i];

        // remote
        wu_var[iCell][iSector].available_remote = 0.0;
        if (demand_remote_tmp > 0) {
            frac_remote_tmp = available_remote_tmp / demand_remote_tmp;
            frac_remote_tmp = min(frac_remote_tmp, 1.0);
            
            for (j = 0; j < wu_con[iCell].nreceiving; j++) {
                iCell2 = wu_con[iCell].receiving[j];

                iSector2 = wu_con_map[iCell2].sidx[i];
                if (iSector2 == NODATA_WU) {
                    continue;
                }

                wu_var[iCell2][iSector2].available_remote_tmp =
                    wu_var[iCell2][iSector2].demand_remote_tmp * frac_remote_tmp;

                available_remote_tmp -=
                    wu_var[iCell2][iSector2].available_remote_tmp;

                wu_var[iCell][iSector].available_remote +=
                    wu_var[iCell2][iSector2].available_remote_tmp;
            }
        }
    }
}

/******************************************
* @brief   Calculate (cell) withdrawals and return flows from sectors
******************************************/
void
calculate_use_remote(size_t  iCell,
                     double *withdrawn_remote,
                     double *returned)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct      **wu_var;
    extern wu_con_struct       *wu_con;
    extern wu_force_struct    **wu_force;
    extern wu_con_map_struct   *wu_con_map;

    double                      frac;

    size_t                      i;
    size_t                      j;
    int                         iSector;
    size_t                      iCell2;
    int                         iSector2;

    for (i = 0; i < plugin_options.NWUTYPES; i++) {
        iSector = wu_con_map[iCell].sidx[i];
        if (iSector == NODATA_WU) {
            continue;
        }

        // remote
        wu_var[iCell][iSector].withdrawn_remote = 0.0;
        for (j = 0; j < wu_con[iCell].nreceiving; j++) {
            iCell2 = wu_con[iCell].receiving[j];

            iSector2 = wu_con_map[iCell2].sidx[i];
            if (iSector2 == NODATA_WU) {
                continue;
            }

            if (wu_var[iCell2][iSector2].available_remote_tmp > 0) {
                frac = wu_var[iCell2][iSector2].demand_remote_tmp /
                       wu_var[iCell2][iSector2].available_remote_tmp;
                frac = min(frac, 1);

                wu_var[iCell2][iSector2].withdrawn_remote_tmp =
                    wu_var[iCell2][iSector2].available_remote_tmp * frac;
            }
            else {
                wu_var[iCell2][iSector2].withdrawn_remote_tmp = 0;
            }
            wu_var[iCell2][iSector2].returned_remote_tmp = 
                wu_var[iCell2][iSector2].withdrawn_remote_tmp *
                (1 - wu_force[iCell2][iSector2].consumption_frac);
            wu_var[iCell2][iSector2].consumed_remote_tmp = 
                wu_var[iCell2][iSector2].withdrawn_remote_tmp *
                wu_force[iCell2][iSector2].consumption_frac;
            
            // total accounting
            wu_var[iCell2][iSector2].consumed +=
                wu_var[iCell2][iSector2].consumed_remote_tmp;

            wu_var[iCell][iSector].withdrawn_remote +=
                wu_var[iCell2][iSector2].withdrawn_remote_tmp;
            (*withdrawn_remote) +=
                wu_var[iCell2][iSector2].withdrawn_remote_tmp;

            wu_var[iCell][iSector].returned +=
                wu_var[iCell2][iSector2].returned_remote_tmp;
            (*returned) +=
                wu_var[iCell2][iSector2].returned_remote_tmp;
        }
    }
}

/******************************************
* @brief   Handle withdrawals and return flows
******************************************/
void
calculate_hydrology_remote(size_t iCell,
                           double withdrawn_remote,
                           double returned)
{
    extern plugin_global_param_struct plugin_global_param;
    extern global_param_struct        global_param;
    extern domain_struct              local_domain;
    extern rout_var_struct           *rout_var;
    extern wu_con_struct             *wu_con;

    double                            available_discharge_tmp;
    double                            withdrawn_discharge_tmp;
    double                            returned_discharge_tmp;
    double                            available_nonrenew_tmp;
    double                            returned_nonrenew_tmp;
    double                            discharge_dt;

    size_t                            iStep;
    size_t                            rout_steps_per_dt;
    size_t                            j;
    size_t                            i;
    size_t                            iCell2;
    int                               iSector;
    int                               iSector2;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;
    
    // non-renewable
    if(returned > 0.) {
        for (i = 0; i < plugin_options.NWUTYPES; i++) {
            iSector = wu_con_map[iCell].sidx[i];
            if (iSector == NODATA_WU) {
                continue;
            }

            for (j = 0; j < wu_con[iCell].nreceiving; j++) {
                iCell2 = wu_con[iCell].receiving[j];
                
                iSector2 = wu_con_map[iCell2].sidx[i];
                if (iSector2 == NODATA_WU) {
                    continue;
                }
                
                // get available nonrenewable requirements
                available_nonrenew_tmp = rout_var[iCell2].nonrenew_deficit;

                // get returned irrigation withdrawals
                returned_nonrenew_tmp = wu_var[iCell2][iSector2].returned_remote_tmp;

                // add returned resources to nonrenewable
                returned_nonrenew_tmp = min(available_nonrenew_tmp, returned_nonrenew_tmp);
                if(available_nonrenew_tmp > 0) {
                    rout_var[iCell2].nonrenew_deficit -= returned_nonrenew_tmp;

                    if(rout_var[iCell2].nonrenew_deficit < 0){
                        rout_var[iCell2].nonrenew_deficit = 0;
                    }

                    wu_var[iCell][iSector].returned -= returned_nonrenew_tmp;
                    wu_var[iCell2][iSector2].returned += returned_nonrenew_tmp;
                }

                // decrease returned
                returned -= returned_nonrenew_tmp;
                if(returned < 0){
                    returned = 0;
                }
            }
        }
    }

    // surface
    if (withdrawn_remote > 0. || returned > 0.) {
        available_discharge_tmp = 0.;
        for (iStep = 0;
             iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
             iStep++) {
            available_discharge_tmp += rout_var[iCell].dt_discharge[iStep];
        }

        withdrawn_discharge_tmp = withdrawn_remote /
            MM_PER_M * local_domain.locations[iCell].area / global_param.dt;
        returned_discharge_tmp = returned /
            MM_PER_M * local_domain.locations[iCell].area / global_param.dt;

        for (iStep = 0;
             iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
             iStep++) {
                
            discharge_dt = rout_var[iCell].dt_discharge[iStep];
            
            // Discharge withdrawals
            if (available_discharge_tmp > 0) {
                // Scale withdrawal proportionally to discharge availability
                rout_var[iCell].dt_discharge[iStep] -=
                    withdrawn_discharge_tmp *
                    (discharge_dt / available_discharge_tmp);
            }
            else {
                log_err("Wateruse discharge withdrawn while no discharge is available");
            }
            
            // Stream returns
            if (available_discharge_tmp > 0) {
                // Scale returns proportionally to stream availability
                    rout_var[iCell].dt_discharge[iStep] +=
                        returned_discharge_tmp *
                    (discharge_dt / available_discharge_tmp);
            }
            else {
                // Scale returns proportionally to stream length
                rout_var[iCell].dt_discharge[iStep] +=
                    returned_discharge_tmp / (plugin_options.UH_LENGTH - 1);
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
* @brief   Do remote water-use accounting
******************************************/
void
account_remote(size_t iCell)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct      **wu_var;
    extern wu_con_struct       *wu_con;
    extern wu_con_map_struct   *wu_con_map;

    size_t                      i;
    size_t                      j;
    int                         iSector;
    size_t                      iCell2;
    int                         iSector2;

    for (i = 0; i < plugin_options.NWUTYPES; i++) {
        iSector = wu_con_map[iCell].sidx[i];
        if (iSector == NODATA_WU) {
            continue;
        }

        // remote
        for (j = 0; j < wu_con[iCell].nreceiving; j++) {
            iCell2 = wu_con[iCell].receiving[j];

            iSector2 = wu_con_map[iCell2].sidx[i];
            if (iSector2 == NODATA_WU) {
                continue;
            }

            wu_var[iCell2][iSector2].demand_tremote =
                max(wu_var[iCell2][iSector2].demand_remote_tmp,
                    wu_var[iCell2][iSector2].demand_tremote);
            wu_var[iCell2][iSector2].withdrawn_tremote +=
                wu_var[iCell2][iSector2].withdrawn_remote_tmp;
            wu_var[iCell2][iSector2].available_tremote +=
                wu_var[iCell2][iSector2].available_remote_tmp;
        }
    }
}

/******************************************
* @brief   Check the water-use water balance
******************************************/
void
check_water_use_balance_remote(size_t iCell,
                               double available_remote,
                               double *demand_remote,
                               double withdrawn_remote)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct      **wu_var;
    extern wu_con_struct       *wu_con;
    extern wu_con_map_struct   *wu_con_map;
    
    double demand_remote_tmp;

    size_t                      i;
    size_t                      j;
    int                         iSector;
    size_t                      iCell2;
    int                         iSector2;
    
    demand_remote_tmp = 0.0;
    for (i = 0; i < plugin_options.NWUTYPES; i++) {
        demand_remote_tmp += demand_remote[i];
    }
    
    for (i = 0; i < plugin_options.NWUTYPES; i++) {
        iSector = wu_con_map[iCell].sidx[i];
        if (iSector == NODATA_WU) {
            continue;
        }

        if (wu_var[iCell][iSector].withdrawn_remote -
            wu_var[iCell][iSector].available_remote > WU_BALANCE_ERROR_THRESH ||
            wu_var[iCell][iSector].withdrawn_remote -
            wu_var[iCell][iSector].demand_remote > WU_BALANCE_ERROR_THRESH) {
            log_err("Water-use water balance error for sector (using remote withdrawals) %zu:\n"
                    "groundwater:\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n"
                    "surface-water:\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n"
                    "dam:\t\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n"
                    "remote:\t\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n",
                    i,
                    wu_var[iCell2][iSector2].withdrawn_gw,
                    wu_var[iCell2][iSector2].demand_gw,
                    wu_var[iCell2][iSector2].available_gw,
                    wu_var[iCell2][iSector2].withdrawn_surf,
                    wu_var[iCell2][iSector2].demand_surf,
                    wu_var[iCell2][iSector2].available_surf,
                    wu_var[iCell2][iSector2].withdrawn_dam,
                    wu_var[iCell2][iSector2].demand_surf,
                    wu_var[iCell2][iSector2].available_dam,
                    wu_var[iCell2][iSector2].withdrawn_remote,
                    wu_var[iCell2][iSector2].demand_remote,
                    wu_var[iCell2][iSector2].available_remote);
        }

        for (j = 0; j < wu_con[iCell].nreceiving; j++) {
            iCell2 = wu_con[iCell].receiving[j];

            iSector2 = wu_con_map[iCell2].sidx[i];
            if (iSector2 == NODATA_WU) {
                continue;
            }

            if (wu_var[iCell2][iSector2].withdrawn_tremote -
                wu_var[iCell2][iSector2].available_tremote >
                WU_BALANCE_ERROR_THRESH ||
                wu_var[iCell2][iSector2].withdrawn_tremote -
                wu_var[iCell2][iSector2].demand_tremote >
                WU_BALANCE_ERROR_THRESH) {
                log_err("Water-use water balance error for remote sector %zu:\n"
                        "groundwater:\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n"
                        "surface-water:\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n"
                        "dam:\t\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n"
                        "remote:\t\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n",
                        i,
                        wu_var[iCell2][iSector2].withdrawn_gw,
                        wu_var[iCell2][iSector2].demand_gw,
                        wu_var[iCell2][iSector2].available_gw,
                        wu_var[iCell2][iSector2].withdrawn_surf,
                        wu_var[iCell2][iSector2].demand_surf,
                        wu_var[iCell2][iSector2].available_surf,
                        wu_var[iCell2][iSector2].withdrawn_dam,
                        wu_var[iCell2][iSector2].demand_surf,
                        wu_var[iCell2][iSector2].available_dam,
                        wu_var[iCell2][iSector2].withdrawn_tremote,
                        wu_var[iCell2][iSector2].demand_tremote,
                        wu_var[iCell2][iSector2].available_tremote);
            }
        }
    }
    if (withdrawn_remote - available_remote >
        WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES ||
        withdrawn_remote - demand_remote_tmp >
        WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES) {
        log_err("Water-use water balance error for cell %zu:\n"
                "remote:\t\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n",
                iCell,
                withdrawn_remote,
                demand_remote_tmp,
                available_remote);
    }
}

/******************************************
* @brief   Run water-use module
******************************************/
void
wu_remote(size_t iCell)
{
    extern plugin_option_struct plugin_options;
    
    double available_remote;
    double *demand_remote;
    double withdrawn_remote;
    double returned;
    
    size_t                     iSector;

    /******************************************
       Alloc
    ******************************************/
    demand_remote = malloc(plugin_options.NWUTYPES * sizeof(*demand_remote));
    check_alloc_status(demand_remote, "Memory allocation error.");
    
    /******************************************
       Init
    ******************************************/
    available_remote = 0.;
    withdrawn_remote = 0.;
    returned = 0.;
    
    for (iSector = 0; iSector < plugin_options.NWUTYPES; iSector++) {
        demand_remote[iSector] = 0.0;
    }

    reset_wu_remote(iCell);

    /******************************************
       Demand
    ******************************************/
    calculate_demand_remote(iCell,
                            demand_remote);

    /******************************************
       Availability
    ******************************************/
    calculate_availability_remote(iCell,
                                  &available_remote);

    /******************************************
       Divide
    ******************************************/
    calculate_division_remote(iCell,
                              available_remote, demand_remote);

    /******************************************
       Withdrawals & Consumption
    ******************************************/
    calculate_use_remote(iCell,
                         &withdrawn_remote, &returned);

    /******************************************
       Accounting
    ******************************************/
    account_remote(iCell);

    /******************************************
       Return
    ******************************************/
    calculate_hydrology_remote(iCell,
                               withdrawn_remote,
                               returned);

    /******************************************
       Check balance
    ******************************************/
    check_water_use_balance_remote(iCell,
                                   available_remote, demand_remote,
                                   withdrawn_remote);

    /******************************************
       Free
    ******************************************/
    free(demand_remote);
}
