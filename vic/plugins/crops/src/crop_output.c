/******************************************************************************
 * @section DESCRIPTION
 *
 * Crop output functions
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
* @brief   Set output metadata
******************************************/
void
crop_set_output_met_data_info(void)
{
    extern metadata_struct out_metadata[];

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_CULTIVATE].varname,
             MAXSTRING,
             "%s", "OUT_CROP_CULTIVATE");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_CULTIVATE].long_name,
             MAXSTRING,
             "%s", "cultivation_days");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_CULTIVATE].standard_name,
             MAXSTRING, "%s", "crop_cultivation_days");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_CULTIVATE].units, MAXSTRING,
             "%s", "days");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_CULTIVATE].description,
             MAXSTRING, "%s", "crop cultivation days");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_GROW].varname, MAXSTRING,
             "%s", "OUT_CROP_GROW");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_GROW].long_name, MAXSTRING,
             "%s", "growing_days");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_GROW].standard_name,
             MAXSTRING, "%s", "crop_growing_days");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_GROW].units, MAXSTRING,
             "%s", "days");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_GROW].description,
             MAXSTRING, "%s", "crop growing days");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP].varname, MAXSTRING,
             "%s", "OUT_CROP_EVAP");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP].long_name, MAXSTRING,
             "%s", "evapotranspiration");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP].standard_name,
             MAXSTRING, "%s", "crop_evapotranspiration");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP].units, MAXSTRING,
             "%s", "days");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP].description,
             MAXSTRING, "%s",
             "crop evapotranspiration (transpiration + bare soil evaporation + canopy evaporation)");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP_BARE].varname,
             MAXSTRING,
             "%s", "OUT_CROP_EVAP_BARE");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP_BARE].long_name,
             MAXSTRING,
             "%s", "bare_evaporation");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP_BARE].standard_name,
             MAXSTRING, "%s", "crop_bare_evaporation");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP_BARE].units, MAXSTRING,
             "%s", "days");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP_BARE].description,
             MAXSTRING, "%s", "crop bare soil evaporation");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP_CANOP].varname,
             MAXSTRING,
             "%s", "OUT_CROP_EVAP_CANOP");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP_CANOP].long_name,
             MAXSTRING,
             "%s", "canopy_evaporation");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP_CANOP].standard_name,
             MAXSTRING, "%s", "crop_canopy_evaporation");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP_CANOP].units,
             MAXSTRING,
             "%s", "days");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_EVAP_CANOP].description,
             MAXSTRING, "%s", "crop canopy evaporation");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_TRANSP_VEG].varname,
             MAXSTRING,
             "%s", "OUT_CROP_TRANSP_VEG");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_TRANSP_VEG].long_name,
             MAXSTRING,
             "%s", "transpiration");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_TRANSP_VEG].standard_name,
             MAXSTRING, "%s", "crop_transpiration");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_TRANSP_VEG].units,
             MAXSTRING,
             "%s", "days");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_TRANSP_VEG].description,
             MAXSTRING, "%s", "crop transpiration");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_DVS].varname, MAXSTRING,
             "%s", "OUT_CROP_DVS");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_DVS].long_name, MAXSTRING,
             "%s", "development_stage");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_DVS].standard_name,
             MAXSTRING, "%s", "crop_development_stage");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_DVS].units, MAXSTRING,
             "%s", "-");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_DVS].description,
             MAXSTRING, "%s", "0 = emergence, 1 = flowering, 2 = maturity");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WLV].varname, MAXSTRING,
             "%s", "OUT_CROP_WLV");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WLV].long_name, MAXSTRING,
             "%s", "leaf_drymatter");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WLV].standard_name,
             MAXSTRING, "%s", "crop_leaf_drymatter");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WLV].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WLV].description,
             MAXSTRING, "%s", "crop leaf dry matter");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WST].varname, MAXSTRING,
             "%s", "OUT_CROP_WST");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WST].long_name, MAXSTRING,
             "%s", "stem_drymatter");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WST].standard_name,
             MAXSTRING, "%s", "crop_stem_drymatter");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WST].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WST].description,
             MAXSTRING, "%s", "crop stem dry matter");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WSO].varname, MAXSTRING,
             "%s", "OUT_CROP_WSO");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WSO].long_name, MAXSTRING,
             "%s", "storage_drymatter");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WSO].standard_name,
             MAXSTRING, "%s", "crop_storage_drymatter");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WSO].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WSO].description,
             MAXSTRING, "%s", "crop storage organ dry matter");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WRT].varname, MAXSTRING,
             "%s", "OUT_CROP_WRT");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WRT].long_name, MAXSTRING,
             "%s", "root_drymatter");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WRT].standard_name,
             MAXSTRING, "%s", "crop_root_drymatter");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WRT].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WRT].description,
             MAXSTRING, "%s", "crop root dry matter");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_LAI].varname, MAXSTRING,
             "%s", "OUT_CROP_LAI");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_LAI].long_name, MAXSTRING,
             "%s", "lai");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_LAI].standard_name,
             MAXSTRING, "%s", "crop_lai");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_LAI].units, MAXSTRING,
             "%s", "m2 m-2");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_LAI].description,
             MAXSTRING, "%s", "crop leaf area index");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WSTRESS].varname, MAXSTRING,
             "%s", "OUT_CROP_WSTRESS");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WSTRESS].long_name,
             MAXSTRING,
             "%s", "water_stress");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WSTRESS].standard_name,
             MAXSTRING, "%s", "crop_water_stress");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WSTRESS].units, MAXSTRING,
             "%s", "-");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_WSTRESS].description,
             MAXSTRING, "%s", "crop water stress");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NSTRESS].varname, MAXSTRING,
             "%s", "OUT_CROP_NSTRESS");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NSTRESS].long_name,
             MAXSTRING,
             "%s", "nutrient_stress");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NSTRESS].standard_name,
             MAXSTRING, "%s", "crop_nutrient_stress");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NSTRESS].units, MAXSTRING,
             "%s", "-");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NSTRESS].description,
             MAXSTRING, "%s", "crop nutrient stress");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NPKI].varname, MAXSTRING,
             "%s", "OUT_CROP_NPKI");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NPKI].long_name, MAXSTRING,
             "%s", "nutrient_index");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NPKI].standard_name,
             MAXSTRING, "%s", "crop_nutrient_index");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NPKI].units, MAXSTRING,
             "%s", "kg kg-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NPKI].description,
             MAXSTRING, "%s", "crop nutrient index");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NNI].varname, MAXSTRING,
             "%s", "OUT_CROP_NNI");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NNI].long_name, MAXSTRING,
             "%s", "N_index");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NNI].standard_name,
             MAXSTRING, "%s", "crop_N_index");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NNI].units, MAXSTRING,
             "%s", "kg kg-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NNI].description,
             MAXSTRING, "%s", "crop natrium nutrient index");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NSOIL].varname, MAXSTRING,
             "%s", "OUT_CROP_NSOIL");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NSOIL].long_name, MAXSTRING,
             "%s", "N_soil");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NSOIL].standard_name,
             MAXSTRING, "%s", "crop_N_soil");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NSOIL].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NSOIL].description,
             MAXSTRING, "%s", "crop natrium available in the soil");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NUPT].varname, MAXSTRING,
             "%s", "OUT_CROP_NUPT");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NUPT].long_name, MAXSTRING,
             "%s", "N_uptake");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NUPT].standard_name,
             MAXSTRING, "%s", "crop_N_uptake");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NUPT].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NUPT].description,
             MAXSTRING, "%s", "crop natrium uptake");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NDEM].varname, MAXSTRING,
             "%s", "OUT_CROP_NDEM");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NDEM].long_name, MAXSTRING,
             "%s", "N_demand");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NDEM].standard_name,
             MAXSTRING, "%s", "crop_N_demand");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NDEM].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_NDEM].description,
             MAXSTRING, "%s", "crop natrium demand");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PNI].varname, MAXSTRING,
             "%s", "OUT_CROP_PNI");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PNI].long_name, MAXSTRING,
             "%s", "P_index");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PNI].standard_name,
             MAXSTRING, "%s", "crop_P_index");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PNI].units, MAXSTRING,
             "%s", "kg kg-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PNI].description,
             MAXSTRING, "%s", "crop phosporous nutrient index");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PSOIL].varname, MAXSTRING,
             "%s", "OUT_CROP_PSOIL");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PSOIL].long_name, MAXSTRING,
             "%s", "P_soil");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PSOIL].standard_name,
             MAXSTRING, "%s", "crop_P_soil");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PSOIL].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PSOIL].description,
             MAXSTRING, "%s", "crop phosporous available in the soil");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PUPT].varname, MAXSTRING,
             "%s", "OUT_CROP_PUPT");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PUPT].long_name, MAXSTRING,
             "%s", "P_uptake");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PUPT].standard_name,
             MAXSTRING, "%s", "crop_P_uptake");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PUPT].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PUPT].description,
             MAXSTRING, "%s", "crop phosporous uptake");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PDEM].varname, MAXSTRING,
             "%s", "OUT_CROP_PDEM");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PDEM].long_name, MAXSTRING,
             "%s", "P_demand");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PDEM].standard_name,
             MAXSTRING, "%s", "crop_P_demand");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PDEM].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_PDEM].description,
             MAXSTRING, "%s", "crop phosporous demand");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KNI].varname, MAXSTRING,
             "%s", "OUT_CROP_KNI");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KNI].long_name, MAXSTRING,
             "%s", "K_index");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KNI].standard_name,
             MAXSTRING, "%s", "crop_K_index");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KNI].units, MAXSTRING,
             "%s", "kg kg-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KNI].description,
             MAXSTRING, "%s", "crop potassium nutrient index");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KSOIL].varname, MAXSTRING,
             "%s", "OUT_CROP_KSOIL");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KSOIL].long_name, MAXSTRING,
             "%s", "K_soil");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KSOIL].standard_name,
             MAXSTRING, "%s", "crop_K_soil");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KSOIL].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KSOIL].description,
             MAXSTRING, "%s", "crop potassium available in the soil");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KUPT].varname, MAXSTRING,
             "%s", "OUT_CROP_KUPT");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KUPT].long_name, MAXSTRING,
             "%s", "K_uptake");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KUPT].standard_name,
             MAXSTRING, "%s", "crop_K_uptake");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KUPT].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KUPT].description,
             MAXSTRING, "%s", "crop potassium uptake");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KDEM].varname, MAXSTRING,
             "%s", "OUT_CROP_KDEM");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KDEM].long_name, MAXSTRING,
             "%s", "K_demand");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KDEM].standard_name,
             MAXSTRING, "%s", "crop_K_demand");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KDEM].units, MAXSTRING,
             "%s", "kg ha-1");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_KDEM].description,
             MAXSTRING, "%s", "crop potassium demand");

    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_ROOTDEPTH].varname,
             MAXSTRING,
             "%s", "OUT_CROP_ROOTDEPTH");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_ROOTDEPTH].long_name,
             MAXSTRING,
             "%s", "root_depth");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_ROOTDEPTH].standard_name,
             MAXSTRING, "%s", "crop_root_depth");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_ROOTDEPTH].units, MAXSTRING,
             "%s", "cm");
    snprintf(out_metadata[N_OUTVAR_TYPES + OUT_CROP_ROOTDEPTH].description,
             MAXSTRING, "%s", "crop root depth");

    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_CULTIVATE].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_GROW].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_EVAP].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_EVAP_BARE].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_EVAP_CANOP].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_TRANSP_VEG].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_DVS].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_WLV].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_WST].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_WSO].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_WRT].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_LAI].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_NNI].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_PNI].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_KNI].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_NPKI].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_WSTRESS].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_NSTRESS].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_NSOIL].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_PSOIL].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_KSOIL].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_NUPT].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_PUPT].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_KUPT].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_NDEM].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_PDEM].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_KDEM].nelem = plugin_options.NCROPTYPES;
    out_metadata[N_OUTVAR_TYPES +
                 OUT_CROP_ROOTDEPTH].nelem = plugin_options.NCROPTYPES;
}

