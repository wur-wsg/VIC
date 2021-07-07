# Crop Model Parameters

!!! Note
	It is the user's responsibility to ensure that parameter files are formatted appropriately.


The crop parameters serve the following main purposes:

*   Define crop types (and associated vegetation types)
*   Define crop growing season (planting, harvesting, extent, and TSUMS)
*   Define default fertilizer application
*   Define default mineralization rates
*   Define Temperature factor

The crop parameters are supplied to VIC-WUR in a NetCDF file, with a separate variable for each crop parameter.

Below is a list of crop parameters.

| Variable Name         | Dimension   | Units                                          | Type   | Number of Values | Description                                     |
|-----------------------|-------------|------------------------------------------------|--------|------------------|-------------------------------------------------|
| Ncrop             | [lat, lon] | N/A                                            | int    | 1                | Number of crop in cell        |
| Cc                 | [lat, lon, crop, month] | fraction | double    | number of crops * months per year                | Crop coverage per months (used for multiple seasons per crop)                  |
| crop_veg_class  | [lat, lon, crop]  | vegetation class                            | int | number of crops                | Crop vegetation class |
| plant_day | [lat, lon, crop]  | day of year                                       | double | number of crops                | Crop plantin day  |
| harvest_day | [lat, lon, crop]  | day of year                                       | double | number of crops                | Crop harvest day  |
| TSUM1 | [lat, lon, crop]  | degree celsius                                       | double | number of crops                | Crop temperature sum to anthesis  |
| TSUM2 | [lat, lon, crop]  | degree celsius                                       | double | number of crops                | Crop temperature sum to maturity  |
| Tfactor | [lat, lon]  | degree celsius                                       | double | 1                | Crop temperature factor  |
| DVS_point | [lat, lon, crop, fert]  | fraction                                       | double | number of crops * fertilization times                | Crop development stage of fertilize application  |
| N_amount | [lat, lon, crop, fert]  | kg ha-1                                       | double | number of crops * fertilization times                | Crop nitrogen application  |
| P_amount | [lat, lon, crop, fert]  | kg ha-1                                       | double | number of crops * fertilization times                | Crop phosphorus application  |
| K_amount | [lat, lon, crop, fert]  | kg ha-1                                       | double | number of crops * fertilization times                | Crop potassium application  |
| N_mins | [lat, lon, crop]  | kg ha-1                                       | double | number of crops                | Crop nitrogen mineralization rate  |
| P_mins | [lat, lon, crop]  | kg ha-1                                       | double | number of crops                | Crop phosphorus mineralization rate |
| K_mins | [lat, lon, crop]  | kg ha-1                                       | double | number of crops                | Crop potassium mineralization rate |
| N_recovery | [lat, lon, crop]  | fraction                                       | double | number of crops                | Crop nitrogen recovery rate |
| P_recovery | [lat, lon, crop]  | fraction                                       | double | number of crops                | Crop phosphorus recovery rate |
| K_recovery | [lat, lon, crop]  | fraction                                       | double | number of crops                | Crop potassium recovery rate |

# Example netCDF format VIC-WUR image driver crop parameters

