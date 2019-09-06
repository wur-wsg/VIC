# Routing Model Parameters

!!! Note
	It is the user's responsibility to ensure that parameter files are formatted appropriately. Notably, the variables `uh_runoff` and `uh_inflow` must sum exactly to 1.0.


The Routing Parameters serve three main purposes:

*   Define the cell ID number of each grid cell. This ID number is used to identify downstream cells.
*   Define the downstream cells
*   Define the unit-hydrographs.

The routing parameters are supplied to VIC-WUR in a NetCDF file, with a separate variable for each routing parameter.

Below is a list of routing parameters.

| Variable Name | Dimension        | Units    | Type   | Number of Values   | Description                     |
|---------------|------------------|----------|--------|--------------------|---------------------------------|
| downstream_id | [lat, lon]       | N/A      | int    | 1                  | Current cell grid cell ID       |
| downstream    | [lat, lon]       | N/A      | int    | 1                  | Downstream cell grid cell ID    |
| uh_runoff     | [time, lat, lon] | fraction | double | routing max length | Convolution fraction for runoff |
| uh_inflow     | [time, lat, lon] | fraction | double | routing max length | Convolution fraction for inflow |

# Example netCDF format VIC-WUR image driver routing parameters

```
netcdf rout_params_global {
dimensions:
	lon = 720 ;
	lat = 360 ;
	time = 48 ;
variables:
	double lon(lon) ;
		lon:units = "degrees_east" ;
		lon:long_name = "longitude of grid cell center" ;
	double lat(lat) ;
		lat:units = "degrees_north" ;
		lat:long_name = "latitude of grid cell center" ;
	float downstream_id(lat, lon) ;
		downstream_id:units = "ID" ;
		downstream_id:_FillValue = -1.f ;
		downstream_id:long_name = "ID used to identify downstream cell" ;
	float downstream(lat, lon) ;
		downstream:units = "ID" ;
		downstream:_FillValue = -1.f ;
		downstream:long_name = "ID of the downstream cell" ;
		downstream:Description = "Unit hydrograph generated based on Lohmann et al (1996) with flow velocity 1 [m s-1] and diffusion 2000 [m s-2]" ;
	double time(time) ;
		time:units = "seconds" ;
		time:long_name = "Time in the unit hydrograph" ;
	float uh_inflow(time, lat, lon) ;
		uh_inflow:units = "[-]" ;
		uh_inflow:_FillValue = -1.f ;
		uh_inflow:long_name = "Unit hydrograph of the inflow (river)" ;
	float uh_runoff(time, lat, lon) ;
		uh_runoff:units = "[-]" ;
		uh_runoff:_FillValue = -1.f ;
		uh_runoff:long_name = "Unit hydrograph of the runoff (cell)" ;

// global attributes:
		:Description = "Routing parameters for VIC. Created by Bram Droppers" ;
}
```