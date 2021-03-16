#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------*/
/*  function SoilNutrientRates()                                  */
/*  Purpose: Calculation of the soil nutrient rates kg ha-1 d-1   */
/* ---------------------------------------------------------------*/

void
SoilNutrientRates(SimUnit *Grid)
{
    extern plugin_global_param_struct plugin_global_param;

    float                             N_fert;
    float                             P_fert;
    float                             K_fert;

    if (Grid->crp->st.Development > 0. &&
        Grid->crp->st.Development <= Grid->crp->prm.DevelopmentStageNLimit) { /* NPK rates that come available through mineralization, cannot exceed */
                                                                              /* the available NPK for mineralization                                */
        Grid->ste->rt_N_mins = min(Grid->mng->N_Mins * Grid->mng->NRecoveryFrac,
                                   Grid->ste->st_N_mins);
        Grid->ste->rt_P_mins = min(Grid->mng->P_Mins * Grid->mng->PRecoveryFrac,
                                   Grid->ste->st_P_mins);
        Grid->ste->rt_K_mins = min(Grid->mng->K_Mins * Grid->mng->KRecoveryFrac,
                                   Grid->ste->st_K_mins);
    }
    else {
        Grid->ste->rt_N_mins = 0.;
        Grid->ste->rt_P_mins = 0.;
        Grid->ste->rt_K_mins = 0.;
    }

    /* NPK amount that comes available for the crop at day_fl through fertilizer applications */
    N_fert = List(Grid->mng->N_Fert_table) * Grid->mng->N_Uptake_frac;
    P_fert = List(Grid->mng->P_Fert_table) * Grid->mng->P_Uptake_frac;
    K_fert = List(Grid->mng->K_Fert_table) * Grid->mng->K_Uptake_frac;

    /* Change in total inorganic NPK in soil as function of fertilizer input, */
    /* soil NPK mineralization and crop uptake                                */
    Grid->ste->rt_N_tot = (N_fert / plugin_global_param.wofost_steps_per_day) -
                          Grid->crp->N_rt.Uptake + Grid->ste->rt_N_mins +
                          Grid->mng->N_external;
    Grid->ste->rt_P_tot = (P_fert / plugin_global_param.wofost_steps_per_day) -
                          Grid->crp->P_rt.Uptake + Grid->ste->rt_P_mins +
                          Grid->mng->P_external;
    Grid->ste->rt_K_tot = (K_fert / plugin_global_param.wofost_steps_per_day) -
                          Grid->crp->K_rt.Uptake + Grid->ste->rt_K_mins +
                          Grid->mng->K_external;
}
