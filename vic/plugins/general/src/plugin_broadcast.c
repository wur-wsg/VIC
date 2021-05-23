/******************************************************************************
 * @section DESCRIPTION
 *
 * Functions to initialize and broadcast plugin structures
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
* @brief   Setup global param broadcasting structure
******************************************/
void
plugin_create_MPI_global_struct_type(MPI_Datatype *mpi_type)
{
    extern MPI_Comm MPI_COMM_VIC;
    int             nitems; // number of elements in struct
    int             status;
    int            *blocklengths;
    size_t          i;
    MPI_Aint       *offsets;
    MPI_Datatype   *mpi_types;

    // nitems has to equal the number of elements in global_param_struct
    nitems = 14;
    blocklengths = malloc(nitems * sizeof(*blocklengths));
    check_alloc_status(blocklengths, "Memory allocation error.");
    offsets = malloc(nitems * sizeof(*offsets));
    check_alloc_status(offsets, "Memory allocation error.");
    mpi_types = malloc(nitems * sizeof(*mpi_types));
    check_alloc_status(mpi_types, "Memory allocation error.")

    // most of the elements in global_param_struct are not arrays. Use 1 as
    // the default block length and reset as needed
    for (i = 0; i < (size_t) nitems; i++) {
        blocklengths[i] = 1;
    }
    i = 0;

    // size_t rout_steps_per_day;
    offsets[i] = offsetof(plugin_global_param_struct, rout_steps_per_day);
    mpi_types[i++] = MPI_AINT;
    // double rout_dt;
    offsets[i] = offsetof(plugin_global_param_struct, rout_dt);
    mpi_types[i++] = MPI_DOUBLE;
    // size_t wofost_steps_per_day;
    offsets[i] = offsetof(plugin_global_param_struct, wofost_steps_per_day);
    mpi_types[i++] = MPI_AINT;
    // double wofost_dt;
    offsets[i] = offsetof(plugin_global_param_struct, wofost_dt);
    mpi_types[i++] = MPI_DOUBLE;
    // size_t force_steps_per_year[PLUGIN_N_FORCING_TYPES];
    offsets[i] = offsetof(plugin_global_param_struct, force_steps_per_year);
    blocklengths[i] = PLUGIN_N_FORCING_TYPES;
    mpi_types[i++] = MPI_AINT;
    // double force_dt[PLUGIN_N_FORCING_TYPES];
    offsets[i] = offsetof(plugin_global_param_struct, force_dt);
    blocklengths[i] = PLUGIN_N_FORCING_TYPES;
    mpi_types[i++] = MPI_DOUBLE;
    // unsigned int forcesec[PLUGIN_N_FORCING_TYPES];
    offsets[i] = offsetof(plugin_global_param_struct, forcesec);
    blocklengths[i] = PLUGIN_N_FORCING_TYPES;
    mpi_types[i++] = MPI_UNSIGNED;
    // unsigned short int forceday[PLUGIN_N_FORCING_TYPES];
    offsets[i] = offsetof(plugin_global_param_struct, forceday);
    blocklengths[i] = PLUGIN_N_FORCING_TYPES;
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // unsigned short int forcemonth[PLUGIN_N_FORCING_TYPES];
    offsets[i] = offsetof(plugin_global_param_struct, forcemonth);
    blocklengths[i] = PLUGIN_N_FORCING_TYPES;
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // unsigned short int forceyear[PLUGIN_N_FORCING_TYPES];
    offsets[i] = offsetof(plugin_global_param_struct, forceyear);
    blocklengths[i] = PLUGIN_N_FORCING_TYPES;
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // unsigned short int forcefreq[PLUGIN_N_FORCING_TYPES];
    offsets[i] = offsetof(plugin_global_param_struct, forcefreq);
    blocklengths[i] = PLUGIN_N_FORCING_TYPES;
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // unsigned short int forceoffset[PLUGIN_N_FORCING_TYPES];
    offsets[i] = offsetof(plugin_global_param_struct, forceoffset);
    blocklengths[i] = PLUGIN_N_FORCING_TYPES;
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // bool forcerun[PLUGIN_N_FORCING_TYPES];
    offsets[i] = offsetof(plugin_global_param_struct, forcerun);
    blocklengths[i] = PLUGIN_N_FORCING_TYPES;
    mpi_types[i++] = MPI_C_BOOL;
    // unsigned int forceskip[PLUGIN_N_FORCING_TYPES];
    offsets[i] = offsetof(plugin_global_param_struct, forceskip);
    blocklengths[i] = PLUGIN_N_FORCING_TYPES;
    mpi_types[i++] = MPI_UNSIGNED;

    // make sure that the we have the right number of elements
    if (i != (size_t) nitems) {
        log_err("Miscount: %zd not equal to %d.", i, nitems);
    }
    status = MPI_Type_create_struct(nitems, blocklengths, offsets, mpi_types,
                                    mpi_type);
    check_mpi_status(status, "MPI error.");
    status = MPI_Type_commit(mpi_type);
    check_mpi_status(status, "MPI error.");

    // cleanup
    free(blocklengths);
    free(offsets);
    free(mpi_types);
}

