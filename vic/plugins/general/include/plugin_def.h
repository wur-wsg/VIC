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
    RANDOM_DECOMPOSITION,               /**< native VIC (random) decomposition */
    BASIN_DECOMPOSITION,                /**< basin based decomposition */
    FILE_DECOMPOSITION,                 /**< file based decomposition */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_DECOMPOSITION              /**< used as a loop counter*/
};

/******************************************************************************
 * @brief   State Variable Type
 *****************************************************************************/
enum {
    // routing
    STATE_DISCHARGE_DT,                 /**< routing sub-step discharge [m3 s-1] */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_STATE_VARS                 /**< used as a loop counter*/
};

/******************************************************************************
 * @brief   Output Variable Type
 *****************************************************************************/
enum {
    // water balance
    OUT_ROUTING_ERROR,                  /**< routing budget error [mm] */
    // routing
    OUT_STREAM_INFLOW,                  /**< river (inflow) discharge [m3 s-1] */
    OUT_DISCHARGE,                      /**< river (outflow) discharge [m3 s-1] */
    OUT_STREAM_MOIST,                   /**< river (in-cell) stream moisture [mm] */
    OUT_NONREN_DEFICIT,                 /**< non-renewable storage deficit [mm] */
    // land-use
    OUT_CV,                             /**< vegetation coverage fraction [-] */
    // efr
    OUT_EFR_DISCHARGE,                  /**< environmental river discharge [m3 s-1] */
    OUT_EFR_BASEFLOW,                   /**< environmental baseflow [mm] */
    OUT_EFR_MOIST,                      /**< environmental third-layer soil-moisture [mm] */
    // dams
    OUT_LDAM_INFLOW,                    /**< local dam inflow [hm3] */
    OUT_LDAM_DEMAND,                    /**< local dam water demand [hm3] */
    OUT_LDAM_EFR,                       /**< local dam environmental requirements [hm3] */
    OUT_LDAM_RELEASE,                   /**< local dam release [hm3] */
    OUT_LDAM_STORAGE,                   /**< local dam storage [hm3] */
    OUT_LDAM_HIST_INFLOW,               /**< local dam historical inflow for this month [hm3] */
    OUT_LDAM_HIST_DEMAND,               /**< local dam historical demand for this month [hm3] */
    OUT_LDAM_HIST_EFR,                  /**< local dam historical environmental requirements for this month [hm3] */
    OUT_LDAM_OP_RELEASE,                /**< local dam calculated release [hm3] */
    OUT_LDAM_OP_STORAGE,                /**< local dam calculated storage [hm3] */
    OUT_GDAM_INFLOW,                    /**< global dam inflow [hm3] */
    OUT_GDAM_DEMAND,                    /**< global dam water demand [hm3] */
    OUT_GDAM_EFR,                       /**< global dam environmental requirements [hm3] */
    OUT_GDAM_RELEASE,                   /**< global dam release [hm3] */
    OUT_GDAM_STORAGE,                   /**< global dam storage [hm3] */
    OUT_GDAM_HIST_INFLOW,               /**< global dam historical inflow for this month [hm3] */
    OUT_GDAM_HIST_DEMAND,               /**< global dam historical demand for this month [hm3] */
    OUT_GDAM_HIST_EFR,                  /**< global dam historical environmental requirements for this month [hm3] */
    OUT_GDAM_OP_RELEASE,                /**< global dam calculated release [hm3] */
    OUT_GDAM_OP_STORAGE,                /**< global dam calculated storage [hm3] */
    // water-use
    OUT_DE_GW_SECT,                     /**< demand for groundwater resources per sector [mm] */
    OUT_DE_SURF_SECT,                   /**< demand for surface water resources per sector [mm] */
    OUT_DE_NREN_SECT,                   /**< demand for non-renewable resources per sector [mm] */
    OUT_DE_REM_SECT,                    /**< demand for remote resources per sector (from this cell) [mm] */
    OUT_DE_TREM_SECT,                   /**< demand for remote resources per sector (for this cell) [mm] */
    OUT_WI_GW_SECT,                     /**< withdrawn groundwater resources per sector [mm] */
    OUT_WI_SURF_SECT,                   /**< withdrawn surface water resources per sector [mm] */
    OUT_WI_DAM_SECT,                    /**< withdrawn dam reservoir resources per sector [mm] */
    OUT_WI_REM_SECT,                    /**< withdrawn remote resources per sector (from this cell) [mm] */
    OUT_WI_TREM_SECT,                   /**< withdrawn remote resources per sector (for this cell) [mm] */
    OUT_WI_NREN_SECT,                   /**< withdrawn non-renewable resources per sector [mm] */
    OUT_DEMAND,                         /**< demand for water resources [mm] */
    OUT_WITHDRAWN,                      /**< withdrawn water resources [mm] */
    OUT_RETURNED,                       /**< returned water resources [mm] */
    OUT_CONSUMED,                       /**< consumed water resources [mm] */
    // irrigation
    OUT_SHORTAGE,                       /**< average irrigation shortage (below critical soil moisture point) [mm] */
    OUT_REQUIREMENT,                    /**< average irrigation requirement (between field capacity and critical soil moisture point) [mm] */
    OUT_RECEIVED,                       /**< irrigation water received [mm] */
    OUT_LEFTOVER,                       /**< irrigation water leftover [mm] */
    OUT_APPLIED,                        /**< irrigation water applied [mm] */
    // crop
    OUT_CROP_CULTIVATE,                 /**< crop cultivate days [day] */
    OUT_CROP_GROW,                      /**< crop growing days [day] */
    OUT_CROP_EVAP,                      /**< crop total net evaporation [mm] */
    OUT_CROP_EVAP_BARE,                 /**< crop net evaporation from bare soil [mm] */
    OUT_CROP_EVAP_CANOP,                /**< crop net evaporation from canopy interception [mm] */
    OUT_CROP_TRANSP_VEG,                /**< crop net transpiration from vegetation [mm] */
    OUT_CROP_DVS,                       /**< crop development stage [-] */
    OUT_CROP_WLV,                       /**< crop leafs dry matter [kg ha-1] */
    OUT_CROP_WST,                       /**< crop stems dry matter [kg ha-1] */
    OUT_CROP_WSO,                       /**< crop storage organs dry matter [kg ha-1] */
    OUT_CROP_WRT,                       /**< crop roots dry matter [kg ha-1] */
    OUT_CROP_LAI,                       /**< crop leaf area index [m2 m-2] */
    OUT_CROP_NNI,                       /**< crop nitrogen index [kg kg-1] */
    OUT_CROP_PNI,                       /**< crop phosphorous index [kg kg-1] */
    OUT_CROP_KNI,                       /**< crop potassium index [kg kg-1] */
    OUT_CROP_NPKI,                      /**< crop nutrient index [kg kg-1] */
    OUT_CROP_WSTRESS,                   /**< crop water stress [-] */
    OUT_CROP_NSTRESS,                   /**< crop nutrient stress [-] */
    OUT_CROP_NSOIL,                     /**< crop soil nitrogen [kg ha-1] */
    OUT_CROP_PSOIL,                     /**< crop soil phosporous [kg ha-1] */
    OUT_CROP_KSOIL,                     /**< crop soil potassium [kg ha-1] */
    OUT_CROP_NUPT,                      /**< crop nitrogen uptake [kg ha-1] */
    OUT_CROP_PUPT,                      /**< crop phosporous uptake [kg ha-1] */
    OUT_CROP_KUPT,                      /**< crop potassium uptake [kg ha-1] */
    OUT_CROP_NDEM,                      /**< crop nitrogen demand [kg ha-1] */
    OUT_CROP_PDEM,                      /**< crop phosporous demand [kg ha-1] */
    OUT_CROP_KDEM,                      /**< crop potassium demand [kg ha-1] */
    OUT_CROP_ROOTDEPTH,                 /**< crop root depth [cm] */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_OUTVAR_TYPES               /**< used as a loop counter*/
};

