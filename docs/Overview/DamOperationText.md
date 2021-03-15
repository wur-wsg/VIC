# Dam Operation

VIC-WUR uses a dam operation scheme based on Hanasaki et al. (2006). Target release (i.e. the estimated optimal release) is calculated at the start of the operational year. The operational year starts at the month where the inflow drops below the average annual inflow, and thus the storage should be at its desired maximum. The scheme distinguished between two dam types: (1) dams that did not account for water demands downstream (e.g. hydropower dams or flood control) and (2) dams that did account for water demands downstream (e.g. irrigation dams). The original dam operation scheme is adapted slightly to account for monthly varying EFRs.

For dams that did not account for demands, the initial release is set at the mean annual inflow corrected by the variable EFRs (Eq. A.1). For dams that do account for demands, the initial release is increased during periods of higher water demand. If demands are relatively high compared to the annual inflow, the release is corrected by the demand relative to the mean demand (Eq. A.2). If demands are relatively low compared to the annual inflow, release is corrected based on the actual water demand (Eq. A.3). 

> R'm = EFRm + Iy - EFRy   
> where Dy = 0  
> Eq. (A.1)

> R'm = EFRm + (Iy - EFRy) * Dm / Dy  
> where Dy > 0 and Dy > Iy - EFRy  
> Eq. (A.2)

> R'm = EFRm + (Iy - EFRy) - Dy + Dm  
> where Dy > 0 and Dy ≤ Iy - EFRy  
> Eq. (A.3)

Where R’m is the initial monthly target release [m3 s-1], EFRm is the average monthly EFR for streamflow demand [m3 s-1], Iy is the average yearly inflow [m3 s-1], EFRy is the average yearly EFR for streamflow [m3 s-1], Dm is the average monthly water demand [m3 s-1] and Dy is the average yearly water demand [m3 s-1].

As in Hanasaki et al. (2006), the initial target release is adjusted based on storage and capacity. Target release is adjusted to compensate differences between the current storage and the desired maximum storage (Eq. A.4). Target release is additionally adjusted if the storage capacity is relatively low compared to the annual inflow, and unable to store large portions of the inflow for later release (Eq. A.5).

> Rm = k * R'm  
> where c ≥ 0.5  
> Eq. (A.4)

> Rm = (c / 0.5)^2 * k * Rm' + (1 - (c / 0.5)^2 ) * Im  
> where 0 ≤ c ≤ 0.5  
> Eq. (A.5)

Where Iy is the average monthly inflow [m3 s-1], c the capacity parameter [-] calculated as the storage capacity divided by the mean annual inflow and k the storage parameter [-] calculated as current storage divided by the desired maximum storage. 

Water inflow, demand and EFRs are estimated based on the average of the past five years. Water demands are based on the water demands from the water use module. Only a fraction of water demands (`servicing_fraction`) are taken into account for each servicing cell (`servicing`).

Dam release is increased during high-storage events to prevent overflow and accompanying high discharge events. If dam storage is above the desired maximum storage, target dam release is increased to negate the difference (Eq. A.6). If dam storage is below the desired minimum storage, release is decreased (Eq. A.7). Dam release is adjusted exponentially based on the relative storage difference: small storage differences are only corrected slightly, but if the dam is close to overflowing or emptying, the difference is corrected strongly.

> Ra = Rm + ((S - Cα)) / γ * ((S / C- α) / (1 - α))^b  
> where S > Cα  
> Eq. (A.6)

> Ra = Rm + ((S - C(1 - α))) / γ * (((1 - α) - S / C) / (1 - α))^b  
> where S < C (1 - α)  
> Eq. (A.7)


Where Ra is the actual dam release [m3 s-1], S the dam storage capacity [m3], α (`DAM_ALPHA`) the fraction of the capacity that is the desired maximum [-], β (`DAM_BETA`) the exponent determining the correction increase [-] and γ (`DAM_GAMMA`) the parameter determining the period when the release is corrected [s-1].
