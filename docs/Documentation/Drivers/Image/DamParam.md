# Dam Model Parameters

!!! Note
	It is the user's responsibility to ensure that parameter files are formatted appropriately. Note that it is possible to have multiple dams per grid cell (which will be processed in order).


The Dam Parameters serve three main purposes:

*   Define the large dam info
*   Define the small dam info
*	Define the dam servicing info

The dam parameters are supplied to VIC-WUR in a NetCDF file, with a separate variable for each dam parameter.

Below is a list of dam parameters.

| Variable Name          | Dimension                          | Units    | Type   | Number of Values | Description                                         |
|------------------------|------------------------------------|----------|--------|------------------|-----------------------------------------------------|
| service_id             | [lat, lon]                         | N/A      | int    | 1                | Current cell grid cell ID                           |
| Ndam_local             | [lat, lon]                         | N/A      | int    | 1                | Number of local dams in the grid cell               |
| year_local             | [dam_class, lat, lon]              | year     | int    | Ndams            | Building year of local dam                          |
| capacity_local         | [dam_class, lat, lon]              | hm3      | double | Ndams            | Capacity of local dam                               |
| inflow_fraction_local  | [dam_class, lat, lon]              | fraction | double | Ndams            | Fraction of inflow going to the local dam reservoir |
| Nservice_local         | [dam_class, lat, lon]              | N/A      | int    | Ndams            | Numer of service cells for local dam                |
| service_local          | [dam_class, dam_service, lat, lon] | N/A      | int    | Ndams, Nservice  | Service cell grid cell ID                           |
| service_fraction_local | [dam_class, dam_service, lat, lon] | fraction | double | Ndams, Nservice  | Fraction of demand of service cell for local dam    |
| Ndam_global             | [lat, lon]                         | N/A      | int    | 1                | Number of global dams in the grid cell               |
| year_global             | [dam_class, lat, lon]              | year     | int    | Ndams            | Building year of global dam                          |
| capacity_global         | [dam_class, lat, lon]              | hm3      | double | Ndams            | Capacity of global dam                               |
| inflow_fraction_global  | [dam_class, lat, lon]              | fraction | double | Ndams            | Fraction of inflow going to the global dam reservoir |
| Nservice_global         | [dam_class, lat, lon]              | N/A      | int    | Ndams            | Numer of service cells for global dam                |
| service_global          | [dam_class, dam_service, lat, lon] | N/A      | int    | Ndams, Nservice  | Service cell grid cell ID                           |
| service_fraction_global | [dam_class, dam_service, lat, lon] | fraction | double | Ndams, Nservice  | Fraction of demand of service cell for global dam    |

# Example netCDF format VIC-WUR image driver dam parameters

```
netcdf dam_params_global {
dimensions:
	lon = 720 ;
	lat = 360 ;
	dam_class = 1 ;
	dam_service = 72 ;
variables:
	double lon(lon) ;
		lon:units = "degrees_east" ;
		lon:long_name = "longitude of grid cell center" ;
	double lat(lat) ;
		lat:units = "degrees_north" ;
		lat:long_name = "latitude of grid cell center" ;
	float service_id(lat, lon) ;
		service_id:units = "ID" ;
		service_id:_FillValue = -1.f ;
		service_id:long_name = "ID used to identify servicing cell" ;
	float Ndam_local(lat, lon) ;
		Ndam_local:units = "#" ;
		Ndam_local:_FillValue = -1.f ;
		Ndam_local:long_name = "Number of local dams in the grid cell" ;
	int dam_class(dam_class) ;
		dam_class:units = "#" ;
		dam_class:long_name = "Dam class" ;
	float year_local(dam_class, lat, lon) ;
		year_local:units = "years AD" ;
		year_local:_FillValue = -1.f ;
		year_local:long_name = "Building year of local dam" ;
	float capacity_local(dam_class, lat, lon) ;
		capacity_local:units = "hm3" ;
		capacity_local:_FillValue = -1.f ;
		capacity_local:long_name = "Capacity of local dam" ;
	float inflow_fraction_local(dam_class, lat, lon) ;
		inflow_fraction_local:units = "-" ;
		inflow_fraction_local:_FillValue = -1.f ;
		inflow_fraction_local:long_name = "Fraction of inflow going to the local dam reservoir" ;
	float Nservice_local(dam_class, lat, lon) ;
		Nservice_local:units = "#" ;
		Nservice_local:_FillValue = -1.f ;
		Nservice_local:long_name = "Numer of service cells for local dam" ;
	int dam_service(dam_service) ;
		dam_service:units = "#" ;
		dam_service:long_name = "Dam service class" ;
	float service_local(dam_class, dam_service, lat, lon) ;
		service_local:units = "ID" ;
		service_local:_FillValue = -1.f ;
		service_local:long_name = "Service cell ID for local dam" ;
	float service_fraction_local(dam_class, dam_service, lat, lon) ;
		service_fraction_local:units = "ID" ;
		service_fraction_local:_FillValue = -1.f ;
		service_fraction_local:long_name = "Fraction of demand for service cell of local dam" ;
	float Ndam_global(lat, lon) ;
		Ndam_global:units = "#" ;
		Ndam_global:_FillValue = -1.f ;
		Ndam_global:long_name = "Number of global dams in the grid cell" ;
	float year_global(dam_class, lat, lon) ;
		year_global:units = "years AD" ;
		year_global:_FillValue = -1.f ;
		year_global:long_name = "Building year of global dam" ;
	float capacity_global(dam_class, lat, lon) ;
		capacity_global:units = "hm3" ;
		capacity_global:_FillValue = -1.f ;
		capacity_global:long_name = "Capacity of global dam" ;
	float inflow_fraction_global(dam_class, lat, lon) ;
		inflow_fraction_global:units = "-" ;
		inflow_fraction_global:_FillValue = -1.f ;
		inflow_fraction_global:long_name = "Fraction of inflow going to the global dam reservoir" ;
	float Nservice_global(dam_class, lat, lon) ;
		Nservice_global:units = "#" ;
		Nservice_global:_FillValue = -1.f ;
		Nservice_global:long_name = "Numer of service cells for global dam" ;
	float service_global(dam_class, dam_service, lat, lon) ;
		service_global:units = "ID" ;
		service_global:_FillValue = -1.f ;
		service_global:long_name = "Service cell ID for global dam" ;
	float service_fraction_global(dam_class, dam_service, lat, lon) ;
		service_fraction_global:units = "ID" ;
		service_fraction_global:_FillValue = -1.f ;
		service_fraction_global:long_name = "Fraction of demand for service cell of globak dam" ;

// global attributes:
		:Description = "Dam parameters for VIC. Created by Bram Droppers" ;
}
```