/******************************************************************************
 * @brief   Forcing Variable Type
 *****************************************************************************/
enum {
    // routing
    FORCING_DISCHARGE,                  /**< river (inflow) discharge [m3 s-1] */
    // landuse
    FORCING_CV, /**< vegetation coverage [-] */
    // efr
    FORCING_EFR_DISCHARGE,              /**< environmental river discharge [m3 s-1] */
    FORCING_EFR_BASEFLOW,               /**< environmental baseflow [mm] */
    // water-use
    FORCING_PUMPING_CAP,                /**< pumping capacity [mm day-1] */
    FORCING_IRR_DEMAND,                 /**< irrigation demand [mm] */
    FORCING_IRR_GROUNDWATER,            /**< irrigation groundwater fraction [-] */
    FORCING_IRR_CONSUMPTION,            /**< irrigation consumption fraction [-] */
    FORCING_MUN_DEMAND,                 /**< domestic demand [mm] */
    FORCING_MUN_GROUNDWATER,            /**< domestic groundwater fraction [-] */
    FORCING_MUN_CONSUMPTION,            /**< domestic consumption fraction [-] */
    FORCING_LIV_DEMAND,                 /**< livestock demand [mm] */
    FORCING_LIV_GROUNDWATER,            /**< livestock groundwater fraction [-] */
    FORCING_LIV_CONSUMPTION,            /**< livestock consumption fraction [-] */
    FORCING_MAN_DEMAND,                 /**< manufacturing demand [mm] */
    FORCING_MAN_GROUNDWATER,            /**< manufacturing groundwater fraction [-] */
    FORCING_MAN_CONSUMPTION,            /**< manufacturing consumption fraction [-] */
    FORCING_ENE_DEMAND,                 /**< energy demand [mm] */
    FORCING_ENE_GROUNDWATER,            /**< energy groundwater fraction [-] */
    FORCING_ENE_CONSUMPTION,            /**< energy consumption fraction [-] */
    // crops
    FORCING_CO2,                        /**< CO2 concentration [ppm] */
    FORCING_FERT_DVS,                   /**< Fertilizer application DVS point [-] */
    FORCING_FERT_N,                     /**< Fertilizer application N amount [kg ha-1] */
    FORCING_FERT_P,                     /**< Fertilizer application P amount [kg ha-1] */
    FORCING_FERT_K,                     /**< Fertilizer application K amount [kg ha-1] */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_FORCING_TYPES              /**< used as a loop counter*/
};

