# WOFOST crop simulations

VIC-WOFOST uses the WOFOST crop model of Wit et al (2019) and Supit et al (1994). WOFOST is a field-scale crop model that simulates crop growth for a single crop variety. Crop varieties are defined by their crop parameters and phenological development length. Phenological development is expressed in two stages: emergence to anthesis and anthesis to maturity. Phenological development depends on air temperature only (no vernalization or day-length effects). Here, a short description is given of the processes involved in simulating crop growth, which is the main factor in calculating crop yields.

Crop growth is the results of carbon assimilation and nutrient uptake.
Potential carbon assimilation is calculated based on the incoming (direct and diffuse) radiation during three periods of the day and in three depths in the crop canopy.
Assimilation is constrained by suboptimal daytime temperatures, low nighttime temperatures, nutrient stress and water stress. 
Net carbon assimilation accounts for maintenance respiration, which is a function of the plant biomass, development stage and temperature. Assimilates, after conversion to dry matter, are subsequently partitioned over the crop organs (stems, leaves, roots and storage) as a function of the plant development stage.
Crop nutrient demand is based on the difference between the current and maximum crop organ nutrient content as a fraction of the crop organ biomass \cite{shibu10. Nutrient uptake is limited by nutrient availability from fertilization, mineralization and fixation.

Nutrient stress is calculated using the following equation, and and can vary between 1 (no nutrient stress) and 0 (severe nutrient stress) based on nutrient contents in stems and leaves.

> Nstress = 1 - Neff * (1 - Nidx)^2  \in [0,1]   
> Nidx = (Nveg - Nres) / (Nopt - Nres) \in [0,1]   

Where Nstress is the nutrient stress [-], Neff is the crop specific nutrient use efficiency factor due to nutrient stress [-], Nidx is the nutrient index [-], Nveg is the nutrient content in the stems and leaves [kg N kg-1 dry mass ha-1], Nres is the residual nutrient content in the stems and leaves [kg N kg-1 dry mass ha-1], and Nopt is the optimal nutrient content in the stems and leaves [kg N kg-1 dry mass ha-1].
