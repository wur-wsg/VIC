#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function RatesToZero()                                                    */
/*  Purpose: Set all rates to zero in one go                                  */
/* ---------------------------------------------------------------------------*/

void
RatesToZero(SimUnit *Grid)
{
    /* Set the dying rates */
    Grid->crp->drt.roots = 0.;
    Grid->crp->drt.leaves = 0.;
    Grid->crp->drt.stems = 0.;

    /* Set the development rate */
    Grid->crp->rt.Development = 0.;
    Grid->crp->rt.vernalization = 0.;

    /* Set the rooting depth */
    Grid->crp->rt.RootDepth = 0.;

    /* Set the crop growth rates */
    Grid->crp->rt.roots = 0.;
    Grid->crp->rt.leaves = 0.;
    Grid->crp->rt.stems = 0.;
    Grid->crp->rt.storage = 0.;
    Grid->crp->rt.LAIExp = 0.;

    /* Set the nutrient loss rates due to dying */
    Grid->crp->N_rt.death_lv = 0.;
    Grid->crp->N_rt.death_st = 0.;
    Grid->crp->N_rt.death_ro = 0.;

    Grid->crp->P_rt.death_lv = 0.;
    Grid->crp->P_rt.death_st = 0.;
    Grid->crp->P_rt.death_ro = 0.;

    Grid->crp->K_rt.death_lv = 0.;
    Grid->crp->K_rt.death_st = 0.;
    Grid->crp->K_rt.death_ro = 0.;

    /* Set nutrient rates to various crop organs */
    Grid->crp->N_rt.leaves = 0.;
    Grid->crp->N_rt.stems = 0.;
    Grid->crp->N_rt.storage = 0.;
    Grid->crp->N_rt.roots = 0.;

    Grid->crp->P_rt.leaves = 0.;
    Grid->crp->P_rt.stems = 0.;
    Grid->crp->P_rt.roots = 0.;
    Grid->crp->P_rt.storage = 0.;

    Grid->crp->K_rt.leaves = 0.;
    Grid->crp->K_rt.stems = 0.;
    Grid->crp->K_rt.roots = 0.;
    Grid->crp->K_rt.storage = 0.;

    /* Set the nutrient demand rates */
    Grid->crp->N_rt.Demand = 0.;
    Grid->crp->N_rt.Demand_lv = 0.;
    Grid->crp->N_rt.Demand_st = 0.;
    Grid->crp->N_rt.Demand_ro = 0.;
    Grid->crp->N_rt.Demand_so = 0.;

    Grid->crp->P_rt.Demand = 0.;
    Grid->crp->P_rt.Demand_lv = 0.;
    Grid->crp->P_rt.Demand_st = 0.;
    Grid->crp->P_rt.Demand_ro = 0.;
    Grid->crp->P_rt.Demand_so = 0.;

    Grid->crp->K_rt.Demand = 0.;
    Grid->crp->K_rt.Demand_lv = 0.;
    Grid->crp->K_rt.Demand_st = 0.;
    Grid->crp->K_rt.Demand_ro = 0.;
    Grid->crp->K_rt.Demand_so = 0.;

    /* Set the nutrient translocation rates */
    Grid->crp->N_rt.Transloc = 0.;
    Grid->crp->P_rt.Transloc = 0.;
    Grid->crp->K_rt.Transloc = 0.;

    /* Set the nutrient uptake rates */

    Grid->crp->N_rt.Uptake = 0.;
    Grid->crp->N_rt.Uptake_lv = 0.;
    Grid->crp->N_rt.Uptake_st = 0.;
    Grid->crp->N_rt.Uptake_ro = 0.;

    Grid->crp->P_rt.Uptake = 0.;
    Grid->crp->P_rt.Uptake_lv = 0.;
    Grid->crp->P_rt.Uptake_st = 0.;
    Grid->crp->P_rt.Uptake_ro = 0.;

    Grid->crp->K_rt.Uptake = 0.;
    Grid->crp->K_rt.Uptake_lv = 0.;
    Grid->crp->K_rt.Uptake_st = 0.;
    Grid->crp->K_rt.Uptake_ro = 0.;

    /* Set the soil nutrient rates */
    Grid->ste->rt_N_mins = 0.;
    Grid->ste->rt_P_mins = 0.;
    Grid->ste->rt_K_mins = 0.;

    Grid->ste->rt_N_tot = 0.;
    Grid->ste->rt_P_tot = 0.;
    Grid->ste->rt_K_tot = 0.;

    /* Set the water balance rates */
    Grid->soil->rt.EvapWater = 0.;
    Grid->soil->rt.EvapSoil = 0.;
    Grid->soil->rt.Infiltration = 0.;
    Grid->soil->rt.Irrigation = 0.;
    Grid->soil->rt.Loss = 0.;
    Grid->soil->rt.Moisture = 0.;
    Grid->soil->rt.MoistureLOW = 0.;
    Grid->soil->rt.Percolation = 0.;
    Grid->soil->rt.RootZoneMoisture = 0.;
    Grid->soil->rt.Runoff = 0.;
    Grid->soil->rt.WaterRootExt = 0.;
}
