# Irrigation Model Parameters

!!! Note
	It is the user's responsibility to ensure that parameter files are formatted appropriately.


The Irrigation Parameters serve three main purposes:

*   Define vegetation types that are irrigated and paddy irrigated
*   Define the irrigation efficiency and groundwater withdrawal fraction

The irrigation parameters are supplied to VIC-WUR in a NetCDF file, with a separate variable for each irrigation parameter.

Below is a list of irrigation parameters.

| Variable Name         | Dimension   | Units                                          | Type   | Number of Values | Description                                     |
|-----------------------|-------------|------------------------------------------------|--------|------------------|-------------------------------------------------|
| veg_class             | [irr_class] | N/A                                            | int    | 1                | Vegetation class of the irrigation class        |
| paddy                 | [irr_class] | 0 = not paddy irrigation, 1 = paddy irrigation | int    | 1                | Indicator for paddy irrigation                  |
| groundwater_fraction  | [lat, lon]  | fraction                                       | double | 1                | Fraction of irrigation comming from groundwater |
| irrigation_efficiency | [lat, lon]  | fraction                                       | double | 1                | Fraction of water withdrawn per water required  |

# Example netCDF format VIC-WUR image driver irrigation parameters

```
netcdf irr_params_MIRCA2000_global {
dimensions:
	irr_class = 2 ;
	lon = 720 ;
	lat = 360 ;
variables:
	int irr_class(irr_class) ;
		irr_class:units = "class" ;
		irr_class:long_name = "Irrigation class" ;
	int veg_class(irr_class) ;
		veg_class:units = "class" ;
		veg_class:_FillValue = -1 ;
		veg_class:long_name = "Vegetation class of irrigation class" ;
	int paddy(irr_class) ;
		paddy:units = "0 = not paddy irrigation, 1 = paddy irrigation" ;
		paddy:_FillValue = -1 ;
		paddy:long_name = "Paddy indicator" ;
	double lon(lon) ;
		lon:units = "degrees_east" ;
		lon:long_name = "lon" ;
	double lat(lat) ;
		lat:units = "degrees_north" ;
		lat:long_name = "lat" ;
	double groundwater_fraction(lat, lon) ;
		groundwater_fraction:units = "-" ;
		groundwater_fraction:_FillValue = -1. ;
		groundwater_fraction:long_name = "Fraction of irrigation comming from groundwater" ;
	double irrigation_efficiency(lat, lon) ;
		irrigation_efficiency:units = "mm mm-1" ;
		irrigation_efficiency:_FillValue = -1. ;
		irrigation_efficiency:long_name = "Fraction of water withdrawn per water required" ;

// global attributes:
		:Description = "Routing parameters for VIC. Created by Bram Droppers" ;
}
```