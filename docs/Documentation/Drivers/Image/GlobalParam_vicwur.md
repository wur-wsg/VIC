# VIC-WUR Run-Time Options - Global Parameter File

The global parameter file serves two main purposes:

1.  Tells VIC-WUR the names, locations, and formats of input and output files
2.  Defines global parameters of the simulation (known as _run-time_ options)

The order of the options in the global parameter file is not important, but the complete option name must be followed by the required option type information. To help in understanding this file, an example file has been attached at the bottom of this page.

# Define Simulation Parameters

The following options determine the type of simulation that will be performed.

## Main Simulation Parameters

| Name                 | Type    | Units  | Description                                                                                                                                                                                      |
|----------------------|---------|--------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| NLAYER               | integer | N/A    | Number of   moisture layers used by the model                                                                                                                                                    |
| NODES                | integer | N/A    | Number of   thermal solution nodes in the soil column                                                                                                                                            |
| MODEL_STEPS_PER_DAY  | integer | steps  | Number of   simulation time steps per day. NOTE: MODEL_STEPS_PER_DAY should be > 4 for   FULL_ENERGY=TRUE or FROZEN_SOIL=TRUE.                                                                   |
| SNOW_STEPS_PER_DAY   | integer | steps  | Number of   time steps per day used to solve the snow model (if MODEL_STEPS_PER_DAY >   1, SNOW_STEPS_PER_DAY should = MODEL_STEPS_PER_DAY)                                                      |
| RUNOFF_STEPS_PER_DAY | integer | steps  | Number of   time steps per day used to solve the runoff model (should be >=   MODEL_STEPS_PER_DAY)                                                                                               |
| ROUT_STEPS_PER_DAY   | integer | steps  | Number of   time steps per day used to solve the routing model (should be >=   MODEL_STEPS_PER_DAY)                                                                                              |
| STARTYEAR            | integer | year   | Year   model simulation starts. **NOTE**: STARTYEAR, STARTMONTH, STARTDAY and STARTSEC togetehr specify the begenning time point of the first simulation time step. |
| STARTMONTH           | integer | month  | Month   model simulation starts                                                                                                                                                                  |
| STARTDAY             | integer | day    | Day model   simulation starts                                                                                                                                                                    |
| STARTSEC             | integer | second | Second   model simulation starts (e.g., STARTSEC=0 for starting from the beginning of   a day; STARTSEC=3600 for starting from the beginning of the second hour of a   day) <br><br>Default = 0. |
| **EITHER:**          |         |        | *Note:*   **either** NRECS or ENDYEAR, ENDMONTH, and ENDDAY must be specified, but   **not both**                                                                                                |
| NRECS                | integer | N/A    | Number of   time steps over which to run model. ***NOTE: The number of records must be   defined such that the model completes the last day.***                                                  |
| **OR:**              |         |        | *Note:*   **either** NRECS or ENDYEAR, ENDMONTH, and ENDDAY must be specified, but   **not both**                                                                                                |
| ENDYEAR              | integer | year   | Year   model simulation ends. **NOTE**: ENDYEAR, ENDMONTH and ENDDAY together specify the last day on which the simulation runs. For sub-daily model run timestep, VIC always runs until the last timestep of this day. |
| ENDMONTH             | integer | month  | Month   model simulation ends                                                                                                                                                                    |
| ENDDAY               | integer | day    | Day model   simulation ends                                                                                                                                                                      |
| CALENDAR             | string  | N/A    | Calendar   to use. Valid calendars: 'standard', 'gregorian', 'proleptic_gregorian'   'noleap', '365_day', '360_day', 'julian', 'all_leap', '366_day'. Must match the time variable type in the forcing netCDF file. |
| OUT_TIME_UNITS       | string  | N/A    | Units for output time variables. Valid options: 'SECONDS', 'MINUTES', 'HOURS', 'DAYS'. <br><br>Default = DAYS.                                                                                       |

## Define Plugin Options

The following options determine the method of resolving the surface energy balance.

