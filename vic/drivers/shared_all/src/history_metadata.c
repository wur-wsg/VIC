/******************************************************************************
 * @section DESCRIPTION
 *
 * This routine sets the metadata structure for VIC output variables
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

/******************************************************************************
 * @brief    Set output met data information
 *****************************************************************************/
void
set_output_met_data_info()
{
    size_t                 v;

    extern option_struct   options;
    extern metadata_struct out_metadata[];

    // Build the list of supported output variables

    // Set missing and/or default values
    for (v = 0; v < N_OUTVAR_TYPES; v++) {
        // Set default string values
        snprintf(out_metadata[v].varname, MAXSTRING, "%s", MISSING_S);
        snprintf(out_metadata[v].long_name, MAXSTRING, "%s", MISSING_S);
        snprintf(out_metadata[v].standard_name, MAXSTRING, "%s", MISSING_S);
        snprintf(out_metadata[v].units, MAXSTRING, "%s", MISSING_S);
        snprintf(out_metadata[v].description, MAXSTRING, "%s", MISSING_S);
        // Set default number of elements
        out_metadata[v].nelem = 1;
    }

    // Water Balance Terms - state variables
    /* saturated area fraction */
    snprintf(out_metadata[OUT_ASAT].varname, MAXSTRING, "%s", "OUT_ASAT");
    snprintf(out_metadata[OUT_ASAT].long_name, MAXSTRING, "%s", "asat");
    snprintf(out_metadata[OUT_ASAT].standard_name, MAXSTRING, "%s",
             "saturated_area_fraction");
    snprintf(out_metadata[OUT_ASAT].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_ASAT].description, MAXSTRING, "%s",
             "saturated area fraction");

    /* lake surface area as fraction of grid cell area [fraction] */
    snprintf(out_metadata[OUT_LAKE_AREA_FRAC].varname, MAXSTRING, "%s",
             "OUT_LAKE_AREA_FRAC");
    snprintf(out_metadata[OUT_LAKE_AREA_FRAC].long_name, MAXSTRING, "%s",
             "lake_area_frac");
    snprintf(out_metadata[OUT_LAKE_AREA_FRAC].standard_name, MAXSTRING, "%s",
             "lake_area_fraction");
    snprintf(out_metadata[OUT_LAKE_AREA_FRAC].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_LAKE_AREA_FRAC].description, MAXSTRING, "%s",
             "lake surface area as fraction of grid cell area");

    /* lake depth [m] */
    snprintf(out_metadata[OUT_LAKE_DEPTH].varname, MAXSTRING, "%s",
             "OUT_LAKE_DEPTH");
    snprintf(out_metadata[OUT_LAKE_DEPTH].long_name, MAXSTRING, "%s",
             "lake_depth");
    snprintf(out_metadata[OUT_LAKE_DEPTH].standard_name, MAXSTRING, "%s",
             "lake_depth");
    snprintf(out_metadata[OUT_LAKE_DEPTH].units, MAXSTRING, "%s", "m");
    snprintf(out_metadata[OUT_LAKE_DEPTH].description, MAXSTRING, "%s",
             "lake depth");

    /* moisture stored as lake ice [mm] */
    snprintf(out_metadata[OUT_LAKE_ICE].varname, MAXSTRING, "%s",
             "OUT_LAKE_ICE");
    snprintf(out_metadata[OUT_LAKE_ICE].long_name, MAXSTRING, "%s", "lake_ice");
    snprintf(out_metadata[OUT_LAKE_ICE].standard_name, MAXSTRING, "%s",
             "lake_ice");
    snprintf(out_metadata[OUT_LAKE_ICE].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_LAKE_ICE].description, MAXSTRING, "%s",
             "moisture stored as lake ice");

    /* fractional coverage of lake ice [fraction] */
    snprintf(out_metadata[OUT_LAKE_ICE_FRACT].varname, MAXSTRING, "%s",
             "OUT_LAKE_ICE_FRACT");
    snprintf(out_metadata[OUT_LAKE_ICE_FRACT].long_name, MAXSTRING, "%s",
             "lake_ice_fract");
    snprintf(out_metadata[OUT_LAKE_ICE_FRACT].standard_name, MAXSTRING, "%s",
             "lake_ice_area_fraction");
    snprintf(out_metadata[OUT_LAKE_ICE_FRACT].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_LAKE_ICE_FRACT].description, MAXSTRING, "%s",
             "fractional coverage of lake ice");

    /* thickness of lake ice [cm] */
    snprintf(out_metadata[OUT_LAKE_ICE_HEIGHT].varname, MAXSTRING, "%s",
             "OUT_LAKE_ICE_HEIGHT");
    snprintf(out_metadata[OUT_LAKE_ICE_HEIGHT].long_name, MAXSTRING, "%s",
             "lake_ice_height");
    snprintf(out_metadata[OUT_LAKE_ICE_HEIGHT].standard_name, MAXSTRING, "%s",
             "lake_ice_height");
    snprintf(out_metadata[OUT_LAKE_ICE_HEIGHT].units, MAXSTRING, "%s", "cm");
    snprintf(out_metadata[OUT_LAKE_ICE_HEIGHT].description, MAXSTRING, "%s",
             "thickness of lake ice");

    /* liquid water stored in lake [mm over lake area?] */
    snprintf(out_metadata[OUT_LAKE_MOIST].varname, MAXSTRING, "%s",
             "OUT_LAKE_MOIST");
    snprintf(out_metadata[OUT_LAKE_MOIST].long_name, MAXSTRING, "%s",
             "lake_moist");
    snprintf(out_metadata[OUT_LAKE_MOIST].standard_name, MAXSTRING, "%s",
             "lake_moisture");
    snprintf(out_metadata[OUT_LAKE_MOIST].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_LAKE_MOIST].description, MAXSTRING, "%s",
             "liquid water stored in lake");

    /* lake surface area [m2] */
    snprintf(out_metadata[OUT_LAKE_SURF_AREA].varname, MAXSTRING, "%s",
             "OUT_LAKE_SURF_AREA");
    snprintf(out_metadata[OUT_LAKE_SURF_AREA].long_name, MAXSTRING, "%s",
             "lake_surf_area");
    snprintf(out_metadata[OUT_LAKE_SURF_AREA].standard_name, MAXSTRING, "%s",
             "lake_area");
    snprintf(out_metadata[OUT_LAKE_SURF_AREA].units, MAXSTRING, "%s", "m2");
    snprintf(out_metadata[OUT_LAKE_SURF_AREA].description, MAXSTRING, "%s",
             "lake surface area");

    /* liquid water equivalent of snow on top of lake ice [m over lake ice] */
    snprintf(out_metadata[OUT_LAKE_SWE].varname, MAXSTRING, "%s",
             "OUT_LAKE_SWE");
    snprintf(out_metadata[OUT_LAKE_SWE].long_name, MAXSTRING, "%s", "lake_swe");
    snprintf(out_metadata[OUT_LAKE_SWE].standard_name, MAXSTRING, "%s",
             "lwe_thickness_of_snow_on_lake");
    snprintf(out_metadata[OUT_LAKE_SWE].units, MAXSTRING, "%s", "m");
    snprintf(out_metadata[OUT_LAKE_SWE].description, MAXSTRING, "%s",
             "liquid water equivalent of snow on top of lake ice");

    /* volumetric liquid water equivalent of snow on top of lake ice [m3] */
    snprintf(out_metadata[OUT_LAKE_SWE_V].varname, MAXSTRING, "%s",
             "OUT_LAKE_SWE_V");
    snprintf(out_metadata[OUT_LAKE_SWE_V].long_name, MAXSTRING, "%s",
             "lake_swe_v");
    snprintf(out_metadata[OUT_LAKE_SWE_V].standard_name, MAXSTRING, "%s",
             "lwe_volume_of_snow_on_lake");
    snprintf(out_metadata[OUT_LAKE_SWE_V].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_SWE_V].description, MAXSTRING, "%s",
             "liquid water equivalent of snow on top of lake ice");

    /* lake volume [m3] */
    snprintf(out_metadata[OUT_LAKE_VOLUME].varname, MAXSTRING, "%s",
             "OUT_LAKE_VOLUME");
    snprintf(out_metadata[OUT_LAKE_VOLUME].long_name, MAXSTRING, "%s",
             "lake_volume");
    snprintf(out_metadata[OUT_LAKE_VOLUME].standard_name, MAXSTRING, "%s",
             "lake_volume");
    snprintf(out_metadata[OUT_LAKE_VOLUME].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_VOLUME].description, MAXSTRING, "%s",
             "lake volume");

    /* root zone soil moisture [mm] */
    snprintf(out_metadata[OUT_ROOTMOIST].varname, MAXSTRING, "%s",
             "OUT_ROOTMOIST");
    snprintf(out_metadata[OUT_ROOTMOIST].long_name, MAXSTRING, "%s",
             "rootmoist");
    snprintf(out_metadata[OUT_ROOTMOIST].standard_name, MAXSTRING, "%s",
             "soil_moisture_in_root_zone");
    snprintf(out_metadata[OUT_ROOTMOIST].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_ROOTMOIST].description, MAXSTRING, "%s",
             "root zone soil moisture");

    /* fraction of soil moisture (by mass) that is ice, for each soil layer */
    snprintf(out_metadata[OUT_SMFROZFRAC].varname, MAXSTRING, "%s",
             "OUT_SMFROZFRAC");
    snprintf(out_metadata[OUT_SMFROZFRAC].long_name, MAXSTRING, "%s",
             "smfrozfrac");
    snprintf(out_metadata[OUT_SMFROZFRAC].standard_name, MAXSTRING, "%s",
             "soil_moisture_ice_fraction");
    snprintf(out_metadata[OUT_SMFROZFRAC].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_SMFROZFRAC].description, MAXSTRING, "%s",
             "fraction of soil moisture (by mass) that is ice, for each soil layer");

    /* fraction of soil moisture (by mass) that is liquid, for each soil layer */
    snprintf(out_metadata[OUT_SMLIQFRAC].varname, MAXSTRING, "%s",
             "OUT_SMLIQFRAC");
    snprintf(out_metadata[OUT_SMLIQFRAC].long_name, MAXSTRING, "%s",
             "smliqfrac");
    snprintf(out_metadata[OUT_SMLIQFRAC].standard_name, MAXSTRING, "%s",
             "soil_moisture_liquid_fraction");
    snprintf(out_metadata[OUT_SMLIQFRAC].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_SMLIQFRAC].description, MAXSTRING, "%s",
             "fraction of soil moisture (by mass) that is liquid, for each soil layer");

    /* snow interception storage in canopy [mm] */
    snprintf(out_metadata[OUT_SNOW_CANOPY].varname, MAXSTRING, "%s",
             "OUT_SNOW_CANOPY");
    snprintf(out_metadata[OUT_SNOW_CANOPY].long_name, MAXSTRING, "%s",
             "snow_canopy");
    snprintf(out_metadata[OUT_SNOW_CANOPY].standard_name, MAXSTRING, "%s",
             "lwe_thickness_of_canopy_intercepted_snow");
    snprintf(out_metadata[OUT_SNOW_CANOPY].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_SNOW_CANOPY].description, MAXSTRING, "%s",
             "snow interception storage in canopy");

    /* fractional area of snow cover [fraction] */
    snprintf(out_metadata[OUT_SNOW_COVER].varname, MAXSTRING, "%s",
             "OUT_SNOW_COVER");
    snprintf(out_metadata[OUT_SNOW_COVER].long_name, MAXSTRING, "%s",
             "snow_cover");
    snprintf(out_metadata[OUT_SNOW_COVER].standard_name, MAXSTRING, "%s",
             "snow_cover_area_fraction");
    snprintf(out_metadata[OUT_SNOW_COVER].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_SNOW_COVER].description, MAXSTRING, "%s",
             "fractional area of snow cover");

    /* depth of snow pack [cm] */
    snprintf(out_metadata[OUT_SNOW_DEPTH].varname, MAXSTRING, "%s",
             "OUT_SNOW_DEPTH");
    snprintf(out_metadata[OUT_SNOW_DEPTH].long_name, MAXSTRING, "%s",
             "snow_depth");
    snprintf(out_metadata[OUT_SNOW_DEPTH].standard_name, MAXSTRING, "%s",
             "thickness_of_snow");
    snprintf(out_metadata[OUT_SNOW_DEPTH].units, MAXSTRING, "%s", "cm");
    snprintf(out_metadata[OUT_SNOW_DEPTH].description, MAXSTRING, "%s",
             "depth of snow pack");

    /* soil ice content [mm] for each soil layer */
    snprintf(out_metadata[OUT_SOIL_ICE].varname, MAXSTRING, "%s",
             "OUT_SOIL_ICE");
    snprintf(out_metadata[OUT_SOIL_ICE].long_name, MAXSTRING, "%s", "soil_ice");
    snprintf(out_metadata[OUT_SOIL_ICE].standard_name, MAXSTRING, "%s",
             "soil_moisture_ice_depth");
    snprintf(out_metadata[OUT_SOIL_ICE].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_SOIL_ICE].description, MAXSTRING, "%s",
             "soil ice content for each soil layer");

    /* soil liquid moisture content [mm] for each soil layer */
    snprintf(out_metadata[OUT_SOIL_LIQ].varname, MAXSTRING, "%s",
             "OUT_SOIL_LIQ");
    snprintf(out_metadata[OUT_SOIL_LIQ].long_name, MAXSTRING, "%s", "soil_liq");
    snprintf(out_metadata[OUT_SOIL_LIQ].standard_name, MAXSTRING, "%s",
             "soil_moisture_liquid_depth");
    snprintf(out_metadata[OUT_SOIL_LIQ].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_SOIL_LIQ].description, MAXSTRING, "%s",
             "soil liquid moisture content for each soil layer");

    /* soil ice content [1] for each soil layer */
    snprintf(out_metadata[OUT_SOIL_ICE_FRAC].varname, MAXSTRING, "%s",
             "OUT_SOIL_ICE_FRAC");
    snprintf(out_metadata[OUT_SOIL_ICE_FRAC].long_name, MAXSTRING, "%s",
             "soil_ice_frac");
    snprintf(out_metadata[OUT_SOIL_ICE_FRAC].standard_name, MAXSTRING, "%s",
             "soil_moisture_ice_depth_fraction");
    snprintf(out_metadata[OUT_SOIL_ICE_FRAC].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_SOIL_ICE_FRAC].description, MAXSTRING, "%s",
             "soil ice content fraction for each soil layer");

    /* soil liquid moisture content [1] for each soil layer */
    snprintf(out_metadata[OUT_SOIL_LIQ_FRAC].varname, MAXSTRING, "%s",
             "OUT_SOIL_LIQ_FRAC");
    snprintf(out_metadata[OUT_SOIL_LIQ_FRAC].long_name, MAXSTRING, "%s",
             "soil_liq_frac");
    snprintf(out_metadata[OUT_SOIL_LIQ_FRAC].standard_name, MAXSTRING, "%s",
             "soil_moisture_liquid_depth_fraction");
    snprintf(out_metadata[OUT_SOIL_LIQ_FRAC].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_SOIL_LIQ_FRAC].description, MAXSTRING, "%s",
             "soil liquid moisture content fraction for each soil layer");

    /* soil total moisture content [mm] for each soil layer */
    snprintf(out_metadata[OUT_SOIL_MOIST].varname, MAXSTRING, "%s",
             "OUT_SOIL_MOIST");
    snprintf(out_metadata[OUT_SOIL_MOIST].long_name, MAXSTRING, "%s",
             "soil_moist");
    snprintf(out_metadata[OUT_SOIL_MOIST].standard_name, MAXSTRING, "%s",
             "soil_moisture");
    snprintf(out_metadata[OUT_SOIL_MOIST].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_SOIL_MOIST].description, MAXSTRING, "%s",
             "soil total moisture content");

    /* soil effective saturation [-] for each soil layer */
    snprintf(out_metadata[OUT_SOIL_EFF_SAT].varname, MAXSTRING, "%s",
             "OUT_SOIL_EFF_SAT");
    snprintf(out_metadata[OUT_SOIL_EFF_SAT].long_name, MAXSTRING, "%s",
             "soil_effective_saturation");
    snprintf(out_metadata[OUT_SOIL_EFF_SAT].standard_name, MAXSTRING, "%s",
             "soil_effective_saturation");
    snprintf(out_metadata[OUT_SOIL_EFF_SAT].units, MAXSTRING, "%s", "-");
    snprintf(out_metadata[OUT_SOIL_EFF_SAT].description, MAXSTRING, "%s",
             "soil effective saturation (between residual- and maximum moisture content)");

    /* vertical average of (soil moisture - wilting point)/(maximum soil moisture - wilting point) [mm/mm] */
    snprintf(out_metadata[OUT_SOIL_WET].varname, MAXSTRING, "%s",
             "OUT_SOIL_WET");
    snprintf(out_metadata[OUT_SOIL_WET].long_name, MAXSTRING, "%s", "soil_wet");
    snprintf(out_metadata[OUT_SOIL_WET].standard_name, MAXSTRING, "%s",
             "soil_moisture_wetness_fraction");
    snprintf(out_metadata[OUT_SOIL_WET].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_SOIL_WET].description, MAXSTRING, "%s",
             "vertical average of (soil moisture - wilting point)/(maximum "
             "soil moisture - wilting point)");

    /* storage of liquid water on surface (ponding) [mm] */
    snprintf(out_metadata[OUT_SURFSTOR].varname, MAXSTRING, "%s",
             "OUT_SURFSTOR");
    snprintf(out_metadata[OUT_SURFSTOR].long_name, MAXSTRING, "%s", "surfstor");
    snprintf(out_metadata[OUT_SURFSTOR].standard_name, MAXSTRING, "%s",
             "surface_liquid_water_storage");
    snprintf(out_metadata[OUT_SURFSTOR].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_SURFSTOR].description, MAXSTRING, "%s",
             "storage of liquid water on surface (ponding)");

    /* fraction of soil surface that is frozen [fraction] */
    snprintf(out_metadata[OUT_SURF_FROST_FRAC].varname, MAXSTRING, "%s",
             "OUT_SURF_FROST_FRAC");
    snprintf(out_metadata[OUT_SURF_FROST_FRAC].long_name, MAXSTRING, "%s",
             "surf_frost_frac");
    snprintf(out_metadata[OUT_SURF_FROST_FRAC].standard_name, MAXSTRING, "%s",
             "frozen_soil_surface_fraction");
    snprintf(out_metadata[OUT_SURF_FROST_FRAC].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_SURF_FROST_FRAC].description, MAXSTRING, "%s",
             "fraction of soil surface that is frozen");

    /* snow water equivalent in snow pack [mm] */
    snprintf(out_metadata[OUT_SWE].varname, MAXSTRING, "%s", "OUT_SWE");
    snprintf(out_metadata[OUT_SWE].long_name, MAXSTRING, "%s", "swe");
    snprintf(out_metadata[OUT_SWE].standard_name, MAXSTRING, "%s",
             "lwe_thickness_of_snow");
    snprintf(out_metadata[OUT_SWE].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_SWE].description, MAXSTRING, "%s",
             "snow water equivalent in snow pack");

    /* total moisture interception storage in canopy [mm] */
    snprintf(out_metadata[OUT_WDEW].varname, MAXSTRING, "%s", "OUT_WDEW");
    snprintf(out_metadata[OUT_WDEW].long_name, MAXSTRING, "%s", "wdew");
    snprintf(out_metadata[OUT_WDEW].standard_name, MAXSTRING, "%s",
             "soil_moisture_storage_depth_in_canopy");
    snprintf(out_metadata[OUT_WDEW].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_WDEW].description, MAXSTRING, "%s",
             "total moisture interception storage in canopy");

    /* water table position [cm] (zwt within lowest unsaturated layer) */
    snprintf(out_metadata[OUT_ZWT].varname, MAXSTRING, "%s", "OUT_ZWT");
    snprintf(out_metadata[OUT_ZWT].long_name, MAXSTRING, "%s", "zwt");
    snprintf(out_metadata[OUT_ZWT].standard_name, MAXSTRING, "%s",
             "water_table_position_lowest_layer");
    snprintf(out_metadata[OUT_ZWT].units, MAXSTRING, "%s", "cm");
    snprintf(out_metadata[OUT_ZWT].description, MAXSTRING, "%s",
             "water table position (zwt within lowest unsaturated layer)");

    /* lumped water table position [cm] (zwt of total moisture across all layers, lumped together) */
    snprintf(out_metadata[OUT_ZWT_LUMPED].varname, MAXSTRING, "%s",
             "OUT_ZWT_LUMPED");
    snprintf(out_metadata[OUT_ZWT_LUMPED].long_name, MAXSTRING, "%s",
             "zwt_lumped");
    snprintf(out_metadata[OUT_ZWT_LUMPED].standard_name, MAXSTRING, "%s",
             "lumped_water_table_position");
    snprintf(out_metadata[OUT_ZWT_LUMPED].units, MAXSTRING, "%s", "cm");
    snprintf(out_metadata[OUT_ZWT_LUMPED].description, MAXSTRING, "%s",
             "lumped water table position (zwt of total moisture across all layers, lumped together)");

    // Water Balance Terms - fluxes
    /* recharge to the bottom layer [mm] */
    snprintf(out_metadata[OUT_RECHARGE].varname, MAXSTRING, "%s",
             "OUT_RECHARGE");
    snprintf(out_metadata[OUT_RECHARGE].long_name, MAXSTRING, "%s", "recharge");
    snprintf(out_metadata[OUT_RECHARGE].standard_name, MAXSTRING, "%s",
             "recharge_amount");
    snprintf(out_metadata[OUT_RECHARGE].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_RECHARGE].description, MAXSTRING, "%s",
             "recharge to the bottom layer");

    /* baseflow out of the bottom layer [mm] */
    snprintf(out_metadata[OUT_BASEFLOW].varname, MAXSTRING, "%s",
             "OUT_BASEFLOW");
    snprintf(out_metadata[OUT_BASEFLOW].long_name, MAXSTRING, "%s", "baseflow");
    snprintf(out_metadata[OUT_BASEFLOW].standard_name, MAXSTRING, "%s",
             "baseflow_amount");
    snprintf(out_metadata[OUT_BASEFLOW].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_BASEFLOW].description, MAXSTRING, "%s",
             "baseflow out of the bottom layer");

    /* change in canopy interception storage [mm] */
    snprintf(out_metadata[OUT_DELINTERCEPT].varname, MAXSTRING, "%s",
             "OUT_DELINTERCEPT");
    snprintf(out_metadata[OUT_DELINTERCEPT].long_name, MAXSTRING, "%s",
             "delintercept");
    snprintf(out_metadata[OUT_DELINTERCEPT].standard_name, MAXSTRING, "%s",
             "change_in_canopy_interception_amount");
    snprintf(out_metadata[OUT_DELINTERCEPT].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_DELINTERCEPT].description, MAXSTRING, "%s",
             "change in canopy interception storage");

    /* change in soil water content [mm] */
    snprintf(out_metadata[OUT_DELSOILMOIST].varname, MAXSTRING, "%s",
             "OUT_DELSOILMOIST");
    snprintf(out_metadata[OUT_DELSOILMOIST].long_name, MAXSTRING, "%s",
             "delsoilmoist");
    snprintf(out_metadata[OUT_DELSOILMOIST].standard_name, MAXSTRING, "%s",
             "change_in_soil_water_amount");
    snprintf(out_metadata[OUT_DELSOILMOIST].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_DELSOILMOIST].description, MAXSTRING, "%s",
             "change in soil water content");

    /* change in snow water equivalent [mm] */
    snprintf(out_metadata[OUT_DELSWE].varname, MAXSTRING, "%s", "OUT_DELSWE");
    snprintf(out_metadata[OUT_DELSWE].long_name, MAXSTRING, "%s", "delswe");
    snprintf(out_metadata[OUT_DELSWE].standard_name, MAXSTRING, "%s",
             "change_in_snow_lwe_thickness");
    snprintf(out_metadata[OUT_DELSWE].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_DELSWE].description, MAXSTRING, "%s",
             "change in snow water equivalent");

    /* change in surface liquid water storage  [mm] */
    snprintf(out_metadata[OUT_DELSURFSTOR].varname, MAXSTRING, "%s",
             "OUT_DELSURFSTOR");
    snprintf(out_metadata[OUT_DELSURFSTOR].long_name, MAXSTRING, "%s",
             "delsurfstor");
    snprintf(out_metadata[OUT_DELSURFSTOR].standard_name, MAXSTRING, "%s",
             "change_in_surface_liquid_water_storage");
    snprintf(out_metadata[OUT_DELSURFSTOR].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_DELSURFSTOR].description, MAXSTRING, "%s",
             "change in surface liquid water storage");

    /* total net evaporation [mm] */
    snprintf(out_metadata[OUT_EVAP].varname, MAXSTRING, "%s", "OUT_EVAP");
    snprintf(out_metadata[OUT_EVAP].long_name, MAXSTRING, "%s", "evap");
    snprintf(out_metadata[OUT_EVAP].standard_name, MAXSTRING, "%s",
             "water_evaporation_flux_net");
    snprintf(out_metadata[OUT_EVAP].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_EVAP].description, MAXSTRING, "%s",
             "total net evaporation");

    /* net evaporation from bare soil [mm] */
    snprintf(out_metadata[OUT_EVAP_BARE].varname, MAXSTRING, "%s",
             "OUT_EVAP_BARE");
    snprintf(out_metadata[OUT_EVAP_BARE].long_name, MAXSTRING, "%s",
             "evap_bare");
    snprintf(out_metadata[OUT_EVAP_BARE].standard_name, MAXSTRING, "%s",
             "water_evaporation_from_soil");
    snprintf(out_metadata[OUT_EVAP_BARE].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_EVAP_BARE].description, MAXSTRING, "%s",
             "net evaporation from bare soil");

    /* net evaporation from canopy interception [mm] */
    snprintf(out_metadata[OUT_EVAP_CANOP].varname, MAXSTRING, "%s",
             "OUT_EVAP_CANOP");
    snprintf(out_metadata[OUT_EVAP_CANOP].long_name, MAXSTRING, "%s",
             "evap_canop");
    snprintf(out_metadata[OUT_EVAP_CANOP].standard_name, MAXSTRING, "%s",
             "water_evaporation_from_canopy");
    snprintf(out_metadata[OUT_EVAP_CANOP].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_EVAP_CANOP].description, MAXSTRING, "%s",
             "net evaporation from canopy interception");

    /* moisture that reaches top of soil column [mm] */
    snprintf(out_metadata[OUT_INFLOW].varname, MAXSTRING, "%s", "OUT_INFLOW");
    snprintf(out_metadata[OUT_INFLOW].long_name, MAXSTRING, "%s", "inflow");
    snprintf(out_metadata[OUT_INFLOW].standard_name, MAXSTRING, "%s",
             "soil_column_inflow");
    snprintf(out_metadata[OUT_INFLOW].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_INFLOW].description, MAXSTRING, "%s",
             "moisture that reaches top of soil column");

    /* incoming baseflow from lake catchment [mm] */
    snprintf(out_metadata[OUT_LAKE_BF_IN].varname, MAXSTRING, "%s",
             "OUT_LAKE_BF_IN");
    snprintf(out_metadata[OUT_LAKE_BF_IN].long_name, MAXSTRING, "%s",
             "lake_bf_in");
    snprintf(out_metadata[OUT_LAKE_BF_IN].standard_name, MAXSTRING, "%s",
             "infiltration_amount");
    snprintf(out_metadata[OUT_LAKE_BF_IN].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_LAKE_BF_IN].description, MAXSTRING, "%s",
             "incoming baseflow from lake catchment");

    /* incoming volumetric baseflow from lake catchment [m3] */
    snprintf(out_metadata[OUT_LAKE_BF_IN_V].varname, MAXSTRING, "%s",
             "OUT_LAKE_BF_IN_V");
    snprintf(out_metadata[OUT_LAKE_BF_IN_V].long_name, MAXSTRING, "%s",
             "lake_bf_in_v");
    snprintf(out_metadata[OUT_LAKE_BF_IN_V].standard_name, MAXSTRING, "%s",
             "baseflow_volume_from_lake_catchment");
    snprintf(out_metadata[OUT_LAKE_BF_IN_V].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_BF_IN_V].description, MAXSTRING, "%s",
             "incoming volumetric baseflow from lake catchment");

    /* outgoing baseflow lake [mm] */
    snprintf(out_metadata[OUT_LAKE_BF_OUT].varname, MAXSTRING, "%s",
             "OUT_LAKE_BF_OUT");
    snprintf(out_metadata[OUT_LAKE_BF_OUT].long_name, MAXSTRING, "%s",
             "lake_bf_out");
    snprintf(out_metadata[OUT_LAKE_BF_OUT].standard_name, MAXSTRING, "%s",
             "baseflow_outgoing_lake");
    snprintf(out_metadata[OUT_LAKE_BF_OUT].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_LAKE_BF_OUT].description, MAXSTRING, "%s",
             "outgoing baseflow lake");

    /* outgoing volumetric baseflow from lake [m3] */
    snprintf(out_metadata[OUT_LAKE_BF_OUT_V].varname, MAXSTRING, "%s",
             "OUT_LAKE_BF_OUT_V");
    snprintf(out_metadata[OUT_LAKE_BF_OUT_V].long_name, MAXSTRING, "%s",
             "lake_bf_out_v");
    snprintf(out_metadata[OUT_LAKE_BF_OUT_V].standard_name, MAXSTRING, "%s",
             "baseflow_outgoing_volume_lake");
    snprintf(out_metadata[OUT_LAKE_BF_OUT_V].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_BF_OUT_V].description, MAXSTRING, "%s",
             "outgoing volumetric baseflow from lake");

    /* channel inflow into lake [mm] */
    snprintf(out_metadata[OUT_LAKE_CHAN_IN].varname, MAXSTRING, "%s",
             "OUT_LAKE_CHAN_IN");
    snprintf(out_metadata[OUT_LAKE_CHAN_IN].long_name, MAXSTRING, "%s",
             "lake_chan_in");
    snprintf(out_metadata[OUT_LAKE_CHAN_IN].standard_name, MAXSTRING, "%s",
             "channel_inflow_into_lake");
    snprintf(out_metadata[OUT_LAKE_CHAN_IN].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_LAKE_CHAN_IN].description, MAXSTRING, "%s",
             "channel inflow into lake");

    /* volumetric channel inflow into lake [m3] */
    snprintf(out_metadata[OUT_LAKE_CHAN_IN_V].varname, MAXSTRING, "%s",
             "OUT_LAKE_CHAN_IN_V");
    snprintf(out_metadata[OUT_LAKE_CHAN_IN_V].long_name, MAXSTRING, "%s",
             "lake_chan_in_v");
    snprintf(out_metadata[OUT_LAKE_CHAN_IN_V].standard_name, MAXSTRING, "%s",
             "channel_inflow_volume_into_lake");
    snprintf(out_metadata[OUT_LAKE_CHAN_IN_V].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_CHAN_IN_V].description, MAXSTRING, "%s",
             "volumetric channel inflow into lake");

    /* channel outflow from lake [mm] */
    snprintf(out_metadata[OUT_LAKE_CHAN_OUT].varname, MAXSTRING, "%s",
             "OUT_LAKE_CHAN_OUT");
    snprintf(out_metadata[OUT_LAKE_CHAN_OUT].long_name, MAXSTRING, "%s",
             "lake_chan_out");
    snprintf(out_metadata[OUT_LAKE_CHAN_OUT].standard_name, MAXSTRING, "%s",
             "channel_outflow_from_lake");
    snprintf(out_metadata[OUT_LAKE_CHAN_OUT].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_LAKE_CHAN_OUT].description, MAXSTRING, "%s",
             "channel outflow from lake");

    /* volumetric channel outflow from lake [m3] */
    snprintf(out_metadata[OUT_LAKE_CHAN_OUT_V].varname, MAXSTRING, "%s",
             "OUT_LAKE_CHAN_OUT_V");
    snprintf(out_metadata[OUT_LAKE_CHAN_OUT_V].long_name, MAXSTRING, "%s",
             "lake_chan_out_v");
    snprintf(out_metadata[OUT_LAKE_CHAN_OUT_V].standard_name, MAXSTRING, "%s",
             "channel_outflow_volume_from_lake");
    snprintf(out_metadata[OUT_LAKE_CHAN_OUT_V].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_CHAN_OUT_V].description, MAXSTRING, "%s",
             "volumetric channel outflow from lake");

    /* change in lake moisture storage (liquid plus ice cover) [mm] */
    snprintf(out_metadata[OUT_LAKE_DSTOR].varname, MAXSTRING, "%s",
             "OUT_LAKE_DSTOR");
    snprintf(out_metadata[OUT_LAKE_DSTOR].long_name, MAXSTRING, "%s",
             "lake_dstor");
    snprintf(out_metadata[OUT_LAKE_DSTOR].standard_name, MAXSTRING, "%s",
             "change_in_lake_moisture_amount");
    snprintf(out_metadata[OUT_LAKE_DSTOR].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_LAKE_DSTOR].description, MAXSTRING, "%s",
             "change in lake moisture storage (liquid plus ice cover)");

    /* volumetric change in lake moisture storage (liquid plus ice cover) [m3] */
    snprintf(out_metadata[OUT_LAKE_DSTOR_V].varname, MAXSTRING, "%s",
             "OUT_LAKE_DSTOR_V");
    snprintf(out_metadata[OUT_LAKE_DSTOR_V].long_name, MAXSTRING, "%s",
             "lake_dstor_v");
    snprintf(out_metadata[OUT_LAKE_DSTOR_V].standard_name, MAXSTRING, "%s",
             "change_in_lake_moisture_volume");
    snprintf(out_metadata[OUT_LAKE_DSTOR_V].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_DSTOR_V].description, MAXSTRING, "%s",
             "volumetric change in lake moisture storage (liquid plus ice cover)");

    /* change in snowpack on top of lake ice [mm] */
    snprintf(out_metadata[OUT_LAKE_DSWE].varname, MAXSTRING, "%s",
             "OUT_LAKE_DSWE");
    snprintf(out_metadata[OUT_LAKE_DSWE].long_name, MAXSTRING, "%s",
             "lake_dswe");
    snprintf(out_metadata[OUT_LAKE_DSWE].standard_name, MAXSTRING, "%s",
             "change_in_snow_lwe_thickness_on_lake_ice");
    snprintf(out_metadata[OUT_LAKE_DSWE].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_LAKE_DSWE].description, MAXSTRING, "%s",
             "change in snowpack on top of lake ice");

    /* volumetric change in snowpack on top of lake ice [m3] */
    snprintf(out_metadata[OUT_LAKE_DSWE_V].varname, MAXSTRING, "%s",
             "OUT_LAKE_DSWE_V");
    snprintf(out_metadata[OUT_LAKE_DSWE_V].long_name, MAXSTRING, "%s",
             "lake_dswe_v");
    snprintf(out_metadata[OUT_LAKE_DSWE_V].standard_name, MAXSTRING, "%s",
             "change_in_snow_lwe_volume_on_lake_ice");
    snprintf(out_metadata[OUT_LAKE_DSWE_V].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_DSWE_V].description, MAXSTRING, "%s",
             "volumetric change in snowpack on top of lake ice");

    /* net evaporation from lake surface [mm] */
    snprintf(out_metadata[OUT_LAKE_EVAP].varname, MAXSTRING, "%s",
             "OUT_LAKE_EVAP");
    snprintf(out_metadata[OUT_LAKE_EVAP].long_name, MAXSTRING, "%s",
             "lake_evap");
    snprintf(out_metadata[OUT_LAKE_EVAP].standard_name, MAXSTRING, "%s",
             "water_evaporation_from_lake");
    snprintf(out_metadata[OUT_LAKE_EVAP].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_LAKE_EVAP].description, MAXSTRING, "%s",
             "net evaporation from lake surface");

    /* net volumetric evaporation from lake surface [m3] */
    snprintf(out_metadata[OUT_LAKE_EVAP_V].varname, MAXSTRING, "%s",
             "OUT_LAKE_EVAP_V");
    snprintf(out_metadata[OUT_LAKE_EVAP_V].long_name, MAXSTRING, "%s",
             "lake_evap_v");
    snprintf(out_metadata[OUT_LAKE_EVAP_V].standard_name, MAXSTRING, "%s",
             "water_evaporation_volume_from_lake");
    snprintf(out_metadata[OUT_LAKE_EVAP_V].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_EVAP_V].description, MAXSTRING, "%s",
             "net volumetric evaporation from lake surface");

    /* volumetric precipitation over lake surface [m3] */
    snprintf(out_metadata[OUT_LAKE_PREC_V].varname, MAXSTRING, "%s",
             "OUT_LAKE_PREC_V");
    snprintf(out_metadata[OUT_LAKE_PREC_V].long_name, MAXSTRING, "%s",
             "lake_prec_v");
    snprintf(out_metadata[OUT_LAKE_PREC_V].standard_name, MAXSTRING, "%s",
             "precipitation_over_lake_volume");
    snprintf(out_metadata[OUT_LAKE_PREC_V].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_PREC_V].description, MAXSTRING, "%s",
             "volumetric precipitation over lake surface");

    /* recharge from lake to surrounding wetland [mm] */
    snprintf(out_metadata[OUT_LAKE_RCHRG].varname, MAXSTRING, "%s",
             "OUT_LAKE_RCHRG");
    snprintf(out_metadata[OUT_LAKE_RCHRG].long_name, MAXSTRING, "%s",
             "lake_rchrg");
    snprintf(out_metadata[OUT_LAKE_RCHRG].standard_name, MAXSTRING, "%s",
             "recharge_from_lake_to_wetland");
    snprintf(out_metadata[OUT_LAKE_RCHRG].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_LAKE_RCHRG].description, MAXSTRING, "%s",
             "recharge from lake to surrounding wetland");

    /* volumetric recharge from lake to surrounding wetland [m3] */
    snprintf(out_metadata[OUT_LAKE_RCHRG_V].varname, MAXSTRING, "%s",
             "OUT_LAKE_RCHRG_V");
    snprintf(out_metadata[OUT_LAKE_RCHRG_V].long_name, MAXSTRING, "%s",
             "lake_rchrg_v");
    snprintf(out_metadata[OUT_LAKE_RCHRG_V].standard_name, MAXSTRING, "%s", "");
    snprintf(out_metadata[OUT_LAKE_RCHRG_V].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_RCHRG_V].description, MAXSTRING, "%s",
             "volumetric recharge from lake to surrounding wetland");

    /* incoming runoff from lake catchment [mm] */
    snprintf(out_metadata[OUT_LAKE_RO_IN].varname, MAXSTRING, "%s",
             "OUT_LAKE_RO_IN");
    snprintf(out_metadata[OUT_LAKE_RO_IN].long_name, MAXSTRING, "%s",
             "lake_ro_in");
    snprintf(out_metadata[OUT_LAKE_RO_IN].standard_name, MAXSTRING, "%s",
             "recharge_volume_from_lake_to_wetland");
    snprintf(out_metadata[OUT_LAKE_RO_IN].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_LAKE_RO_IN].description, MAXSTRING, "%s",
             "incoming runoff from lake catchment");

    /* incoming volumetric runoff from lake catchment [m3] */
    snprintf(out_metadata[OUT_LAKE_RO_IN_V].varname, MAXSTRING, "%s",
             "OUT_LAKE_RO_IN_V");
    snprintf(out_metadata[OUT_LAKE_RO_IN_V].long_name, MAXSTRING, "%s",
             "lake_ro_in_v");
    snprintf(out_metadata[OUT_LAKE_RO_IN_V].standard_name, MAXSTRING, "%s",
             "runoff_volume_from_lake_catchment");
    snprintf(out_metadata[OUT_LAKE_RO_IN_V].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_RO_IN_V].description, MAXSTRING, "%s",
             "incoming volumetric runoff from lake catchment");

    /* sublimation from lake snow pack [mm] */
    snprintf(out_metadata[OUT_LAKE_VAPFLX].varname, MAXSTRING, "%s",
             "OUT_LAKE_VAPFLX");
    snprintf(out_metadata[OUT_LAKE_VAPFLX].long_name, MAXSTRING, "%s",
             "lake_vapflx");
    snprintf(out_metadata[OUT_LAKE_VAPFLX].standard_name, MAXSTRING, "%s",
             "water_sublimation_flux_from_lake_snow");
    snprintf(out_metadata[OUT_LAKE_VAPFLX].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_LAKE_VAPFLX].description, MAXSTRING, "%s",
             "sublimation from lake snow pack");

    /* volumetric sublimation from lake snow pack [m3] */
    snprintf(out_metadata[OUT_LAKE_VAPFLX_V].varname, MAXSTRING, "%s",
             "OUT_LAKE_VAPFLX_V");
    snprintf(out_metadata[OUT_LAKE_VAPFLX_V].long_name, MAXSTRING, "%s",
             "lake_vapflx_v");
    snprintf(out_metadata[OUT_LAKE_VAPFLX_V].standard_name, MAXSTRING, "%s",
             "water_sublimation_flux_volume_from_lake_snow");
    snprintf(out_metadata[OUT_LAKE_VAPFLX_V].units, MAXSTRING, "%s", "m3");
    snprintf(out_metadata[OUT_LAKE_VAPFLX_V].description, MAXSTRING, "%s",
             "volumetric sublimation from lake snow pack");

    /* Potential evapotranspiration (= area-weighted sum of potential
       transpiration and potential soil evaporation). [mm] */
    snprintf(out_metadata[OUT_PET].varname, MAXSTRING, "%s", "OUT_PET");
    snprintf(out_metadata[OUT_PET].long_name, MAXSTRING, "%s", "pet");
    snprintf(out_metadata[OUT_PET].standard_name, MAXSTRING, "%s",
             "water_potential_evaporation_amount");
    snprintf(out_metadata[OUT_PET].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_PET].description, MAXSTRING, "%s",
             "Potential evapotranspiration (= area-weighted sum of potential "
             "transpiration and potential soil evaporation)");

    /* incoming precipitation [mm] */
    snprintf(out_metadata[OUT_PREC].varname, MAXSTRING, "%s", "OUT_PREC");
    snprintf(out_metadata[OUT_PREC].long_name, MAXSTRING, "%s", "prec");
    snprintf(out_metadata[OUT_PREC].standard_name, MAXSTRING, "%s",
             "precipitation_amount");
    snprintf(out_metadata[OUT_PREC].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_PREC].description, MAXSTRING, "%s",
             "incoming precipitation");

    /* rainfall [mm] */
    snprintf(out_metadata[OUT_RAINF].varname, MAXSTRING, "%s", "OUT_RAINF");
    snprintf(out_metadata[OUT_RAINF].long_name, MAXSTRING, "%s", "rainf");
    snprintf(out_metadata[OUT_RAINF].standard_name, MAXSTRING, "%s",
             "rainfall_amount");
    snprintf(out_metadata[OUT_RAINF].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_RAINF].description, MAXSTRING, "%s",
             "liquid rainfall amount");

    /* refreezing of water in the snow [mm] */
    snprintf(out_metadata[OUT_REFREEZE].varname, MAXSTRING, "%s",
             "OUT_REFREEZE");
    snprintf(out_metadata[OUT_REFREEZE].long_name, MAXSTRING, "%s", "refreeze");
    snprintf(out_metadata[OUT_REFREEZE].standard_name, MAXSTRING, "%s",
             "lwe_thickness_of_refreezing_water_in_snow");
    snprintf(out_metadata[OUT_REFREEZE].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_REFREEZE].description, MAXSTRING, "%s",
             "refreezing of water in the snow");

    /* surface runoff [mm] */
    snprintf(out_metadata[OUT_RUNOFF].varname, MAXSTRING, "%s", "OUT_RUNOFF");
    snprintf(out_metadata[OUT_RUNOFF].long_name, MAXSTRING, "%s", "runoff");
    snprintf(out_metadata[OUT_RUNOFF].standard_name, MAXSTRING, "%s",
             "runoff_amount");
    snprintf(out_metadata[OUT_RUNOFF].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_RUNOFF].description, MAXSTRING, "%s",
             "surface runoff");

    /* snow melt [mm] */
    snprintf(out_metadata[OUT_SNOW_MELT].varname, MAXSTRING, "%s",
             "OUT_SNOW_MELT");
    snprintf(out_metadata[OUT_SNOW_MELT].long_name, MAXSTRING, "%s",
             "snow_melt");
    snprintf(out_metadata[OUT_SNOW_MELT].standard_name, MAXSTRING, "%s",
             "snow_melt_amount");
    snprintf(out_metadata[OUT_SNOW_MELT].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_SNOW_MELT].description, MAXSTRING, "%s",
             "snow melt");

    /* snowfall [mm] */
    snprintf(out_metadata[OUT_SNOWF].varname, MAXSTRING, "%s", "OUT_SNOWF");
    snprintf(out_metadata[OUT_SNOWF].long_name, MAXSTRING, "%s", "snowf");
    snprintf(out_metadata[OUT_SNOWF].standard_name, MAXSTRING, "%s",
             "snowfall_lwe_amount");
    snprintf(out_metadata[OUT_SNOWF].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_SNOWF].description, MAXSTRING, "%s", "snowfall");

    /* net sublimation of blowing snow [mm] */
    snprintf(out_metadata[OUT_SUB_BLOWING].varname, MAXSTRING, "%s",
             "OUT_SUB_BLOWING");
    snprintf(out_metadata[OUT_SUB_BLOWING].long_name, MAXSTRING, "%s",
             "sub_blowing");
    snprintf(out_metadata[OUT_SUB_BLOWING].standard_name, MAXSTRING, "%s",
             "submlimation_amount_from_blowing_snow");
    snprintf(out_metadata[OUT_SUB_BLOWING].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_SUB_BLOWING].description, MAXSTRING, "%s",
             "net sublimation of blowing snow");

    /* net sublimation from snow stored in canopy [mm] */
    snprintf(out_metadata[OUT_SUB_CANOP].varname, MAXSTRING, "%s",
             "OUT_SUB_CANOP");
    snprintf(out_metadata[OUT_SUB_CANOP].long_name, MAXSTRING, "%s",
             "sub_canop");
    snprintf(out_metadata[OUT_SUB_CANOP].standard_name, MAXSTRING, "%s",
             "sublimation_amount_from_canopy_snow");
    snprintf(out_metadata[OUT_SUB_CANOP].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_SUB_CANOP].description, MAXSTRING, "%s",
             "net sublimation from snow stored in canopy");

    /* net sublimation from snow pack (surface and blowing) [mm] */
    snprintf(out_metadata[OUT_SUB_SNOW].varname, MAXSTRING, "%s",
             "OUT_SUB_SNOW");
    snprintf(out_metadata[OUT_SUB_SNOW].long_name, MAXSTRING, "%s", "sub_snow");
    snprintf(out_metadata[OUT_SUB_SNOW].standard_name, MAXSTRING, "%s",
             "sublimation_amount_from_snow_pack");
    snprintf(out_metadata[OUT_SUB_SNOW].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_SUB_SNOW].description, MAXSTRING, "%s",
             "net sublimation from snow pack (surface and blowing)");

    /* net sublimation from snow pack surface [mm] */
    snprintf(out_metadata[OUT_SUB_SURFACE].varname, MAXSTRING, "%s",
             "OUT_SUB_SURFACE");
    snprintf(out_metadata[OUT_SUB_SURFACE].long_name, MAXSTRING, "%s",
             "sub_surface");
    snprintf(out_metadata[OUT_SUB_SURFACE].standard_name, MAXSTRING, "%s",
             "sublimation_amount_from_snow_pack_surface");
    snprintf(out_metadata[OUT_SUB_SURFACE].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_SUB_SURFACE].description, MAXSTRING, "%s",
             "net sublimation from snow pack surface");

    /* net transpiration from vegetation [mm] */
    snprintf(out_metadata[OUT_TRANSP_VEG].varname, MAXSTRING, "%s",
             "OUT_TRANSP_VEG");
    snprintf(out_metadata[OUT_TRANSP_VEG].long_name, MAXSTRING, "%s",
             "transp_veg");
    snprintf(out_metadata[OUT_TRANSP_VEG].standard_name, MAXSTRING, "%s",
             "transpiration_amount");
    snprintf(out_metadata[OUT_TRANSP_VEG].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_TRANSP_VEG].description, MAXSTRING, "%s",
             "net transpiration from vegetation");

    // Energy Balance Terms - state variables
    /* albedo [fraction] */
    snprintf(out_metadata[OUT_ALBEDO].varname, MAXSTRING, "%s", "OUT_ALBEDO");
    snprintf(out_metadata[OUT_ALBEDO].long_name, MAXSTRING, "%s", "albedo");
    snprintf(out_metadata[OUT_ALBEDO].standard_name, MAXSTRING, "%s",
             "surface_albedo");
    snprintf(out_metadata[OUT_ALBEDO].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_ALBEDO].description, MAXSTRING, "%s", "albedo");

    /* bare soil surface temperature [C] */
    snprintf(out_metadata[OUT_BARESOILT].varname, MAXSTRING, "%s",
             "OUT_BARESOILT");
    snprintf(out_metadata[OUT_BARESOILT].long_name, MAXSTRING, "%s",
             "baresoilt");
    snprintf(out_metadata[OUT_BARESOILT].standard_name, MAXSTRING, "%s",
             "surface_temperature_bare_soil");
    snprintf(out_metadata[OUT_BARESOILT].units, MAXSTRING, "%s", "C");
    snprintf(out_metadata[OUT_BARESOILT].description, MAXSTRING, "%s",
             "bare soil surface temperature");

    /* depth of freezing fronts [cm] for each freezing front */
    snprintf(out_metadata[OUT_FDEPTH].varname, MAXSTRING, "%s", "OUT_FDEPTH");
    snprintf(out_metadata[OUT_FDEPTH].long_name, MAXSTRING, "%s", "fdepth");
    snprintf(out_metadata[OUT_FDEPTH].standard_name, MAXSTRING, "%s",
             "freezing_fronts_depth");
    snprintf(out_metadata[OUT_FDEPTH].units, MAXSTRING, "%s", "cm");
    snprintf(out_metadata[OUT_FDEPTH].description, MAXSTRING, "%s",
             "depth of freezing fronts for each freezing front");

    /* lake ice temperature [K] */
    snprintf(out_metadata[OUT_LAKE_ICE_TEMP].varname, MAXSTRING, "%s",
             "OUT_LAKE_ICE_TEMP");
    snprintf(out_metadata[OUT_LAKE_ICE_TEMP].long_name, MAXSTRING, "%s",
             "lake_ice_temp");
    snprintf(out_metadata[OUT_LAKE_ICE_TEMP].standard_name, MAXSTRING, "%s",
             "lake_ice_temperature");
    snprintf(out_metadata[OUT_LAKE_ICE_TEMP].units, MAXSTRING, "%s", "K");
    snprintf(out_metadata[OUT_LAKE_ICE_TEMP].description, MAXSTRING, "%s",
             "lake ice temperature");

    /* lake surface temperature [K] */
    snprintf(out_metadata[OUT_LAKE_SURF_TEMP].varname, MAXSTRING, "%s",
             "OUT_LAKE_SURF_TEMP");
    snprintf(out_metadata[OUT_LAKE_SURF_TEMP].long_name, MAXSTRING, "%s",
             "lake_surf_temp");
    snprintf(out_metadata[OUT_LAKE_SURF_TEMP].standard_name, MAXSTRING, "%s",
             "lake_surface_temperature");
    snprintf(out_metadata[OUT_LAKE_SURF_TEMP].units, MAXSTRING, "%s", "K");
    snprintf(out_metadata[OUT_LAKE_SURF_TEMP].description, MAXSTRING, "%s",
             "lake surface temperature");

    /* average radiative surface temperature [K] */
    snprintf(out_metadata[OUT_RAD_TEMP].varname, MAXSTRING, "%s",
             "OUT_RAD_TEMP");
    snprintf(out_metadata[OUT_RAD_TEMP].long_name, MAXSTRING, "%s", "rad_temp");
    snprintf(out_metadata[OUT_RAD_TEMP].standard_name, MAXSTRING, "%s",
             "surface_radiative_temperature");
    snprintf(out_metadata[OUT_RAD_TEMP].units, MAXSTRING, "%s", "K");
    snprintf(out_metadata[OUT_RAD_TEMP].description, MAXSTRING, "%s",
             "average radiative surface temperature");

    /* snow albedo [fraction] */
    snprintf(out_metadata[OUT_SALBEDO].varname, MAXSTRING, "%s", "OUT_SALBEDO");
    snprintf(out_metadata[OUT_SALBEDO].long_name, MAXSTRING, "%s", "salbedo");
    snprintf(out_metadata[OUT_SALBEDO].standard_name, MAXSTRING, "%s",
             "snow_albedo");
    snprintf(out_metadata[OUT_SALBEDO].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_SALBEDO].description, MAXSTRING, "%s",
             "snow albedo");

    /* snow pack temperature [C] */
    snprintf(out_metadata[OUT_SNOW_PACK_TEMP].varname, MAXSTRING, "%s",
             "OUT_SNOW_PACK_TEMP");
    snprintf(out_metadata[OUT_SNOW_PACK_TEMP].long_name, MAXSTRING, "%s",
             "snow_pack_temp");
    snprintf(out_metadata[OUT_SNOW_PACK_TEMP].standard_name, MAXSTRING, "%s",
             "snow_pack_temperature");
    snprintf(out_metadata[OUT_SNOW_PACK_TEMP].units, MAXSTRING, "%s", "C");
    snprintf(out_metadata[OUT_SNOW_PACK_TEMP].description, MAXSTRING, "%s",
             "snow pack temperature");

    /* snow surface temperature [C] */
    snprintf(out_metadata[OUT_SNOW_SURF_TEMP].varname, MAXSTRING, "%s",
             "OUT_SNOW_SURF_TEMP");
    snprintf(out_metadata[OUT_SNOW_SURF_TEMP].long_name, MAXSTRING, "%s",
             "snow_surf_temp");
    snprintf(out_metadata[OUT_SNOW_SURF_TEMP].standard_name, MAXSTRING, "%s",
             "snow_surface_temperature");
    snprintf(out_metadata[OUT_SNOW_SURF_TEMP].units, MAXSTRING, "%s", "C");
    snprintf(out_metadata[OUT_SNOW_SURF_TEMP].description, MAXSTRING, "%s",
             "snow surface temperature");

    /* snow surface temperature flag */
    snprintf(out_metadata[OUT_SNOWT_FBFLAG].varname, MAXSTRING, "%s",
             "OUT_SNOWT_FBFLAG");
    snprintf(out_metadata[OUT_SNOWT_FBFLAG].long_name, MAXSTRING, "%s",
             "snowt_fbflag");
    snprintf(out_metadata[OUT_SNOWT_FBFLAG].standard_name, MAXSTRING, "%s",
             "snow_surface_temperature_flag");
    snprintf(out_metadata[OUT_SNOWT_FBFLAG].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_SNOWT_FBFLAG].description, MAXSTRING, "%s",
             "snow surface temperature flag");

    /* soil temperature [C] for each soil layer */
    snprintf(out_metadata[OUT_SOIL_TEMP].varname, MAXSTRING, "%s",
             "OUT_SOIL_TEMP");
    snprintf(out_metadata[OUT_SOIL_TEMP].long_name, MAXSTRING, "%s",
             "soil_temp");
    snprintf(out_metadata[OUT_SOIL_TEMP].standard_name, MAXSTRING, "%s",
             "soil_temperature");
    snprintf(out_metadata[OUT_SOIL_TEMP].units, MAXSTRING, "%s", "C");
    snprintf(out_metadata[OUT_SOIL_TEMP].description, MAXSTRING, "%s",
             "soil temperature for each soil layer");

    /* soil temperature [C] for each soil thermal node */
    snprintf(out_metadata[OUT_SOIL_TNODE].varname, MAXSTRING, "%s",
             "OUT_SOIL_TNODE");
    snprintf(out_metadata[OUT_SOIL_TNODE].long_name, MAXSTRING, "%s",
             "soil_tnode");
    snprintf(out_metadata[OUT_SOIL_TNODE].standard_name, MAXSTRING, "%s",
             "soil_temperature");
    snprintf(out_metadata[OUT_SOIL_TNODE].units, MAXSTRING, "%s", "C");
    snprintf(out_metadata[OUT_SOIL_TNODE].description, MAXSTRING, "%s",
             "soil temperature for each soil thermal node");

    /* soil temperature [C] for each soil thermal node in the wetland */
    snprintf(out_metadata[OUT_SOIL_TNODE_WL].varname, MAXSTRING, "%s",
             "OUT_SOIL_TNODE_WL");
    snprintf(out_metadata[OUT_SOIL_TNODE_WL].long_name, MAXSTRING, "%s",
             "soil_tnode_wl");
    snprintf(out_metadata[OUT_SOIL_TNODE_WL].standard_name, MAXSTRING, "%s",
             "soil_temperature");
    snprintf(out_metadata[OUT_SOIL_TNODE_WL].units, MAXSTRING, "%s", "C");
    snprintf(out_metadata[OUT_SOIL_TNODE_WL].description, MAXSTRING, "%s",
             "soil temperature for each soil thermal node in the wetland");

    /* soil temperature flag for each soil thermal node */
    snprintf(out_metadata[OUT_SOILT_FBFLAG].varname, MAXSTRING, "%s",
             "OUT_SOILT_FBFLAG");
    snprintf(out_metadata[OUT_SOILT_FBFLAG].long_name, MAXSTRING, "%s",
             "soilt_fbflag");
    snprintf(out_metadata[OUT_SOILT_FBFLAG].standard_name, MAXSTRING, "%s",
             "soil_temperature_flag");
    snprintf(out_metadata[OUT_SOILT_FBFLAG].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_SOILT_FBFLAG].description, MAXSTRING, "%s",
             "soil temperature flag for each soil thermal node");

    /* average surface temperature [C] */
    snprintf(out_metadata[OUT_SURF_TEMP].varname, MAXSTRING, "%s",
             "OUT_SURF_TEMP");
    snprintf(out_metadata[OUT_SURF_TEMP].long_name, MAXSTRING, "%s",
             "surf_temp");
    snprintf(out_metadata[OUT_SURF_TEMP].standard_name, MAXSTRING, "%s",
             "surface_temperature");
    snprintf(out_metadata[OUT_SURF_TEMP].units, MAXSTRING, "%s", "C");
    snprintf(out_metadata[OUT_SURF_TEMP].description, MAXSTRING, "%s",
             "average surface temperature");

    /* surface temperature flag */
    snprintf(out_metadata[OUT_SURFT_FBFLAG].varname, MAXSTRING, "%s",
             "OUT_SURFT_FBFLAG");
    snprintf(out_metadata[OUT_SURFT_FBFLAG].long_name, MAXSTRING, "%s",
             "surft_fbflag");
    snprintf(out_metadata[OUT_SURFT_FBFLAG].standard_name, MAXSTRING, "%s",
             "surface_temperature_flag");
    snprintf(out_metadata[OUT_SURFT_FBFLAG].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_SURFT_FBFLAG].description, MAXSTRING, "%s",
             "surface temperature flag");

    /* Tcanopy flag */
    snprintf(out_metadata[OUT_TCAN_FBFLAG].varname, MAXSTRING, "%s",
             "OUT_TCAN_FBFLAG");
    snprintf(out_metadata[OUT_TCAN_FBFLAG].long_name, MAXSTRING, "%s",
             "tcan_fbflag");
    snprintf(out_metadata[OUT_TCAN_FBFLAG].standard_name, MAXSTRING, "%s",
             "canopy_temperature_flag");
    snprintf(out_metadata[OUT_TCAN_FBFLAG].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_TCAN_FBFLAG].description, MAXSTRING, "%s",
             "Canopy temperature fallback flag");

    /* depth of thawing fronts [cm] for each thawing front */
    snprintf(out_metadata[OUT_TDEPTH].varname, MAXSTRING, "%s", "OUT_TDEPTH");
    snprintf(out_metadata[OUT_TDEPTH].long_name, MAXSTRING, "%s", "tdepth");
    snprintf(out_metadata[OUT_TDEPTH].standard_name, MAXSTRING, "%s",
             "depth_of_thawing_fronts");
    snprintf(out_metadata[OUT_TDEPTH].units, MAXSTRING, "%s", "cm");
    snprintf(out_metadata[OUT_TDEPTH].description, MAXSTRING, "%s",
             "depth of thawing fronts for each thawing front");

    /* Tfoliage flag */
    snprintf(out_metadata[OUT_TFOL_FBFLAG].varname, MAXSTRING, "%s",
             "OUT_TFOL_FBFLAG");
    snprintf(out_metadata[OUT_TFOL_FBFLAG].long_name, MAXSTRING, "%s",
             "tfol_fbflag");
    snprintf(out_metadata[OUT_TFOL_FBFLAG].standard_name, MAXSTRING, "%s",
             "foliage_temperature_flag");
    snprintf(out_metadata[OUT_TFOL_FBFLAG].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_TFOL_FBFLAG].description, MAXSTRING, "%s",
             "foilage temperature fallback flag");

    /* average vegetation canopy temperature [C] */
    snprintf(out_metadata[OUT_VEGT].varname, MAXSTRING, "%s", "OUT_VEGT");
    snprintf(out_metadata[OUT_VEGT].long_name, MAXSTRING, "%s", "vegt");
    snprintf(out_metadata[OUT_VEGT].standard_name, MAXSTRING, "%s",
             "canopy_temperature");
    snprintf(out_metadata[OUT_VEGT].units, MAXSTRING, "%s", "C");
    snprintf(out_metadata[OUT_TFOL_FBFLAG].description, MAXSTRING, "%s",
             "average vegetation canopy temperature");

    // Energy Balance Terms - fluxes
    /* net sensible heat advected to snow pack [W m-2] */
    snprintf(out_metadata[OUT_ADV_SENS].varname, MAXSTRING, "%s",
             "OUT_ADV_SENS");
    snprintf(out_metadata[OUT_ADV_SENS].long_name, MAXSTRING, "%s", "adv_sens");
    snprintf(out_metadata[OUT_ADV_SENS].standard_name, MAXSTRING, "%s",
             "net_sensible_heat_flux_to_snow_pack");
    snprintf(out_metadata[OUT_ADV_SENS].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_ADV_SENS].description, MAXSTRING, "%s",
             "net sensible heat advected to snow pack");

    /* advected energy [W m-2] */
    snprintf(out_metadata[OUT_ADVECTION].varname, MAXSTRING, "%s",
             "OUT_ADVECTION");
    snprintf(out_metadata[OUT_ADVECTION].long_name, MAXSTRING, "%s",
             "advection");
    snprintf(out_metadata[OUT_ADVECTION].standard_name, MAXSTRING, "%s",
             "advected_energy");
    snprintf(out_metadata[OUT_ADVECTION].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_ADVECTION].description, MAXSTRING, "%s",
             "advected energy ");

    /* rate of change in cold content in snow pack [W m-2] */
    snprintf(out_metadata[OUT_DELTACC].varname, MAXSTRING, "%s", "OUT_DELTACC");
    snprintf(out_metadata[OUT_DELTACC].long_name, MAXSTRING, "%s", "deltacc");
    snprintf(out_metadata[OUT_DELTACC].standard_name, MAXSTRING, "%s",
             "rate_change_in_snow_pack_cold_content");
    snprintf(out_metadata[OUT_DELTACC].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_DELTACC].description, MAXSTRING, "%s",
             "rate of change in cold content in snow pack");

    /* rate of change in heat storage [W m-2] */
    snprintf(out_metadata[OUT_DELTAH].varname, MAXSTRING, "%s", "OUT_DELTAH");
    snprintf(out_metadata[OUT_DELTAH].long_name, MAXSTRING, "%s", "deltah");
    snprintf(out_metadata[OUT_DELTAH].standard_name, MAXSTRING, "%s",
             "rate_change_in_heat_storage");
    snprintf(out_metadata[OUT_DELTAH].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_DELTAH].description, MAXSTRING, "%s",
             "rate of change in heat storage");

    /* energy budget error [W m-2] */
    snprintf(out_metadata[OUT_ENERGY_ERROR].varname, MAXSTRING, "%s",
             "OUT_ENERGY_ERROR");
    snprintf(out_metadata[OUT_ENERGY_ERROR].long_name, MAXSTRING, "%s",
             "energy_error");
    snprintf(out_metadata[OUT_ENERGY_ERROR].standard_name, MAXSTRING, "%s",
             "energy_budget_error");
    snprintf(out_metadata[OUT_ENERGY_ERROR].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_ENERGY_ERROR].description, MAXSTRING, "%s",
             "energy budget error");

    /* water budget error [mm] */
    snprintf(out_metadata[OUT_WATER_ERROR].varname, MAXSTRING, "%s",
             "OUT_WATER_ERROR");
    snprintf(out_metadata[OUT_WATER_ERROR].long_name, MAXSTRING, "%s",
             "water_error");
    snprintf(out_metadata[OUT_WATER_ERROR].standard_name, MAXSTRING, "%s",
             "water_budget_error");
    snprintf(out_metadata[OUT_WATER_ERROR].units, MAXSTRING, "%s", "mm");
    snprintf(out_metadata[OUT_WATER_ERROR].description, MAXSTRING, "%s",
             "water budget error");

    /* net energy used to melt/freeze soil moisture [W m-2] */
    snprintf(out_metadata[OUT_FUSION].varname, MAXSTRING, "%s", "OUT_FUSION");
    snprintf(out_metadata[OUT_FUSION].long_name, MAXSTRING, "%s", "fusion");
    snprintf(out_metadata[OUT_FUSION].standard_name, MAXSTRING, "%s",
             "energy_of_fusion_in_soil_moisture");
    snprintf(out_metadata[OUT_FUSION].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_FUSION].description, MAXSTRING, "%s",
             "net energy used to melt/freeze soil moisture");

    /* net heat flux into ground [W m-2] */
    snprintf(out_metadata[OUT_GRND_FLUX].varname, MAXSTRING, "%s",
             "OUT_GRND_FLUX");
    snprintf(out_metadata[OUT_GRND_FLUX].long_name, MAXSTRING, "%s",
             "grnd_flux");
    snprintf(out_metadata[OUT_GRND_FLUX].standard_name, MAXSTRING, "%s",
             "downward_heat_flux_in_soil");
    snprintf(out_metadata[OUT_GRND_FLUX].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_GRND_FLUX].description, MAXSTRING, "%s",
             "net heat flux into ground");

    /* incoming longwave flux at surface (under veg) [W m-2] */
    snprintf(out_metadata[OUT_IN_LONG].varname, MAXSTRING, "%s", "OUT_IN_LONG");
    snprintf(out_metadata[OUT_IN_LONG].long_name, MAXSTRING, "%s", "in_long");
    snprintf(out_metadata[OUT_IN_LONG].standard_name, MAXSTRING, "%s",
             "downwelling_longwave_flux_at_ground_surface");
    snprintf(out_metadata[OUT_IN_LONG].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_IN_LONG].description, MAXSTRING, "%s",
             "incoming longwave flux at surface (under veg)");

    /* net upward latent heat flux [W m-2] */
    snprintf(out_metadata[OUT_LATENT].varname, MAXSTRING, "%s", "OUT_LATENT");
    snprintf(out_metadata[OUT_LATENT].long_name, MAXSTRING, "%s", "latent");
    snprintf(out_metadata[OUT_LATENT].standard_name, MAXSTRING, "%s",
             "surface_upward_latent_heat_flux");
    snprintf(out_metadata[OUT_LATENT].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_LATENT].description, MAXSTRING, "%s",
             "net upward latent heat flux");

    /* net upward latent heat flux from sublimation [W m-2] */
    snprintf(out_metadata[OUT_LATENT_SUB].varname, MAXSTRING, "%s",
             "OUT_LATENT_SUB");
    snprintf(out_metadata[OUT_LATENT_SUB].long_name, MAXSTRING, "%s",
             "latent_sub");
    snprintf(out_metadata[OUT_LATENT_SUB].standard_name, MAXSTRING, "%s",
             "surface_net_latent_heat_flux_from_sublimation");
    snprintf(out_metadata[OUT_LATENT_SUB].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_LATENT_SUB].description, MAXSTRING, "%s",
             "net upward latent heat flux from sublimation");

    /* energy of fusion (melting) [W m-2] */
    snprintf(out_metadata[OUT_MELT_ENERGY].varname, MAXSTRING, "%s",
             "OUT_MELT_ENERGY");
    snprintf(out_metadata[OUT_MELT_ENERGY].long_name, MAXSTRING, "%s",
             "melt_energy");
    snprintf(out_metadata[OUT_MELT_ENERGY].standard_name, MAXSTRING, "%s",
             "energy_of_fusion");
    snprintf(out_metadata[OUT_MELT_ENERGY].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_MELT_ENERGY].description, MAXSTRING, "%s",
             "energy of fusion (melting)");

    /* net downward longwave flux [W m-2] */
    snprintf(out_metadata[OUT_LWNET].varname, MAXSTRING, "%s", "OUT_LWNET");
    snprintf(out_metadata[OUT_LWNET].long_name, MAXSTRING, "%s", "lwnet");
    snprintf(out_metadata[OUT_LWNET].standard_name, MAXSTRING, "%s",
             "net_downward_longwave_flux_at_surface");
    snprintf(out_metadata[OUT_LWNET].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_LWNET].description, MAXSTRING, "%s",
             "net downward longwave flux");

    /* net downward shortwave flux [W m-2] */
    snprintf(out_metadata[OUT_SWNET].varname, MAXSTRING, "%s", "OUT_SWNET");
    snprintf(out_metadata[OUT_SWNET].long_name, MAXSTRING, "%s", "swnet");
    snprintf(out_metadata[OUT_SWNET].standard_name, MAXSTRING, "%s",
             "net_downward_shortwave_flux_at_surface");
    snprintf(out_metadata[OUT_SWNET].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_SWNET].description, MAXSTRING, "%s",
             "net downward shortwave flux");

    /* net downward radiation flux [W m-2] */
    snprintf(out_metadata[OUT_R_NET].varname, MAXSTRING, "%s", "OUT_R_NET");
    snprintf(out_metadata[OUT_R_NET].long_name, MAXSTRING, "%s", "r_net");
    snprintf(out_metadata[OUT_R_NET].standard_name, MAXSTRING, "%s",
             "net_downward_radiation_flux_at_surface");
    snprintf(out_metadata[OUT_R_NET].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_R_NET].description, MAXSTRING, "%s",
             "net downward radiation flux");

    /* net energy used to refreeze liquid water in snowpack [W m-2] */
    snprintf(out_metadata[OUT_RFRZ_ENERGY].varname, MAXSTRING, "%s",
             "OUT_RFRZ_ENERGY");
    snprintf(out_metadata[OUT_RFRZ_ENERGY].long_name, MAXSTRING, "%s",
             "rfrz_energy");
    snprintf(out_metadata[OUT_RFRZ_ENERGY].standard_name, MAXSTRING, "%s",
             "net_energy_used_to_refreeze_water_in_snowpack");
    snprintf(out_metadata[OUT_RFRZ_ENERGY].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_RFRZ_ENERGY].description, MAXSTRING, "%s",
             "net energy used to refreeze liquid water in snowpack");

    /* net upward sensible heat flux [W m-2] */
    snprintf(out_metadata[OUT_SENSIBLE].varname, MAXSTRING, "%s",
             "OUT_SENSIBLE");
    snprintf(out_metadata[OUT_SENSIBLE].long_name, MAXSTRING, "%s", "sensible");
    snprintf(out_metadata[OUT_SENSIBLE].standard_name, MAXSTRING, "%s",
             "surface_upward_net_sensible_heat_flux");
    snprintf(out_metadata[OUT_SENSIBLE].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_SENSIBLE].description, MAXSTRING, "%s",
             "net upward sensible heat flux");

    /* energy flux through snow pack [W m-2] */
    snprintf(out_metadata[OUT_SNOW_FLUX].varname, MAXSTRING, "%s",
             "OUT_SNOW_FLUX");
    snprintf(out_metadata[OUT_SNOW_FLUX].long_name, MAXSTRING, "%s",
             "snow_flux");
    snprintf(out_metadata[OUT_SNOW_FLUX].standard_name, MAXSTRING, "%s",
             "energy_flux_through_snow_pack");
    snprintf(out_metadata[OUT_SNOW_FLUX].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_SNOW_FLUX].description, MAXSTRING, "%s",
             "energy flux through snow pack");

    // Miscellaneous Terms

    /* "scene" aerodynamic conductance [m/s] (tiles with overstory contribute
        overstory conductance; others contribue surface conductance) */
    snprintf(out_metadata[OUT_AERO_COND].varname, MAXSTRING, "%s",
             "OUT_AERO_COND");
    snprintf(out_metadata[OUT_AERO_COND].long_name, MAXSTRING, "%s",
             "aero_cond");
    snprintf(out_metadata[OUT_AERO_COND].standard_name, MAXSTRING, "%s",
             "aerodynamic_conductance");
    snprintf(out_metadata[OUT_AERO_COND].units, MAXSTRING, "%s", "m/s");
    snprintf(out_metadata[OUT_AERO_COND].description, MAXSTRING, "%s",
             "scene aerodynamic conductance (tiles with overstory contribute "
             "overstory conductance; others contribue surface conductance)");

    /* surface aerodynamic conductance [m/s] */
    snprintf(out_metadata[OUT_AERO_COND1].varname, MAXSTRING, "%s",
             "OUT_AERO_COND1");
    snprintf(out_metadata[OUT_AERO_COND1].long_name, MAXSTRING, "%s",
             "aero_cond1");
    snprintf(out_metadata[OUT_AERO_COND1].standard_name, MAXSTRING, "%s",
             "aerodynamic_conductance_surface");
    snprintf(out_metadata[OUT_AERO_COND1].units, MAXSTRING, "%s", "m/s");
    snprintf(out_metadata[OUT_AERO_COND1].description, MAXSTRING, "%s",
             "surface aerodynamic conductance");

    /* overstory aerodynamic conductance [m/s] */
    snprintf(out_metadata[OUT_AERO_COND2].varname, MAXSTRING, "%s",
             "OUT_AERO_COND2");
    snprintf(out_metadata[OUT_AERO_COND2].long_name, MAXSTRING, "%s",
             "aero_cond2");
    snprintf(out_metadata[OUT_AERO_COND2].standard_name, MAXSTRING, "%s",
             "aerodynamic_conductance_overstory");
    snprintf(out_metadata[OUT_AERO_COND2].units, MAXSTRING, "%s", "m/s");
    snprintf(out_metadata[OUT_AERO_COND2].description, MAXSTRING, "%s",
             "overstory aerodynamic conductance");

    /* "scene" aerodynamic resistance [s m-1] (tiles with overstory contribute overstory resistance; others contribue surface resistance)*/
    snprintf(out_metadata[OUT_AERO_RESIST].varname, MAXSTRING, "%s",
             "OUT_AERO_RESIST");
    snprintf(out_metadata[OUT_AERO_RESIST].long_name, MAXSTRING, "%s",
             "aero_resist");
    snprintf(out_metadata[OUT_AERO_RESIST].standard_name, MAXSTRING, "%s",
             "aerodynamic_resistance");
    snprintf(out_metadata[OUT_AERO_RESIST].units, MAXSTRING, "%s", "s m-1");
    snprintf(out_metadata[OUT_AERO_RESIST].description, MAXSTRING, "%s",
             "scene aerodynamic resistance (tiles with overstory contribute overstory resistance; others contribue surface resistance)");

    /* surface aerodynamic resistance [m/s] */
    snprintf(out_metadata[OUT_AERO_RESIST1].varname, MAXSTRING, "%s",
             "OUT_AERO_RESIST1");
    snprintf(out_metadata[OUT_AERO_RESIST1].long_name, MAXSTRING, "%s",
             "aero_resist1");
    snprintf(out_metadata[OUT_AERO_RESIST1].standard_name, MAXSTRING, "%s",
             "aerodynamic_resistance_surface");
    snprintf(out_metadata[OUT_AERO_RESIST1].units, MAXSTRING, "%s", "s m-1");
    snprintf(out_metadata[OUT_AERO_RESIST1].description, MAXSTRING, "%s",
             "surface aerodynamic resistance");

    /* overstory aerodynamic resistance [m/s] */
    snprintf(out_metadata[OUT_AERO_RESIST2].varname, MAXSTRING, "%s",
             "OUT_AERO_RESIST2");
    snprintf(out_metadata[OUT_AERO_RESIST2].long_name, MAXSTRING, "%s",
             "aero_resist2");
    snprintf(out_metadata[OUT_AERO_RESIST2].standard_name, MAXSTRING, "%s",
             "aerodynamic_resistance_overstory");
    snprintf(out_metadata[OUT_AERO_RESIST2].units, MAXSTRING, "%s", "s m-1");
    snprintf(out_metadata[OUT_AERO_RESIST2].description, MAXSTRING, "%s",
             "overstory aerodynamic resistance");

    /* air temperature [C] */
    snprintf(out_metadata[OUT_AIR_TEMP].varname, MAXSTRING, "%s",
             "OUT_AIR_TEMP");
    snprintf(out_metadata[OUT_AIR_TEMP].long_name, MAXSTRING, "%s", "air_temp");
    snprintf(out_metadata[OUT_AIR_TEMP].standard_name, MAXSTRING, "%s",
             "air_temperature");
    snprintf(out_metadata[OUT_AIR_TEMP].units, MAXSTRING, "%s", "C");
    snprintf(out_metadata[OUT_AIR_TEMP].description, MAXSTRING, "%s",
             "air temperature");

    /* atmospheric CO2 concentration [ppm] */
    snprintf(out_metadata[OUT_CATM].varname, MAXSTRING, "%s", "OUT_CATM");
    snprintf(out_metadata[OUT_CATM].long_name, MAXSTRING, "%s", "catm");
    snprintf(out_metadata[OUT_CATM].standard_name, MAXSTRING, "%s",
             "concentration_of_carbon_dioxide_in_air");
    snprintf(out_metadata[OUT_CATM].units, MAXSTRING, "%s", "ppm");
    snprintf(out_metadata[OUT_CATM].description, MAXSTRING, "%s",
             "atmospheric CO2 concentration");

    /* near-surface atmospheric density [kg m-3] */
    snprintf(out_metadata[OUT_DENSITY].varname, MAXSTRING, "%s", "OUT_DENSITY");
    snprintf(out_metadata[OUT_DENSITY].long_name, MAXSTRING, "%s", "density");
    snprintf(out_metadata[OUT_DENSITY].standard_name, MAXSTRING, "%s",
             "air_density");
    snprintf(out_metadata[OUT_DENSITY].units, MAXSTRING, "%s", "kg m-3");
    snprintf(out_metadata[OUT_DENSITY].description, MAXSTRING, "%s",
             "near-surface atmospheric density");

    /* fractional area covered by plant canopy [fraction] */
    snprintf(out_metadata[OUT_FCANOPY].varname, MAXSTRING, "%s", "OUT_FCANOPY");
    snprintf(out_metadata[OUT_FCANOPY].long_name, MAXSTRING, "%s", "fcanopy");
    snprintf(out_metadata[OUT_FCANOPY].standard_name, MAXSTRING, "%s",
             "canopy_cover_area_fraction");
    snprintf(out_metadata[OUT_FCANOPY].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_FCANOPY].description, MAXSTRING, "%s",
             "fractional area covered by plant canopy");

    /* fraction of incoming shortwave that is direct [fraction] */
    snprintf(out_metadata[OUT_FDIR].varname, MAXSTRING, "%s", "OUT_FDIR");
    snprintf(out_metadata[OUT_FDIR].long_name, MAXSTRING, "%s", "fdir");
    snprintf(out_metadata[OUT_FDIR].standard_name, MAXSTRING, "%s",
             "fraction_of_incoming_shorwave_radiation_that_is_direct");
    snprintf(out_metadata[OUT_FDIR].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_FDIR].description, MAXSTRING, "%s",
             "fraction of incoming shortwave that is direct");

    /* leaf area index [1] */
    snprintf(out_metadata[OUT_LAI].varname, MAXSTRING, "%s", "OUT_LAI");
    snprintf(out_metadata[OUT_LAI].long_name, MAXSTRING, "%s", "lai");
    snprintf(out_metadata[OUT_LAI].standard_name, MAXSTRING, "%s",
             "leaf_area_index");
    snprintf(out_metadata[OUT_LAI].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_LAI].description, MAXSTRING, "%s",
             "leaf area index");

    /* incoming longwave [W m-2] */
    snprintf(out_metadata[OUT_LWDOWN].varname, MAXSTRING, "%s", "OUT_LWDOWN");
    snprintf(out_metadata[OUT_LWDOWN].long_name, MAXSTRING, "%s", "lwdown");
    snprintf(out_metadata[OUT_LWDOWN].standard_name, MAXSTRING, "%s",
             "downwelling_longwave_flux_in_air");
    snprintf(out_metadata[OUT_LWDOWN].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_LWDOWN].description, MAXSTRING, "%s",
             "incoming longwave");

    /* incoming photosynthetically active radiation [W m-2] */
    snprintf(out_metadata[OUT_PAR].varname, MAXSTRING, "%s", "OUT_PAR");
    snprintf(out_metadata[OUT_PAR].long_name, MAXSTRING, "%s", "par");
    snprintf(out_metadata[OUT_PAR].standard_name, MAXSTRING, "%s",
             "surface_downwelling_photosynthetic_radiative_flux_in_air");
    snprintf(out_metadata[OUT_PAR].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_PAR].description, MAXSTRING, "%s",
             "incoming photosynthetically active radiation");

    /* near surface atmospheric pressure [kPa] */
    snprintf(out_metadata[OUT_PRESSURE].varname, MAXSTRING, "%s",
             "OUT_PRESSURE");
    snprintf(out_metadata[OUT_PRESSURE].long_name, MAXSTRING, "%s", "pressure");
    snprintf(out_metadata[OUT_PRESSURE].standard_name, MAXSTRING, "%s",
             "surface_air_pressure");
    snprintf(out_metadata[OUT_PRESSURE].units, MAXSTRING, "%s", "kPa");
    snprintf(out_metadata[OUT_PRESSURE].description, MAXSTRING, "%s",
             "near surface atmospheric pressure");

    /* specific humidity [kg/kg] */
    snprintf(out_metadata[OUT_QAIR].varname, MAXSTRING, "%s", "OUT_QAIR");
    snprintf(out_metadata[OUT_QAIR].long_name, MAXSTRING, "%s", "qair");
    snprintf(out_metadata[OUT_QAIR].standard_name, MAXSTRING, "%s",
             "specific_humidity");
    snprintf(out_metadata[OUT_QAIR].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_QAIR].description, MAXSTRING, "%s",
             "specific humidity");

    /* relative humidity [fraction]*/
    snprintf(out_metadata[OUT_REL_HUMID].varname, MAXSTRING, "%s",
             "OUT_REL_HUMID");
    snprintf(out_metadata[OUT_REL_HUMID].long_name, MAXSTRING, "%s",
             "rel_humid");
    snprintf(out_metadata[OUT_REL_HUMID].standard_name, MAXSTRING, "%s",
             "relative_humidity");
    snprintf(out_metadata[OUT_REL_HUMID].units, MAXSTRING, "%s", "1");
    snprintf(out_metadata[OUT_REL_HUMID].description, MAXSTRING, "%s",
             "relative humidity");

    /* incoming shortwave [W m-2] */
    snprintf(out_metadata[OUT_SWDOWN].varname, MAXSTRING, "%s", "OUT_SWDOWN");
    snprintf(out_metadata[OUT_SWDOWN].long_name, MAXSTRING, "%s", "swdown");
    snprintf(out_metadata[OUT_SWDOWN].standard_name, MAXSTRING, "%s",
             "incoming shortwave");
    snprintf(out_metadata[OUT_SWDOWN].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_SWDOWN].description, MAXSTRING, "%s",
             "incoming shortwave");

    /* surface conductance [m/s] */
    snprintf(out_metadata[OUT_SURF_COND].varname, MAXSTRING, "%s",
             "OUT_SURF_COND");
    snprintf(out_metadata[OUT_SURF_COND].long_name, MAXSTRING, "%s",
             "surf_cond");
    snprintf(out_metadata[OUT_SURF_COND].standard_name, MAXSTRING, "%s",
             "surface_conductance");
    snprintf(out_metadata[OUT_SURF_COND].units, MAXSTRING, "%s", "m s-1");
    snprintf(out_metadata[OUT_SURF_COND].description, MAXSTRING, "%s",
             "surface conductance");

    /* near surface vapor pressure [kPa] */
    snprintf(out_metadata[OUT_VP].varname, MAXSTRING, "%s", "OUT_VP");
    snprintf(out_metadata[OUT_VP].long_name, MAXSTRING, "%s", "vp");
    snprintf(out_metadata[OUT_VP].standard_name, MAXSTRING, "%s",
             "water_vapor_pressure");
    snprintf(out_metadata[OUT_VP].units, MAXSTRING, "%s", "kPa");
    snprintf(out_metadata[OUT_VP].description, MAXSTRING, "%s",
             "near surface vapor pressure");

    /* near surface vapor pressure deficit [kPa] */
    snprintf(out_metadata[OUT_VPD].varname, MAXSTRING, "%s", "OUT_VPD");
    snprintf(out_metadata[OUT_VPD].long_name, MAXSTRING, "%s", "vpd");
    snprintf(out_metadata[OUT_VPD].standard_name, MAXSTRING, "%s",
             "water_vapor_saturation_deficit");
    snprintf(out_metadata[OUT_VPD].units, MAXSTRING, "%s", "kPa");
    snprintf(out_metadata[OUT_VPD].description, MAXSTRING, "%s",
             "near surface vapor pressure deficit");

    /* near surface wind speed [m/s] */
    snprintf(out_metadata[OUT_WIND].varname, MAXSTRING, "%s", "OUT_WIND");
    snprintf(out_metadata[OUT_WIND].long_name, MAXSTRING, "%s", "wind");
    snprintf(out_metadata[OUT_WIND].standard_name, MAXSTRING, "%s",
             "wind_speed");
    snprintf(out_metadata[OUT_WIND].units, MAXSTRING, "%s", "m s-1");
    snprintf(out_metadata[OUT_WIND].description, MAXSTRING, "%s",
             "near surface wind speed");

    // Carbon-cycling Terms
    /* absorbed PAR [W m-2] */
    snprintf(out_metadata[OUT_APAR].varname, MAXSTRING, "%s", "OUT_APAR");
    snprintf(out_metadata[OUT_APAR].long_name, MAXSTRING, "%s", "apar");
    snprintf(out_metadata[OUT_APAR].standard_name, MAXSTRING, "%s",
             "absorbed_surface_diffuse_downwelling_photosynthetic_radiative_flux");
    snprintf(out_metadata[OUT_APAR].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_APAR].description, MAXSTRING, "%s",
             "absorbed PAR");

    /* gross primary productivity [g C/m2s] */
    snprintf(out_metadata[OUT_GPP].varname, MAXSTRING, "%s", "OUT_GPP");
    snprintf(out_metadata[OUT_GPP].long_name, MAXSTRING, "%s", "gpp");
    snprintf(out_metadata[OUT_GPP].standard_name, MAXSTRING, "%s",
             "gross_primary_productivity_of_biomass_expressed_as_carbon");
    snprintf(out_metadata[OUT_GPP].units, MAXSTRING, "%s", "g m-2 s-1");
    snprintf(out_metadata[OUT_GPP].description, MAXSTRING, "%s",
             "gross primary productivity");

    /* autotrophic respiration [g C/m2s] */
    snprintf(out_metadata[OUT_RAUT].varname, MAXSTRING, "%s", "OUT_RAUT");
    snprintf(out_metadata[OUT_RAUT].long_name, MAXSTRING, "%s", "raut");
    snprintf(out_metadata[OUT_RAUT].standard_name, MAXSTRING, "%s",
             "autotrophic_respiration_carbon_flux");
    snprintf(out_metadata[OUT_RAUT].units, MAXSTRING, "%s", "g m-2 s-1");
    snprintf(out_metadata[OUT_RAUT].description, MAXSTRING, "%s",
             "autotrophic respiration");

    /* net primary productivity [g C/m2s] */
    snprintf(out_metadata[OUT_NPP].varname, MAXSTRING, "%s", "OUT_NPP");
    snprintf(out_metadata[OUT_NPP].long_name, MAXSTRING, "%s", "npp");
    snprintf(out_metadata[OUT_NPP].standard_name, MAXSTRING, "%s",
             "net_primary_productivity_of_biomass_expressed_as_carbon");
    snprintf(out_metadata[OUT_NPP].units, MAXSTRING, "%s", "g m-2 s-1");
    snprintf(out_metadata[OUT_NPP].description, MAXSTRING, "%s",
             "et primary productivity");

    /* flux of carbon from living biomass into soil [g C/m2d] */
    snprintf(out_metadata[OUT_LITTERFALL].varname, MAXSTRING, "%s",
             "OUT_LITTERFALL");
    snprintf(out_metadata[OUT_LITTERFALL].long_name, MAXSTRING, "%s",
             "litterfall");
    snprintf(out_metadata[OUT_LITTERFALL].standard_name, MAXSTRING, "%s",
             "carbon_mass_flux_into_soil_from_litter");
    snprintf(out_metadata[OUT_LITTERFALL].units, MAXSTRING, "%s", "g m-2 d-1");
    snprintf(out_metadata[OUT_LITTERFALL].description, MAXSTRING, "%s",
             "flux of carbon from living biomass into soil");

    /* heterotrophic respiration [g C/m2d] */
    snprintf(out_metadata[OUT_RHET].varname, MAXSTRING, "%s", "OUT_RHET");
    snprintf(out_metadata[OUT_RHET].long_name, MAXSTRING, "%s", "rhet");
    snprintf(out_metadata[OUT_RHET].standard_name, MAXSTRING, "%s",
             "heterotrophic_respiration");
    snprintf(out_metadata[OUT_RHET].units, MAXSTRING, "%s", "g m-2 d-1");
    snprintf(out_metadata[OUT_RHET].description, MAXSTRING, "%s",
             "heterotrophic respiration");

    /* net ecosystem exchange [g C/m2d] */
    snprintf(out_metadata[OUT_NEE].varname, MAXSTRING, "%s", "OUT_NEE");
    snprintf(out_metadata[OUT_NEE].long_name, MAXSTRING, "%s", "nee");
    snprintf(out_metadata[OUT_NEE].standard_name, MAXSTRING, "%s",
             "net_ecosystem_exhanged_expressed_as_carbon");
    snprintf(out_metadata[OUT_NEE].units, MAXSTRING, "%s", "g m-2 d-1");
    snprintf(out_metadata[OUT_NEE].description, MAXSTRING, "%s",
             "net ecosystem exchange");

    /* litter pool carbon density [g C/m2] */
    snprintf(out_metadata[OUT_CLITTER].varname, MAXSTRING, "%s", "OUT_CLITTER");
    snprintf(out_metadata[OUT_CLITTER].long_name, MAXSTRING, "%s", "clitter");
    snprintf(out_metadata[OUT_CLITTER].standard_name, MAXSTRING, "%s",
             "litter_carbon_content");
    snprintf(out_metadata[OUT_CLITTER].units, MAXSTRING, "%s", "g m-2");
    snprintf(out_metadata[OUT_CLITTER].description, MAXSTRING, "%s",
             "litter pool carbon density");

    /* intermediate pool carbon density [g C/m2] */
    snprintf(out_metadata[OUT_CINTER].varname, MAXSTRING, "%s", "OUT_CINTER");
    snprintf(out_metadata[OUT_CINTER].long_name, MAXSTRING, "%s", "cinter");
    snprintf(out_metadata[OUT_CINTER].standard_name, MAXSTRING, "%s",
             "intermediate_pool_carbon_content");
    snprintf(out_metadata[OUT_CINTER].units, MAXSTRING, "%s", "g m-2");
    snprintf(out_metadata[OUT_CINTER].description, MAXSTRING, "%s",
             "intermediate pool carbon density");

    /* slow pool carbon density [g C/m2] */
    snprintf(out_metadata[OUT_CSLOW].varname, MAXSTRING, "%s", "OUT_CSLOW");
    snprintf(out_metadata[OUT_CSLOW].long_name, MAXSTRING, "%s", "cslow");
    snprintf(out_metadata[OUT_CSLOW].standard_name, MAXSTRING, "%s",
             "slow_pool_carbon_content");
    snprintf(out_metadata[OUT_CSLOW].units, MAXSTRING, "%s", "g m-2");
    snprintf(out_metadata[OUT_CSLOW].description, MAXSTRING, "%s",
             "slow pool carbon density");

    // Band-specific quantities
    /* net sensible heat flux advected to snow pack [W m-2] */
    snprintf(out_metadata[OUT_ADV_SENS_BAND].varname, MAXSTRING, "%s",
             "OUT_ADV_SENS_BAND");
    snprintf(out_metadata[OUT_ADV_SENS_BAND].long_name, MAXSTRING, "%s",
             "adv_sens_band");
    snprintf(out_metadata[OUT_ADV_SENS_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_ADV_SENS].standard_name);
    snprintf(out_metadata[OUT_ADV_SENS_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_ADV_SENS].units);
    snprintf(out_metadata[OUT_ADV_SENS_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_ADV_SENS].description);

    snprintf(out_metadata[OUT_ADV_SENS].varname, MAXSTRING, "%s",
             "OUT_ADV_SENS");
    snprintf(out_metadata[OUT_ADV_SENS].long_name, MAXSTRING, "%s", "adv_sens");
    snprintf(out_metadata[OUT_ADV_SENS].standard_name, MAXSTRING, "%s",
             "net_sensible_heat_flux_to_snow_pack");
    snprintf(out_metadata[OUT_ADV_SENS].units, MAXSTRING, "%s", "W m-2");
    snprintf(out_metadata[OUT_ADV_SENS].description, MAXSTRING, "%s",
             "net sensible heat advected to snow pack");

    /* advected energy [W m-2] */
    snprintf(out_metadata[OUT_ADVECTION_BAND].varname, MAXSTRING, "%s",
             "OUT_ADVECTION_BAND");
    snprintf(out_metadata[OUT_ADVECTION_BAND].long_name, MAXSTRING, "%s",
             "advection_band");
    snprintf(out_metadata[OUT_ADVECTION_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_ADVECTION].standard_name);
    snprintf(out_metadata[OUT_ADVECTION_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_ADVECTION].units);
    snprintf(out_metadata[OUT_ADVECTION_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_ADVECTION].description);

    /* albedo [fraction] */
    snprintf(out_metadata[OUT_ALBEDO_BAND].varname, MAXSTRING, "%s",
             "OUT_ALBEDO_BAND");
    snprintf(out_metadata[OUT_ALBEDO_BAND].long_name, MAXSTRING, "%s",
             "albedo_band");
    snprintf(out_metadata[OUT_ALBEDO_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_ALBEDO].standard_name);
    snprintf(out_metadata[OUT_ALBEDO_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_ALBEDO].units);
    snprintf(out_metadata[OUT_ALBEDO_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_ALBEDO].description);

    /* change in cold content in snow pack [W m-2] */
    snprintf(out_metadata[OUT_DELTACC_BAND].varname, MAXSTRING, "%s",
             "OUT_DELTACC_BAND");
    snprintf(out_metadata[OUT_DELTACC_BAND].long_name, MAXSTRING, "%s",
             "deltacc_band");
    snprintf(out_metadata[OUT_DELTACC_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_DELTACC].standard_name);
    snprintf(out_metadata[OUT_DELTACC_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_DELTACC].units);
    snprintf(out_metadata[OUT_DELTACC_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_DELTACC].description);

    /* net heat flux into ground [W m-2] */
    snprintf(out_metadata[OUT_GRND_FLUX_BAND].varname, MAXSTRING, "%s",
             "OUT_GRND_FLUX_BAND");
    snprintf(out_metadata[OUT_GRND_FLUX_BAND].long_name, MAXSTRING, "%s",
             "grnd_flux_band");
    snprintf(out_metadata[OUT_GRND_FLUX_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_GRND_FLUX].standard_name);
    snprintf(out_metadata[OUT_GRND_FLUX_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_GRND_FLUX].units);
    snprintf(out_metadata[OUT_GRND_FLUX_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_GRND_FLUX].description);

    /* incoming longwave flux at surface (under veg) [W m-2] */
    snprintf(out_metadata[OUT_IN_LONG_BAND].varname, MAXSTRING, "%s",
             "OUT_IN_LONG_BAND");
    snprintf(out_metadata[OUT_IN_LONG_BAND].long_name, MAXSTRING, "%s",
             "in_long_band");
    snprintf(out_metadata[OUT_IN_LONG_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_IN_LONG].standard_name);
    snprintf(out_metadata[OUT_IN_LONG_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_IN_LONG].units);
    snprintf(out_metadata[OUT_IN_LONG_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_IN_LONG].description);

    /* net upward latent heat flux [W m-2] */
    snprintf(out_metadata[OUT_LATENT_BAND].varname, MAXSTRING, "%s",
             "OUT_LATENT_BAND");
    snprintf(out_metadata[OUT_LATENT_BAND].long_name, MAXSTRING, "%s",
             "latent_band");
    snprintf(out_metadata[OUT_LATENT_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_LATENT].standard_name);
    snprintf(out_metadata[OUT_LATENT_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_LATENT].units);
    snprintf(out_metadata[OUT_LATENT_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_LATENT].description);

    /* net upward latent heat flux from sublimation [W m-2] */
    snprintf(out_metadata[OUT_LATENT_SUB_BAND].varname, MAXSTRING, "%s",
             "OUT_LATENT_SUB_BAND");
    snprintf(out_metadata[OUT_LATENT_SUB_BAND].long_name, MAXSTRING, "%s",
             "latent_sub_band");
    snprintf(out_metadata[OUT_LATENT_SUB_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_LATENT_SUB].standard_name);
    snprintf(out_metadata[OUT_LATENT_SUB_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_LATENT_SUB].units);
    snprintf(out_metadata[OUT_LATENT_SUB_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_LATENT_SUB].description);

    /* energy of fusion (melting) [W m-2] */
    snprintf(out_metadata[OUT_MELT_ENERGY_BAND].varname, MAXSTRING, "%s",
             "OUT_MELT_ENERGY_BAND");
    snprintf(out_metadata[OUT_MELT_ENERGY_BAND].long_name, MAXSTRING, "%s",
             "melt_energy_band");
    snprintf(out_metadata[OUT_MELT_ENERGY_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_MELT_ENERGY].standard_name);
    snprintf(out_metadata[OUT_MELT_ENERGY_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_MELT_ENERGY].units);
    snprintf(out_metadata[OUT_MELT_ENERGY_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_MELT_ENERGY].description);

    /* net downward longwave flux [W m-2] */
    snprintf(out_metadata[OUT_LWNET_BAND].varname, MAXSTRING, "%s",
             "OUT_LWNET_BAND");
    snprintf(out_metadata[OUT_LWNET_BAND].long_name, MAXSTRING, "%s",
             "lwnet_band");
    snprintf(out_metadata[OUT_LWNET_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_LWNET].standard_name);
    snprintf(out_metadata[OUT_LWNET_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_LWNET].units);
    snprintf(out_metadata[OUT_LWNET_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_LWNET].description);

    /* net downward shortwave flux [W m-2] */
    snprintf(out_metadata[OUT_SWNET_BAND].varname, MAXSTRING, "%s",
             "OUT_SWNET_BAND");
    snprintf(out_metadata[OUT_SWNET_BAND].long_name, MAXSTRING, "%s",
             "swnet_band");
    snprintf(out_metadata[OUT_SWNET_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_SWNET].standard_name);
    snprintf(out_metadata[OUT_SWNET_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_SWNET].units);
    snprintf(out_metadata[OUT_SWNET_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_SWNET].description);

    /* net energy used to refreeze liquid water in snowpack [W m-2] */
    snprintf(out_metadata[OUT_RFRZ_ENERGY_BAND].varname, MAXSTRING, "%s",
             "OUT_RFRZ_ENERGY_BAND");
    snprintf(out_metadata[OUT_RFRZ_ENERGY_BAND].long_name, MAXSTRING, "%s",
             "rfrz_energy_band");
    snprintf(out_metadata[OUT_RFRZ_ENERGY_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_RFRZ_ENERGY].standard_name);
    snprintf(out_metadata[OUT_RFRZ_ENERGY_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_RFRZ_ENERGY].units);
    snprintf(out_metadata[OUT_RFRZ_ENERGY_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_RFRZ_ENERGY].description);

    /* net upward sensible heat flux [W m-2] */
    snprintf(out_metadata[OUT_SENSIBLE_BAND].varname, MAXSTRING, "%s",
             "OUT_SENSIBLE_BAND");
    snprintf(out_metadata[OUT_SENSIBLE_BAND].long_name, MAXSTRING, "%s",
             "sensible_band");
    snprintf(out_metadata[OUT_SENSIBLE_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_SENSIBLE].standard_name);
    snprintf(out_metadata[OUT_SENSIBLE_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_SENSIBLE].units);
    snprintf(out_metadata[OUT_SENSIBLE_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_SENSIBLE].description);

    /* snow interception storage in canopy [mm] */
    snprintf(out_metadata[OUT_SNOW_CANOPY_BAND].varname, MAXSTRING, "%s",
             "OUT_SNOW_CANOPY_BAND");
    snprintf(out_metadata[OUT_SNOW_CANOPY_BAND].long_name, MAXSTRING, "%s",
             "snow_canopy_band");
    snprintf(out_metadata[OUT_SNOW_CANOPY_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_CANOPY].standard_name);
    snprintf(out_metadata[OUT_SNOW_CANOPY_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_CANOPY].units);
    snprintf(out_metadata[OUT_SNOW_CANOPY_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_CANOPY].description);

    /* fractional area of snow cover [fraction] */
    snprintf(out_metadata[OUT_SNOW_COVER_BAND].varname, MAXSTRING, "%s",
             "OUT_SNOW_COVER_BAND");
    snprintf(out_metadata[OUT_SNOW_COVER_BAND].long_name, MAXSTRING, "%s",
             "snow_cover_band");
    snprintf(out_metadata[OUT_SNOW_COVER_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_COVER].standard_name);
    snprintf(out_metadata[OUT_SNOW_COVER_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_COVER].units);
    snprintf(out_metadata[OUT_SNOW_COVER_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_COVER].description);

    /* depth of snow pack [cm] */
    snprintf(out_metadata[OUT_SNOW_DEPTH_BAND].varname, MAXSTRING, "%s",
             "OUT_SNOW_DEPTH_BAND");
    snprintf(out_metadata[OUT_SNOW_DEPTH_BAND].long_name, MAXSTRING, "%s",
             "snow_depth_band");
    snprintf(out_metadata[OUT_SNOW_DEPTH_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_DEPTH].standard_name);
    snprintf(out_metadata[OUT_SNOW_DEPTH_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_DEPTH].units);
    snprintf(out_metadata[OUT_SNOW_DEPTH_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_DEPTH].description);

    /* energy flux through snow pack [W m-2] */
    snprintf(out_metadata[OUT_SNOW_FLUX_BAND].varname, MAXSTRING, "%s",
             "OUT_SNOW_FLUX_BAND");
    snprintf(out_metadata[OUT_SNOW_FLUX_BAND].long_name, MAXSTRING, "%s",
             "snow_flux_band");
    snprintf(out_metadata[OUT_SNOW_FLUX_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_FLUX].standard_name);
    snprintf(out_metadata[OUT_SNOW_FLUX_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_FLUX].units);
    snprintf(out_metadata[OUT_SNOW_FLUX_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_FLUX].description);

    /* snow melt [mm] */
    snprintf(out_metadata[OUT_SNOW_MELT_BAND].varname, MAXSTRING, "%s",
             "OUT_SNOW_MELT_BAND");
    snprintf(out_metadata[OUT_SNOW_MELT_BAND].long_name, MAXSTRING, "%s",
             "snow_melt_band");
    snprintf(out_metadata[OUT_SNOW_MELT_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_MELT].standard_name);
    snprintf(out_metadata[OUT_SNOW_MELT_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_MELT].units);
    snprintf(out_metadata[OUT_SNOW_MELT_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_MELT].description);

    /* snow pack temperature [C] */
    snprintf(out_metadata[OUT_SNOW_PACKT_BAND].varname, MAXSTRING, "%s",
             "OUT_SNOW_PACKT_BAND");
    snprintf(out_metadata[OUT_SNOW_PACKT_BAND].long_name, MAXSTRING, "%s",
             "snow_packt_band");
    snprintf(out_metadata[OUT_SNOW_PACKT_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_PACK_TEMP].standard_name);
    snprintf(out_metadata[OUT_SNOW_PACKT_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_PACK_TEMP].units);
    snprintf(out_metadata[OUT_SNOW_PACKT_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_PACK_TEMP].description);

    /* snow surface temperature [C] */
    snprintf(out_metadata[OUT_SNOW_SURFT_BAND].varname, MAXSTRING, "%s",
             "OUT_SNOW_SURFT_BAND");
    snprintf(out_metadata[OUT_SNOW_SURFT_BAND].long_name, MAXSTRING, "%s",
             "snow_surft_band");
    snprintf(out_metadata[OUT_SNOW_SURFT_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_SURF_TEMP].standard_name);
    snprintf(out_metadata[OUT_SNOW_SURFT_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_SURF_TEMP].units);
    snprintf(out_metadata[OUT_SNOW_SURFT_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_SNOW_SURF_TEMP].description);

    /* snow water equivalent in snow pack [mm] */
    snprintf(out_metadata[OUT_SWE_BAND].varname, MAXSTRING, "%s",
             "OUT_SWE_BAND");
    snprintf(out_metadata[OUT_SWE_BAND].long_name, MAXSTRING, "%s", "swe_band");
    snprintf(out_metadata[OUT_SWE_BAND].standard_name, MAXSTRING, "%s",
             out_metadata[OUT_SWE].standard_name);
    snprintf(out_metadata[OUT_SWE_BAND].units, MAXSTRING, "%s",
             out_metadata[OUT_SWE].units);
    snprintf(out_metadata[OUT_SWE_BAND].description, MAXSTRING, "%s",
             out_metadata[OUT_SWE].description);

    /* Wall time spent inside vic_run [seconds] */
    snprintf(out_metadata[OUT_TIME_VICRUN_WALL].varname, MAXSTRING, "%s",
             "OUT_TIME_VICRUN_WALL");
    snprintf(out_metadata[OUT_TIME_VICRUN_WALL].long_name, MAXSTRING, "%s",
             "time_vicrun_wall");
    snprintf(out_metadata[OUT_TIME_VICRUN_WALL].standard_name, MAXSTRING, "%s",
             "vic_run_wall_time");
    snprintf(out_metadata[OUT_TIME_VICRUN_WALL].units, MAXSTRING, "%s",
             "seconds");
    snprintf(out_metadata[OUT_TIME_VICRUN_WALL].description, MAXSTRING, "%s",
             "Wall time spent inside vic_run");

    /* CPU time spent inside vic_run [seconds] */
    snprintf(out_metadata[OUT_TIME_VICRUN_CPU].varname, MAXSTRING, "%s",
             "OUT_TIME_VICRUN_CPU");
    snprintf(out_metadata[OUT_TIME_VICRUN_CPU].long_name, MAXSTRING, "%s",
             "time_vicrun_cpu");
    snprintf(out_metadata[OUT_TIME_VICRUN_CPU].standard_name, MAXSTRING, "%s",
             "vic_run_cpu_time");
    snprintf(out_metadata[OUT_TIME_VICRUN_CPU].units, MAXSTRING, "%s",
             "seconds");
    snprintf(out_metadata[OUT_TIME_VICRUN_CPU].description, MAXSTRING, "%s",
             "CPU time spent inside vic_run");

    if (options.FROZEN_SOIL) {
        out_metadata[OUT_FDEPTH].nelem = MAX_FRONTS;
        out_metadata[OUT_TDEPTH].nelem = MAX_FRONTS;
    }

    out_metadata[OUT_SMLIQFRAC].nelem = options.Nlayer;
    out_metadata[OUT_SMFROZFRAC].nelem = options.Nlayer;
    out_metadata[OUT_SOIL_ICE].nelem = options.Nlayer;
    out_metadata[OUT_SOIL_LIQ].nelem = options.Nlayer;
    out_metadata[OUT_SOIL_ICE_FRAC].nelem = options.Nlayer;
    out_metadata[OUT_SOIL_LIQ_FRAC].nelem = options.Nlayer;
    out_metadata[OUT_SOIL_MOIST].nelem = options.Nlayer;
    out_metadata[OUT_SOIL_EFF_SAT].nelem = options.Nlayer;
    out_metadata[OUT_SOIL_TEMP].nelem = options.Nlayer;
    out_metadata[OUT_SOIL_TNODE].nelem = options.Nnode;
    out_metadata[OUT_SOIL_TNODE_WL].nelem = options.Nnode;
    out_metadata[OUT_SOILT_FBFLAG].nelem = options.Nnode;
    out_metadata[OUT_ADV_SENS_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_ADVECTION_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_ALBEDO_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_DELTACC_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_GRND_FLUX_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_IN_LONG_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_LATENT_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_LATENT_SUB_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_MELT_ENERGY_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_LWNET_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_SWNET_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_RFRZ_ENERGY_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_SENSIBLE_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_SNOW_CANOPY_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_SNOW_COVER_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_SNOW_DEPTH_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_SNOW_FLUX_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_SNOW_MELT_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_SNOW_PACKT_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_SNOW_SURFT_BAND].nelem = options.SNOW_BAND;
    out_metadata[OUT_SWE_BAND].nelem = options.SNOW_BAND;
}
