# Dam Model Parameters

!!! Note
	It is the user's responsibility to ensure that parameter files are formatted appropriately. Note that it is possible to have multiple dams per grid cell (which will be processed in order).


The Dam Parameters serve three main purposes:

*   Define the dam info
*	Define the dam servicing info

The dam parameters are supplied to VIC-WUR in a NetCDF file, with a separate variable for each dam parameter.

Below is a list of dam parameters.

| Variable Name          | Dimension                          | Units    | Type   | Number of Values | Description                                         |
|------------------------|------------------------------------|----------|--------|------------------|-----------------------------------------------------|
| id_map                 | [lat, lon]                         | N/A      | int    | 1                | Current cell grid cell ID                           |
| id                     | [dam_class]                        | N/A      | int    | 1                | Number of local dams in the grid cell               |
| type                   | [dam_class]                        | N/A      | int    | 1                | Dam type: 1 = irrigation, 0 = other                 |
| year                   | [dam_class]                        | year     | int    | 1                | Building year of dam                                |
| capacity               | [dam_class]                        | hm3      | double | 1                | Capacity of dam                                     |
| inflow_fraction        | [dam_class]                        | fraction | double | 1                | Fraction of inflow going to the dam reservoir       |
| Nservice               | [dam_class]                        | N/A      | int    | 1                | Numer of service cells for dam                      |
| service                | [lat, lon, dam_class]              | N/A      | int    | Ndams            | Service cell dam ID                                 |
| service_fraction       | [lat, lon, dam_class]              | fraction | double | Ndams            | Fraction of demand of service cell for dam          |

# Example netCDF format VIC-WUR image driver dam parameters

```
netcdf dam_params_global {
dimensions:
	lon = 204 ;
	lat = 180 ;
	dam_class = 86 ;
variables:
	double lon(lon) ;
		lon:units = "degrees_east" ;
		lon:long_name = "longitude of grid cell center" ;
	double lat(lat) ;
		lat:units = "degrees_north" ;
		lat:long_name = "latitude of grid cell center" ;
	int id_map(lat, lon) ;
		id_map:units = "ID" ;
		id_map:_FillValue = -1 ;
		id_map:long_name = "ID used to identify dam cell" ;
	int dam_class(dam_class) ;
		dam_class:units = "#" ;
		dam_class:long_name = "Dam class" ;
	int id(dam_class) ;
		id:units = "ID" ;
		id:_FillValue = -1 ;
		id:long_name = "ID used to identify dam cell" ;
	int type(dam_class) ;
		type:units = "global/local" ;
		type:_FillValue = -1 ;
		type:long_name = "1 = global, 0 = local" ;
	int year(dam_class) ;
		year:units = "years AD" ;
		year:_FillValue = -1 ;
		year:long_name = "Building year of dam" ;
	float capacity(dam_class) ;
		capacity:units = "hm3" ;
		capacity:_FillValue = -1.f ;
		capacity:long_name = "Capacity of dam" ;
	float inflow_fraction(dam_class) ;
		inflow_fraction:units = "-" ;
		inflow_fraction:_FillValue = -1.f ;
		inflow_fraction:long_name = "Fraction of inflow going to the dam reservoir" ;
	int Nservice(dam_class) ;
		Nservice:units = "#" ;
		Nservice:_FillValue = -1 ;
		Nservice:long_name = "Numer of service cells for dam" ;	
    int service(dam_class, lat, lon) ;
		service:units = "ID" ;
		service:_FillValue = -1 ;
		service:long_name = "Service cell ID for dam" ;
	float service_fraction(dam_class, lat, lon) ;
		service_fraction:units = "ID" ;
		service_fraction:_FillValue = -1.f ;
		service_fraction:long_name = "Fraction of demand for service cell of dam" ;
```