| Name          | Type      | Units             | Description                                                                                                                                                                                                                                                                                   |
|-------------- |--------   |---------------    |-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------    |
| ROUTING   | string    | TRUE or FALSE     | Option for computing streamflow routing. <li>**TRUE** = compute.  <li>**FALSE** = do not compute.  <br><br>Default = False.                                                |
| DECOMPOSITION   | string    | TRUE or FALSE     | Options for domain decomposition. <li>**RANDOM** = random decomposition (quick for small domains with limited basins).  <li>**BASIN** = routing basin decomposition (quick for large domains with many basins; requires routing).  <br><br>Default = RANDOM.                                                |
| EFR   | string    | TRUE or FALSE     | Option for computing environmental flow requirments (requires forcing files). <li>**TRUE** = compute.  <li>**FALSE** = do not compute.  <br><br>Default = False.                                                |
| DAMS   | string    | TRUE or FALSE     | Option for computing dam operation. <li>**TRUE** = compute.  <li>**FALSE** = do not compute.  <br><br>Default = False.                                                |
| WATERUSE   | string    | TRUE or FALSE     | Option for computing water-use. <li>**TRUE** = compute.  <li>**FALSE** = do not compute.  <br><br>Default = False.                                                |
| IRRIGATION   | string    | TRUE or FALSE     | Option for computing irrigation. <li>**TRUE** = compute.  <li>**FALSE** = do not compute.  <br><br>Default = False.                                                |
| WOFOST   | string    | TRUE or FALSE     | Option for computing crops with wofost. <li>**TRUE** = compute.  <li>**FALSE** = do not compute.  <br><br>Default = False.                                                |

# Define State Files

The following options control input and output of state files.

| Name         | Type    | Units         | Description                                                                                                                                                                                                                                                                                 |
|--------------|---------|---------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| INIT_STATE   | string  | path/filename | Full path and filename of initial state netCDF file. NOTE: if INIT_STATE is not specified, VIC will take initial soil moistures from the soil parameter file and set all other state variables to a default state.                                                                                 |
| STATENAME    | string  | path/filename | Path and file prefix of the state file to be created on the specified date. The date and second within the simulation at which the state is saved (as well as ".nc") will be appended to the file prefix to form a complete file name. *NOTE*: if STATENAME is not specified, VIC will not save its state in a statefile. |
| STATEYEAR    | integer | year          | Year at which model simulation state should be saved. *NOTE*: if STATENAME is not specified, STATEYEAR will be ignored. Also, STATEYEAR, STATEMONTH, STATEDAY and STATESEC together specify the exact time point when the model state is to be saved. A typical use of this time point is the end of the whole simulation period. For example, if the last day of simulation is 2000-01-10 (VIC always runs until the last time step of this day in the case of sub-daily time step), then user should specify STATEYEAR=2000, STATEMONTH=1, STATEDAY=11, STATESEC=0 to save the state of the end of simulation. [See here for more detail](StateFile.md) |
| STATEMONTH   | integer | month         | Month at which model simulation state should be saved. *NOTE*: if STATENAME is not specified, STATEMONTH will be ignored.                                                                                                                                                                   |
| STATEDAY     | integer | day           | Day at which model simulation state should be saved. *NOTE*: if STATENAME is not specified, STATEDAY will be ignored.                                                                                                                                                                       |
| STATESEC     | integer | second        | Second at which model simulation state should be saved. *NOTE*: if STATENAME is not specified, STATESEC will be ignored.                                                                                                                                                                    |
| STATE_FORMAT | string  | N/A           | Output state netCDF file format. Valid options: NETCDF3_CLASSIC, NETCDF3_64BIT_OFFSET, NETCDF4_CLASSIC, NETCDF4. *NOTE*: if STATENAME is not specified, STATE_FORMAT will be ignored.                                                                                                       |

# Define Frocing Files

This section describes how to define the forcing files needed by the VIC-WUR model.

