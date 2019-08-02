/******************************************************************************
 * @section DESCRIPTION
 *
 * Plugin defenition header file
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

#ifndef PLUGIN_DEF_H
#define PLUGIN_DEF_H

#include <vic_def.h>
#include <plugin_support.h>

/******************************************************************************
 * @brief   Decomposition Type
 *****************************************************************************/
enum {
    RANDOM_DECOMPOSITION, /**< native VIC (random) decomposition */
    BASIN_DECOMPOSITION, /**< basin based decomposition */
    FILE_DECOMPOSITION, /**< file based decomposition */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_DECOMPOSITION                   /**< used as a loop counter*/
};

/******************************************************************************
 * @brief   State Variable Type
 *****************************************************************************/
enum {
    // routing
    STATE_DISCHARGE_DT, /**< routing sub-step discharge [m3 s-1] */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_STATE_VARS                   /**< used as a loop counter*/
};

/******************************************************************************
 * @brief   Output Variable Type
 *****************************************************************************/
enum {
    // routing
    OUT_DISCHARGE,                      /**< river (outflow) discharge [m3 s-1] */
    OUT_STREAM_MOIST,                   /**< river (in-cell) stream moisture [mm] */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_OUTVAR_TYPES                /**< used as a loop counter*/
};

/******************************************************************************
 * @brief   Forcing Variable Type
 *****************************************************************************/
enum {
    // routing
    FORCING_DISCHARGE, /**< river (inflow) discharge [m3 s-1] */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_FORCING_TYPES                /**< used as a loop counter*/
};

/******************************************************************************
 * @brief   Forcing Variable Frequency
 *****************************************************************************/
enum
{
    FORCE_STEP, /**< model step forcing frequency */
    FORCE_DAY, /**< daily forcing frequency */
    FORCE_MONTH, /**< monthly forcing frequency */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_FORCE_FREQS  /**< Number of force frequencies */
};

/******************************************************************************
 * @brief   Plugin options
 *****************************************************************************/
typedef struct {
    short unsigned int DECOMPOSITION; /**< domain decomposition type */
    bool ROUTING; /**< routing module flag */

    short unsigned int UH_LENGTH; /**< routing sub-step (unit-hydrograph) length */
    bool FORCE_ROUTING; /**< routing (inflow) forcing flag */
} plugin_option_struct;

/******************************************************************************
 * @brief   Plugin global parameters
 *****************************************************************************/
typedef struct {
    size_t rout_steps_per_day; /**< number of routing steps [d-1] */
    double rout_dt; /**< routing time step [s] */
    size_t force_steps_per_year[PLUGIN_N_FORCING_TYPES]; /**< number of forcing steps [d-1] */
    double force_dt[PLUGIN_N_FORCING_TYPES]; /**< forcing time step [s] */
    unsigned int forcesec[PLUGIN_N_FORCING_TYPES]; /**< seconds since midnight when forcing files starts */
    unsigned short int forceday[PLUGIN_N_FORCING_TYPES]; /**< day forcing files starts */
    unsigned short int forcemonth[PLUGIN_N_FORCING_TYPES]; /**< month forcing files starts */
    unsigned short int forceyear[PLUGIN_N_FORCING_TYPES]; /**< year forcing files start */
    unsigned short int forcefreq[PLUGIN_N_FORCING_TYPES]; /**< forcing frequency */
    unsigned short int forceoffset[PLUGIN_N_FORCING_TYPES]; /**< counter to keep track of offset in reading forcing files; updated after every read */
    bool forcerun[PLUGIN_N_FORCING_TYPES]; /**< flag to keep track wether to read forcing variable this time step */
    unsigned int forceskip[PLUGIN_N_FORCING_TYPES]; /**< number of model time steps to skip at the start of the forcing file */
} plugin_global_param_struct;

/******************************************************************************
 * @brief   Plugin parameters
 *****************************************************************************/
typedef struct {
    // used for future plugins
} plugin_parameters_struct;

/******************************************************************************
 * @brief   Public structures
 *****************************************************************************/
plugin_option_struct       plugin_options;
plugin_global_param_struct plugin_global_param;
plugin_parameters_struct   plugin_param;

/******************************************************************************
 * @brief   Functions
 *****************************************************************************/
// Setup
void plugin_initialize_global_structures(void);
bool plugin_get_global_param(char *cmdstr);
void plugin_set_force_type(char *cmdstr);
void plugin_validate_global_param(void);
bool plugin_get_parameters(char *cmdstr);
void plugin_validate_parameters(void);

void plugin_initialize_mpi(void);
void plugin_broadcast_filenames(void);
void plugin_broadcast_options(void);
void plugin_broadcast_global_params(void);
void plugin_broadcast_params(void);
void plugin_mpi_map_decomp_domain(size_t, size_t, int **, int **, size_t **);

void plugin_start(void);
void plugin_alloc(void);
void plugin_init(void);
void plugin_finalize(void);

void plugin_generate_default_state(void);
void plugin_compute_derived_state_vars(void);
void plugin_check_init_state_file(void);
void plugin_restore(void);

void plugin_print_decomposition(size_t, int **);
void plugin_print_global_param(plugin_global_param_struct *);
void plugin_print_options(plugin_option_struct *);
void plugin_print_parameters(plugin_parameters_struct *);

// Simulation
void plugin_force(void);
bool plugin_force_get_global_param(char *);
void plugin_force_validate_global_param(void);
void plugin_force_init(void);
void plugin_get_forcing_file_info(short unsigned int);
void plugin_get_forcing_file_skip(short unsigned int);
void plugin_force_start(void);
void plugin_force_end(void);
void plugin_run(void);

#endif /* PLUGIN_H */