/******************************************
* @brief   Initialize output dimensions
******************************************/
void
crop_initialize_nc_file(nc_file_struct *nc_state_file)
{
    extern plugin_option_struct plugin_options;

    nc_state_file->crop_dimid = MISSING;
    nc_state_file->crop_size = plugin_options.NCROPTYPES;
}

/******************************************
* @brief   Set output dimensions
******************************************/
void
crop_add_hist_dim(nc_file_struct *nc,
                  stream_struct  *stream)
{
    int status;

    status = nc_def_dim(nc->nc_id, "crop_class", nc->crop_size,
                        &(nc->crop_dimid));
    check_nc_status(status, "Error defining crop_class bounds dimension in %s",
                    stream->filename);
}

/******************************************
* @brief   Set output variable dimension sizes
******************************************/
void
crop_set_nc_var_info(unsigned int    varid,
                     nc_file_struct *nc_hist_file,
                     nc_var_struct  *nc_var)
{
    // Set the number of dimensions and dimids for each state variable
    switch (varid) {
    case N_OUTVAR_TYPES + OUT_CROP_CULTIVATE:
    case N_OUTVAR_TYPES + OUT_CROP_GROW:
    case N_OUTVAR_TYPES + OUT_CROP_EVAP:
    case N_OUTVAR_TYPES + OUT_CROP_EVAP_BARE:
    case N_OUTVAR_TYPES + OUT_CROP_EVAP_CANOP:
    case N_OUTVAR_TYPES + OUT_CROP_TRANSP_VEG:
    case N_OUTVAR_TYPES + OUT_CROP_DVS:
    case N_OUTVAR_TYPES + OUT_CROP_WLV:
    case N_OUTVAR_TYPES + OUT_CROP_WST:
    case N_OUTVAR_TYPES + OUT_CROP_WSO:
    case N_OUTVAR_TYPES + OUT_CROP_WRT:
    case N_OUTVAR_TYPES + OUT_CROP_LAI:
    case N_OUTVAR_TYPES + OUT_CROP_NNI:
    case N_OUTVAR_TYPES + OUT_CROP_PNI:
    case N_OUTVAR_TYPES + OUT_CROP_KNI:
    case N_OUTVAR_TYPES + OUT_CROP_NPKI:
    case N_OUTVAR_TYPES + OUT_CROP_WSTRESS:
    case N_OUTVAR_TYPES + OUT_CROP_NSTRESS:
    case N_OUTVAR_TYPES + OUT_CROP_NSOIL:
    case N_OUTVAR_TYPES + OUT_CROP_PSOIL:
    case N_OUTVAR_TYPES + OUT_CROP_KSOIL:
    case N_OUTVAR_TYPES + OUT_CROP_NUPT:
    case N_OUTVAR_TYPES + OUT_CROP_PUPT:
    case N_OUTVAR_TYPES + OUT_CROP_KUPT:
    case N_OUTVAR_TYPES + OUT_CROP_NDEM:
    case N_OUTVAR_TYPES + OUT_CROP_PDEM:
    case N_OUTVAR_TYPES + OUT_CROP_KDEM:
    case N_OUTVAR_TYPES + OUT_CROP_ROOTDEPTH:
        nc_var->nc_dims = 4;
        nc_var->nc_counts[1] = nc_hist_file->crop_size;
        nc_var->nc_counts[2] = nc_hist_file->nj_size;
        nc_var->nc_counts[3] = nc_hist_file->ni_size;
        break;
    }
}

