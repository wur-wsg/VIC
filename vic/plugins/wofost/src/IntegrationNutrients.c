#include <vic_driver_image.h>
#include <plugin.h>

/* ----------------------------------------------------------------------*/
/*  function IntegrationNutrients()                                      */
/*  Purpose: Integration of the soil and crop nutrient rates  kg ha-1    */
/* ----------------------------------------------------------------------*/

void
IntegrationNutrients(SimUnit *Grid)
{
    /* Integration of the total of soil N,P,K */
    Grid->ste->st_N_tot = max(0., Grid->ste->st_N_tot + Grid->ste->rt_N_tot);
    Grid->ste->st_P_tot = max(0., Grid->ste->st_P_tot + Grid->ste->rt_P_tot);
    Grid->ste->st_K_tot = max(0., Grid->ste->st_K_tot + Grid->ste->rt_K_tot);

    /* Integration of the total N,P,K soil mineralization */
    Grid->ste->st_N_mins = max(0., Grid->ste->st_N_mins - Grid->ste->rt_N_mins);
    Grid->ste->st_P_mins = max(0., Grid->ste->st_P_mins - Grid->ste->rt_P_mins);
    Grid->ste->st_K_mins = max(0., Grid->ste->st_K_mins - Grid->ste->rt_K_mins);

    /* Demand */
    Grid->crp->N_st.Demand += Grid->crp->N_rt.Demand;
    Grid->crp->P_st.Demand += Grid->crp->N_rt.Demand;
    Grid->crp->K_st.Demand += Grid->crp->N_rt.Demand;

    /* Uptake */
    Grid->crp->N_st.Uptake += Grid->crp->N_rt.Uptake;
    Grid->crp->P_st.Uptake += Grid->crp->P_rt.Uptake;
    Grid->crp->K_st.Uptake += Grid->crp->K_rt.Uptake;

    /* Grid->crps nutrients */
    Grid->crp->N_st.leaves += Grid->crp->N_rt.leaves;
    Grid->crp->N_st.stems += Grid->crp->N_rt.stems;
    Grid->crp->N_st.roots += Grid->crp->N_rt.roots;
    Grid->crp->N_st.storage += Grid->crp->N_rt.storage;

    Grid->crp->P_st.leaves += Grid->crp->P_rt.leaves;
    Grid->crp->P_st.stems += Grid->crp->P_rt.stems;
    Grid->crp->P_st.roots += Grid->crp->P_rt.roots;
    Grid->crp->P_st.storage += Grid->crp->P_rt.storage;

    Grid->crp->K_st.leaves += Grid->crp->K_rt.leaves;
    Grid->crp->K_st.stems += Grid->crp->K_rt.stems;
    Grid->crp->K_st.roots += Grid->crp->K_rt.roots;
    Grid->crp->K_st.storage += Grid->crp->K_rt.storage;

    /* Nutrient losses due to dying plant parts */
    Grid->crp->N_st.death_lv += Grid->crp->N_rt.death_lv;
    Grid->crp->N_st.death_st += Grid->crp->N_rt.death_st;
    Grid->crp->N_st.death_ro += Grid->crp->N_rt.death_ro;

    Grid->crp->P_st.death_lv += Grid->crp->P_rt.death_lv;
    Grid->crp->P_st.death_st += Grid->crp->P_rt.death_st;
    Grid->crp->P_st.death_ro += Grid->crp->P_rt.death_ro;

    Grid->crp->K_st.death_lv += Grid->crp->K_rt.death_lv;
    Grid->crp->K_st.death_st += Grid->crp->K_rt.death_st;
    Grid->crp->K_st.death_ro += Grid->crp->K_rt.death_ro;
}
