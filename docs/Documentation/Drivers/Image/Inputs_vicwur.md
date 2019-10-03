# VIC-WUR Model Inputs

VIC-WUR input files may be constructed using various programs and datasets. Below are general descriptions of each input file along with links to its structure and the methods that may be used to build it. For a comprehensive example, see the [VIC-WUR support github](http://github.com/bramdr/VIC-WUR_support).

To run VIC-wur, a global parameter file is needed to specify all options and inputs:

*	[Global Parameter File](GlobalParam_vicwur.md): File containing all options and inputs needed to run VIC-WUR.

To run VIC-wur, several sets of input data are optional:

*   [Routing Parameter File](RoutingParam.md): File containing routing model parameters. By default, VIC-WUR does not simulate routing.
*   [Irrigation Parameter File](IrrigationParam.md): File containing irrigation model parameters. By default, VIC-WUR does not simulate irrigation.
*   [Dam Parameter File](DamParam.md): File containing dam model parameters. By default, VIC-WUR does not simulate routing.
*   [Water-use Parameter File](WateruseParam.md): File containing water-use model parameters. By default, VIC-WUR does not simulate water-use.
*   [Forcing Files](ForcingData_vicwur.md):  Gridded, timeseries of discharge inflow, required discharge, required baseflow, demand, consumption fraction and groundwater fraction as input.