/******************************************
* @brief   Set output variable dimension ids
******************************************/
void
crop_set_nc_var_dimids(unsigned int    varid,
                       nc_file_struct *nc_hist_file,
                       nc_var_struct  *nc_var)
{
    switch (varid) {
    case N_OUTVAR_TYPES + OUT_CROP_CULTIVATE:
    case N_OUTVAR_TYPES + OUT_CROP_GROW:
    case N_OUTVAR_TYPES + OUT_CROP_EVAP:
    case N_OUTVAR_TYPES + OUT_CROP_EVAP_BARE:
    case N_OUTVAR_TYPES + OUT_CROP_EVAP_CANOP:
    case N_OUTVAR_TYPES + OUT_CROP_TRANSP_VEG:
    case N_OUTVAR_TYPES + OUT_CROP_DVS:
    case N_OUTVAR_TYPES + OUT_CROP_WLV:
    case N_OUTVAR_TYPES + OUT_CROP_WST:
    case N_OUTVAR_TYPES + OUT_CROP_WSO:
    case N_OUTVAR_TYPES + OUT_CROP_WRT:
    case N_OUTVAR_TYPES + OUT_CROP_LAI:
    case N_OUTVAR_TYPES + OUT_CROP_NNI:
    case N_OUTVAR_TYPES + OUT_CROP_PNI:
    case N_OUTVAR_TYPES + OUT_CROP_KNI:
    case N_OUTVAR_TYPES + OUT_CROP_NPKI:
    case N_OUTVAR_TYPES + OUT_CROP_WSTRESS:
    case N_OUTVAR_TYPES + OUT_CROP_NSTRESS:
    case N_OUTVAR_TYPES + OUT_CROP_NSOIL:
    case N_OUTVAR_TYPES + OUT_CROP_PSOIL:
    case N_OUTVAR_TYPES + OUT_CROP_KSOIL:
    case N_OUTVAR_TYPES + OUT_CROP_NUPT:
    case N_OUTVAR_TYPES + OUT_CROP_PUPT:
    case N_OUTVAR_TYPES + OUT_CROP_KUPT:
    case N_OUTVAR_TYPES + OUT_CROP_NDEM:
    case N_OUTVAR_TYPES + OUT_CROP_PDEM:
    case N_OUTVAR_TYPES + OUT_CROP_KDEM:
    case N_OUTVAR_TYPES + OUT_CROP_ROOTDEPTH:
        nc_var->nc_dimids[0] = nc_hist_file->time_dimid;
        nc_var->nc_dimids[1] = nc_hist_file->crop_dimid;
        nc_var->nc_dimids[2] = nc_hist_file->nj_dimid;
        nc_var->nc_dimids[3] = nc_hist_file->ni_dimid;
        break;
    }
}

