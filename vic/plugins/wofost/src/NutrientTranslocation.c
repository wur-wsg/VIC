#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function Translocation()                                                  */
/*  Purpose: To compute the amount of nutrients in the organs that can        */
/*  be translocated kg ha-1                                                   */
/* ---------------------------------------------------------------------------*/
void
NutrientTranslocation(SimUnit *Grid)
{
    /* N amount available for translocation */
    Grid->crp->N_st.Avail_lv = max(0.,
                                   Grid->crp->N_st.leaves - Grid->crp->st.leaves *
                                   Grid->crp->prm.N_ResidualFrac_lv);
    Grid->crp->N_st.Avail_st = max(0.,
                                   Grid->crp->N_st.stems - Grid->crp->st.stems *
                                   Grid->crp->prm.N_ResidualFrac_st);
    Grid->crp->N_st.Avail_ro =
        max((Grid->crp->N_st.Avail_lv + Grid->crp->N_st.Avail_st) *
            Grid->crp->prm.FracTranslocRoots,
            Grid->crp->N_st.roots - Grid->crp->st.roots *
            Grid->crp->prm.N_ResidualFrac_ro);


    /* P amount available for translocation */
    Grid->crp->P_st.Avail_lv = max(0.,
                                   Grid->crp->P_st.leaves - Grid->crp->st.leaves *
                                   Grid->crp->prm.P_ResidualFrac_lv);
    Grid->crp->P_st.Avail_st = max(0.,
                                   Grid->crp->P_st.stems - Grid->crp->st.stems *
                                   Grid->crp->prm.P_ResidualFrac_st);
    Grid->crp->P_st.Avail_ro =
        max(
            (Grid->crp->P_st.Avail_lv + Grid->crp->P_st.Avail_st) * Grid->crp->prm.FracTranslocRoots,
            Grid->crp->P_st.roots - Grid->crp->st.roots *
            Grid->crp->prm.P_ResidualFrac_ro);


    /* K amount available for translocation */
    Grid->crp->K_st.Avail_lv = max(0.,
                                   Grid->crp->K_st.leaves - Grid->crp->st.leaves *
                                   Grid->crp->prm.K_ResidualFrac_lv);
    Grid->crp->K_st.Avail_st = max(0.,
                                   Grid->crp->K_st.stems - Grid->crp->st.stems *
                                   Grid->crp->prm.K_ResidualFrac_st);
    Grid->crp->K_st.Avail_ro =
        max(
            (Grid->crp->K_st.Avail_lv + Grid->crp->K_st.Avail_st) * Grid->crp->prm.FracTranslocRoots,
            Grid->crp->K_st.roots - Grid->crp->st.roots *
            Grid->crp->prm.K_ResidualFrac_ro);

    /* Total available amount of nutrients */
    Grid->crp->N_st.Avail = Grid->crp->N_st.Avail_lv +
                            Grid->crp->N_st.Avail_st + Grid->crp->N_st.Avail_ro;
    Grid->crp->P_st.Avail = Grid->crp->P_st.Avail_lv +
                            Grid->crp->P_st.Avail_st + Grid->crp->P_st.Avail_ro;
    Grid->crp->K_st.Avail = Grid->crp->K_st.Avail_lv +
                            Grid->crp->K_st.Avail_st + Grid->crp->K_st.Avail_ro;


    /* Total available nutrient amount for translocation */
    if (Grid->crp->st.Development > Grid->crp->prm.DevelopmentStageNT) {
        Grid->crp->N_rt.Supply =
            (Grid->crp->N_st.Avail_lv + Grid->crp->N_st.Avail_st +
             Grid->crp->N_st.Avail_ro) / Grid->crp->prm.TCNT;
        Grid->crp->P_rt.Supply =
            (Grid->crp->P_st.Avail_lv + Grid->crp->P_st.Avail_st +
             Grid->crp->P_st.Avail_ro) / Grid->crp->prm.TCPT;
        Grid->crp->K_rt.Supply =
            (Grid->crp->K_st.Avail_lv + Grid->crp->K_st.Avail_st +
             Grid->crp->K_st.Avail_ro) / Grid->crp->prm.TCKT;
    }
    else {
        Grid->crp->N_rt.Supply = 0.;
        Grid->crp->P_rt.Supply = 0.;
        Grid->crp->K_rt.Supply = 0.;
    }
}
