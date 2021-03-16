# VIC-WUR Routing Forcing File

VIC-WUR requires a NetCDF file with gridded forcings. Forcing timesteps can be each model timestep (`STEP`), each day (`DAY`) each month (`MONTH`), or each year (`YEAR`). The required forcing variables and units are listed below and must also be specified in the [Global Parameter File](GlobalParam_vicwur.md). Each forcing variable is supplied to VIC-WUR in one or multiple NetCDF files.

#### Routing Forcings:

| Variable          | Dimension        | Units | Description                            |
|-------------------|------------------|-------|----------------------------------------|
| FORCING_DISCHARGE | [time, lat, lon] | m3/s  | Discharge inflow (from unknown source) |

#### Environmental Flow Forcings:

| Variable      | Dimension        | Units | Description                       |
|---------------|------------------|-------|-----------------------------------|
| EFR_DISCHARGE | [time, lat, lon] | m3/s  | Required environmental streamflow |
| EFR_BASEFLOW  | [time, lat, lon] | mm    | Required environmental baseflow   |

#### Water-use Forcings:

| Variable        | Dimension        | Units | Description                                   |
|-----------------|------------------|-------|-----------------------------------------------|
| MUN_DEMAND      | [time, lat, lon] | mm    | Domestic water demand                         |
| MUN_GROUNDWATER | [time, lat, lon] | mm    | Domestic groundwater withdrawal fraction      |
| MUN_CONSUMPTION | [time, lat, lon] | mm    | Domestic withdrawal consumption               |
| LIV_DEMAND      | [time, lat, lon] | mm    | Livestock water demand                        |
| LIV_GROUNDWATER | [time, lat, lon] | mm    | Livestock groundwater withdrawal fraction     |
| LIV_CONSUMPTION | [time, lat, lon] | mm    | Livestock withdrawal consumption              |
| MAN_DEMAND      | [time, lat, lon] | mm    | Manufacturing water demand                    |
| MAN_GROUNDWATER | [time, lat, lon] | mm    | Manufacturing groundwater withdrawal fraction |
| MAN_CONSUMPTION | [time, lat, lon] | mm    | Manufacturing withdrawal consumption          |
| ENE_DEMAND      | [time, lat, lon] | mm    | Energy water demand                           |
| ENE_GROUNDWATER | [time, lat, lon] | mm    | Energy groundwater withdrawal fraction        |
| ENE_CONSUMPTION | [time, lat, lon] | mm    | Energy withdrawal consumption                 |
| IRR_DEMAND      | [time, lat, lon] | mm    | Irrigation water demand                       |
| IRR_GROUNDWATER | [time, lat, lon] | mm    | Irrigation groundwater withdrawal fraction    |
| IRR_CONSUMPTION | [time, lat, lon] | mm    | Irrigation withdrawal consumption             |

#### Land-cover Forcings:

| Variable        | Dimension        | Units | Description                                   |
|-----------------|------------------|-------|-----------------------------------------------|
| Cv      | [time, lat, lon] | -    | Coverage fraction for each land-cover tile                         |

#### Crop Forcings:

| Variable        | Dimension        | Units | Description                                   |
|-----------------|------------------|-------|-----------------------------------------------|
| CO2      | [time, lat, lon] | ppm    | Atmospheric CO2 concentration                         |
| FERT_DVS | [time, lat, lon] | -    | Fertilizer development stage application timing      |
| FERT_N | [time, lat, lon] | kg ha-1    | Fertilizer nitrogen amount               |
| FERT_P      | [time, lat, lon] | mm    | Fertilizer phosphorus amount                        |
| FERT_K      | [time, lat, lon] | mm    | Fertilizer potassium amount                        |

The forcing data must be chunked by calendar year, with each NetCDF file named by the year, e.g. `prefix.$year.nc`.

```
