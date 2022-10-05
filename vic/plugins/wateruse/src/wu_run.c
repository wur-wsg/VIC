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
reset_wu(size_t iCell)
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

        wu_var[iCell][iSector].available_surf = 0.0;
        wu_var[iCell][iSector].available_gw = 0.0;
        wu_var[iCell][iSector].available_dam = 0.0;
        wu_var[iCell][iSector].demand_surf = 0.0;
        wu_var[iCell][iSector].demand_gw = 0.0;
        wu_var[iCell][iSector].withdrawn_surf = 0.0;
        wu_var[iCell][iSector].withdrawn_gw = 0.0;
        wu_var[iCell][iSector].withdrawn_dam = 0.0;
        wu_var[iCell][iSector].consumed = 0.0;
        wu_var[iCell][iSector].returned = 0.0;

        wu_var[iCell][iSector].available_remote = 0.0;
        wu_var[iCell][iSector].demand_remote = 0.0;
        wu_var[iCell][iSector].withdrawn_remote = 0.0;
        wu_var[iCell][iSector].available_tremote = 0.0;
        wu_var[iCell][iSector].demand_tremote = 0.0;
        wu_var[iCell][iSector].withdrawn_tremote = 0.0;

        wu_var[iCell][iSector].available_nonrenew = 0.0;
        wu_var[iCell][iSector].demand_nonrenew = 0.0;
        wu_var[iCell][iSector].withdrawn_nonrenew = 0.0;
    }
}

/******************************************
* @brief   Get (cell) demand from sectors
******************************************/
void
calculate_demand(size_t iCell)
{
    extern plugin_option_struct plugin_options;
    extern wu_var_struct      **wu_var;
    extern wu_force_struct    **wu_force;
    extern wu_con_map_struct   *wu_con_map;

    size_t                      i;
    int                         iSector;

    for (i = 0; i < plugin_options.NWUTYPES; i++) {
        iSector = wu_con_map[iCell].sidx[i];
        if (iSector == NODATA_WU) {
            continue;
        }

        // groundwater
        wu_var[iCell][iSector].demand_gw = wu_force[iCell][iSector].demand *
                                           wu_force[iCell][iSector].
                                           groundwater_frac;

        // surface water
        wu_var[iCell][iSector].demand_surf = wu_force[iCell][iSector].demand *
                                             (1 -
                                              wu_force[iCell][iSector].
                                              groundwater_frac);
    }
}

/******************************************
* @brief   Run water-use module
******************************************/
void
wu_run(size_t iCell)
{
    /******************************************
       Reset
    ******************************************/
    reset_wu(iCell);

    /******************************************
       Demand
    ******************************************/
    calculate_demand(iCell);
}