```
netcdf crop_params_VICWOFOST_global {
dimensions:
	lon = 720 ;
	lat = 360 ;
	crop_class = 13 ;
	month = 12 ;
	fertilizer_times = 1 ;
variables:
	double lon(lon) ;
		lon:units = "degrees_east" ;
		lon:long_name = "longitude of grid cell center" ;
	double lat(lat) ;
		lat:units = "degrees_north" ;
		lat:long_name = "latitude of grid cell center" ;
	float Ncrop(lat, lon) ;
		Ncrop:units = "#" ;
		Ncrop:_FillValue = -1.f ;
		Ncrop:long_name = "Number of active crop classes" ;
	int crop_class(crop_class) ;
		crop_class:units = "#" ;
		crop_class:long_name = "Crop class" ;
	int month(month) ;
		month:units = "month of year" ;
		month:long_name = "month of year (1-12)" ;
	float Cc(month, crop_class, lat, lon) ;
		Cc:units = "fraction" ;
		Cc:_FillValue = -1.f ;
		Cc:long_name = "Crop coverage per month" ;
	float crop_veg_class(crop_class, lat, lon) ;
		crop_veg_class:units = "#" ;
		crop_veg_class:_FillValue = -1.f ;
		crop_veg_class:long_name = "Crop vegetation class" ;
	float plant_day(crop_class, lat, lon) ;
		plant_day:units = "day of year" ;
		plant_day:_FillValue = -1.f ;
		plant_day:long_name = "Day of year the crop is planted" ;
	float harvest_day(crop_class, lat, lon) ;
		harvest_day:units = "day of year" ;
		harvest_day:_FillValue = -1.f ;
		harvest_day:long_name = "Day of year the crop is harvested" ;
	float TSUM1(crop_class, lat, lon) ;
		TSUM1:units = "K" ;
		TSUM1:_FillValue = -1.f ;
		TSUM1:long_name = "Daily temperature sum from emergence to anthesis" ;
	float TSUM2(crop_class, lat, lon) ;
		TSUM2:units = "K" ;
		TSUM2:_FillValue = -1.f ;
		TSUM2:long_name = "Daily temperature sum from anthesis to maturity" ;
	float Tfactor(lat, lon) ;
		Tfactor:units = "K" ;
		Tfactor:_FillValue = -1.f ;
		Tfactor:long_name = "Temperature factor due to elevation" ;
	double fertilizer_times(fertilizer_times) ;
		fertilizer_times:units = "#" ;
		fertilizer_times:long_name = "Fertilizer time" ;
	float DVS_point(fertilizer_times, crop_class, lat, lon) ;
		DVS_point:units = "-" ;
		DVS_point:_FillValue = -1.f ;
		DVS_point:long_name = "DVS fraction after which fertilizer is applied" ;
	float N_amount(fertilizer_times, crop_class, lat, lon) ;
		N_amount:units = "kg ha-1" ;
		N_amount:_FillValue = -1.f ;
		N_amount:long_name = "N fertilizer amount" ;
	float P_amount(fertilizer_times, crop_class, lat, lon) ;
		P_amount:units = "kg ha-1" ;
		P_amount:_FillValue = -1.f ;
		P_amount:long_name = "P fertilizer amount" ;
	float K_amount(fertilizer_times, crop_class, lat, lon) ;
		K_amount:units = "kg ha-1" ;
		K_amount:_FillValue = -1.f ;
		K_amount:long_name = "K fertilizer amount" ;
	float N_mins(crop_class, lat, lon) ;
		N_mins:units = "kg ha-1" ;
		N_mins:_FillValue = -1.f ;
		N_mins:long_name = "N amount for mineralization" ;
	float N_recovery(crop_class, lat, lon) ;
		N_recovery:units = "kg kg-1" ;
		N_recovery:_FillValue = -1.f ;
		N_recovery:long_name = "N mineralization recovery rate" ;
	float P_mins(crop_class, lat, lon) ;
		P_mins:units = "kg ha-1" ;
		P_mins:_FillValue = -1.f ;
		P_mins:long_name = "N amount for mineralization" ;
	float P_recovery(crop_class, lat, lon) ;
		P_recovery:units = "kg kg-1" ;
		P_recovery:_FillValue = -1.f ;
		P_recovery:long_name = "N mineralization recovery rate" ;
	float K_mins(crop_class, lat, lon) ;
		K_mins:units = "kg ha-1" ;
		K_mins:_FillValue = -1.f ;
		K_mins:long_name = "N amount for mineralization" ;
	float K_recovery(crop_class, lat, lon) ;
		K_recovery:units = "kg kg-1" ;
		K_recovery:_FillValue = -1.f ;
		K_recovery:long_name = "N mineralization recovery rate" ;

// global attributes:
		:Description = "Crop parameters for VIC. Created by Bram Droppers" ;
}
```
