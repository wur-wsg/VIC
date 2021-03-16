#include <vic_driver_image.h>
#include <plugin.h>

/* --------------------------------------------------------------------*/
/*  function NutrientsInitialize()                                     */
/*  Purpose: Initialization of nutrient parameters                     */
/* --------------------------------------------------------------------*/

void
InitializeNutrients(SimUnit *Grid)
{
    /* Initial maximum N concentration in plant organs per kg biomass [kg N kg-1 dry biomass]   */
    Grid->crp->N_st.Max_lv =
        Afgen(Grid->crp->prm.N_MaxLeaves, &(Grid->crp->st.Development));
    Grid->crp->N_st.Max_st = Grid->crp->prm.N_MaxStems * Grid->crp->N_st.Max_lv;
    Grid->crp->N_st.Max_ro = Grid->crp->prm.N_MaxRoots * Grid->crp->N_st.Max_lv;
    Grid->crp->N_st.Max_so = 0.;

    /* Initial maximum N concentration in plant organs [kg N ]           */
    Grid->crp->N_st.leaves = Grid->crp->N_st.Max_lv * Grid->crp->st.leaves;
    Grid->crp->N_st.stems = Grid->crp->N_st.Max_st * Grid->crp->st.stems;
    Grid->crp->N_st.roots = Grid->crp->N_st.Max_ro * Grid->crp->st.roots;
    Grid->crp->N_st.storage = 0.;

    /* Initial maximum P concentration in plant organs per kg biomass [kg N kg-1 dry biomass]   */
    Grid->crp->P_st.Max_lv =
        Afgen(Grid->crp->prm.P_MaxLeaves, &(Grid->crp->st.Development));
    Grid->crp->P_st.Max_st = Grid->crp->prm.P_MaxStems * Grid->crp->P_st.Max_lv;
    Grid->crp->P_st.Max_ro = Grid->crp->prm.P_MaxRoots * Grid->crp->P_st.Max_lv;
    Grid->crp->P_st.Max_so = 0.;

    /* Initial maximum P concentration in plant organs [kg N ] */
    Grid->crp->P_st.leaves = Grid->crp->P_st.Max_lv * Grid->crp->st.leaves;
    Grid->crp->P_st.stems = Grid->crp->P_st.Max_st * Grid->crp->st.stems;
    Grid->crp->P_st.roots = Grid->crp->P_st.Max_ro * Grid->crp->st.roots;
    Grid->crp->P_st.storage = 0.;

    /* Initial maximum K concentration in plant organs per kg biomass [kg N kg-1 dry biomass]    */
    Grid->crp->K_st.Max_lv =
        Afgen(Grid->crp->prm.K_MaxLeaves, &(Grid->crp->st.Development));
    Grid->crp->K_st.Max_st = Grid->crp->prm.K_MaxStems * Grid->crp->K_st.Max_lv;
    Grid->crp->K_st.Max_ro = Grid->crp->prm.K_MaxRoots * Grid->crp->K_st.Max_lv;
    Grid->crp->K_st.Max_so = 0.;

    /* Initial maximum k concentration in plant organs [kg N ] */
    Grid->crp->K_st.leaves = Grid->crp->K_st.Max_lv * Grid->crp->st.leaves;
    Grid->crp->K_st.stems = Grid->crp->K_st.Max_st * Grid->crp->st.stems;
    Grid->crp->K_st.roots = Grid->crp->K_st.Max_ro * Grid->crp->st.roots;
    Grid->crp->K_st.storage = 0.;

    /* No nutrient losses at initialization */
    Grid->crp->N_st.death_lv = 0.;
    Grid->crp->N_st.death_st = 0.;
    Grid->crp->N_st.death_ro = 0.;

    Grid->crp->P_st.death_lv = 0.;
    Grid->crp->P_st.death_st = 0.;
    Grid->crp->P_st.death_ro = 0.;

    Grid->crp->K_st.death_lv = 0.;
    Grid->crp->K_st.death_st = 0.;
    Grid->crp->K_st.death_ro = 0.;

    /* Set the initial demand to zero*/
    Grid->crp->N_st.Demand = 0.;
    Grid->crp->N_st.Demand_lv = 0.;
    Grid->crp->N_st.Demand_st = 0.;
    Grid->crp->N_st.Demand_so = 0.;
    Grid->crp->N_st.Demand_ro = 0.;

    Grid->crp->P_st.Demand = 0.;
    Grid->crp->P_st.Demand_lv = 0.;
    Grid->crp->P_st.Demand_st = 0.;
    Grid->crp->P_st.Demand_so = 0.;
    Grid->crp->P_st.Demand_ro = 0.;

    Grid->crp->K_st.Demand = 0.;
    Grid->crp->K_st.Demand_lv = 0.;
    Grid->crp->K_st.Demand_st = 0.;
    Grid->crp->K_st.Demand_so = 0.;
    Grid->crp->K_st.Demand_ro = 0.;

    /* Set the initial uptake to zero*/
    Grid->crp->N_st.Uptake = 0.;
    Grid->crp->N_st.Uptake_lv = 0.;
    Grid->crp->N_st.Uptake_st = 0.;
    Grid->crp->N_st.Uptake_ro = 0.;

    Grid->crp->P_st.Uptake = 0.;
    Grid->crp->P_st.Uptake_lv = 0.;
    Grid->crp->P_st.Uptake_st = 0.;
    Grid->crp->P_st.Uptake_ro = 0.;

    Grid->crp->K_st.Uptake = 0.;
    Grid->crp->K_st.Uptake_lv = 0.;
    Grid->crp->K_st.Uptake_st = 0.;
    Grid->crp->K_st.Uptake_ro = 0.;


    /* Set the soil nutrient states */
    Grid->ste->st_N_tot = List(Grid->mng->N_Fert_table) *
                          Grid->mng->N_Uptake_frac;
    Grid->ste->st_P_tot = List(Grid->mng->P_Fert_table) *
                          Grid->mng->P_Uptake_frac;
    Grid->ste->st_K_tot = List(Grid->mng->K_Fert_table) *
                          Grid->mng->K_Uptake_frac;

    Grid->ste->st_N_mins = Grid->mng->N_Mins;
    Grid->ste->st_P_mins = Grid->mng->P_Mins;
    Grid->ste->st_K_mins = Grid->mng->K_Mins;

    /* No nutrient stress at initialization */
    Grid->crp->N_st.Indx = 1.;
    Grid->crp->P_st.Indx = 1.;
    Grid->crp->K_st.Indx = 1.;
    Grid->crp->NPK_Indx = 1.;

    /* Set the initial optimal leave concentrations to zero */
    Grid->crp->N_st.Optimum_lv = 0;
    Grid->crp->N_st.Optimum_st = 0;
}
