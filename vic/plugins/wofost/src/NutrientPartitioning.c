#include <vic_driver_image.h>
#include <plugin.h>

/* -------------------------------------------------------------------------*/
/*  function NutrientPartioning()                                           */
/*  Purpose: To compute the partitioning of the total N/P/K uptake rate     */
/*           (N,P,K UPTR) over leaves, stem, and roots kg  ha-1 d-1         */
/* -------------------------------------------------------------------------*/
void
NutrientPartioning(SimUnit *Grid)
{
    extern plugin_option_struct       plugin_options;
    extern plugin_global_param_struct plugin_global_param;

    float                             NutrientLimit;
    float                             N_Fix_rt;

    float                             tiny = 0.001;
    float                             tmp_min;
    float                             tmp_max;

    Grid->crp->N_rt.Demand = Grid->crp->N_rt.Demand_lv +
                             Grid->crp->N_rt.Demand_st +
                             Grid->crp->N_rt.Demand_ro;
    Grid->crp->P_rt.Demand = Grid->crp->P_rt.Demand_lv +
                             Grid->crp->P_rt.Demand_st +
                             Grid->crp->P_rt.Demand_ro;
    Grid->crp->K_rt.Demand = Grid->crp->K_rt.Demand_lv +
                             Grid->crp->K_rt.Demand_st +
                             Grid->crp->K_rt.Demand_ro;

    /* No nutrients are absorbed from the soil after development stage DevelopmentStageNLimit or */
    /* when severe water shortage occurs                                           */
    NutrientLimit = 0.;
    if (Grid->crp->st.Development < Grid->crp->prm.DevelopmentStageNLimit &&
        Grid->soil->WaterStress > 0.01) {
        NutrientLimit = 1.;
    }

    // tmp_max = max(0.02, 1.-Grid->crp->prm.N_fixation);
    // N_Fix_rt= max(0.,Grid->crp->N_rt.Uptake * Grid->crp->prm.N_fixation / tmp_max);
    tmp_max = max(0., Grid->crp->prm.N_fixation * Grid->crp->N_rt.Demand);
    N_Fix_rt = (tmp_max * NutrientLimit);

    /* Nutrient uptake cannot be larger than the availability and is larger or equal to zero */
    tmp_min =
        min(Grid->crp->N_rt.Demand - N_Fix_rt,
            (Grid->ste->st_N_tot + Grid->ste->rt_N_tot));
    if (plugin_options.WOFOST_PFERT) {
        tmp_min = Grid->crp->N_rt.Demand - N_Fix_rt;
    }
    tmp_max = max(0., tmp_min);
    Grid->crp->N_rt.Uptake = tmp_max * NutrientLimit /
                             plugin_global_param.wofost_steps_per_day;

    tmp_min =
        min(Grid->crp->P_rt.Demand,
            (Grid->ste->st_P_tot + Grid->ste->rt_P_tot));
    if (plugin_options.WOFOST_PFERT) {
        tmp_min = Grid->crp->P_rt.Demand;
    }
    tmp_max = max(0., tmp_min);
    Grid->crp->P_rt.Uptake = tmp_max * NutrientLimit /
                             plugin_global_param.wofost_steps_per_day;

    tmp_min =
        min(Grid->crp->K_rt.Demand,
            (Grid->ste->st_K_tot + Grid->ste->rt_K_tot));
    if (plugin_options.WOFOST_PFERT) {
        tmp_min = Grid->crp->K_rt.Demand;
    }
    tmp_max = max(0., tmp_min);
    Grid->crp->K_rt.Uptake = tmp_max * NutrientLimit /
                             plugin_global_param.wofost_steps_per_day;

    /* N uptake per crop organ kg ha-1 d-1*/
    if (Grid->crp->N_rt.Demand > tiny) {
        Grid->crp->N_rt.Uptake_lv =
            (Grid->crp->N_rt.Demand_lv / Grid->crp->N_rt.Demand) *
            (Grid->crp->N_rt.Uptake + N_Fix_rt);
        Grid->crp->N_rt.Uptake_st =
            (Grid->crp->N_rt.Demand_st / Grid->crp->N_rt.Demand) *
            (Grid->crp->N_rt.Uptake + N_Fix_rt);
        Grid->crp->N_rt.Uptake_ro =
            (Grid->crp->N_rt.Demand_ro / Grid->crp->N_rt.Demand) *
            (Grid->crp->N_rt.Uptake + N_Fix_rt);
    }
    else {
        Grid->crp->N_rt.Uptake_lv = 0.;
        Grid->crp->N_rt.Uptake_st = 0.;
        Grid->crp->N_rt.Uptake_ro = 0.;
    }


    /* P uptake per crop organ kg ha-1 d-1 */
    if (Grid->crp->P_rt.Demand > tiny) {
        Grid->crp->P_rt.Uptake_lv =
            (Grid->crp->P_rt.Demand_lv / Grid->crp->P_rt.Demand) *
            Grid->crp->P_rt.Uptake;
        Grid->crp->P_rt.Uptake_st =
            (Grid->crp->P_rt.Demand_st / Grid->crp->P_rt.Demand) *
            Grid->crp->P_rt.Uptake;
        Grid->crp->P_rt.Uptake_ro =
            (Grid->crp->P_rt.Demand_ro / Grid->crp->P_rt.Demand) *
            Grid->crp->P_rt.Uptake;
    }
    else {
        Grid->crp->P_rt.Uptake_lv = 0.;
        Grid->crp->P_rt.Uptake_st = 0.;
        Grid->crp->P_rt.Uptake_ro = 0.;
    }

    /* K uptake per crop organ kg ha-1 d-1*/
    if (Grid->crp->K_rt.Demand > tiny) {
        Grid->crp->K_rt.Uptake_lv =
            (Grid->crp->K_rt.Demand_lv / Grid->crp->K_rt.Demand) *
            Grid->crp->K_rt.Uptake;
        Grid->crp->K_rt.Uptake_st =
            (Grid->crp->K_rt.Demand_st / Grid->crp->K_rt.Demand) *
            Grid->crp->K_rt.Uptake;
        Grid->crp->K_rt.Uptake_ro =
            (Grid->crp->K_rt.Demand_ro / Grid->crp->K_rt.Demand) *
            Grid->crp->K_rt.Uptake;
    }
    else {
        Grid->crp->K_rt.Uptake_lv = 0.;
        Grid->crp->K_rt.Uptake_st = 0.;
        Grid->crp->K_rt.Uptake_ro = 0.;
    }
}
