# Water-use Model Parameters

!!! Note
	It is the user's responsibility to ensure that parameter files are formatted appropriately.


The Water-use Parameters serve three main purposes:

*   Define the cell ID number of each grid cell. This ID number is used to identify receiving cells.
*   Define the receiving cells. (use in remote water withdrawals)

The water-use parameters are supplied to VIC-WUR in a NetCDF file, with a separate variable for each water-use parameter.

Below is a list of water-use parameters.

| Variable Name | Dimension                | Units | Type | Number of Values        | Description                 |
|---------------|--------------------------|-------|------|-------------------------|-----------------------------|
| receiving_id  | [lat, lon]               | N/A   | int  | 1                       | Current cell grid cell ID   |
| Nreceiving    | [lat, lon]               | N/A   | int  | 1                       | Number of receiving cells   |
| receiving     | [wu_receiving, lat, lon] | N/A   | int  | water-use max receiving | Receiving cell grid cell ID |

# Example netCDF format VIC-WUR image driver water-use parameters

```
netcdf wu_params_global {
dimensions:
	lon = 720 ;
	lat = 360 ;
	wu_receiving = 32 ;
variables:
	double lon(lon) ;
		lon:units = "degrees_east" ;
		lon:long_name = "longitude of grid cell center" ;
	double lat(lat) ;
		lat:units = "degrees_north" ;
		lat:long_name = "latitude of grid cell center" ;
	float Nreceiving(lat, lon) ;
		Nreceiving:units = "#" ;
		Nreceiving:_FillValue = -1.f ;
		Nreceiving:long_name = "Number of receiving cells" ;
	int wu_receiving(wu_receiving) ;
		wu_receiving:units = "#" ;
		wu_receiving:long_name = "Dam class" ;
	float receiving(wu_receiving, lat, lon) ;
		receiving:units = "#" ;
		receiving:_FillValue = -1.f ;
		receiving:long_name = "Receiving cell ID" ;
	float receiving_id(lat, lon) ;
		receiving_id:units = "years AD" ;
		receiving_id:_FillValue = -1.f ;
		receiving_id:long_name = "ID used to identify receiving cell" ;

// global attributes:
		:Description = "Water-use parameters for VIC. Created by Bram Droppers" ;
}
```