/******************************************
* @brief   Setup filenames broadcasting structure
******************************************/
void
plugin_create_MPI_filenames_struct_type(MPI_Datatype *mpi_type)
{
    extern MPI_Comm MPI_COMM_VIC;
    int             nitems; // number of elements in struct
    int             status;
    int            *blocklengths;
    size_t          i;
    MPI_Aint       *offsets;
    MPI_Datatype   *mpi_types;

    // nitems has to equal the number of elements in global_param_struct
    nitems = 3;
    blocklengths = malloc(nitems * sizeof(*blocklengths));
    check_alloc_status(blocklengths, "Memory allocation error.");
    offsets = malloc(nitems * sizeof(*offsets));
    check_alloc_status(offsets, "Memory allocation error.");
    mpi_types = malloc(nitems * sizeof(*mpi_types));
    check_alloc_status(mpi_types, "Memory allocation error.")

    // most of the elements in global_param_struct are not arrays. Use 1 as
    // the default block length and reset as needed
    for (i = 0; i < (size_t) nitems; i++) {
        blocklengths[i] = MAXSTRING;
    }
    i = 0;

    // char wofost_text[MAXSTRING]
    offsets[i] = offsetof(plugin_filenames_struct, wofost_text);
    mpi_types[i++] = MPI_CHAR;
    // char f_path_pfx[PLUGIN_N_FORCING_TYPES][MAXSTRING];
    offsets[i] = offsetof(plugin_filenames_struct, f_path_pfx);
    blocklengths[i] *= PLUGIN_N_FORCING_TYPES;
    mpi_types[i++] = MPI_CHAR;
    // char f_varname[PLUGIN_N_FORCING_TYPES][MAXSTRING];
    offsets[i] = offsetof(plugin_filenames_struct, f_varname);
    blocklengths[i] *= PLUGIN_N_FORCING_TYPES;
    mpi_types[i++] = MPI_CHAR;

    // make sure that the we have the right number of elements
    if (i != (size_t) nitems) {
        log_err("Miscount: %zd not equal to %d.", i, nitems);
    }
    status = MPI_Type_create_struct(nitems, blocklengths, offsets, mpi_types,
                                    mpi_type);
    check_mpi_status(status, "MPI error.");
    status = MPI_Type_commit(mpi_type);
    check_mpi_status(status, "MPI error.");

    // cleanup
    free(blocklengths);
    free(offsets);
    free(mpi_types);
}

