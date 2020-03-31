/******************************************************************************
 * @section DESCRIPTION
 *
 * This routine determines the current forcing file data type and stores its
 * location in the description of the current forcing file.
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

#include <vic_driver_shared_image.h>
#include <plugin.h>

/******************************************************************************
 * @brief    This routine determines the current forcing file data type and
 *           stores its location in the description of the current forcing file.
 *****************************************************************************/
void
plugin_set_force_type(char *cmdstr)
{
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_filenames_struct    plugin_filenames;

    char                              nctype[MAXSTRING];
    char                              ncfreq[MAXSTRING];
    char                              ncvarname[MAXSTRING];
    char                              ncfilename[MAXSTRING];
    int                               type = SKIP;
    int                               freq = FORCE_STEP;

    strcpy(nctype, MISSING_S);
    strcpy(ncfreq, MISSING_S);
    strcpy(ncvarname, MISSING_S);
    strcpy(ncfilename, MISSING_S);

    sscanf(cmdstr, "%*s %s %s %s %s", nctype, ncvarname, ncfreq, ncfilename);

    /***************************************
       Get meteorological data forcing info
    ***************************************/

    if (strcasecmp("DISCHARGE", nctype) == 0) {
        type = FORCING_DISCHARGE;
    }
    else if (strcasecmp("EFR_DISCHARGE", nctype) == 0) {
        type = FORCING_EFR_DISCHARGE;
    }
    else if (strcasecmp("EFR_BASEFLOW", nctype) == 0) {
        type = FORCING_EFR_BASEFLOW;
    }
    else if (strcasecmp("PUMPING_CAP", nctype) == 0) {
        type = FORCING_PUMPING_CAP;
    }
    else if (strcasecmp("IRR_DEMAND", nctype) == 0) {
        type = FORCING_IRR_DEMAND;
    }
    else if (strcasecmp("MUN_DEMAND", nctype) == 0) {
        type = FORCING_MUN_DEMAND;
    }
    else if (strcasecmp("LIV_DEMAND", nctype) == 0) {
        type = FORCING_LIV_DEMAND;
    }
    else if (strcasecmp("MAN_DEMAND", nctype) == 0) {
        type = FORCING_MAN_DEMAND;
    }
    else if (strcasecmp("ENE_DEMAND", nctype) == 0) {
        type = FORCING_ENE_DEMAND;
    }
    else if (strcasecmp("IRR_GROUNDWATER", nctype) == 0) {
        type = FORCING_IRR_GROUNDWATER;
    }
    else if (strcasecmp("MUN_GROUNDWATER", nctype) == 0) {
        type = FORCING_MUN_GROUNDWATER;
    }
    else if (strcasecmp("LIV_GROUNDWATER", nctype) == 0) {
        type = FORCING_LIV_GROUNDWATER;
    }
    else if (strcasecmp("MAN_GROUNDWATER", nctype) == 0) {
        type = FORCING_MAN_GROUNDWATER;
    }
    else if (strcasecmp("ENE_GROUNDWATER", nctype) == 0) {
        type = FORCING_ENE_GROUNDWATER;
    }
    else if (strcasecmp("IRR_CONSUMPTION", nctype) == 0) {
        type = FORCING_IRR_CONSUMPTION;
    }
    else if (strcasecmp("MUN_CONSUMPTION", nctype) == 0) {
        type = FORCING_MUN_CONSUMPTION;
    }
    else if (strcasecmp("LIV_CONSUMPTION", nctype) == 0) {
        type = FORCING_LIV_CONSUMPTION;
    }
    else if (strcasecmp("MAN_CONSUMPTION", nctype) == 0) {
        type = FORCING_MAN_CONSUMPTION;
    }
    else if (strcasecmp("ENE_CONSUMPTION", nctype) == 0) {
        type = FORCING_ENE_CONSUMPTION;
    }
    /** Undefined variable type **/
    else {
        log_err("Undefined forcing variable type %s in forcing file.",
                nctype);
    }

    if (strcasecmp("STEP", ncfreq) == 0) {
        freq = FORCE_STEP;
    }
    else if (strcasecmp("DAY", ncfreq) == 0) {
        freq = FORCE_DAY;
    }
    else if (strcasecmp("MONTH", ncfreq) == 0) {
        freq = FORCE_MONTH;
    }
    /** Undefined variable frequency **/
    else {
        log_err("Undefined forcing variable frequency %s in forcing file.",
                ncfreq);
    }
    plugin_global_param.forcefreq[type] = freq;

    if (strcasecmp(MISSING_S, ncvarname) != 0) {
        strcpy(plugin_filenames.f_varname[type], ncvarname);
    }
    else {
        log_err(
            "Must supply netCDF variable name for %s forcing file",
            nctype);
    }

    if (strcasecmp(MISSING_S, ncfilename) != 0) {
        strcpy(plugin_filenames.f_path_pfx[type], ncfilename);
    }
    else {
        log_err(
            "Must supply netCDF file name for %s forcing file",
            nctype);
    }
}
