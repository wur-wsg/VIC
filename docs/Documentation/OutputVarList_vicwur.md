# Output Variables

Using options within the global parameter file, any combination of the variables listed below may be output by VIC-WUR.

## Routing Terms
| Variable           | Description                     | Units  |
|--------------------|---------------------------------|--------|
| OUT_DISCHARGE      | River (outflow) discharge       | m3 s-1 |
| OUT_STREAM_MOIST   | River (in-cell) stream moisture | mm     |
| OUT_NONREN_DEFICIT | Non-renewable water deficit     | mm     |


## EFR Terms
| Variable              | Description                                                                     | Units  |
|-----------------------|---------------------------------------------------------------------------------|--------|
| OUT_EFR_DISCHARGE     | Environmental river discharge                                                   | m3 s-1 |
| OUT_EFR_BASEFLOW      | Environmental baseflow                                                          | mm     |
| OUT_EFR_MOIST         | Environmental third-layer soil-moisture                                         | mm     |

## Irrigation Terms
| Variable              | Description                                                                              | Units  |
|-----------------------|------------------------------------------------------------------------------------------|--------|
| OUT_REQUIREMENT       | Average irrigation requirement (between field capacity and critical soil moisture point) | mm     |
| OUT_SHORTAGE          | Average irrigation shortage (below critical soil moisture point)                         | mm     |
| OUT_RECEIVED          | Sum of irrigation water received                                                         | mm     |
| OUT_LEFTOVER          | Sum of irrigation water leftover (after application)                                     | mm     |
| OUT_APPLIED           | Sum of irrigation water applied                                                          | mm     |

## Dam Terms
| Variable              | Description                                                                      | Units |
|-----------------------|----------------------------------------------------------------------------------|-------|
| OUT_LDAM_INFLOW       | Local dam water inflow                                                           | hm3   |
| OUT_LDAM_DEMAND       | Local dam water demand                                                           | hm3   |
| OUT_LDAM_EFR          | Local dam environmental requirements                            				   | hm3   |
| OUT_LDAM_RELEASE      | Local dam release                                               				   | hm3   |
| OUT_LDAM_STORAGE      | Local dam storage                                               				   | hm3   |
| OUT_LDAM_HIST_INFLOW  | Local dam historical inflow for this month                      				   | hm3   |
| OUT_LDAM_HIST_DEMAND  | Local dam historical demand for this month                      				   | hm3   |
| OUT_LDAM_HIST_EFR     | Local dam historical environmental requirements for this month  				   | hm3   |
| OUT_LDAM_OP_RELEASE   | Local dam calculated release                                    				   | hm3   |
| OUT_LDAM_OP_STORAGE   | Local dam calculated storage                                    				   | hm3   |
| OUT_GDAM_INFLOW       | Global dam inflow                                               				   | hm3   |
| OUT_GDAM_DEMAND       | Global dam water demand                                         				   | hm3   |
| OUT_GDAM_EFR          | Global dam environmental requirements                           				   | hm3   |
| OUT_GDAM_RELEASE      | Global dam release                                             				   | hm3   |
| OUT_GDAM_STORAGE      | Global dam storage                                              				   | hm3   |
| OUT_GDAM_HIST_INFLOW  | Global dam historical inflow for this month                     				   | hm3   |
| OUT_GDAM_HIST_DEMAND  | Global dam historical demand for this month                     				   | hm3   |
| OUT_GDAM_HIST_EFR     | Global dam historical environmental requirements for this month 				   | hm3   |
| OUT_GDAM_OP_RELEASE   | Global dam calculated release                                   				   | hm3   |
| OUT_GDAM_OP_STORAGE   | Global dam calculated storage		                             				   | hm3   |

## Water-use Terms
| Variable          | Description                                   | Units |
|-------------------|-----------------------------------------------|-------|
| OUT_AV_GW_SECT    | Available groundwater resources per sector    | mm    |
| OUT_AV_SURF_SECT  | Available surface water resources per sector  | mm    |
| OUT_AV_DAM_SECT   | Available dam reservoir resources per sector  | mm    |
| OUT_AV_REM_SECT   | Available remote resources per sector         | mm    |
| OUT_DE_GW_SECT    | Demand for groundwater resources per sector   | mm    |
| OUT_DE_SURF_SECT  | Demand for surface water resources per sector | mm    |
| OUT_DE_REM_SECT   | Demand for remote resources per sector        | mm    |
| OUT_WI_GW_SECT    | Withdrawn groundwater resources per sector    | mm    |
| OUT_WI_SURF_SECT  | Withdrawn surface water resources per sector  | mm    |
| OUT_WI_DAM_SECT   | Withdrawn dam reservoir resources per sector  | mm    |
| OUT_WI_REM_SECT   | Withdrawn remote resources per sector         | mm    |
| OUT_AV_GW         | Available groundwater resources               | mm    |
| OUT_AV_SURF       | Available surface water resources             | mm    |
| OUT_AV_DAM        | Available dam reservoir resources             | mm    |
| OUT_AV_REM        | Available remote resources                    | mm    |
| OUT_DE_GW         | Demand for groundwater resources              | mm    |
| OUT_DE_SURF       | Demand for surface water resources            | mm    |
| OUT_DE_REM        | Demand for remote resources                   | mm    |
| OUT_WI_GW         | Withdrawn groundwater resources               | mm    |
| OUT_WI_SURF       | Withdrawn surface water resources             | mm    |
| OUT_WI_DAM        | Withdrawn dam reservoir resources             | mm    |
| OUT_WI_REM        | Withdrawn remote resources                    | mm    |
| OUT_AVAILABLE     | Available water resources                     | mm    |
| OUT_DEMAND        | Demand for water resources                    | mm    |
| OUT_WITHDRAWN     | Withdrawn water resources                     | mm    |
| OUT_RETURNED      | Returned water resources 		                | mm    |