/******************************************
* @brief   Setup options broadcasting structure
******************************************/
void
plugin_create_MPI_option_struct_type(MPI_Datatype *mpi_type)
{
    extern MPI_Comm MPI_COMM_VIC;
    int             nitems; // number of elements in struct
    int             status;
    int            *blocklengths;
    size_t          i;
    MPI_Aint       *offsets;
    MPI_Datatype   *mpi_types;

    // nitems has to equal the number of elements in global_param_struct
    nitems = 34;
    blocklengths = malloc(nitems * sizeof(*blocklengths));
    check_alloc_status(blocklengths, "Memory allocation error.");
    offsets = malloc(nitems * sizeof(*offsets));
    check_alloc_status(offsets, "Memory allocation error.");
    mpi_types = malloc(nitems * sizeof(*mpi_types));
    check_alloc_status(mpi_types, "Memory allocation error.")

    // most of the elements in global_param_struct are not arrays. Use 1 as
    // the default block length and reset as needed
    for (i = 0; i < (size_t) nitems; i++) {
        blocklengths[i] = 1;
    }
    i = 0;

    // short unsigned int DECOMPOSITION;
    offsets[i] = offsetof(plugin_option_struct, DECOMPOSITION);
    mpi_types[i++] = MPI_INT;
    // bool ROUTING;
    offsets[i] = offsetof(plugin_option_struct, ROUTING);
    mpi_types[i++] = MPI_C_BOOL;
    // bool EFR;
    offsets[i] = offsetof(plugin_option_struct, EFR);
    mpi_types[i++] = MPI_C_BOOL;
    // bool DAMS;
    offsets[i] = offsetof(plugin_option_struct, DAMS);
    mpi_types[i++] = MPI_C_BOOL;
    // bool WATERUSE;
    offsets[i] = offsetof(plugin_option_struct, WATERUSE);
    mpi_types[i++] = MPI_C_BOOL;
    // bool IRRIGATION;
    offsets[i] = offsetof(plugin_option_struct, IRRIGATION);
    mpi_types[i++] = MPI_C_BOOL;
    // bool WOFOST;
    offsets[i] = offsetof(plugin_option_struct, WOFOST);
    mpi_types[i++] = MPI_C_BOOL;
    // short unsigned int UH_LENGTH;
    offsets[i] = offsetof(plugin_option_struct, UH_LENGTH);
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // bool FORCE_ROUTING;
    offsets[i] = offsetof(plugin_option_struct, FORCE_ROUTING);
    mpi_types[i++] = MPI_C_BOOL;
    // bool FORCE_LANDUSE;
    offsets[i] = offsetof(plugin_option_struct, FORCE_LANDUSE);
    mpi_types[i++] = MPI_C_BOOL;
    // short unsigned int NDAMTYPES;
    offsets[i] = offsetof(plugin_option_struct, NDAMTYPES);
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // short unsigned int NWUTYPES;
    offsets[i] = offsetof(plugin_option_struct, NWUTYPES);
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // short unsigned int NWURECEIVING;
    offsets[i] = offsetof(plugin_option_struct, NWURECEIVING);
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // short unsigned int WU_INPUT;
    offsets[i] = offsetof(plugin_option_struct, WU_INPUT);
    blocklengths[i] = WU_NSECTORS;
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // size_t NIRRTYPES;
    offsets[i] = offsetof(plugin_option_struct, NIRRTYPES);
    mpi_types[i++] = MPI_AINT;
    // bool POTENTIAL_IRRIGATION;
    offsets[i] = offsetof(plugin_option_struct, POTENTIAL_IRRIGATION);
    mpi_types[i++] = MPI_AINT;
    // bool EFFICIENT_IRRIGATION;
    offsets[i] = offsetof(plugin_option_struct, EFFICIENT_IRRIGATION);
    mpi_types[i++] = MPI_C_BOOL;
    // bool OFFSET_IRRIGATION;
    offsets[i] = offsetof(plugin_option_struct, OFFSET_IRRIGATION);
    mpi_types[i++] = MPI_C_BOOL;
    // bool COMP_WITH;
    offsets[i] = offsetof(plugin_option_struct, COMP_WITH);
    mpi_types[i++] = MPI_C_BOOL;
    // bool REMOTE_WITH;
    offsets[i] = offsetof(plugin_option_struct, REMOTE_WITH);
    mpi_types[i++] = MPI_C_BOOL;
    // bool NONRENEW_WITH;
    offsets[i] = offsetof(plugin_option_struct, NONRENEW_WITH);
    mpi_types[i++] = MPI_C_BOOL;
    // bool NONRENEW_RUNOFF;
    offsets[i] = offsetof(plugin_option_struct, NONRENEW_RUNOFF);
    mpi_types[i++] = MPI_C_BOOL;
    // bool WOFOST_PIRR;
    offsets[i] = offsetof(plugin_option_struct, WOFOST_PIRR);
    mpi_types[i++] = MPI_C_BOOL;
    // bool WOFOST_PFERT;
    offsets[i] = offsetof(plugin_option_struct, WOFOST_PFERT);
    mpi_types[i++] = MPI_C_BOOL;
    // bool WOFOST_DIST_SEASON;
    offsets[i] = offsetof(plugin_option_struct, WOFOST_DIST_SEASON);
    mpi_types[i++] = MPI_C_BOOL;
    // bool WOFOST_DIST_TSUM;
    offsets[i] = offsetof(plugin_option_struct, WOFOST_DIST_TSUM);
    mpi_types[i++] = MPI_C_BOOL;
    // bool WOFOST_DIST_FERT;
    offsets[i] = offsetof(plugin_option_struct, WOFOST_DIST_FERT);
    mpi_types[i++] = MPI_C_BOOL;
    // bool WOFOST_DIST_MIN;
    offsets[i] = offsetof(plugin_option_struct, WOFOST_DIST_MIN);
    mpi_types[i++] = MPI_C_BOOL;
    // bool WOFOST_CALC_MIN;
    offsets[i] = offsetof(plugin_option_struct, WOFOST_CALC_MIN);
    mpi_types[i++] = MPI_C_BOOL;
    // bool WOFOST_CONTINUE;
    offsets[i] = offsetof(plugin_option_struct, WOFOST_CONTINUE);
    mpi_types[i++] = MPI_C_BOOL;
    // bool WOFOST_FORCE_FERT;
    offsets[i] = offsetof(plugin_option_struct, WOFOST_FORCE_FERT);
    mpi_types[i++] = MPI_C_BOOL;
    // short unsigned int NCROPTYPES;
    offsets[i] = offsetof(plugin_option_struct, NCROPTYPES);
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // short unsigned int NFERTTIMES;
    offsets[i] = offsetof(plugin_option_struct, NFERTTIMES);
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // bool FORCE_CO2;
    offsets[i] = offsetof(plugin_option_struct, FORCE_CO2);
    mpi_types[i++] = MPI_C_BOOL;

    // make sure that the we have the right number of elements
    if (i != (size_t) nitems) {
        log_err("Miscount: %zd not equal to %d.", i, nitems);
    }
    status = MPI_Type_create_struct(nitems, blocklengths, offsets, mpi_types,
                                    mpi_type);
    check_mpi_status(status, "MPI error.");
    status = MPI_Type_commit(mpi_type);
    check_mpi_status(status, "MPI error.");

    // cleanup
    free(blocklengths);
    free(offsets);
    free(mpi_types);
}