| Name          | Type                 | Units                    | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
|---------------|----------------------|--------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| FORCE_TYPE    | string string string | N/A                      | Defines what forcing types are read from the file, followed by corresponding netCDF variable name (separated by space or tab), followed by the corresponding netCDF file path (separated by space or tab). The required forcing types are: AIR_TEMP, PREC, PRESSURE, SWDOWN, LWDOWN, VP, WIND.                                                                                                                                                                                                              |
| PLUGIN_FORCE_TYPE    | string string string string | N/A                      | Defines what forcing types are read from the file, followed by corresponding netCDF variable name (separated by space or tab), followed by the forcing aggrigation frequency (STEP, DAY, MONTH, or YEAR; separated by space or tab) by the corresponding netCDF file path (separated by space or tab). Valid forcing types are: DISCHARGE, EFR_DISCHARGE, EFR_BASEFLOW, MUN_DEMAND, MUN_GROUNDWATER, MUN_CONSUMPTION, LIV_DEMAND, LIV_GROUNDWATER, LIV_CONSUMPTION, IRR_DEMAND, IRR_GROUNDWATER, IRR_CONSUMPTION, ENE_DEMAND, ENE_GROUNDWATER, ENE_CONSUMPTION, MAN_DEMAND, MAN_GROUNDWATER, MAN_CONSUMPTION, CO2, CV, FERT_DVS, FERT_N, FERT_P, FERT_K |

# Define Domain File

The folloiwng options describe the input domain file information. See [Domain File](Domain.md) for details about the domain file.

| Name        | Type          | Units    | Description                                                                                                                                  |
|-------------|---------------|----------|----------------------------------------------------------------------------------------------------------------------------------------------|
| DOMAIN      | string        | pathname | Domain netCDF file path.                                                                                                                     |
| DOMAIN_TYPE | string string | N/A      | Domain variable type, followed by corresponding netCDF variable name. Domain variable types include: LAT, LON, MASK, AREA, FRAC, YDIM, XDIM. |


# Define Parameter Files

The following options describe the input parameter files.

| Name               | Type   | Units         | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
|--------------------|--------|---------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| ROUTING_PARAMETERS | string | path/filename | Routing parameter netCDF file path                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
| WATERUSE_PARAMETERS | string | path/filename | Water-use parameter netCDF file path                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
| IRRIGATION_PARAMETERS | string | path/filename | Irrigation parameter netCDF file path                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
| DAMS_PARAMETERS | string | path/filename | Dam parameter netCDF file path                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
| CROP_PARAMETERS | string | path/filename | Crop parameter netCDF file path                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
| WOFOST_PARAMETERS | string | path/filename | Wofost parameter text file path                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |

# Define Output Files

The following options describe the location of the log and model history files. Click [here](OutputFormatting.md) for more information.

| Name                  | Type      | Units             | Description                                                                        |
|---------------------- |---------  |---------------    |----------------------------------------------------------------------------------- |
| LOG_DIR               | string    | path name         | Name of directory where log files should be written (optional, default is stdout)  |
| RESULT_DIR            | string    | path name         | Name of directory where model results are written                                  |

The following options describe the settings for each output stream:

