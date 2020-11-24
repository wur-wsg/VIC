# Model Constants and Miscellaneous Parameters

The VIC model requires the specification of many model parameters to run. While some parameters are spatially distributed (e.g. _soil_, _vegetation_, or _snowbands_), others are more appropriately held constant across the model domain.

## Model Constants and Parameters

Some model constants and model parameters need to be changed more frequently. VIC-WUR supports this through the use of the optional `CONSTANTS` file. The path to this file should be specified in the global parameter file.

The table below lists the constants available for manipulation via the `CONSTANTS` file. Descriptions will be added later or as needed.

| Constant | Description | Unit | Default value |
|---|---|---|---|
| DAM_ALPHA | Preferred dam maximum storage fraction | fraction | 0.85 |
| DAM_BETA | Dam correction exponent | - | 0.6 |
| DAM_GAMMA | Dam correction period | days | 5 |
| Ksat_expt | Paddy irrigation saturated conductivity exponent | - | 0.33 |
| Wfc_fract | Field capacity as fraction of critical moisture content | fraction | 0.8 |
| NONRENEWABLE_LIMIT | Non-renewable withdrawal limit (depth) | mm | Inf |