/******************************************
* @brief   Set output aggregation types
******************************************/
void
crop_history(int           varid,
             unsigned int *agg_type)
{
    switch (varid) {
    case N_OUTVAR_TYPES + OUT_CROP_CULTIVATE:
    case  N_OUTVAR_TYPES + OUT_CROP_GROW:
    case  N_OUTVAR_TYPES + OUT_CROP_WSTRESS:
    case  N_OUTVAR_TYPES + OUT_CROP_NSTRESS:
    case  N_OUTVAR_TYPES + OUT_CROP_NNI:
    case  N_OUTVAR_TYPES + OUT_CROP_PNI:
    case  N_OUTVAR_TYPES + OUT_CROP_KNI:
    case  N_OUTVAR_TYPES + OUT_CROP_NPKI:
    case  N_OUTVAR_TYPES + OUT_CROP_EVAP:
    case  N_OUTVAR_TYPES + OUT_CROP_EVAP_BARE:
    case  N_OUTVAR_TYPES + OUT_CROP_EVAP_CANOP:
    case  N_OUTVAR_TYPES + OUT_CROP_TRANSP_VEG:
    case  N_OUTVAR_TYPES + OUT_CROP_DVS:
    case  N_OUTVAR_TYPES + OUT_CROP_WLV:
    case  N_OUTVAR_TYPES + OUT_CROP_WST:
    case  N_OUTVAR_TYPES + OUT_CROP_WSO:
    case  N_OUTVAR_TYPES + OUT_CROP_WRT:
    case  N_OUTVAR_TYPES + OUT_CROP_NSOIL:
    case  N_OUTVAR_TYPES + OUT_CROP_PSOIL:
    case  N_OUTVAR_TYPES + OUT_CROP_KSOIL:
    case  N_OUTVAR_TYPES + OUT_CROP_NUPT:
    case  N_OUTVAR_TYPES + OUT_CROP_PUPT:
    case  N_OUTVAR_TYPES + OUT_CROP_KUPT:
    case  N_OUTVAR_TYPES + OUT_CROP_NDEM:
    case  N_OUTVAR_TYPES + OUT_CROP_PDEM:
    case  N_OUTVAR_TYPES + OUT_CROP_KDEM:
    case  N_OUTVAR_TYPES + OUT_CROP_ROOTDEPTH:
        (*agg_type) = AGG_TYPE_SUM;
        break;
    case  N_OUTVAR_TYPES + OUT_CROP_LAI:
        (*agg_type) = AGG_TYPE_MAX;
        break;
    }
}