| Name       | Type                                 | Units                                | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         |
|------------|--------------------------------------|--------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| OUTFILE*   | string                               | prefix                               | Information about this output file: Prefix of the output file (to which the first time step of the results contained in a file as well as suffix ".nc" will be appended). This should be specified once for each stream of output files.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
| AGGFREQ    | string [integer/string]              | frequency count                      | Describes aggregation frequency for output stream. Valid options for frequency are: NEVER, NSTEPS, NSECONDS, NMINUTES, NHOURS, NDAYS, NMONTHS, NYEARS, DATE, END. Count may be an positive integer or a string with date format YYYY-MM-DD[-SSSSS] in the case of DATE. Default frequency is NDAYS. <bar><br>Default count is 1.                                                                                                                                                                                                                                                                                                                                                                                                    |
| HISTFREQ   | string [integer/string]              | frequency count                      | Describes the frequency/length of output results to be put in an individual file. Valid options are: NEVER, NSTEPS, NSECONDS, NMINUTES, NHOURS, NDAYS, NMONTHS, NYEARS, DATE, END. <br><br>Default is to output all results to one single file.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
| COMPRESS   | string/integer                       | TRUE, FALSE, or lvl                  | if TRUE or > 0 compress input and output files when done (uses gzip), if an integer [1-9] is supplied, it is used to set thegzip compression level                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |
| OUT_FORMAT | string                               | N/A                                  | Output netCDF format. Valid options:NETCDF3_CLASSIC, NETCDF3_64BIT_OFFSET, NETCDF4_CLASSIC, NETCDF4.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| OUTVAR*    | string string string integer string  | name format type multiplier aggtype  | Information about this output variable: <br>Name (must match a name listed in vic_driver_shared_all.h) <br>Output format (not used in image driver, replaced by "*") <br>Data type (one of: OUT_TYPE_DEFAULT, OUT_TYPE_CHAR, OUT_TYPE_SINT, OUT_TYPE_USINT, OUT_TYPE_INT, OUT_TYPE_FLOAT,OUT_TYPE_DOUBLE) <br>Multiplier - number to multiply the data with in order to recover the original values (only valid with OUT_FORMAT=BINARY) <br>Aggregation method - temporal aggregation method to use (one of: AGG_TYPE_DEFAULT, AGG_TYPE_AVG, AGG_TYPE_BEG, AGG_TYPE_END, AGG_TYPE_MAX, AGG_TYPE_MIN, AGG_TYPE_SUM) This should be specified once for each output variable. [Click here for more information](OutputFormatting.md). |

 - *Note: `OUTFILE`, and `OUTVAR` are optional; if omitted, traditional output files are produced. [Click here for details on using these instructions](OutputFormatting.md).*

