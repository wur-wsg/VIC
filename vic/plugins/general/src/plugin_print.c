/******************************************************************************
 * @section DESCRIPTION
 *
 * Plugin structure printing functions (log printing)
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

#include <vic_driver_shared_all.h>
#include <plugin.h>

/******************************************
* @brief   Print decomposition
******************************************/
void
plugin_print_decomposition(size_t mpi_size,
                           int  **mpi_map_local_array_sizes)
{
    size_t i;

    fprintf(LOG_DEST, "mpi decomposition size:\n");
    for (i = 0; i < mpi_size; i++) {
        fprintf(LOG_DEST, "\tnode %zu             : %d\n", i,
                (*mpi_map_local_array_sizes)[i]);
    }
}

/******************************************
* @brief   Print global parameters
******************************************/
void
plugin_print_global_param(plugin_global_param_struct *gp)
{
    fprintf(LOG_DEST, "plugin global_param:\n");
    fprintf(LOG_DEST, "\trout_steps_per_day  : %zu\n", gp->rout_steps_per_day);
    fprintf(LOG_DEST, "\trout_dt             : %.4f\n", gp->rout_dt);
}

/******************************************
* @brief   Print options
******************************************/
void
plugin_print_options(plugin_option_struct *op)
{
    fprintf(LOG_DEST, "plugin option:\n");
    fprintf(LOG_DEST, "\tDECOMPOSITION        : %d\n",
            op->DECOMPOSITION);
    fprintf(LOG_DEST, "\tROUTING              : %s\n",
            op->ROUTING ? "true" : "false");
    fprintf(LOG_DEST, "\tFORCE_ROUTING        : %s\n",
            op->FORCE_ROUTING ? "true" : "false");

    fprintf(LOG_DEST, "\tUH_LENGTH            : %d\n",
            op->UH_LENGTH);
}

/******************************************
* @brief   Print parameters
******************************************/
void
plugin_print_parameters(plugin_parameters_struct *pa)
{
    /* Unused variables */
    UNUSED(pa);

    fprintf(LOG_DEST, "plugin parameters:\n");
}