/******************************************************************************
 * @brief   Forcing Variable Frequency
 *****************************************************************************/
enum
{
    FORCE_STEP,                         /**< model step forcing frequency */
    FORCE_DAY,                          /**< daily forcing frequency */
    FORCE_MONTH,                        /**< monthly forcing frequency */
    FORCE_YEAR,                         /**< yearly forcing frequency */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_FORCE_FREQS                /**< Number of force frequencies */
};

/******************************************************************************
 * @brief   Water-use Sectors
 *****************************************************************************/
enum {
    WU_MUNICIPAL,                   /**< domestic sector */
    WU_ENERGY,                      /**< energy sector */
    WU_MANUFACTURING,               /**< manufacturing sector */
    WU_LIVESTOCK,                   /**< livestock sector */
    WU_IRRIGATION,                  /**< irrigation sector */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    WU_NSECTORS                     /**< used as a loop counter*/
};

/******************************************************************************
 * @brief   Plugin options
 *****************************************************************************/
typedef struct {
    short unsigned int DECOMPOSITION;   /**< domain decomposition type */
    bool ROUTING;                       /**< routing module flag */
    bool EFR;                           /**< environmental flow module flag */
    bool DAMS;                          /**< dam module flag */
    bool WATERUSE;                      /**< water-use module flag */
    bool IRRIGATION;                    /**< irrigation module flag */
    bool WOFOST;                        /**< wofost module flag */

    short unsigned int UH_LENGTH;       /**< routing sub-step (unit-hydrograph) length */
    bool FORCE_ROUTING;                 /**< routing (inflow) forcing flag */
    bool FORCE_LANDUSE; /**< landuse forcing flag */
    short unsigned int NDAMTYPES;       /**< maximum number of dams per cell */
    short unsigned int NDAMSERVICE;     /**< maximum number of dam service per dam */
    short unsigned int NWUTYPES;        /**< number of water-use sectors */
    short unsigned int NWURECEIVING;    /**< maximum number of remote water-users per cell */
    short unsigned int WU_INPUT[WU_NSECTORS];   /**< water-use input location */
    size_t NIRRTYPES;                   /**< maximum number irrigated vegetation types */
    bool POTENTIAL_IRRIGATION;          /**< potential irrigation flag */
    bool FORCE_PUMP_CAP;                /**< pumping capacity forcing flag */
    bool COMP_WITH;                     /**< compensation water abstractions flag */
    bool REMOTE_WITH;                   /**< remote water abstractions flag */
    bool NONRENEW_WITH;                 /**< non-renewable water abstractions flag */
    bool NONRENEW_RUNOFF;               /**< non-renewable water abstractions flag */
    bool WOFOST_PIRR;                   /**< potential irrigation for wofost module flag */
    bool WOFOST_PFERT;                  /**< potential fertilization for wofost module flag */
    bool WOFOST_DIST_SEASON;            /**< distributed seasons for wofost module flag */
    bool WOFOST_DIST_TSUM;              /**< distributed tsums for wofost module flag */
    bool WOFOST_DIST_FERT;              /**< distributed fertilization for wofost module flag */
    bool WOFOST_DIST_MIN;               /**< distributed mineralization for wofost module flag */
    bool WOFOST_CALC_MIN;               /**< calculated mineralization for wofost module flag */
    bool WOFOST_CONTINUE;               /**< continue on end land-use for wofost module flag */
    bool WOFOST_FORCE_FERT;             /**< distributed fertilizer forcing for wofost module flag */
    short unsigned int NCROPTYPES;      /**< number of crop types */
    short unsigned int NFERTTIMES;      /**< number of fertilizer occations */
} plugin_option_struct;