## Example Global Parameter File:
```
#######################################################################
# VIC-WUR Model Parameters
#######################################################################

#######################################################################
# Time Options
#######################################################################
MODEL_STEPS_PER_DAY     4               # number of model time steps in 24 hour period
SNOW_STEPS_PER_DAY      4               # number of snow model time steps in 24 hour period
RUNOFF_STEPS_PER_DAY    12              # number of runoff time steps in 24 hour period
ROUT_STEPS_PER_DAY		24				# number of routing time steps in 24 hour period

STARTYEAR               1979            # year model simulation starts
STARTMONTH              1               # month model simulation starts
STARTDAY                1               # day model simulation starts
ENDYEAR                 2016
ENDMONTH                12
ENDDAY                  31
BASEFLOW                NIJSSEN2001     # ARNO = columns 5-8 are the standard VIC baseflow parameters; NIJSSEN2001 = columns 5-8 of soil file are baseflow parameters from Nijssen et al (2001) 

#######################################################################
# Simulation Options
#######################################################################
FULL_ENERGY             	FALSE          		# TRUE = calculate energy balance; FALSE = calculate water balance; Default = FALSE
CLOSE_ENERGY				FALSE				# TRUE = close energy balance; Default = FALSE
FROZEN_SOIL             	FALSE          		# TRUE = calculate frozen soils;  Default = FALSE
EXP_TRANS					FALSE				# TRUE = exponential thermal nodes in soil;  Default = FALSE
SNOW_BAND					FALSE          		# TRUE = calculate snow bands;  Default = FALSE
MATRIC						FALSE				# TRUE = calculate matric potential;  Default = FALSE

# Plugins (routing)
ROUTING						TRUE				# TRUE = calculate routing; Default = FALSE
DECOMPOSITION				BASIN				# RANDOM, BASIN or FILE; Default = RANDOM
EFR							TRUE				# TRUE = calculate enviromental flow requirements;  Default = FALSE
DAMS						TRUE				# TRUE = calculate dam operation;  Default = FALSE
WATERUSE					TRUE				# TRUE = calculate water use;  Default = FALSE
IRRIGATION					TRUE				# TRUE = calculate irrigation;  Default = FALSE
WOFOST					    TRUE		        # TRUE = calculate crops with wofost;  Default = FALSE

#######################################################################
# Domain Files and Info
#######################################################################
DOMAIN          				parameters/domain_global.nc
DOMAIN_TYPE    	LAT     		lat
DOMAIN_TYPE    	LON     		lon
DOMAIN_TYPE    	MASK    		mask
DOMAIN_TYPE    	AREA    		area
DOMAIN_TYPE    	FRAC    		frac
DOMAIN_TYPE    	YDIM    		lat
DOMAIN_TYPE    	XDIM    		lon

#######################################################################
# Forcing Files and Info
# netcdf forcing files will be of the form: FILE.YYYY.nc
#######################################################################
# FORCE_TYPE	TYPE			VARIABLE NAME		FILE
# Metreorological forcing
FORCE_TYPE    	AIR_TEMP		tas					forcing/tas_6hourly_WFDEI/tas_6hourly_WFDEI_
FORCE_TYPE    	PREC			pr					forcing/pr_6hourly_WFDEI/pr_6hourly_WFDEI_
FORCE_TYPE    	PRESSURE		psurf    			forcing/psurf_6hourly_WFDEI/psurf_6hourly_WFDEI_
FORCE_TYPE    	SWDOWN			swdown				forcing/swdown_6hourly_WFDEI/swdown_6hourly_WFDEI_
FORCE_TYPE    	LWDOWN			lwdown				forcing/lwdown_6hourly_WFDEI/lwdown_6hourly_WFDEI_
FORCE_TYPE    	VP				vp					forcing/vp_6hourly_WFDEI/vp_6hourly_WFDEI_
FORCE_TYPE    	WIND			wind				forcing/wind_6hourly_WFDEI/wind_6hourly_WFDEI_

# PLUGIN_FORCE_TYPE	TYPE				VARIABLE NAME			FREQ	FILE
# Discharge forcing
#PLUGIN_FORCE_TYPE	FORCING_DISCHARGE	discharge				STEP	forcing/discharge_6hourly/discharge_6hourly_
# Environmental flow forcing
PLUGIN_FORCE_TYPE	EFR_DISCHARGE		discharge				DAY		forcing/efr_discharge_6hourly/efr_discharge_6hourly_
PLUGIN_FORCE_TYPE	EFR_BASEFLOW		baseflow				DAY		forcing/efr_baseflow_6hourly/efr_baseflow_6hourly_
# Water use forcing
PLUGIN_FORCE_TYPE	MUN_DEMAND			demand					MONTH	forcing/dom_demand_6hourly/dom_demand_6hourly_
PLUGIN_FORCE_TYPE	MUN_GROUNDWATER		groundwater_fraction	MONTH	forcing/dom_ground_6hourly/dom_ground_6hourly_
PLUGIN_FORCE_TYPE	MUN_CONSUMPTION		consumption_fraction	MONTH	forcing/dom_consump_6hourly/dom_consump_6hourly_
PLUGIN_FORCE_TYPE	LIV_DEMAND			demand					MONTH	forcing/liv_demand_6hourly/liv_demand_6hourly_
PLUGIN_FORCE_TYPE	LIV_GROUNDWATER		groundwater_fraction	MONTH	forcing/liv_ground_6hourly/liv_ground_6hourly_
PLUGIN_FORCE_TYPE	LIV_CONSUMPTION		consumption_fraction	MONTH	forcing/liv_consump_6hourly/liv_consump_6hourly_
PLUGIN_FORCE_TYPE	MAN_DEMAND			demand					MONTH	forcing/man_demand_6hourly/man_demand_6hourly_
PLUGIN_FORCE_TYPE	MAN_GROUNDWATER		groundwater_fraction	MONTH	forcing/man_ground_6hourly/man_ground_6hourly_
PLUGIN_FORCE_TYPE	MAN_CONSUMPTION		consumption_fraction	MONTH	forcing/man_consump_6hourly/man_consump_6hourly_
PLUGIN_FORCE_TYPE	ENE_DEMAND			demand					MONTH	forcing/ene_demand_6hourly/ene_demand_6hourly_
PLUGIN_FORCE_TYPE	ENE_GROUNDWATER		groundwater_fraction	MONTH	forcing/ene_ground_6hourly/ene_ground_6hourly_
PLUGIN_FORCE_TYPE	ENE_CONSUMPTION		consumption_fraction	MONTH	forcing/ene_consump_6hourly/ene_consump_6hourly_
# CO2 forcing
PLUGIN_FORCE_TYPE	CO2 		        co2			            YEAR   forcing/co2_yearly_fixed/co2_yearly_fixed_
# Fertilizer forcing
PLUGIN_FORCE_TYPE	FERT_DVS	        DVS_point	            YEAR   forcing/fertilizer_yearly/fertilizer_yearly_
PLUGIN_FORCE_TYPE	FERT_N	            N_amount	            YEAR   forcing/fertilizer_yearly/fertilizer_yearly_
PLUGIN_FORCE_TYPE	FERT_P	            P_amount	            YEAR   forcing/fertilizer_yearly/fertilizer_yearly_
PLUGIN_FORCE_TYPE	FERT_K    	        K_amount	            YEAR   forcing/fertilizer_yearly/fertilizer_yearly_
# Land-cover forcing
PLUGIN_FORCE_TYPE	  CV			    coverage                MONTH  forcing/coverage_monthly_VICWOFOST/coverage_monthly_VICWOFOST_

#######################################################################
# Land Surface Files and Info
#######################################################################
CONSTANTS						parameters/VIC_constants_low.txt
PARAMETERS        				parameters/VIC_params_MIRCA2000_global.nc
ROUTING_PARAMETERS				parameters/rout_params_global.nc
WATERUSE_PARAMETERS				parameters/wu_params_global.nc
IRRIGATION_PARAMETERS			parameters/irr_params_MIRCA2000_global.nc
DAMS_PARAMETERS					parameters/dam_params_global.nc
CROP_PARAMETERS 			    parameters/crop_params_VICWOFOST_global.nc
WOFOST_TEXT_PARAMETERS 		    parameters/wofost_params_VICWOFOST_global.txt

#######################################################################
# Output Options
#######################################################################
RESULT_DIR						output
OUTFILE							fluxes_global

OUT_FORMAT						NETCDF4
COMPRESS						9
AGGFREQ							NYEARS	1

## VIC
# fluxes
OUTVAR							OUT_DISCHARGE
OUTVAR							OUT_BASEFLOW

## IRRIGATION
# states
OUTVAR							OUT_SHORTAGE
OUTVAR							OUT_REQUIREMENT

## WATER-USE
# fluxes
OUTVAR							OUT_AV_GW_SECT
OUTVAR							OUT_AV_SURF_SECT
OUTVAR							OUT_AV_REM_SECT
OUTVAR							OUT_AV_DAM_SECT
OUTVAR							OUT_DE_GW_SECT
OUTVAR							OUT_DE_SURF_SECT
OUTVAR							OUT_DE_REM_SECT
OUTVAR							OUT_WI_GW_SECT
OUTVAR							OUT_WI_SURF_SECT
OUTVAR							OUT_WI_REM_SECT
OUTVAR							OUT_WI_DAM_SECT

# WOFOST
# fluxes
OUTVAR                          			OUT_CROP_GROW
OUTVAR							OUT_CROP_EVAP
OUTVAR							OUT_CROP_DVS
OUTVAR							OUT_CROP_WLV
OUTVAR							OUT_CROP_WST
OUTVAR							OUT_CROP_WSO
OUTVAR							OUT_CROP_WRT
OUTVAR							OUT_CROP_LAI
OUTVAR							OUT_CROP_WSTRESS
OUTVAR							OUT_CROP_NSTRESS
OUTVAR							OUT_CROP_NUPT

```
