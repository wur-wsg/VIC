#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function NutrientMax()                                                    */
/*  Purpose: To calculate the max nutrient concentration in the stems, leaves */
/*           and roots organs (kg N,P,K ha-1)                                 */
/* ---------------------------------------------------------------------------*/
void
NutrientMax(SimUnit *Grid)
{
    /* Maximum N,P,K concentration in the leaves, from which the */
    /* N,P,K concentration in the stems and roots is derived     */
    /* as a function of development stage (kg N kg-1 DM)         */
    Grid->crp->N_st.Max_lv =
        Afgen(Grid->crp->prm.N_MaxLeaves, &(Grid->crp->st.Development));
    Grid->crp->P_st.Max_lv =
        Afgen(Grid->crp->prm.P_MaxLeaves, &(Grid->crp->st.Development));
    Grid->crp->K_st.Max_lv =
        Afgen(Grid->crp->prm.K_MaxLeaves, &(Grid->crp->st.Development));

    /* Maximum N concentrations in stems and roots (kg N kg-1) */
    Grid->crp->N_st.Max_st = Grid->crp->prm.N_MaxStems * Grid->crp->N_st.Max_lv;
    Grid->crp->N_st.Max_ro = Grid->crp->prm.N_MaxRoots * Grid->crp->N_st.Max_lv;
    Grid->crp->N_st.Max_so = Grid->crp->prm.Max_N_storage;

    /* Maximum P concentrations in stems and roots (kg P kg-1) */
    Grid->crp->P_st.Max_st = Grid->crp->prm.P_MaxStems * Grid->crp->P_st.Max_lv;
    Grid->crp->P_st.Max_ro = Grid->crp->prm.P_MaxRoots * Grid->crp->P_st.Max_lv;
    Grid->crp->P_st.Max_so = Grid->crp->prm.Max_P_storage;

    /* Maximum K concentrations in stems and roots (kg K kg-1) */
    Grid->crp->K_st.Max_st = Grid->crp->prm.K_MaxStems * Grid->crp->K_st.Max_lv;
    Grid->crp->K_st.Max_ro = Grid->crp->prm.K_MaxRoots * Grid->crp->K_st.Max_lv;
    Grid->crp->K_st.Max_so = Grid->crp->prm.Max_K_storage;
}

/* ---------------------------------------------------------------------------*/
/*  function NutrientOptimum()                                                */
/*  Purpose: To compute the optimal nutrient concentration in the crop        */
/*  organs (kg N,P,K ha-1 )                                                  */
/* ---------------------------------------------------------------------------*/
void
NutrientOptimum(SimUnit *Grid)
{
    /* Optimum N,P,K amount in vegetative above-ground living biomass */
    /* and its N concentration                                        */
    Grid->crp->N_st.Optimum_lv = Grid->crp->prm.Opt_N_Frac *
                                 Grid->crp->N_st.Max_lv * Grid->crp->st.leaves;
    Grid->crp->N_st.Optimum_st = Grid->crp->prm.Opt_N_Frac *
                                 Grid->crp->N_st.Max_st * Grid->crp->st.stems;

    Grid->crp->P_st.Optimum_lv = Grid->crp->prm.Opt_P_Frac *
                                 Grid->crp->P_st.Max_lv * Grid->crp->st.leaves;
    Grid->crp->P_st.Optimum_st = Grid->crp->prm.Opt_P_Frac *
                                 Grid->crp->P_st.Max_st * Grid->crp->st.stems;

    Grid->crp->K_st.Optimum_lv = Grid->crp->prm.Opt_K_Frac *
                                 Grid->crp->K_st.Max_lv * Grid->crp->st.leaves;
    Grid->crp->K_st.Optimum_st = Grid->crp->prm.Opt_K_Frac *
                                 Grid->crp->K_st.Max_st * Grid->crp->st.stems;
}

/* ----------------------------------------------------------------------------*/
/*  function NutrientDemand()                                                  */
/*  Purpose: To compute the nutrient demand of crop organs (kg N,P,K ha-1 d-1) */
/* ----------------------------------------------------------------------------*/
void
NutrientDemand(SimUnit *Grid)
{
    float tmp_max;

    Grid->crp->N_rt.Demand_lv = max(
        Grid->crp->N_st.Max_lv * Grid->crp->st.leaves - Grid->crp->N_st.leaves,
        0.);
    Grid->crp->N_rt.Demand_st = max(
        Grid->crp->N_st.Max_st * Grid->crp->st.stems - Grid->crp->N_st.stems,
        0.);
    Grid->crp->N_rt.Demand_ro = max(
        Grid->crp->N_st.Max_ro * Grid->crp->st.roots - Grid->crp->N_st.roots,
        0.);
    tmp_max = max(
        Grid->crp->N_st.Max_so * Grid->crp->st.storage - Grid->crp->N_st.storage,
        0.);
    Grid->crp->N_rt.Demand_so = tmp_max / Grid->crp->prm.TCNT;

    Grid->crp->P_rt.Demand_lv = max(
        Grid->crp->P_st.Max_lv * Grid->crp->st.leaves - Grid->crp->P_st.leaves,
        0.);
    Grid->crp->P_rt.Demand_st = max(
        Grid->crp->P_st.Max_st * Grid->crp->st.stems - Grid->crp->P_st.stems,
        0.);
    Grid->crp->P_rt.Demand_ro = max(
        Grid->crp->P_st.Max_ro * Grid->crp->st.roots - Grid->crp->P_st.roots,
        0.);
    tmp_max = max(
        Grid->crp->P_st.Max_so * Grid->crp->st.storage - Grid->crp->P_st.storage,
        0.);
    Grid->crp->P_rt.Demand_so = tmp_max / Grid->crp->prm.TCPT;

    Grid->crp->K_rt.Demand_lv = max(
        Grid->crp->K_st.Max_lv * Grid->crp->st.leaves - Grid->crp->K_st.leaves,
        0.);
    Grid->crp->K_rt.Demand_st = max(
        Grid->crp->K_st.Max_st * Grid->crp->st.stems - Grid->crp->K_st.stems,
        0.);
    Grid->crp->K_rt.Demand_ro = max(
        Grid->crp->K_st.Max_ro * Grid->crp->st.roots - Grid->crp->K_st.roots,
        0.);
    tmp_max = max(
        Grid->crp->K_st.Max_so * Grid->crp->st.storage - Grid->crp->K_st.storage,
        0.);
    Grid->crp->K_rt.Demand_so = tmp_max / Grid->crp->prm.TCKT;
}

void
RateCalcultionNutrients(SimUnit *Grid)
{
    NutrientMax(Grid);

    NutrientDemand(Grid);

    /* Establish the optimum nutrient concentrations in the crop organs */
    NutrientOptimum(Grid);

    NutrientLoss(Grid);

    SoilNutrientRates(Grid);

    NutrientPartioning(Grid);

    NutrientTranslocation(Grid);

    CropNutrientRates(Grid);

    SoilNutrientRates(Grid);

    /* Calculate the nutrition index */
    NutritionINDX(Grid);
}