/******************************************
* @brief   Setup parameters broadcasting structure
******************************************/
void
plugin_create_MPI_param_struct_type(MPI_Datatype *mpi_type)
{
    extern MPI_Comm MPI_COMM_VIC;
    int             nitems; // number of elements in struct
    int             status;
    int            *blocklengths;
    size_t          i;
    MPI_Aint       *offsets;
    MPI_Datatype   *mpi_types;

    // nitems has to equal the number of elements in global_param_struct
    nitems = 5;
    blocklengths = malloc(nitems * sizeof(*blocklengths));
    check_alloc_status(blocklengths, "Memory allocation error.");
    offsets = malloc(nitems * sizeof(*offsets));
    check_alloc_status(offsets, "Memory allocation error.");
    mpi_types = malloc(nitems * sizeof(*mpi_types));
    check_alloc_status(mpi_types, "Memory allocation error.")

    // most of the elements in global_param_struct are not arrays. Use 1 as
    // the default block length and reset as needed
    for (i = 0; i < (size_t) nitems; i++) {
        blocklengths[i] = 1;
    }
    i = 0;

    // double DAM_ALPHA;
    offsets[i] = offsetof(plugin_parameters_struct, DAM_ALPHA);
    mpi_types[i++] = MPI_DOUBLE;
    // double DAM_BETA;
    offsets[i] = offsetof(plugin_parameters_struct, DAM_BETA);
    mpi_types[i++] = MPI_DOUBLE;
    // double DAM_GAMMA;
    offsets[i] = offsetof(plugin_parameters_struct, DAM_GAMMA);
    mpi_types[i++] = MPI_DOUBLE;
    // double NREN_LIM;
    offsets[i] = offsetof(plugin_parameters_struct, NREN_LIM);
    mpi_types[i++] = MPI_DOUBLE;
    // double Ksat_expt;
    offsets[i] = offsetof(plugin_parameters_struct, Ksat_expt);
    mpi_types[i++] = MPI_DOUBLE;

    // make sure that the we have the right number of elements
    if (i != (size_t) nitems) {
        log_err("Miscount: %zd not equal to %d.", i, nitems);
    }
    status = MPI_Type_create_struct(nitems, blocklengths, offsets, mpi_types,
                                    mpi_type);
    check_mpi_status(status, "MPI error.");
    status = MPI_Type_commit(mpi_type);
    check_mpi_status(status, "MPI error.");

    // cleanup
    free(blocklengths);
    free(offsets);
    free(mpi_types);
}

