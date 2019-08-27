# VIC-WUR Model Overview

!!! note
    You are viewing the documentation for VIC-WUR. VIC-WUR extends the VIC model with several modules to simulate the human impact on global water resources.

## Main Features

The VIC-WUR model ([Droppers et al., 2019](../Documentation/References.md)) is a large-scale semi-distributed hydrological model which is used to assess the human impact on global water resources. As such, several modules are included:

1. Anthropogenic water withdrawal, consumption and return flow for the irrigation, domestic, energy, manufacturing and livestock sector. Water demands for the irrigation sector are calculated by the model while the other sectors require demand (potential withdrawal) inputs.
	- This feature is combined with an integrated routing module
2. Environmental flow requirements for terrestrial ecosystems. Flow requirements are seperated into:
	- Streamflow requirements that limit surface water withdrawals
	- Baseflow requirements that limit groundwater withdrawals
3. Dam reservoir operation. Dam operation is seperated into operations for 'small' (within grid) dams and 'large' dams.

![VIC-WUR framework](../img/VIC-WUR_framework.svg)

Figure 1: The VIC-WUR model framework

## Water withdrawal, consumption and return flow
### Dynamic irrigation demand calculation
### Water demand, groundwater and consumption forcing
VIC-WUR supports water demand, groundwater and consumption forcing data for the:

1. Irrigation sector
2. Domestic sector
3. Energy sector
4. Manufacturing sector
5. Livestock sector

Forcings can be provided at various temporal resolutions (e.g. `FORCE_STEP`, `FORCE_DAY`, `FORCE_MONTH`). Sectors are dynamically included if all forcing files (e.g. demand, groundwater and consumption) are provided. See [Droppers et al., 2019](../Documentation/References.md) for more information on how this has been calculated in the past.

## Integrated routing

### Routing inflow forcing
VIC-WUR supports routing inflow forcing data to provided additional water inflow to specific cells. Forcings can be provided at various temporal resolutions (e.g. `FORCE_STEP`, `FORCE_DAY`, `FORCE_MONTH`).

## Environmental flow requirements

## Dam operation
