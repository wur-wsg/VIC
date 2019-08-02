/******************************************************************************
 * @section DESCRIPTION
 *
 * Plugin setup functions
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
* @brief    Get configuration file options
******************************************/
bool
plugin_get_global_param(char cmdstr[MAXSTRING])
{
    if (plugin_force_get_global_param(cmdstr)) {
    }
    else if (rout_get_global_param(cmdstr)) {
    }
    else if (efr_get_global_param(cmdstr)) {
    }
    else if(dam_get_global_param(cmdstr)){
    }
    else if (wu_get_global_param(cmdstr)) {
    }
    else if (irr_get_global_param(cmdstr)) {
    }
    else {
        return false;
    }

    return true;
}

/******************************************
* @brief    Validate configuration file options
******************************************/
void
plugin_validate_global_param(void)
{
    extern plugin_option_struct plugin_options;

    plugin_force_validate_global_param();
    if (plugin_options.ROUTING) {
        rout_validate_global_param();
    }
    if (plugin_options.EFR) {
        efr_validate_global_param();
    }
    if (plugin_options.DAMS) {
        dam_validate_global_param();
    }
    if (plugin_options.WATERUSE) {
        wu_validate_global_param();
    }
    if (plugin_options.IRRIGATION) {
        irr_validate_global_param();
    }
}

/******************************************
* @brief    Get parameter file values
******************************************/
bool
plugin_get_parameters(char cmdstr[MAXSTRING])
{
    if(dam_get_parameters(cmdstr)){
    }
    else if (irr_get_parameters(cmdstr)) {
    } 
    else {
        return false;
    }
    
    return true;
}

/******************************************
* @brief    Validate parameter file values
******************************************/
void
plugin_validate_parameters(void)
{
    extern plugin_option_struct    plugin_options;
    
    if(plugin_options.DAMS) {
        dam_validate_parameters();
    }
    if (plugin_options.IRRIGATION) {
        irr_validate_parameters();
    }
}

/******************************************
* @brief    Start plugins
******************************************/
void
plugin_start(void)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.ROUTING) {
        rout_start();
    }
    if (plugin_options.EFR) {
        efr_start();
    }
    if (plugin_options.DAMS) {
        dam_start();
    }
    if (plugin_options.WATERUSE) {
        wu_start();
    }
    if (plugin_options.IRRIGATION) {
        irr_start();
    }
}

/******************************************
* @brief    Allocate plugins
******************************************/
void
plugin_alloc(void)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.ROUTING) {
        rout_alloc();
    }
    if (plugin_options.EFR) {
        efr_alloc();
    }
    if (plugin_options.DAMS) {
        dam_alloc();
    }
    if (plugin_options.WATERUSE) {
        wu_alloc();
    }
}

/******************************************
* @brief    Initialize plugins
******************************************/
void
plugin_init(void)
{
    extern plugin_option_struct plugin_options;

    plugin_force_init();
    if (plugin_options.ROUTING) {
        rout_init();
    }
    if (plugin_options.DAMS) {
        dam_init();
    }
    if (plugin_options.WATERUSE) {
        wu_init();
    }
    if (plugin_options.IRRIGATION) {
        irr_late_alloc();
        irr_init();
    }

    plugin_set_state_meta_data_info();
}

/******************************************
* @brief    Populate plugins by default
******************************************/
void
plugin_generate_default_state(void)
{
    extern plugin_option_struct    plugin_options;
 
    if (plugin_options.DAMS) {
        dam_generate_default_state();
    }
}

/******************************************
* @brief    Plugin check state file
******************************************/
void
plugin_check_init_state_file(void)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.ROUTING) {
        rout_check_init_state_file();
    }
}

/******************************************
* @brief    Populate plugins from state file
******************************************/
void
plugin_restore(void)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.ROUTING) {
        rout_restore();
    }
    if (plugin_options.DAMS) {
        log_warn("DAM state restore not implemented yet...");
    }
}

/******************************************
* @brief    Populate plugins from derived values
******************************************/
void
plugin_compute_derived_state_vars(void)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.ROUTING) {
        rout_compute_derived_state_vars();
    }
}

/******************************************
* @brief    Finalize plugins
******************************************/
void
plugin_finalize(void)
{
    extern plugin_option_struct plugin_options;
    extern MPI_Datatype         plugin_mpi_global_struct_type;
    extern MPI_Datatype         plugin_mpi_filenames_struct_type;
    extern MPI_Datatype         plugin_mpi_option_struct_type;
    extern MPI_Datatype         plugin_mpi_param_struct_type;

    MPI_Type_free(&plugin_mpi_global_struct_type);
    MPI_Type_free(&plugin_mpi_filenames_struct_type);
    MPI_Type_free(&plugin_mpi_option_struct_type);
    MPI_Type_free(&plugin_mpi_param_struct_type);

    if (plugin_options.ROUTING) {
        rout_finalize();
    }
    if (plugin_options.EFR) {
        efr_finalize();
    }
    if (plugin_options.DAMS) {
        dam_finalize();
    }
    if (plugin_options.WATERUSE) {
        wu_finalize();
    }
    if (plugin_options.IRRIGATION) {
        irr_finalize();
    }
}
