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
            (*demand_remote) += wu_var[iCell2][iSector2].demand_remote_tmp;
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
    for (iStep = rout_steps_per_dt;
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
                          double demand_remote)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct      **wu_var;
    extern wu_con_struct       *wu_con;
    extern wu_con_map_struct   *wu_con_map;

    double                      available_remote_tmp;

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

        // remote
        wu_var[iCell][iSector].available_remote = 0.0;
        if (demand_remote > 0) {
            for (j = 0; j < wu_con[iCell].nreceiving; j++) {
                iCell2 = wu_con[iCell].receiving[j];

                iSector2 = wu_con_map[iCell2].sidx[i];
                if (iSector2 == NODATA_WU) {
                    continue;
                }

                if (wu_var[iCell2][iSector2].demand_remote_tmp >=
                    available_remote_tmp) {
                    wu_var[iCell2][iSector2].available_remote_tmp =
                        available_remote_tmp;

                    available_remote_tmp = 0.0;
                }
                else {
                    wu_var[iCell2][iSector2].available_remote_tmp =
                        wu_var[iCell2][iSector2].demand_remote_tmp;

                    available_remote_tmp -=
                        wu_var[iCell2][iSector2].demand_remote_tmp;
                }

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

            wu_var[iCell2][iSector2].consumed +=
                wu_var[iCell2][iSector2].withdrawn_remote_tmp *
                wu_force[iCell2][iSector2].consumption_frac;

            wu_var[iCell][iSector].withdrawn_remote +=
                wu_var[iCell2][iSector2].withdrawn_remote_tmp;
            (*withdrawn_remote) +=
                wu_var[iCell2][iSector2].withdrawn_remote_tmp;

            wu_var[iCell][iSector].returned +=
                wu_var[iCell2][iSector2].withdrawn_remote_tmp *
                (1 - wu_force[iCell2][iSector2].consumption_frac);
            (*returned) +=
                wu_var[iCell2][iSector2].withdrawn_remote_tmp *
                (1 - wu_force[iCell2][iSector2].consumption_frac);
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

    double                            withdrawn_discharge_tmp;
    double                            available_discharge_tmp;

    size_t                            rout_steps_per_dt;
    size_t                            iStep;

    rout_steps_per_dt = plugin_global_param.rout_steps_per_day /
                        global_param.model_steps_per_day;

    // remote
    if (withdrawn_remote - returned != 0.) {
        available_discharge_tmp = 0.;
        for (iStep = rout_steps_per_dt;
             iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
             iStep++) {
            available_discharge_tmp += rout_var[iCell].dt_discharge[iStep];
        }

        withdrawn_discharge_tmp =
            (withdrawn_remote - returned) /
            MM_PER_M * local_domain.locations[iCell].area / global_param.dt;

        for (iStep = rout_steps_per_dt;
             iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
             iStep++) {
            if (available_discharge_tmp > 0) {
                // Scale withdrawal proportionally to availability
                rout_var[iCell].dt_discharge[iStep] -=
                    withdrawn_discharge_tmp *
                    (rout_var[iCell].dt_discharge[iStep] /
                     available_discharge_tmp);
            }
            else {
                // Scale withdrawal proportionally to length
                rout_var[iCell].dt_discharge[iStep] -=
                    withdrawn_discharge_tmp / (plugin_options.UH_LENGTH - 1);
            }
            if (rout_var[iCell].dt_discharge[iStep] < 0) {
                rout_var[iCell].dt_discharge[iStep] = 0.;
            }
        }

        rout_var[iCell].discharge = 0.;
        rout_var[iCell].stream = 0.;
        for (iStep = 0;
             iStep < plugin_options.UH_LENGTH + rout_steps_per_dt - 1;
             iStep++) {
            // Recalculate discharge and stream moisture
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
                               double demand_remote,
                               double withdrawn_remote)
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
        withdrawn_remote - demand_remote >
        WU_BALANCE_ERROR_THRESH * plugin_options.NWUTYPES) {
        log_err("Water-use water balance error for cell %zu:\n"
                "remote:\t\twithdrawn [%.4f]\tdemand [%.4f]\tavailable [%.4f]\n",
                iCell,
                withdrawn_remote,
                demand_remote,
                available_remote);
    }
}

/******************************************
* @brief   Run water-use module
******************************************/
void
wu_remote(size_t iCell)
{
    double available_remote;
    double demand_remote;
    double withdrawn_remote;
    double returned;

    /******************************************
       Init
    ******************************************/
    available_remote = 0.;
    demand_remote = 0.;
    withdrawn_remote = 0.;
    returned = 0.;

    reset_wu_remote(iCell);

    /******************************************
       Demand
    ******************************************/
    calculate_demand_remote(iCell,
                            &demand_remote);

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
}
