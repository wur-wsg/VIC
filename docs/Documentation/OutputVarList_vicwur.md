# Output Variables

Using options within the global parameter file, any combination of the variables listed below may be output by VIC-WUR.

## Routing Terms
| Variable              | Description                                           | Units  |
|-----------------------|---------------------------------------------------------------------------------|--------|
| OUT_DISCHARGE,        | River (outflow) discharge                                                       | m3 s-1 |
| OUT_STREAM_MOIST,     | River (in-cell) stream moisture                                                 | mm     |

## EFR Terms
| Variable              | Description                                                                     | Units  |
|-----------------------|---------------------------------------------------------------------------------|--------|
| OUT_EFR_DISCHARGE,    | Environmental river discharge                                                   | m3 s-1 |
| OUT_EFR_BASEFLOW,     | Environmental baseflow                                                          | mm     |
| OUT_EFR_MOIST,        | Environmental third-layer soil-moisture                                         | mm     |

## Irrigation Terms
| Variable              | Description                                                                              | Units  |
|-----------------------|------------------------------------------------------------------------------------------|--------|
| OUT_REQUIREMENT,      | Average irrigation requirement (between field capacity and critical soil moisture point) | mm     |
| OUT_SHORTAGE,         |  Average irrigation shortage (below critical soil moisture point)                        | mm     |
| OUT_RECEIVED,         |  Irrigation water received                        | mm     |
| OUT_LEFTOVER,         |  Irrigation water leftover                        | mm     |
| OUT_APPLIED,         |  Irrigation water applied                       | mm     |

## Dam Terms
| Variable              | Description                                                                      | Units |
|-----------------------|----------------------------------------------------------------------------------|-------|
| OUT_LDAM_INFLOW       | Local dam water inflow                                                           | hm3   |
| OUT_LDAM_DEMAND,      | Local dam water demand                                                           | hm3   |
| OUT_LDAM_EFR,         | Local dam environmental requirements                            				   | hm3   |
| OUT_LDAM_RELEASE,     | Local dam release                                               				   | hm3   |
| OUT_LDAM_STORAGE,     | Local dam storage                                               				   | hm3   |
| OUT_LDAM_HIST_INFLOW, | Local dam historical inflow for this month                      				   | hm3   |
| OUT_LDAM_HIST_DEMAND, | Local dam historical demand for this month                      				   | hm3   |
| OUT_LDAM_HIST_EFR,    | Local dam historical environmental requirements for this month  				   | hm3   |
| OUT_LDAM_OP_RELEASE,  | Local dam calculated release                                    				   | hm3   |
| OUT_LDAM_OP_STORAGE,  | Local dam calculated storage                                    				   | hm3   |
| OUT_GDAM_INFLOW,      | Global dam inflow                                               				   | hm3   |
| OUT_GDAM_DEMAND,      | Global dam water demand                                         				   | hm3   |
| OUT_GDAM_EFR,         | Global dam environmental requirements                           				   | hm3   |
| OUT_GDAM_RELEASE,     | Global dam release                                             				   | hm3   |
| OUT_GDAM_STORAGE,     | Global dam storage                                              				   | hm3   |
| OUT_GDAM_HIST_INFLOW, | Global dam historical inflow for this month                     				   | hm3   |
| OUT_GDAM_HIST_DEMAND, | Global dam historical demand for this month                     				   | hm3   |
| OUT_GDAM_HIST_EFR,    | Global dam historical environmental requirements for this month 				   | hm3   |
| OUT_GDAM_OP_RELEASE,  | Global dam calculated release                                   				   | hm3   |
| OUT_GDAM_OP_STORAGE,  | Global dam calculated storage		                             				   | hm3   |

## Water-use Terms
| Variable          | Description                                           | Units |
|-------------------|-------------------------------------------------------|-------|
| OUT_DE_GW_SECT,   | Demand for groundwater resources per sector   | mm    |
| OUT_DE_SURF_SECT, | Demand for surface water resources per sector | mm    |
| OUT_DE_REM_SECT,  | Demand for remote resources per sector (from this cell)       | mm    |
| OUT_DE_TREM_SECT,  | Demand for remote resources per sector (for this cell)       | mm    |
| OUT_WI_GW_SECT,   | Withdrawn groundwater resources per sector    | mm    |
| OUT_WI_SURF_SECT, | Withdrawn surface water resources per sector  | mm    |
| OUT_WI_DAM_SECT,  | Withdrawn dam reservoir resources per sector  | mm    |
| OUT_WI_REM_SECT,  | Withdrawn remote resources per sector (from this cell)        | mm    |
| OUT_WI_TREM_SECT,  | withdrawn remote resources per sector (for this cell)         | mm    |
| OUT_AVAILABLE,    | Available water resources                     | mm    |
| OUT_DEMAND,       | Demand for water resources                    | mm    |
| OUT_WITHDRAWN,    | Withdrawn water resources                     | mm    |
| OUT_RETURNED,     | Returned water resources 		                | mm    |

## WOFOST terms
| Variable          | Description                                           | Units |
|-------------------|-------------------------------------------------------|-------|
| OUT_CROP_GROW,     | Crop growing days		                | days    |
| OUT_CROP_CULTIVATE,     | Crop cultivate days	                | days    |
| OUT_CROP_DVS,     | Crop development stage 		                | -    |
| OUT_CROP_WSO,     | Crop storage organ dry matter 		                | kg ha-1    |
| OUT_CROP_WST,     | Crop stem dry matter 		                | kg ha-1    |
| OUT_CROP_WLV,     | Crop leafs dry matter		                | kg ha-1    |
| OUT_CROP_WRT,     | Crop roots dry matter 		                | kg ha-1    |
| OUT_CROP_WSTRESS,     | Crop water stress 		                | -    |
| OUT_CROP_NSTRESS,     | Crop nutrient stress 		                | -    |
| OUT_CROP_NUPT,     | Crop nitrogen uptake 		                | kg ha-1    |
| OUT_CROP_PUPT,     | Crop phosphorus uptake 		                | kg ha-1    |
| OUT_CROP_KUPT,     | Crop potassium uptake 		                | kg ha-1    |
