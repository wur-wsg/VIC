/******************************************************************************
 * @section DESCRIPTION
 *
 * This routine sets the metadata structure for VIC state variables
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
 * @brief    Set output met data information
 *****************************************************************************/
void
set_state_meta_data_info()
{
    size_t                 v;

    extern option_struct   options;
    extern metadata_struct state_metadata[];

    // Build the list of state variables

    // Set missing and/or default values
    for (v = 0; v < (N_STATE_VARS + PLUGIN_N_STATE_VARS); v++) {
        // Set default string values
        snprintf(state_metadata[v].varname, MAXSTRING, "%s", MISSING_S);
        snprintf(state_metadata[v].long_name, MAXSTRING, "%s", MISSING_S);
        snprintf(state_metadata[v].standard_name, MAXSTRING, "%s", MISSING_S);
        snprintf(state_metadata[v].units, MAXSTRING, "%s", MISSING_S);
        snprintf(state_metadata[v].description, MAXSTRING, "%s", MISSING_S);
        // Set default number of elements
        state_metadata[v].nelem = 1;
    }

    // STATE_SOIL_MOISTURE
    snprintf(state_metadata[STATE_SOIL_MOISTURE].varname, MAXSTRING, "%s",
             "STATE_SOIL_MOISTURE");
    snprintf(state_metadata[STATE_SOIL_MOISTURE].long_name, MAXSTRING, "%s",
             "soil_moisture");
    snprintf(state_metadata[STATE_SOIL_MOISTURE].standard_name, MAXSTRING, "%s",
             "soil_layer_moisture");
    snprintf(state_metadata[STATE_SOIL_MOISTURE].units, MAXSTRING, "%s", "mm");
    snprintf(state_metadata[STATE_SOIL_MOISTURE].description, MAXSTRING, "%s",
             "soil total moisture contents including ice for each soil layer");

    // STATE_SOIL_ICE
    snprintf(state_metadata[STATE_SOIL_ICE].varname, MAXSTRING, "%s",
             "STATE_SOIL_ICE");
    snprintf(state_metadata[STATE_SOIL_ICE].long_name, MAXSTRING, "%s",
             "soil_ice");
    snprintf(state_metadata[STATE_SOIL_ICE].standard_name, MAXSTRING, "%s",
             "soil_moisture_ice_depth");
    snprintf(state_metadata[STATE_SOIL_ICE].units, MAXSTRING, "%s", "mm");
    snprintf(state_metadata[STATE_SOIL_ICE].description, MAXSTRING, "%s",
             "soil ice content for each soil layer");

    // STATE_CANOPY_WATER
    snprintf(state_metadata[STATE_CANOPY_WATER].varname, MAXSTRING, "%s",
             "STATE_CANOPY_WATER");
    snprintf(state_metadata[STATE_CANOPY_WATER].long_name, MAXSTRING, "%s",
             "canopy_water");
    snprintf(state_metadata[STATE_CANOPY_WATER].standard_name, MAXSTRING, "%s",
             "water_in_canopy");
    snprintf(state_metadata[STATE_CANOPY_WATER].units, MAXSTRING, "%s", "mm");
    snprintf(state_metadata[STATE_CANOPY_WATER].description, MAXSTRING, "%s",
             "amount of water stored in the vegetation canopy");

    if (options.CARBON) {
        // STATE_ANNUALNPP
        snprintf(state_metadata[STATE_ANNUALNPP].varname, MAXSTRING, "%s",
                 "STATE_ANNUALNPP");
        snprintf(state_metadata[STATE_ANNUALNPP].long_name, MAXSTRING, "%s",
                 "annualnpp");
        snprintf(state_metadata[STATE_ANNUALNPP].standard_name, MAXSTRING, "%s",
                 "running_total_annual_NPP");
        snprintf(state_metadata[STATE_ANNUALNPP].units, MAXSTRING, "%s",
                 "g m-2");
        snprintf(state_metadata[STATE_ANNUALNPP].description, MAXSTRING, "%s",
                 "running total annual NPP");

        // STATE_ANNUALNPPPREV
        snprintf(state_metadata[STATE_ANNUALNPPPREV].varname, MAXSTRING, "%s",
                 "STATE_ANNUALNPPPREV");
        snprintf(state_metadata[STATE_ANNUALNPPPREV].long_name, MAXSTRING, "%s",
                 "annualnppprev");
        snprintf(state_metadata[STATE_ANNUALNPPPREV].standard_name, MAXSTRING,
                 "%s",
                 "previous_year_total_annual_NPP");
        snprintf(state_metadata[STATE_ANNUALNPPPREV].units, MAXSTRING, "%s",
                 "g m-2");
        snprintf(state_metadata[STATE_ANNUALNPPPREV].description, MAXSTRING,
                 "%s",
                 "total annual NPP from previous year");

        // STATE_CLITTER
        snprintf(state_metadata[STATE_CLITTER].varname, MAXSTRING, "%s",
                 "STATE_CLITTER");
        snprintf(state_metadata[STATE_CLITTER].long_name, MAXSTRING, "%s",
                 "clitter");
        snprintf(state_metadata[STATE_CLITTER].standard_name, MAXSTRING, "%s",
                 "carbon_in_litter_pool");
        snprintf(state_metadata[STATE_CLITTER].units, MAXSTRING, "%s", "g m-2");
        snprintf(state_metadata[STATE_CLITTER].description, MAXSTRING, "%s",
                 "carbon storage in litter pool");

        // STATE_CINTER
        snprintf(state_metadata[STATE_CINTER].varname, MAXSTRING, "%s",
                 "STATE_CINTER");
        snprintf(state_metadata[STATE_CINTER].long_name, MAXSTRING, "%s",
                 "cinter");
        snprintf(state_metadata[STATE_CINTER].standard_name, MAXSTRING, "%s",
                 "carbon_in_intermediate_pool");
        snprintf(state_metadata[STATE_CINTER].units, MAXSTRING, "%s", "g m-2");
        snprintf(state_metadata[STATE_CINTER].description, MAXSTRING, "%s",
                 "carbon storage in intermediate pool");

        // STATE_CSLOW
        snprintf(state_metadata[STATE_CSLOW].varname, MAXSTRING, "%s",
                 "STATE_CSLOW");
        snprintf(state_metadata[STATE_CSLOW].long_name, MAXSTRING, "%s",
                 "cslow");
        snprintf(state_metadata[STATE_CSLOW].standard_name, MAXSTRING, "%s",
                 "carbon_in_slow_pool");
        snprintf(state_metadata[STATE_CSLOW].units, MAXSTRING, "%s", "g m-2");
        snprintf(state_metadata[STATE_CSLOW].description, MAXSTRING, "%s",
                 "carbon storage in slow pool");
    }
    // STATE_SNOW_AGE
    snprintf(state_metadata[STATE_SNOW_AGE].varname, MAXSTRING, "%s",
             "STATE_SNOW_AGE");
    snprintf(state_metadata[STATE_SNOW_AGE].long_name, MAXSTRING, "%s",
             "snow_age");
    snprintf(state_metadata[STATE_SNOW_AGE].standard_name, MAXSTRING, "%s",
             "age_since_last_new_snow");
    snprintf(state_metadata[STATE_SNOW_AGE].units, MAXSTRING, "%s",
             "model_time_step");
    snprintf(state_metadata[STATE_SNOW_AGE].description, MAXSTRING, "%s",
             "number of model time steps since the last new snow");

    // STATE_SNOW_MELT_STATE
    snprintf(state_metadata[STATE_SNOW_MELT_STATE].varname, MAXSTRING, "%s",
             "STATE_SNOW_MELT_STATE");
    snprintf(state_metadata[STATE_SNOW_MELT_STATE].long_name, MAXSTRING, "%s",
             "snow_melt_state");
    snprintf(state_metadata[STATE_SNOW_MELT_STATE].standard_name, MAXSTRING,
             "%s",
             "snow_melting_phase");
    snprintf(state_metadata[STATE_SNOW_MELT_STATE].units, MAXSTRING, "%s",
             "1 melting, 0 not melting");
    snprintf(
        state_metadata[STATE_SNOW_MELT_STATE].description, MAXSTRING, "%s",
        "flag to indicate whether snowpack is in accumulation or melting phase");

    // STATE_SNOW_COVERAGE
    snprintf(state_metadata[STATE_SNOW_COVERAGE].varname, MAXSTRING, "%s",
             "STATE_SNOW_COVERAGE");
    snprintf(state_metadata[STATE_SNOW_COVERAGE].long_name, MAXSTRING, "%s",
             "snow_coverage");
    snprintf(state_metadata[STATE_SNOW_COVERAGE].standard_name, MAXSTRING, "%s",
             "snow_coverage_fraction");
    snprintf(state_metadata[STATE_SNOW_COVERAGE].units, MAXSTRING, "%s", "1");
    snprintf(state_metadata[STATE_SNOW_COVERAGE].description, MAXSTRING, "%s",
             "fraction of grid cell area covered by snow");

    // STATE_SNOW_WATER_EQUIVALENT
    snprintf(state_metadata[STATE_SNOW_WATER_EQUIVALENT].varname, MAXSTRING,
             "%s",
             "STATE_SNOW_WATER_EQUIVALENT");
    snprintf(state_metadata[STATE_SNOW_WATER_EQUIVALENT].long_name, MAXSTRING,
             "%s",
             "snow_water_equivalent");
    snprintf(state_metadata[STATE_SNOW_WATER_EQUIVALENT].standard_name,
             MAXSTRING, "%s",
             "snow_water_equivalent");
    snprintf(state_metadata[STATE_SNOW_WATER_EQUIVALENT].units, MAXSTRING, "%s",
             "m");
    snprintf(state_metadata[STATE_SNOW_WATER_EQUIVALENT].description, MAXSTRING,
             "%s",
             "snow water equivalent");

    // STATE_SNOW_SURF_TEMP
    snprintf(state_metadata[STATE_SNOW_SURF_TEMP].varname, MAXSTRING, "%s",
             "STATE_SNOW_SURF_TEMP");
    snprintf(state_metadata[STATE_SNOW_SURF_TEMP].long_name, MAXSTRING, "%s",
             "snow_surf_temp");
    snprintf(state_metadata[STATE_SNOW_SURF_TEMP].standard_name, MAXSTRING,
             "%s",
             "snow_surface_temperature");
    snprintf(state_metadata[STATE_SNOW_SURF_TEMP].units, MAXSTRING, "%s", "C");
    snprintf(state_metadata[STATE_SNOW_SURF_TEMP].description, MAXSTRING, "%s",
             "snow surface layer temperature");

    // STATE_SNOW_SURF_WATER
    snprintf(state_metadata[STATE_SNOW_SURF_WATER].varname, MAXSTRING, "%s",
             "STATE_SNOW_SURF_WATER");
    snprintf(state_metadata[STATE_SNOW_SURF_WATER].long_name, MAXSTRING, "%s",
             "snow_surf_water");
    snprintf(state_metadata[STATE_SNOW_SURF_WATER].standard_name, MAXSTRING,
             "%s",
             "snow_surface_liquid_water");
    snprintf(state_metadata[STATE_SNOW_SURF_WATER].units, MAXSTRING, "%s", "m");
    snprintf(state_metadata[STATE_SNOW_SURF_WATER].description, MAXSTRING, "%s",
             "liquid water content of the snow surface layer");

    // STATE_SNOW_PACK_TEMP
    snprintf(state_metadata[STATE_SNOW_PACK_TEMP].varname, MAXSTRING, "%s",
             "STATE_SNOW_PACK_TEMP");
    snprintf(state_metadata[STATE_SNOW_PACK_TEMP].long_name, MAXSTRING, "%s",
             "snow_pack_temp");
    snprintf(state_metadata[STATE_SNOW_PACK_TEMP].standard_name, MAXSTRING,
             "%s",
             "snow_pack_temperature");
    snprintf(state_metadata[STATE_SNOW_PACK_TEMP].units, MAXSTRING, "%s", "C");
    snprintf(state_metadata[STATE_SNOW_PACK_TEMP].description, MAXSTRING, "%s",
             "snow pack layer temperature");

    // STATE_SNOW_PACK_WATER
    snprintf(state_metadata[STATE_SNOW_PACK_WATER].varname, MAXSTRING, "%s",
             "STATE_SNOW_PACK_WATER");
    snprintf(state_metadata[STATE_SNOW_PACK_WATER].long_name, MAXSTRING, "%s",
             "snow_pack_water");
    snprintf(state_metadata[STATE_SNOW_PACK_WATER].standard_name, MAXSTRING,
             "%s",
             "snow_pack_liquid_water");
    snprintf(state_metadata[STATE_SNOW_PACK_WATER].units, MAXSTRING, "%s", "m");
    snprintf(state_metadata[STATE_SNOW_PACK_WATER].description, MAXSTRING, "%s",
             "liquid water content of the snow pack layer");

    // STATE_SNOW_DENSITY
    snprintf(state_metadata[STATE_SNOW_DENSITY].varname, MAXSTRING, "%s",
             "STATE_SNOW_DENSITY");
    snprintf(state_metadata[STATE_SNOW_DENSITY].long_name, MAXSTRING, "%s",
             "snow_density");
    snprintf(state_metadata[STATE_SNOW_DENSITY].standard_name, MAXSTRING, "%s",
             "snowpack_density");
    snprintf(state_metadata[STATE_SNOW_DENSITY].units, MAXSTRING, "%s",
             "kg m-3");
    snprintf(state_metadata[STATE_SNOW_DENSITY].description, MAXSTRING, "%s",
             "snowpack density");

    // STATE_SNOW_COLD_CONTENT
    snprintf(state_metadata[STATE_SNOW_COLD_CONTENT].varname, MAXSTRING, "%s",
             "STATE_SNOW_COLD_CONTENT");
    snprintf(state_metadata[STATE_SNOW_COLD_CONTENT].long_name, MAXSTRING, "%s",
             "snow_cold_content");
    snprintf(state_metadata[STATE_SNOW_COLD_CONTENT].standard_name, MAXSTRING,
             "%s",
             "snowpack_cold_content");
    snprintf(state_metadata[STATE_SNOW_COLD_CONTENT].units, MAXSTRING, "%s",
             "J m-2");
    snprintf(state_metadata[STATE_SNOW_COLD_CONTENT].description, MAXSTRING,
             "%s",
             "snowpack cold content");

    // STATE_SNOW_CANOPY
    snprintf(state_metadata[STATE_SNOW_CANOPY].varname, MAXSTRING, "%s",
             "STATE_SNOW_CANOPY");
    snprintf(state_metadata[STATE_SNOW_CANOPY].long_name, MAXSTRING, "%s",
             "snow_canopy");
    snprintf(state_metadata[STATE_SNOW_CANOPY].standard_name, MAXSTRING, "%s",
             "snow_water_equivalent_intercepted_in_canopy");
    snprintf(state_metadata[STATE_SNOW_CANOPY].units, MAXSTRING, "%s", "m");
    snprintf(state_metadata[STATE_SNOW_CANOPY].description, MAXSTRING, "%s",
             "snow interception storage in canopy");

    // STATE_SOIL_NODE_TEMP
    snprintf(state_metadata[STATE_SOIL_NODE_TEMP].varname, MAXSTRING, "%s",
             "STATE_SOIL_NODE_TEMP");
    snprintf(state_metadata[STATE_SOIL_NODE_TEMP].long_name, MAXSTRING, "%s",
             "soil_node_temp");
    snprintf(state_metadata[STATE_SOIL_NODE_TEMP].standard_name, MAXSTRING,
             "%s",
             "soil_node_temperature");
    snprintf(state_metadata[STATE_SOIL_NODE_TEMP].units, MAXSTRING, "%s", "C");
    snprintf(state_metadata[STATE_SOIL_NODE_TEMP].description, MAXSTRING, "%s",
             "soil temperature of each soil thermal node");

    // STATE_FOLIAGE_TEMPERATURE
    snprintf(state_metadata[STATE_FOLIAGE_TEMPERATURE].varname, MAXSTRING, "%s",
             "STATE_FOLIAGE_TEMPERATURE");
    snprintf(state_metadata[STATE_FOLIAGE_TEMPERATURE].long_name, MAXSTRING,
             "%s",
             "foliage_temperature");
    snprintf(state_metadata[STATE_FOLIAGE_TEMPERATURE].standard_name, MAXSTRING,
             "%s",
             "foliage_temperature");
    snprintf(state_metadata[STATE_FOLIAGE_TEMPERATURE].units, MAXSTRING, "%s",
             "C");
    snprintf(state_metadata[STATE_FOLIAGE_TEMPERATURE].description, MAXSTRING,
             "%s",
             "overstory vegetaion temperature");

    // STATE_ENERGY_LONGUNDEROUT
    snprintf(state_metadata[STATE_ENERGY_LONGUNDEROUT].varname, MAXSTRING, "%s",
             "STATE_ENERGY_LONGUNDEROUT");
    snprintf(state_metadata[STATE_ENERGY_LONGUNDEROUT].long_name, MAXSTRING,
             "%s",
             "energy_longunderout");
    snprintf(state_metadata[STATE_ENERGY_LONGUNDEROUT].standard_name, MAXSTRING,
             "%s",
             "longwave_out_from_understory");
    snprintf(state_metadata[STATE_ENERGY_LONGUNDEROUT].units, MAXSTRING, "%s",
             "W m-2");
    snprintf(state_metadata[STATE_ENERGY_LONGUNDEROUT].description, MAXSTRING,
             "%s",
             "outgoing longwave flux from understory vegetation");

    // STATE_ENERGY_SNOW_FLUX
    snprintf(state_metadata[STATE_ENERGY_SNOW_FLUX].varname, MAXSTRING, "%s",
             "STATE_ENERGY_SNOW_FLUX");
    snprintf(state_metadata[STATE_ENERGY_SNOW_FLUX].long_name, MAXSTRING, "%s",
             "energy_snow_flux");
    snprintf(state_metadata[STATE_ENERGY_SNOW_FLUX].standard_name, MAXSTRING,
             "%s",
             "snowpack_thermal_flux");
    snprintf(state_metadata[STATE_ENERGY_SNOW_FLUX].units, MAXSTRING, "%s",
             "W m-2");
    snprintf(state_metadata[STATE_ENERGY_SNOW_FLUX].description, MAXSTRING,
             "%s",
             "thermal flux through snowpack");

    // STATE_GRIDCELL_AVG_ALBEDO
    snprintf(state_metadata[STATE_AVG_ALBEDO].varname, MAXSTRING, "%s",
             "STATE_AVG_ALBEDO");
    snprintf(state_metadata[STATE_AVG_ALBEDO].long_name, MAXSTRING, "%s",
             "state_avg_albedo");
    snprintf(state_metadata[STATE_AVG_ALBEDO].standard_name, MAXSTRING, "%s",
             "state_gridcell_avg_albedo");
    snprintf(state_metadata[STATE_AVG_ALBEDO].units, MAXSTRING, "%s",
             "fraction");
    snprintf(state_metadata[STATE_AVG_ALBEDO].description, MAXSTRING, "%s",
             "gridcell averaged albedo");

    if (options.LAKES) {
        // STATE_LAKE_SOIL_MOISTURE
        snprintf(state_metadata[STATE_LAKE_SOIL_MOISTURE].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_SOIL_MOISTURE");
        snprintf(state_metadata[STATE_LAKE_SOIL_MOISTURE].long_name, MAXSTRING,
                 "%s",
                 "lake_soil_moisture");
        snprintf(state_metadata[STATE_LAKE_SOIL_MOISTURE].standard_name,
                 MAXSTRING, "%s",
                 "lake_soil_moisture");
        snprintf(state_metadata[STATE_LAKE_SOIL_MOISTURE].units, MAXSTRING,
                 "%s", "mm");
        snprintf(state_metadata[STATE_LAKE_SOIL_MOISTURE].description,
                 MAXSTRING, "%s",
                 "soil moisture below lake");

        // STATE_LAKE_SOIL_ICE
        snprintf(state_metadata[STATE_LAKE_SOIL_ICE].varname, MAXSTRING, "%s",
                 "STATE_LAKE_SOIL_ICE");
        snprintf(state_metadata[STATE_LAKE_SOIL_ICE].long_name, MAXSTRING, "%s",
                 "lake_soil_ice");
        snprintf(state_metadata[STATE_LAKE_SOIL_ICE].standard_name, MAXSTRING,
                 "%s",
                 "lake_soil_ice_content");
        snprintf(state_metadata[STATE_LAKE_SOIL_ICE].units, MAXSTRING, "%s",
                 "mm");
        snprintf(state_metadata[STATE_LAKE_SOIL_ICE].description, MAXSTRING,
                 "%s",
                 "soil ice content below lake");

        if (options.CARBON) {
            // STATE_LAKE_CLITTER
            snprintf(state_metadata[STATE_LAKE_CLITTER].varname, MAXSTRING,
                     "%s",
                     "STATE_LAKE_CLITTER");
            snprintf(state_metadata[STATE_LAKE_CLITTER].long_name, MAXSTRING,
                     "%s",
                     "lake_clitter");
            snprintf(state_metadata[STATE_LAKE_CLITTER].standard_name,
                     MAXSTRING, "%s",
                     "lake_carbon_in_litter_pool");
            snprintf(state_metadata[STATE_LAKE_CLITTER].units, MAXSTRING, "%s",
                     "g m-2");
            snprintf(state_metadata[STATE_LAKE_CLITTER].description, MAXSTRING,
                     "%s",
                     "carbon storage in litter pool below lake");

            // STATE_LAKE_CINTER
            snprintf(state_metadata[STATE_LAKE_CINTER].varname, MAXSTRING, "%s",
                     "STATE_LAKE_CINTER");
            snprintf(state_metadata[STATE_LAKE_CINTER].long_name, MAXSTRING,
                     "%s", "lake_cinter");
            snprintf(state_metadata[STATE_LAKE_CINTER].standard_name, MAXSTRING,
                     "%s",
                     "lake_carbon_in_intermediate_pool");
            snprintf(state_metadata[STATE_LAKE_CINTER].units, MAXSTRING, "%s",
                     "g m-2");
            snprintf(state_metadata[STATE_LAKE_CINTER].description, MAXSTRING,
                     "%s",
                     "carbon storage in intermediate pool below lake");

            // STATE_LAKE_CSLOW
            snprintf(state_metadata[STATE_LAKE_CSLOW].varname, MAXSTRING, "%s",
                     "STATE_LAKE_CSLOW");
            snprintf(state_metadata[STATE_LAKE_CSLOW].long_name, MAXSTRING,
                     "%s", "lake_cslow");
            snprintf(state_metadata[STATE_LAKE_CSLOW].standard_name, MAXSTRING,
                     "%s",
                     "lake_carbon_in_slow_pool");
            snprintf(state_metadata[STATE_LAKE_CSLOW].units, MAXSTRING, "%s",
                     "g m-2");
            snprintf(state_metadata[STATE_LAKE_CSLOW].description, MAXSTRING,
                     "%s",
                     "carbon storage in slow pool below lake");
        }

        // STATE_LAKE_SNOW_AGE
        snprintf(state_metadata[STATE_LAKE_SNOW_AGE].varname, MAXSTRING, "%s",
                 "STATE_LAKE_SNOW_AGE");
        snprintf(state_metadata[STATE_LAKE_SNOW_AGE].long_name, MAXSTRING, "%s",
                 "lake_snow_age");
        snprintf(state_metadata[STATE_LAKE_SNOW_AGE].standard_name, MAXSTRING,
                 "%s",
                 "lake_age_since_last_new_snow");
        snprintf(state_metadata[STATE_LAKE_SNOW_AGE].units, MAXSTRING, "%s",
                 "model_time_step");
        snprintf(
            state_metadata[STATE_LAKE_SNOW_AGE].description, MAXSTRING,
            "%s",
            "number of model time steps since the last new snow on lake ice");

        // STATE_LAKE_SNOW_MELT_STATE
        snprintf(state_metadata[STATE_LAKE_SNOW_MELT_STATE].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_SNOW_MELT_STATE");
        snprintf(state_metadata[STATE_LAKE_SNOW_MELT_STATE].long_name,
                 MAXSTRING, "%s",
                 "lake_snow_melt_state");
        snprintf(state_metadata[STATE_LAKE_SNOW_MELT_STATE].standard_name,
                 MAXSTRING, "%s",
                 "lake_snow_melting_phase");
        snprintf(state_metadata[STATE_LAKE_SNOW_MELT_STATE].units, MAXSTRING,
                 "%s",
                 "1 melting, 0 not melting");
        snprintf(
            state_metadata[STATE_LAKE_SNOW_MELT_STATE].description,
            MAXSTRING, "%s",
            "flag to indicate whether snowpack is in accumulation or melting phase on lake ice");

        // STATE_LAKE_SNOW_COVERAGE
        snprintf(state_metadata[STATE_LAKE_SNOW_COVERAGE].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_SNOW_COVERAGE");
        snprintf(state_metadata[STATE_LAKE_SNOW_COVERAGE].long_name, MAXSTRING,
                 "%s",
                 "lake_snow_coverage");
        snprintf(state_metadata[STATE_LAKE_SNOW_COVERAGE].standard_name,
                 MAXSTRING, "%s",
                 "lake_snow_coverage_fraction");
        snprintf(state_metadata[STATE_LAKE_SNOW_COVERAGE].units, MAXSTRING,
                 "%s", "1");
        snprintf(state_metadata[STATE_LAKE_SNOW_COVERAGE].description,
                 MAXSTRING, "%s",
                 "fraction of grid cell area covered by snow on lake ice");

        // STATE_LAKE_SNOW_WATER_EQUIVALENT
        snprintf(state_metadata[STATE_LAKE_SNOW_WATER_EQUIVALENT].varname,
                 MAXSTRING, "%s",
                 "STATE_LAKE_SNOW_WATER_EQUIVALENT");
        snprintf(state_metadata[STATE_LAKE_SNOW_WATER_EQUIVALENT].long_name,
                 MAXSTRING, "%s",
                 "lake_snow_water_equivalent");
        snprintf(state_metadata[STATE_LAKE_SNOW_WATER_EQUIVALENT].standard_name,
                 MAXSTRING, "%s",
                 "lake_snow_water_equivalent");
        snprintf(state_metadata[STATE_LAKE_SNOW_WATER_EQUIVALENT].units,
                 MAXSTRING, "%s", "m");
        snprintf(state_metadata[STATE_LAKE_SNOW_WATER_EQUIVALENT].description,
                 MAXSTRING, "%s",
                 "lake snow water equivalent on lake ice");

        // STATE_LAKE_SNOW_SURF_TEMP
        snprintf(state_metadata[STATE_LAKE_SNOW_SURF_TEMP].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_SNOW_SURF_TEMP");
        snprintf(state_metadata[STATE_LAKE_SNOW_SURF_TEMP].long_name, MAXSTRING,
                 "%s",
                 "lake_snow_surf_temp");
        snprintf(state_metadata[STATE_LAKE_SNOW_SURF_TEMP].standard_name,
                 MAXSTRING, "%s",
                 "lake_snow_surface_temperature");
        snprintf(state_metadata[STATE_LAKE_SNOW_SURF_TEMP].units, MAXSTRING,
                 "%s", "C");
        snprintf(state_metadata[STATE_LAKE_SNOW_SURF_TEMP].description,
                 MAXSTRING, "%s",
                 "snow surface layer temperature on lake ice");

        // STATE_LAKE_SNOW_SURF_WATER
        snprintf(state_metadata[STATE_LAKE_SNOW_SURF_WATER].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_SNOW_SURF_WATER");
        snprintf(state_metadata[STATE_LAKE_SNOW_SURF_WATER].long_name,
                 MAXSTRING, "%s",
                 "lake_snow_surf_water");
        snprintf(state_metadata[STATE_LAKE_SNOW_SURF_WATER].standard_name,
                 MAXSTRING, "%s",
                 "lake_snow_surface_temperature");
        snprintf(state_metadata[STATE_LAKE_SNOW_SURF_WATER].units, MAXSTRING,
                 "%s", "m");
        snprintf(state_metadata[STATE_LAKE_SNOW_SURF_WATER].description,
                 MAXSTRING, "%s",
                 "liquid water content of the snow surface layer on lake ice");

        // STATE_LAKE_SNOW_PACK_TEMP
        snprintf(state_metadata[STATE_LAKE_SNOW_PACK_TEMP].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_SNOW_PACK_TEMP");
        snprintf(state_metadata[STATE_LAKE_SNOW_PACK_TEMP].long_name, MAXSTRING,
                 "%s",
                 "lake_snow_pack_temp");
        snprintf(state_metadata[STATE_LAKE_SNOW_PACK_TEMP].standard_name,
                 MAXSTRING, "%s",
                 "lake_snow_pack_temperature");
        snprintf(state_metadata[STATE_LAKE_SNOW_PACK_TEMP].units, MAXSTRING,
                 "%s", "C");
        snprintf(state_metadata[STATE_LAKE_SNOW_PACK_TEMP].description,
                 MAXSTRING, "%s",
                 "snow pack layer temperature on lake ice");

        // STATE_LAKE_SNOW_PACK_WATER
        snprintf(state_metadata[STATE_LAKE_SNOW_PACK_WATER].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_SNOW_PACK_WATER");
        snprintf(state_metadata[STATE_LAKE_SNOW_PACK_WATER].long_name,
                 MAXSTRING, "%s",
                 "lake_snow_pack_water");
        snprintf(state_metadata[STATE_LAKE_SNOW_PACK_WATER].standard_name,
                 MAXSTRING, "%s",
                 "lake_snow_surface_liquid_water");
        snprintf(state_metadata[STATE_LAKE_SNOW_PACK_WATER].units, MAXSTRING,
                 "%s", "m");
        snprintf(state_metadata[STATE_LAKE_SNOW_PACK_WATER].description,
                 MAXSTRING, "%s",
                 "liquid water content of the snow surface layer on lake ice");

        // STATE_LAKE_SNOW_DENSITY
        snprintf(state_metadata[STATE_LAKE_SNOW_DENSITY].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_SNOW_DENSITY");
        snprintf(state_metadata[STATE_LAKE_SNOW_DENSITY].long_name, MAXSTRING,
                 "%s",
                 "lake_snow_density");
        snprintf(state_metadata[STATE_LAKE_SNOW_DENSITY].standard_name,
                 MAXSTRING, "%s",
                 "lake_snowpack_density");
        snprintf(state_metadata[STATE_LAKE_SNOW_DENSITY].units, MAXSTRING, "%s",
                 "kg m-3");
        snprintf(state_metadata[STATE_LAKE_SNOW_DENSITY].description, MAXSTRING,
                 "%s",
                 "snowpack density on lake ice");

        // STATE_LAKE_SNOW_COLD_CONTENT
        snprintf(state_metadata[STATE_LAKE_SNOW_COLD_CONTENT].varname,
                 MAXSTRING, "%s",
                 "STATE_LAKE_SNOW_COLD_CONTENT");
        snprintf(state_metadata[STATE_LAKE_SNOW_COLD_CONTENT].long_name,
                 MAXSTRING, "%s",
                 "lake_snow_cold_content");
        snprintf(state_metadata[STATE_LAKE_SNOW_COLD_CONTENT].standard_name,
                 MAXSTRING, "%s",
                 "lake_snowpack_cold_content");
        snprintf(state_metadata[STATE_LAKE_SNOW_COLD_CONTENT].units, MAXSTRING,
                 "%s", "J m-2");
        snprintf(state_metadata[STATE_LAKE_SNOW_COLD_CONTENT].description,
                 MAXSTRING, "%s",
                 "snowpack cold content on lake ice");

        // STATE_LAKE_SNOW_CANOPY
        snprintf(state_metadata[STATE_LAKE_SNOW_CANOPY].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_SNOW_CANOPY");
        snprintf(state_metadata[STATE_LAKE_SNOW_CANOPY].long_name, MAXSTRING,
                 "%s",
                 "lake_snow_canopy");
        snprintf(state_metadata[STATE_LAKE_SNOW_CANOPY].standard_name,
                 MAXSTRING, "%s",
                 "lake_snow_water_equivalent_intercepted_in_canopy");
        snprintf(state_metadata[STATE_LAKE_SNOW_CANOPY].units, MAXSTRING, "%s",
                 "m");
        snprintf(state_metadata[STATE_LAKE_SNOW_CANOPY].description, MAXSTRING,
                 "%s",
                 "snow interception storage in canopy on lake ice");

        // STATE_LAKE_SOIL_NODE_TEMP
        snprintf(state_metadata[STATE_LAKE_SOIL_NODE_TEMP].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_SOIL_NODE_TEMP");
        snprintf(state_metadata[STATE_LAKE_SOIL_NODE_TEMP].long_name, MAXSTRING,
                 "%s",
                 "lake_soil_node_temp");
        snprintf(state_metadata[STATE_LAKE_SOIL_NODE_TEMP].standard_name,
                 MAXSTRING, "%s",
                 "lake_soil_node_temperature");
        snprintf(state_metadata[STATE_LAKE_SOIL_NODE_TEMP].units, MAXSTRING,
                 "%s", "C");
        snprintf(state_metadata[STATE_LAKE_SOIL_NODE_TEMP].description,
                 MAXSTRING, "%s",
                 "soil temperature of each soil thermal node below lake");

        // STATE_LAKE_ACTIVE_LAYERS
        snprintf(state_metadata[STATE_LAKE_ACTIVE_LAYERS].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_ACTIVE_LAYERS");
        snprintf(state_metadata[STATE_LAKE_ACTIVE_LAYERS].long_name, MAXSTRING,
                 "%s",
                 "lake_active_layers");
        snprintf(state_metadata[STATE_LAKE_ACTIVE_LAYERS].standard_name,
                 MAXSTRING, "%s",
                 "lake_active_layers");
        snprintf(state_metadata[STATE_LAKE_ACTIVE_LAYERS].units, MAXSTRING,
                 "%s", "-");
        snprintf(
            state_metadata[STATE_LAKE_ACTIVE_LAYERS].description,
            MAXSTRING, "%s",
            "number of nodes whose corresponding layers currently contain water");

        // STATE_LAKE_LAYER_DZ
        snprintf(state_metadata[STATE_LAKE_LAYER_DZ].varname, MAXSTRING, "%s",
                 "STATE_LAKE_LAYER_DZ");
        snprintf(state_metadata[STATE_LAKE_LAYER_DZ].long_name, MAXSTRING, "%s",
                 "lake_layer_dz");
        snprintf(state_metadata[STATE_LAKE_LAYER_DZ].standard_name, MAXSTRING,
                 "%s",
                 "lake_thickness_layer_below_surface");
        snprintf(state_metadata[STATE_LAKE_LAYER_DZ].units, MAXSTRING, "%s",
                 "m");
        snprintf(
            state_metadata[STATE_LAKE_LAYER_DZ].description, MAXSTRING,
            "%s",
            "vertical thickness of all horizontal lake water layers below the surface layer");

        // STATE_LAKE_SURF_LAYER_DZ
        snprintf(state_metadata[STATE_LAKE_SURF_LAYER_DZ].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_SURF_LAYER_DZ");
        snprintf(state_metadata[STATE_LAKE_SURF_LAYER_DZ].long_name, MAXSTRING,
                 "%s",
                 "lake_surf_layer_dz");
        snprintf(state_metadata[STATE_LAKE_SURF_LAYER_DZ].standard_name,
                 MAXSTRING, "%s",
                 "lake_thickness_surface_layer");
        snprintf(state_metadata[STATE_LAKE_SURF_LAYER_DZ].units, MAXSTRING,
                 "%s", "m");
        snprintf(state_metadata[STATE_LAKE_SURF_LAYER_DZ].description,
                 MAXSTRING, "%s",
                 "vertical thickness of surface water layer in lake");

        // STATE_LAKE_DEPTH
        snprintf(state_metadata[STATE_LAKE_DEPTH].varname, MAXSTRING, "%s",
                 "STATE_LAKE_DEPTH");
        snprintf(state_metadata[STATE_LAKE_DEPTH].long_name, MAXSTRING, "%s",
                 "lake_depth");
        snprintf(state_metadata[STATE_LAKE_DEPTH].standard_name, MAXSTRING,
                 "%s",
                 "lake_liquid_water_depth");
        snprintf(state_metadata[STATE_LAKE_DEPTH].units, MAXSTRING, "%s", "m");
        snprintf(state_metadata[STATE_LAKE_DEPTH].description, MAXSTRING, "%s",
                 "distance from surface to deepest point in lake");

        // STATE_LAKE_LAYER_SURF_AREA
        snprintf(state_metadata[STATE_LAKE_LAYER_SURF_AREA].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_LAYER_SURF_AREA");
        snprintf(state_metadata[STATE_LAKE_LAYER_SURF_AREA].long_name,
                 MAXSTRING, "%s",
                 "lake_layer_surf_area");
        snprintf(state_metadata[STATE_LAKE_LAYER_SURF_AREA].standard_name,
                 MAXSTRING, "%s",
                 "lake_node_surface_area");
        snprintf(state_metadata[STATE_LAKE_LAYER_SURF_AREA].units, MAXSTRING,
                 "%s", "m2");
        snprintf(state_metadata[STATE_LAKE_LAYER_SURF_AREA].description,
                 MAXSTRING, "%s",
                 "surface area of liquid water in lake at each node");

        // STATE_LAKE_SURF_AREA
        snprintf(state_metadata[STATE_LAKE_SURF_AREA].varname, MAXSTRING, "%s",
                 "STATE_LAKE_SURF_AREA");
        snprintf(state_metadata[STATE_LAKE_SURF_AREA].long_name, MAXSTRING,
                 "%s",
                 "lake_surf_area");
        snprintf(state_metadata[STATE_LAKE_SURF_AREA].standard_name, MAXSTRING,
                 "%s",
                 "lake_surface_area");
        snprintf(state_metadata[STATE_LAKE_SURF_AREA].units, MAXSTRING, "%s",
                 "m2");
        snprintf(state_metadata[STATE_LAKE_SURF_AREA].description, MAXSTRING,
                 "%s",
                 "surface area of liquid plus ice water on lake surface");

        // STATE_LAKE_VOLUME
        snprintf(state_metadata[STATE_LAKE_VOLUME].varname, MAXSTRING, "%s",
                 "STATE_LAKE_VOLUME");
        snprintf(state_metadata[STATE_LAKE_VOLUME].long_name, MAXSTRING, "%s",
                 "lake_volume");
        snprintf(state_metadata[STATE_LAKE_VOLUME].standard_name, MAXSTRING,
                 "%s", "lake_volume");
        snprintf(state_metadata[STATE_LAKE_VOLUME].units, MAXSTRING, "%s",
                 "m3");
        snprintf(
            state_metadata[STATE_LAKE_VOLUME].description, MAXSTRING, "%s",
            "lake total volume including liquid water equivalent of lake ice");

        // STATE_LAKE_LAYER_TEMP
        snprintf(state_metadata[STATE_LAKE_LAYER_TEMP].varname, MAXSTRING, "%s",
                 "STATE_LAKE_LAYER_TEMP");
        snprintf(state_metadata[STATE_LAKE_LAYER_TEMP].long_name, MAXSTRING,
                 "%s",
                 "lake_layer_temp");
        snprintf(state_metadata[STATE_LAKE_LAYER_TEMP].standard_name, MAXSTRING,
                 "%s",
                 "lake_layer_temp");
        snprintf(state_metadata[STATE_LAKE_LAYER_TEMP].units, MAXSTRING, "%s",
                 "C");
        snprintf(state_metadata[STATE_LAKE_LAYER_TEMP].description, MAXSTRING,
                 "%s",
                 "lake water temperature at each node");

        // STATE_LAKE_AVERAGE_TEMP
        snprintf(state_metadata[STATE_LAKE_AVERAGE_TEMP].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_AVERAGE_TEMP");
        snprintf(state_metadata[STATE_LAKE_AVERAGE_TEMP].long_name, MAXSTRING,
                 "%s",
                 "lake_average_temp");
        snprintf(state_metadata[STATE_LAKE_AVERAGE_TEMP].standard_name,
                 MAXSTRING, "%s",
                 "lake_average_temperature");
        snprintf(state_metadata[STATE_LAKE_AVERAGE_TEMP].units, MAXSTRING, "%s",
                 "C");
        snprintf(state_metadata[STATE_LAKE_AVERAGE_TEMP].description, MAXSTRING,
                 "%s",
                 "average liquid water temperature of entire lake");

        // STATE_LAKE_ICE_AREA_FRAC
        snprintf(state_metadata[STATE_LAKE_ICE_AREA_FRAC].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_ICE_AREA");
        snprintf(state_metadata[STATE_LAKE_ICE_AREA_FRAC].long_name, MAXSTRING,
                 "%s",
                 "lake_ice_area");
        snprintf(state_metadata[STATE_LAKE_ICE_AREA_FRAC].standard_name,
                 MAXSTRING, "%s",
                 "lake_ice_coverage");
        snprintf(state_metadata[STATE_LAKE_ICE_AREA_FRAC].units, MAXSTRING,
                 "%s", "m2");
        snprintf(state_metadata[STATE_LAKE_ICE_AREA_FRAC].description,
                 MAXSTRING, "%s",
                 "area of ice coverage on lake at beginning of time step");

        // STATE_LAKE_ICE_AREA_FRAC_NEW
        snprintf(state_metadata[STATE_LAKE_ICE_AREA_FRAC_NEW].varname,
                 MAXSTRING, "%s",
                 "STATE_LAKE_ICE_AREA_NEW");
        snprintf(state_metadata[STATE_LAKE_ICE_AREA_FRAC_NEW].long_name,
                 MAXSTRING, "%s",
                 "lake_ice_area_new");
        snprintf(state_metadata[STATE_LAKE_ICE_AREA_FRAC_NEW].standard_name,
                 MAXSTRING, "%s",
                 "lake_ice_area_new");
        snprintf(state_metadata[STATE_LAKE_ICE_AREA_FRAC_NEW].units, MAXSTRING,
                 "%s", "m2");
        snprintf(state_metadata[STATE_LAKE_ICE_AREA_FRAC_NEW].description,
                 MAXSTRING, "%s",
                 "area of ice coverage on lake at end of time step");

        // STATE_LAKE_ICE_WATER_EQUIVALENT
        snprintf(state_metadata[STATE_LAKE_ICE_WATER_EQUIVALENT].varname,
                 MAXSTRING, "%s",
                 "STATE_LAKE_ICE_WATER_EQUIVALENT");
        snprintf(state_metadata[STATE_LAKE_ICE_WATER_EQUIVALENT].long_name,
                 MAXSTRING, "%s",
                 "lake_ice_water_equivalent");
        snprintf(state_metadata[STATE_LAKE_ICE_WATER_EQUIVALENT].standard_name,
                 MAXSTRING, "%s",
                 "lake_ice_water_equivalent");
        snprintf(state_metadata[STATE_LAKE_ICE_WATER_EQUIVALENT].units,
                 MAXSTRING, "%s", "m3");
        snprintf(state_metadata[STATE_LAKE_ICE_WATER_EQUIVALENT].description,
                 MAXSTRING, "%s",
                 "liquid water equivalent volume of lake ice");

        // STATE_LAKE_ICE_HEIGHT
        snprintf(state_metadata[STATE_LAKE_ICE_HEIGHT].varname, MAXSTRING, "%s",
                 "STATE_LAKE_ICE_HEIGHT");
        snprintf(state_metadata[STATE_LAKE_ICE_HEIGHT].long_name, MAXSTRING,
                 "%s",
                 "lake_ice_height");
        snprintf(state_metadata[STATE_LAKE_ICE_HEIGHT].standard_name, MAXSTRING,
                 "%s",
                 "lake_ice_height_thickest");
        snprintf(state_metadata[STATE_LAKE_ICE_HEIGHT].units, MAXSTRING, "%s",
                 "m");
        snprintf(state_metadata[STATE_LAKE_ICE_HEIGHT].description, MAXSTRING,
                 "%s",
                 "lake ice height at ghickest point");

        // STATE_LAKE_ICE_TEMP
        snprintf(state_metadata[STATE_LAKE_ICE_TEMP].varname, MAXSTRING, "%s",
                 "STATE_LAKE_ICE_TEMP");
        snprintf(state_metadata[STATE_LAKE_ICE_TEMP].long_name, MAXSTRING, "%s",
                 "lake_ice_temp");
        snprintf(state_metadata[STATE_LAKE_ICE_TEMP].standard_name, MAXSTRING,
                 "%s",
                 "lake_ice_temperature");
        snprintf(state_metadata[STATE_LAKE_ICE_TEMP].units, MAXSTRING, "%s",
                 "C");
        snprintf(state_metadata[STATE_LAKE_ICE_TEMP].description, MAXSTRING,
                 "%s",
                 "lake ice temperature");

        // STATE_LAKE_ICE_SWE
        snprintf(state_metadata[STATE_LAKE_ICE_SWE].varname, MAXSTRING, "%s",
                 "STATE_LAKE_ICE_SWE");
        snprintf(state_metadata[STATE_LAKE_ICE_SWE].long_name, MAXSTRING, "%s",
                 "lake_ice_swe");
        snprintf(state_metadata[STATE_LAKE_ICE_SWE].standard_name, MAXSTRING,
                 "%s",
                 "lake_snow_water_equivalent");
        snprintf(state_metadata[STATE_LAKE_ICE_SWE].units, MAXSTRING, "%s",
                 "m");
        snprintf(state_metadata[STATE_LAKE_ICE_SWE].description, MAXSTRING,
                 "%s",
                 "liquid water equivalent depth of lake snow");

        // STATE_LAKE_ICE_SNOW_SURF_TEMP
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_SURF_TEMP].varname,
                 MAXSTRING, "%s",
                 "STATE_LAKE_ICE_SNOW_SURF_TEMP");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_SURF_TEMP].long_name,
                 MAXSTRING, "%s",
                 "lake_ice_snow_surf_temp");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_SURF_TEMP].standard_name,
                 MAXSTRING, "%s",
                 "lake_snow_surface_temperature");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_SURF_TEMP].units, MAXSTRING,
                 "%s", "C");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_SURF_TEMP].description,
                 MAXSTRING, "%s",
                 "temperature of snow surface layer of lake snow");

        // STATE_LAKE_ICE_SNOW_PACK_TEMP
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_PACK_TEMP].varname,
                 MAXSTRING, "%s",
                 "STATE_LAKE_ICE_SNOW_PACK_TEMP");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_PACK_TEMP].long_name,
                 MAXSTRING, "%s",
                 "lake_ice_snow_pack_temp");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_PACK_TEMP].standard_name,
                 MAXSTRING, "%s",
                 "lake_ice_snow_pack_temperature");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_PACK_TEMP].units, MAXSTRING,
                 "%s", "C");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_PACK_TEMP].description,
                 MAXSTRING, "%s",
                 "temperature of snow pack layer of lake snow");

        // STATE_LAKE_ICE_SNOW_COLD_CONTENT
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_COLD_CONTENT].varname,
                 MAXSTRING, "%s",
                 "STATE_LAKE_ICE_SNOW_COLD_CONTENT");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_COLD_CONTENT].long_name,
                 MAXSTRING, "%s",
                 "lake_ice_snow_cold_content");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_COLD_CONTENT].standard_name,
                 MAXSTRING, "%s",
                 "lake_ice_snow_cold_content");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_COLD_CONTENT].units,
                 MAXSTRING, "%s", "J m-2");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_COLD_CONTENT].description,
                 MAXSTRING, "%s",
                 "snowpack cold content of snow lake");

        // STATE_LAKE_ICE_SNOW_SURF_WATER
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_SURF_WATER].varname,
                 MAXSTRING, "%s",
                 "STATE_LAKE_ICE_SNOW_SURF_WATER");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_SURF_WATER].long_name,
                 MAXSTRING, "%s",
                 "lake_ice_snow_surf_water");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_SURF_WATER].standard_name,
                 MAXSTRING, "%s",
                 "lake_ice_snow_surface_liquid_water");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_SURF_WATER].units,
                 MAXSTRING, "%s", "m");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_SURF_WATER].description,
                 MAXSTRING, "%s",
                 "liquid water content of surface snow layer of lake snow");

        // STATE_LAKE_ICE_SNOW_PACK_WATER
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_PACK_WATER].varname,
                 MAXSTRING, "%s",
                 "STATE_LAKE_ICE_SNOW_PACK_WATER");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_PACK_WATER].long_name,
                 MAXSTRING, "%s",
                 "lake_ice_snow_pack_water");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_PACK_WATER].standard_name,
                 MAXSTRING, "%s",
                 "lake_ice_snow_pack_liquid_water");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_PACK_WATER].units,
                 MAXSTRING, "%s", "m");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_PACK_WATER].description,
                 MAXSTRING, "%s",
                 "liquid water content of pack snow layer of lake snow");

        // STATE_LAKE_ICE_SNOW_ALBEDO
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_ALBEDO].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_ICE_SNOW_ALBEDO");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_ALBEDO].long_name,
                 MAXSTRING, "%s",
                 "lake_ice_snow_albedo");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_ALBEDO].standard_name,
                 MAXSTRING, "%s",
                 "lake_ice_snow_albedo");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_ALBEDO].units, MAXSTRING,
                 "%s", "1");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_ALBEDO].description,
                 MAXSTRING, "%s",
                 "albedo of lake snow");

        // STATE_LAKE_ICE_SNOW_DEPTH
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_DEPTH].varname, MAXSTRING,
                 "%s",
                 "STATE_LAKE_ICE_SNOW_DEPTH");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_DEPTH].long_name, MAXSTRING,
                 "%s",
                 "lake_ice_snow_depth");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_DEPTH].standard_name,
                 MAXSTRING, "%s",
                 "lake_ice_snow_depth");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_DEPTH].units, MAXSTRING,
                 "%s", "m");
        snprintf(state_metadata[STATE_LAKE_ICE_SNOW_DEPTH].description,
                 MAXSTRING, "%s",
                 "depth of snow on lake ice");
    }
}