/******************************************
* @brief   Write output
******************************************/
void
crop_put_data(size_t iCell)
{
    extern option_struct       options;
    extern SimUnit          ***Grid;
    extern double           ***out_data;
    extern soil_con_struct    *soil_con;
    extern all_vars_struct    *all_vars;
    extern veg_con_map_struct *veg_con_map;
    extern dmy_struct         *dmy;
    extern size_t              current;

    double                     area_fract;
    double                     cc_fract;
    float                      tmp_develop;

    size_t                     iBand;
    size_t                     iVeg;
    size_t                     iLayer;
    size_t                     crop_class;
    size_t                     veg_class;

    SimUnit                   *cgrid;

    /* Store VIC crop fluxes */
    for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
        area_fract = soil_con[iCell].AreaFract[iBand];
        cgrid = Grid[iCell][iBand];

        while (cgrid) {
            crop_class = cgrid->met->crop_class;
            veg_class = crop_con_map[iCell].veg_class[crop_class];
            iVeg = veg_con_map[iCell].vidx[veg_class];
            cc_fract =
                crop_con_map[iCell].Cc[crop_class][dmy[current].month - 1];

            for (iLayer = 0; iLayer < options.Nlayer; iLayer++) {
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_EVAP_BARE][crop_class] +=
                    all_vars[iCell].cell[iVeg][iBand].layer[iLayer].esoil *
                    area_fract * cc_fract * cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_TRANSP_VEG][crop_class] +=
                    all_vars[iCell].cell[iVeg][iBand].layer[iLayer].transp *
                    area_fract * cc_fract * cgrid->growing;
            }
            out_data[iCell][N_OUTVAR_TYPES + OUT_CROP_EVAP_CANOP][crop_class] +=
                all_vars[iCell].veg_var[iVeg][iBand].canopyevap *
                area_fract * cc_fract * cgrid->growing;
            out_data[iCell][N_OUTVAR_TYPES + OUT_CROP_EVAP][crop_class] +=
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_EVAP_BARE][crop_class] +
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_TRANSP_VEG][crop_class] +
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_EVAP_CANOP][crop_class];

            cgrid = cgrid->next;
        }
    }

    if (crop_run_flag()) {
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            area_fract = soil_con[iCell].AreaFract[iBand];
            cgrid = Grid[iCell][iBand];

            while (cgrid) {
                crop_class = cgrid->met->crop_class;

                /* Store crop fluxes */
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_CULTIVATE][crop_class] +=
                    cgrid->cultivating *
                    area_fract;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_GROW][crop_class] += cgrid->growing *
                                                              area_fract;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_DVS][crop_class] +=
                    cgrid->crp->rt.Development *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_WLV][crop_class] +=
                    cgrid->crp->rt.leaves *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_WST][crop_class] +=
                    cgrid->crp->rt.stems * area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_WSO][crop_class] +=
                    cgrid->crp->rt.storage *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_WRT][crop_class] +=
                    cgrid->crp->rt.roots * area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_NSOIL][crop_class] +=
                    cgrid->ste->rt_N_tot *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_PSOIL][crop_class] +=
                    cgrid->ste->rt_P_tot *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_KSOIL][crop_class] +=
                    cgrid->ste->rt_K_tot *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_NUPT][crop_class] +=
                    cgrid->crp->N_rt.Uptake *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_PUPT][crop_class] +=
                    cgrid->crp->P_rt.Uptake *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_KUPT][crop_class] +=
                    cgrid->crp->K_rt.Uptake *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_NDEM][crop_class] +=
                    cgrid->crp->N_rt.Demand *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_PDEM][crop_class] +=
                    cgrid->crp->P_rt.Demand *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_KDEM][crop_class] +=
                    cgrid->crp->K_rt.Demand *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_ROOTDEPTH][crop_class] +=
                    cgrid->crp->rt.RootDepth *
                    area_fract *
                    cgrid->growing;

                /* Add initial crop rates (GrowthDay starts at 1) */
                if (cgrid->crp->GrowthDay == 2) {
                    tmp_develop = cgrid->crp->st.Development -
                                  cgrid->crp->rt.Development;

                    out_data[iCell][N_OUTVAR_TYPES +
                                    OUT_CROP_DVS][crop_class] +=
                        cgrid->crp->prm.InitialDVS *
                        area_fract
                        * cgrid->
                        growing;
                    out_data[iCell][N_OUTVAR_TYPES +
                                    OUT_CROP_WLV][crop_class] +=
                        cgrid->crp->prm.InitialDryWeight *
                        (1 - Afgen(cgrid->crp->prm.Roots, &tmp_develop)) *
                        Afgen(cgrid->crp->prm.Leaves, &tmp_develop) *
                        area_fract * cgrid->growing;
                    out_data[iCell][N_OUTVAR_TYPES +
                                    OUT_CROP_WST][crop_class] +=
                        cgrid->crp->prm.InitialDryWeight *
                        (1 - Afgen(cgrid->crp->prm.Roots, &tmp_develop)) *
                        Afgen(cgrid->crp->prm.Stems, &tmp_develop) *
                        area_fract * cgrid->growing;
                    out_data[iCell][N_OUTVAR_TYPES +
                                    OUT_CROP_WSO][crop_class] +=
                        cgrid->crp->prm.InitialDryWeight *
                        (1 - Afgen(cgrid->crp->prm.Roots, &tmp_develop)) *
                        Afgen(cgrid->crp->prm.Storage, &tmp_develop) *
                        area_fract * cgrid->growing;
                    out_data[iCell][N_OUTVAR_TYPES +
                                    OUT_CROP_WRT][crop_class] +=
                        cgrid->crp->prm.InitialDryWeight *Afgen(
                            cgrid->crp->prm.Roots, &tmp_develop) *
                        area_fract * cgrid->growing;

                    out_data[iCell][N_OUTVAR_TYPES +
                                    OUT_CROP_NSOIL][crop_class] +=
                        List(cgrid->mng->N_Fert_table) *
                        cgrid->mng->N_Uptake_frac *
                        area_fract * cgrid->growing;
                    out_data[iCell][N_OUTVAR_TYPES +
                                    OUT_CROP_PSOIL][crop_class] +=
                        List(cgrid->mng->P_Fert_table) *
                        cgrid->mng->P_Uptake_frac *
                        area_fract * cgrid->growing;
                    out_data[iCell][N_OUTVAR_TYPES +
                                    OUT_CROP_KSOIL][crop_class] +=
                        List(cgrid->mng->K_Fert_table) *
                        cgrid->mng->K_Uptake_frac *
                        area_fract * cgrid->growing;
                }

                /* Store crop states - divide through number of growing days for average */
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_LAI][crop_class] +=
                    cgrid->crp->st.LAI * area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_NNI][crop_class] +=
                    cgrid->crp->N_st.Indx *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_PNI][crop_class] +=
                    cgrid->crp->P_st.Indx *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_KNI][crop_class] +=
                    cgrid->crp->K_st.Indx *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_NPKI][crop_class] +=
                    cgrid->crp->NPK_Indx *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_WSTRESS][crop_class] +=
                    cgrid->soil->WaterStress *
                    area_fract *
                    cgrid->growing;
                out_data[iCell][N_OUTVAR_TYPES +
                                OUT_CROP_NSTRESS][crop_class] +=
                    cgrid->crp->NutrientStress *
                    area_fract *
                    cgrid->growing;

                cgrid = cgrid->next;
            }
        }
    }
}
