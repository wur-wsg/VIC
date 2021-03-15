#include <vic_driver_image.h>
#include <plugin.h>

/* --------------------------------------------------------------------*/
/*  function NutritionINDX()                                           */
/*  Purpose: To compute N,P,K Nutrition Index (-)                      */
/* --------------------------------------------------------------------*/
void
NutritionINDX(SimUnit *Grid)
{
    float VegetativeMass;

    float N_opt_veg = 0.;
    float P_opt_veg = 0.;
    float K_opt_veg = 0.;

    float N_Veg = 0.;
    float P_Veg = 0.;
    float K_Veg = 0.;

    float N_res = 0.;
    float P_res = 0.;
    float K_res = 0.;

    /* Total vegetative living above-ground biomass (kg DM ha-1)     */
    VegetativeMass = Grid->crp->st.leaves + Grid->crp->st.stems;


    if (VegetativeMass > 0.) {
        /* N,P,K concentration in total vegetative living per */
        /* kg above-ground biomass  (kg N,P,K kg-1 DM)        */
        N_Veg =
            (Grid->crp->N_st.leaves + Grid->crp->N_st.stems +
             Grid->crp->N_rt.Uptake_lv + Grid->crp->N_rt.Uptake_st) /
            VegetativeMass;
        P_Veg =
            (Grid->crp->P_st.leaves + Grid->crp->P_st.stems +
             Grid->crp->P_rt.Uptake_lv + Grid->crp->P_rt.Uptake_st) /
            VegetativeMass;
        K_Veg =
            (Grid->crp->K_st.leaves + Grid->crp->K_st.stems +
             Grid->crp->K_rt.Uptake_lv + Grid->crp->K_rt.Uptake_st) /
            VegetativeMass;

        /* Residual N,P,K concentration in total vegetative living */
        /* above-ground biomass  (kg N,P,K kg-1 DM)                */
        N_res =
            (Grid->crp->st.leaves * Grid->crp->prm.N_ResidualFrac_lv +
             Grid->crp->st.stems * Grid->crp->prm.N_ResidualFrac_st) /
            VegetativeMass;
        P_res =
            (Grid->crp->st.leaves * Grid->crp->prm.P_ResidualFrac_lv +
             Grid->crp->st.stems * Grid->crp->prm.P_ResidualFrac_st) /
            VegetativeMass;
        K_res =
            (Grid->crp->st.leaves * Grid->crp->prm.K_ResidualFrac_lv +
             Grid->crp->st.stems * Grid->crp->prm.K_ResidualFrac_st) /
            VegetativeMass;

        N_opt_veg = (Grid->crp->N_st.Optimum_lv + Grid->crp->N_st.Optimum_st) /
                    VegetativeMass;
        P_opt_veg = (Grid->crp->P_st.Optimum_lv + Grid->crp->P_st.Optimum_st) /
                    VegetativeMass;
        K_opt_veg = (Grid->crp->K_st.Optimum_lv + Grid->crp->K_st.Optimum_st) /
                    VegetativeMass;
    }

    float tiny = 0.001;
    if ((N_opt_veg - N_res) > tiny) {
        Grid->crp->N_st.Indx =
            limit(tiny, 1.0, (N_Veg - N_res) / (N_opt_veg - N_res));
    }
    else {
        Grid->crp->N_st.Indx = tiny;
    }

    if ((P_opt_veg - P_res) > tiny) {
        Grid->crp->P_st.Indx =
            limit(tiny, 1.0, (P_Veg - P_res) / (P_opt_veg - P_res));
    }
    else {
        Grid->crp->P_st.Indx = tiny;
    }

    if ((K_opt_veg - K_res) > tiny) {
        Grid->crp->K_st.Indx =
            limit(tiny, 1.0, (K_Veg - K_res) / (K_opt_veg - K_res));
    }
    else {
        Grid->crp->K_st.Indx = tiny;
    }

    // Grid->crp->N_st.Indx = 1.0;
    // Grid->crp->P_st.Indx = 1.0;
    // Grid->crp->K_st.Indx = 1.0;


    Grid->crp->NPK_Indx =
        (Grid->crp->N_st.Indx <
         Grid->crp->P_st.Indx) ? Grid->crp->N_st.Indx : Grid->crp->P_st.Indx;
    Grid->crp->NPK_Indx =
        (Grid->crp->NPK_Indx <
         Grid->crp->K_st.Indx) ? Grid->crp->NPK_Indx : Grid->crp->K_st.Indx;

    /* Nutrient reduction factor */
    Grid->crp->NutrientStress =
        limit(0., 1.0,
              1. - Grid->crp->prm.NLUE *
              pow((1.0001 - Grid->crp->NPK_Indx), 2));
}
