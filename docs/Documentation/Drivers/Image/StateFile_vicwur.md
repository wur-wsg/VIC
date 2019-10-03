# VIC Model State File - Image Driver

VIC-WUR can save the hydrologic state from any point in the simulation (usually the final state) to a file for the purpose of re-starting the simulation later (as an initial state file). This is useful for simulations that require lengthy spin-up periods or ensemble methods. The model state file contains all information needed by the VIC model to "warm"-start a simulation (i.e. start from "realistic" conditions, or re-start a simulation exactly where the model previously stopped).

The timestamp of the state file represents the instantaneous time for which the values of the state variables are valid. This corresponds to the end of the time interval after which the state file is written out and the beginning of the time interval for which the model is started. For example, if the MODEL_STEPS_PER_DAY is 24 (hourly) and the last time step for which the model is run is 1999-09-20 23:00:00, the the state file will be stamped 1999-09-21 00:00:00. This state file can then be used to restart a model run whose starting time will be 1999-09-21 00:00:00.

* * *

## netCDF State File Dimensions

The netCDF state file dimensions include:

| Dimension name | Description                                      |
|----------------|--------------------------------------------------|
| lon            | Number of longitudes                             |
| lat            | Number of latitudes                              |
| nlayer         | Number of soil layers                            |
| soil_node      | Number of soil thermal nodes                     |
| veg_class      | Number of vegetation types (including bare soil) |
| snow_band      | Number of snow bands                             |
| frost_area     | Number of frost areas                            |
| discharge_dt   | Number of routing timesteps                      |

* * *

## Routing Information (only when ROUT are turned on in the [global parameter file](GlobalParam_vicwur.md#DefineStateFiles))

| State   variable name            | Type              | Description                                  |
|----------------------------------|-------------------|----------------------------------------------|
| STATE_DISCHARGE_DT               | double            | routing sub-step discharge stored [m3 s-1]   |
