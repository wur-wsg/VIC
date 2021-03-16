# WOFOST Model Parameters

!!! Note
	It is the user's responsibility to ensure that parameter files are formatted appropriately.


The WOFOST parameters serve the following main purposes:

*   Define crop specific parameters
*   Define management specific parameters

The crop parameters are supplied to VIC-WUR in a text file. The file should contain a single line per VIC-WOFOST crop. Each line contains the following information (in order)

Below is a list of crop parameters.

| Variable Name         | Type   | Description                                     |
|-----------------------|--------|-------------------------------------------------|
| Base path | Path | Root WOFOST working directory |
| Crop path | Path | WOFOST crop parameter file |
| Management path | Path | WOFOST management parameter file |
| Default planting date | dd-mm | Default WOFOST planting date (overwitten by crop parameters) |
| Emergence flag | N/A | Do simulations start at emergence? |

# Example text format VIC-WUR image driver WOFOST parameters

```
** WOFOST PARAMETER CONFIGURATION FILE for use with WOFOST-C
** For use in VIC-WOFOST

./WOFOST/ ./Parameters/Crop/global/crop_params_wheat.txt ./Parameters/Management/global/management_params_default.txt 01-01 0
./WOFOST/ ./Parameters/Crop/global/crop_params_wheat.txt ./Parameters/Management/global/management_params_default.txt 01-01 0
./WOFOST/ ./Parameters/Crop/global/crop_params_wheat.txt ./Parameters/Management/global/management_params_default.txt 01-01 0
./WOFOST/ ./Parameters/Crop/global/crop_params_wheat.txt ./Parameters/Management/global/management_params_default.txt 01-01 0
./WOFOST/ ./Parameters/Crop/global/crop_params_maize.txt ./Parameters/Management/global/management_params_default.txt 01-01 0
./WOFOST/ ./Parameters/Crop/global/crop_params_maize.txt ./Parameters/Management/global/management_params_default.txt 01-01 0
./WOFOST/ ./Parameters/Crop/global/crop_params_rice.txt ./Parameters/Management/global/management_params_default.txt 01-01 1
./WOFOST/ ./Parameters/Crop/global/crop_params_rice.txt ./Parameters/Management/global/management_params_default.txt 01-01 1
./WOFOST/ ./Parameters/Crop/global/crop_params_rice.txt ./Parameters/Management/global/management_params_default.txt 01-01 1
./WOFOST/ ./Parameters/Crop/global/crop_params_rice.txt ./Parameters/Management/global/management_params_default.txt 01-01 1
./WOFOST/ ./Parameters/Crop/global/crop_params_rice.txt ./Parameters/Management/global/management_params_default.txt 01-01 1
./WOFOST/ ./Parameters/Crop/global/crop_params_soybean.txt ./Parameters/Management/global/management_params_default.txt 01-01 0
./WOFOST/ ./Parameters/Crop/global/crop_params_soybean.txt ./Parameters/Management/global/management_params_default.txt 01-01 0
```