/******************************************
* @brief   Setup plugin broadcasting structure
******************************************/
void
plugin_initialize_mpi(void)
{
    extern MPI_Datatype plugin_mpi_global_struct_type;
    extern MPI_Datatype plugin_mpi_filenames_struct_type;
    extern MPI_Datatype plugin_mpi_option_struct_type;
    extern MPI_Datatype plugin_mpi_param_struct_type;

    // initialize MPI data structures
    plugin_create_MPI_global_struct_type(&plugin_mpi_global_struct_type);
    plugin_create_MPI_filenames_struct_type(&plugin_mpi_filenames_struct_type);
    plugin_create_MPI_option_struct_type(&plugin_mpi_option_struct_type);
    plugin_create_MPI_param_struct_type(&plugin_mpi_param_struct_type);
}

/******************************************
* @brief   Broadcast filenames structure
******************************************/
void
plugin_broadcast_filenames(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern MPI_Datatype            plugin_mpi_filenames_struct_type;
    extern MPI_Comm                MPI_COMM_VIC;

    int                            status;

    status = MPI_Bcast(&plugin_filenames, 1, plugin_mpi_filenames_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
}

/******************************************
* @brief   Broadcast global param structure
******************************************/
void
plugin_broadcast_global_params(void)
{
    extern plugin_global_param_struct plugin_global_param;
    extern MPI_Datatype               plugin_mpi_global_struct_type;
    extern MPI_Comm                   MPI_COMM_VIC;

    int                               status;

    status = MPI_Bcast(&plugin_global_param, 1, plugin_mpi_global_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
}

/******************************************
* @brief   Broadcast options structure
******************************************/
void
plugin_broadcast_options(void)
{
    extern plugin_option_struct plugin_options;
    extern MPI_Datatype         plugin_mpi_option_struct_type;
    extern MPI_Comm             MPI_COMM_VIC;

    int                         status;

    status = MPI_Bcast(&plugin_options, 1, plugin_mpi_option_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
}

/******************************************
* @brief   Broadcast parameters structure
******************************************/
void
plugin_broadcast_params(void)
{
    extern plugin_parameters_struct plugin_param;
    extern MPI_Datatype             plugin_mpi_param_struct_type;
    extern MPI_Comm                 MPI_COMM_VIC;

    int                             status;

    status = MPI_Bcast(&plugin_param, 1, plugin_mpi_param_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
}