/******************************************************************************
 * @brief   Plugin global parameters
 *****************************************************************************/
typedef struct {
    size_t rout_steps_per_day;          /**< number of routing steps [d-1] */
    double rout_dt;                     /**< routing time step [s] */
    size_t wofost_steps_per_day;        /**< number of wofost steps [d-1] */
    double wofost_dt;                   /**< wofost time step [s] */
    size_t force_steps_per_year[PLUGIN_N_FORCING_TYPES];        /**< number of forcing steps [d-1] */
    double force_dt[PLUGIN_N_FORCING_TYPES];                    /**< forcing time step [s] */
    unsigned int forcesec[PLUGIN_N_FORCING_TYPES];              /**< seconds since midnight when forcing files starts */
    unsigned short int forceday[PLUGIN_N_FORCING_TYPES];        /**< day forcing files starts */
    unsigned short int forcemonth[PLUGIN_N_FORCING_TYPES];      /**< month forcing files starts */
    unsigned short int forceyear[PLUGIN_N_FORCING_TYPES];       /**< year forcing files start */
    unsigned short int forcefreq[PLUGIN_N_FORCING_TYPES];       /**< forcing frequency */
    unsigned short int forceoffset[PLUGIN_N_FORCING_TYPES];     /**< counter to keep track of offset in reading forcing files; updated after every read */
    bool forcerun[PLUGIN_N_FORCING_TYPES];                      /**< flag to keep track wether to read forcing variable this time step */
    unsigned int forceskip[PLUGIN_N_FORCING_TYPES];             /**< number of model time steps to skip at the start of the forcing file */
} plugin_global_param_struct;

/******************************************************************************
 * @brief   Plugin parameters
 *****************************************************************************/
typedef struct {
    double DAM_ALPHA;                   /**< dam preferred maximum storage fraction [-] */
    double DAM_BETA;                    /**< dam correction exponent [-] */
    double DAM_GAMMA;                   /**< dam correction period [d-1] */
    double NREN_LIM;                    /**< non-renewable withdrawal limit [mm] */
    double Wfc_fract;                   /**< field capacity fraction (of critical soil moisture) [-] */
    double Ksat_expt;                   /**< paddy saturated irrigation conductivity exponent [-] */
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
void plugin_force_start(void);
void plugin_force_init(void);
void plugin_get_forcing_file_info(short unsigned int);
void plugin_get_forcing_file_skip(short unsigned int);
void plugin_start_forcing(void);
void plugin_end_forcing(void);
void plugin_update_step_vars(void);
void plugin_run(void);

#endif /* PLUGIN_H */
