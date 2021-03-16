#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function NutrientRates()                                                  */
/*  Purpose: To calculate nutrient changes (i.e. rates) in the plant          */
/*           organs (kg N,P,K ha-1 d-1)                                       */
/* ---------------------------------------------------------------------------*/

void
CropNutrientRates(SimUnit *Grid)
{
    float Avail_N;
    float Avail_P;
    float Avail_K;

    Grid->crp->N_rt.leaves = Grid->crp->N_rt.Uptake_lv -
                             Grid->crp->N_rt.Transloc_lv -
                             Grid->crp->N_rt.death_lv;
    Grid->crp->N_rt.stems = Grid->crp->N_rt.Uptake_st -
                            Grid->crp->N_rt.Transloc_st -
                            Grid->crp->N_rt.death_st;
    Grid->crp->N_rt.roots = Grid->crp->N_rt.Uptake_ro -
                            Grid->crp->N_rt.Transloc_ro -
                            Grid->crp->N_rt.death_ro;

    Grid->crp->P_rt.leaves = Grid->crp->P_rt.Uptake_lv -
                             Grid->crp->P_rt.Transloc_lv -
                             Grid->crp->P_rt.death_lv;
    Grid->crp->P_rt.stems = Grid->crp->P_rt.Uptake_st -
                            Grid->crp->P_rt.Transloc_st -
                            Grid->crp->P_rt.death_st;
    Grid->crp->P_rt.roots = Grid->crp->P_rt.Uptake_ro -
                            Grid->crp->P_rt.Transloc_ro -
                            Grid->crp->P_rt.death_ro;

    Grid->crp->K_rt.leaves = Grid->crp->K_rt.Uptake_lv -
                             Grid->crp->K_rt.Transloc_lv -
                             Grid->crp->K_rt.death_lv;
    Grid->crp->K_rt.stems = Grid->crp->K_rt.Uptake_st -
                            Grid->crp->K_rt.Transloc_st -
                            Grid->crp->K_rt.death_st;
    Grid->crp->K_rt.roots = Grid->crp->K_rt.Uptake_ro -
                            Grid->crp->K_rt.Transloc_ro -
                            Grid->crp->K_rt.death_ro;


    /* Rate of N,P,K uptake in storage organs (kg N,P,K ha-1 d-1) */
    if (Grid->crp->st.Development > Grid->crp->prm.DevelopmentStageNT) {
        Grid->crp->N_rt.storage = min(Grid->crp->N_rt.Demand_so,
                                      Grid->crp->N_rt.Supply);
        Grid->crp->P_rt.storage = min(Grid->crp->P_rt.Demand_so,
                                      Grid->crp->P_rt.Supply);
        Grid->crp->K_rt.storage = min(Grid->crp->K_rt.Demand_so,
                                      Grid->crp->K_rt.Supply);
    }

    /* Total available nutrients for translocation */
    Avail_N = Grid->crp->N_st.Avail_lv + Grid->crp->N_st.Avail_st +
              Grid->crp->N_st.Avail_ro;
    Avail_P = Grid->crp->P_st.Avail_lv + Grid->crp->P_st.Avail_st +
              Grid->crp->P_st.Avail_ro;
    Avail_K = Grid->crp->K_st.Avail_lv + Grid->crp->K_st.Avail_st +
              Grid->crp->K_st.Avail_ro;


    /* Actual N translocation rate partitioned over the organs */
    if (Avail_N > 0.001) {
        Grid->crp->N_rt.Transloc_lv = Grid->crp->N_rt.storage *
                                      Grid->crp->N_st.Avail_lv / Avail_N;
        Grid->crp->N_rt.Transloc_st = Grid->crp->N_rt.storage *
                                      Grid->crp->N_st.Avail_st / Avail_N;
        Grid->crp->N_rt.Transloc_ro = Grid->crp->N_rt.storage *
                                      Grid->crp->N_st.Avail_ro / Avail_N;
    }
    else {
        Grid->crp->N_rt.Transloc_lv = 0.;
        Grid->crp->N_rt.Transloc_st = 0.;
        Grid->crp->N_rt.Transloc_ro = 0.;
    }

    /* Actual P translocation rate partitioned over the organs */
    if (Avail_P > 0.001) {
        Grid->crp->P_rt.Transloc_lv = Grid->crp->P_rt.storage *
                                      Grid->crp->P_st.Avail_lv / Avail_P;
        Grid->crp->P_rt.Transloc_st = Grid->crp->P_rt.storage *
                                      Grid->crp->P_st.Avail_st / Avail_P;
        Grid->crp->P_rt.Transloc_ro = Grid->crp->P_rt.storage *
                                      Grid->crp->P_st.Avail_ro / Avail_P;
    }
    else {
        Grid->crp->P_rt.Transloc_lv = 0.;
        Grid->crp->P_rt.Transloc_st = 0.;
        Grid->crp->P_rt.Transloc_ro = 0.;
    }

    /* Actual P translocation rate partitioned over the organs */
    if (Avail_K > 0.001) {
        Grid->crp->K_rt.Transloc_lv = Grid->crp->K_rt.storage *
                                      Grid->crp->K_st.Avail_lv / Avail_K;
        Grid->crp->K_rt.Transloc_st = Grid->crp->K_rt.storage *
                                      Grid->crp->K_st.Avail_st / Avail_K;
        Grid->crp->K_rt.Transloc_ro = Grid->crp->K_rt.storage *
                                      Grid->crp->K_st.Avail_ro / Avail_K;
    }
    else {
        Grid->crp->K_rt.Transloc_lv = 0.;
        Grid->crp->K_rt.Transloc_st = 0.;
        Grid->crp->K_rt.Transloc_ro = 0.;
    }
}
