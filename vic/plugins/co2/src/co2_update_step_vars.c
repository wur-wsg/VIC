#include <vic_driver_image.h>
#include <plugin.h>

void
co2_update_step_vars(size_t iCell)
{
    extern force_data_struct  *force;
    extern co2_force_struct   *co2_force;
    extern size_t              NF;
    extern size_t              NR;
    
    size_t                     i;

    for (i = 0; i < NF; i++) {
        force[iCell].Catm[i] = (double) co2_force[iCell].CO2 * PPM_to_MIXRATIO;
    }
    force[iCell].Catm[NR] = average(force[iCell].Catm, NF);
}
