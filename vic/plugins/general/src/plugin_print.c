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
    fprintf(LOG_DEST, "\twofost_steps_per_day: %zu\n",
            gp->wofost_steps_per_day);
    fprintf(LOG_DEST, "\twofost_dt           : %.4f\n", gp->wofost_dt);
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
    fprintf(LOG_DEST, "\tEFR                  : %s\n",
            op->EFR ? "true" : "false");
    fprintf(LOG_DEST, "\tDAMS                 : %s\n",
            op->DAMS ? "true" : "false");
    fprintf(LOG_DEST, "\tWATERUSE             : %s\n",
            op->WATERUSE ? "true" : "false");
    fprintf(LOG_DEST, "\tIRRIGATION           : %s\n",
            op->IRRIGATION ? "true" : "false");
    fprintf(LOG_DEST, "\tWOFOST               : %s\n",
            op->WOFOST ? "true" : "false");

    fprintf(LOG_DEST, "\tFORCE_ROUTING        : %s\n",
            op->FORCE_ROUTING ? "true" : "false");
    fprintf(LOG_DEST, "\tFORCE_LANDUSE        : %s\n",
            op->FORCE_LANDUSE ? "true" : "false");
    fprintf(LOG_DEST, "\tPOTENTIAL_IRRIGATION : %s\n",
            op->POTENTIAL_IRRIGATION ? "true" : "false");
    fprintf(LOG_DEST, "\tEFFICIENT_IRRIGATION : %s\n",
            op->EFFICIENT_IRRIGATION ? "true" : "false");
    fprintf(LOG_DEST, "\tOFFSET_IRRIGATION    : %s\n",
            op->OFFSET_IRRIGATION ? "true" : "false");
    fprintf(LOG_DEST, "\tPbare                : %zu\n",
            op->Pbare);
    fprintf(LOG_DEST, "\tCOMP_WITH            : %s\n",
            op->COMP_WITH ? "true" : "false");
    fprintf(LOG_DEST, "\tLOCAL_WITH           : %s\n",
            op->LOCAL_WITH ? "true" : "false");
    fprintf(LOG_DEST, "\tREMOTE_WITH          : %s\n",
            op->REMOTE_WITH ? "true" : "false");
    fprintf(LOG_DEST, "\tNONRENEW_WITH        : %s\n",
            op->NONRENEW_WITH ? "true" : "false");
    fprintf(LOG_DEST, "\tNONRENEW_RUNOFF      : %s\n",
            op->NONRENEW_RUNOFF ? "true" : "false");
    fprintf(LOG_DEST, "\tWOFOST_DIST_SEASON   : %s\n",
            op->WOFOST_DIST_SEASON ? "true" : "false");
    fprintf(LOG_DEST, "\tWOFOST_DIST_TSUM     : %s\n",
            op->WOFOST_DIST_TSUM ? "true" : "false");
    fprintf(LOG_DEST, "\tWOFOST_DIST_FERT     : %s\n",
            op->WOFOST_DIST_FERT ? "true" : "false");
    fprintf(LOG_DEST, "\tWOFOST_DIST_MIN      : %s\n",
            op->WOFOST_DIST_MIN ? "true" : "false");
    fprintf(LOG_DEST, "\tWOFOST_CALC_MIN      : %s\n",
            op->WOFOST_CALC_MIN ? "true" : "false");
    fprintf(LOG_DEST, "\tWOFOST_CONTINUE      : %s\n",
            op->WOFOST_CONTINUE ? "true" : "false");
    fprintf(LOG_DEST, "\tWOFOST_FOREC_FERT    : %s\n",
            op->WOFOST_FORCE_FERT ? "true" : "false");
    fprintf(LOG_DEST, "\tWOFOST_PIRR          : %s\n",
            op->WOFOST_PIRR ? "true" : "false");
    fprintf(LOG_DEST, "\tWOFOST_PFERT         : %s\n",
            op->WOFOST_PFERT ? "true" : "false");
    fprintf(LOG_DEST, "\tFORCE_CO2            : %s\n",
            op->FORCE_CO2 ? "true" : "false");

    fprintf(LOG_DEST, "\tUH_LENGTH            : %d\n",
            op->UH_LENGTH);
    fprintf(LOG_DEST, "\tNDAMTYPES            : %d\n",
            op->NDAMTYPES);
    fprintf(LOG_DEST, "\tNWUTYPES             : %d\n",
            op->NWUTYPES);
    fprintf(LOG_DEST, "\tNWURECEIVING         : %d\n",
            op->NWURECEIVING);
    fprintf(LOG_DEST, "\tNIRRTYPES            : %zu\n",
            op->NIRRTYPES);
    fprintf(LOG_DEST, "\tNCROPTYPES           : %d\n",
            op->NCROPTYPES);
    fprintf(LOG_DEST, "\tNFERTTIMES           : %d\n",
            op->NFERTTIMES);
}

/******************************************
* @brief   Print parameters
******************************************/
void
plugin_print_parameters(plugin_parameters_struct *pa)
{
    fprintf(LOG_DEST, "plugin parameters:\n");
    fprintf(LOG_DEST, "\tDAM_ALPHA            : %.4f\n", pa->DAM_ALPHA);
    fprintf(LOG_DEST, "\tDAM_BETA             : %.4f\n", pa->DAM_BETA);
    fprintf(LOG_DEST, "\tDAM_GAMMA            : %.4f\n", pa->DAM_GAMMA);
    fprintf(LOG_DEST, "\tNREN_LIM             : %.4f\n", pa->NREN_LIM);
    fprintf(LOG_DEST, "\tKsat_expt            : %.4f\n", pa->Ksat_expt);
}
