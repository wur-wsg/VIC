#include <vic_driver_image.h>
#include <plugin.h>

/* -----------------------------------------------------------------------------------------*/
/*  function Partioning()                                                                   */
/*  Purpose: Calculate the partioning factors and correct them for nutrient or water stress */
/* -----------------------------------------------------------------------------------------*/

void
Partioning(SimUnit *Grid)
{
    float factor;
    float flv;

    if (Grid->soil->WaterStress < Grid->crp->N_st.Indx) {
        factor = max(1., 1. / (Grid->soil->WaterStress + 0.5));
        Grid->crp->fac_ro =
            min(0.6, Afgen(Grid->crp->prm.Roots,
                           &(Grid->crp->st.Development)) * factor);
        Grid->crp->fac_lv =
            Afgen(Grid->crp->prm.Leaves, &(Grid->crp->st.Development));
        Grid->crp->fac_st =
            Afgen(Grid->crp->prm.Stems, &(Grid->crp->st.Development));
        Grid->crp->fac_so =
            Afgen(Grid->crp->prm.Storage, &(Grid->crp->st.Development));
    }
    else {
        flv = Afgen(Grid->crp->prm.Leaves, &(Grid->crp->st.Development));
        factor =
            exp(-Grid->crp->prm.N_lv_partitioning *
                (1. - Grid->crp->N_st.Indx));
        Grid->crp->fac_lv = flv * factor;
        Grid->crp->fac_ro =
            Afgen(Grid->crp->prm.Roots, &(Grid->crp->st.Development));
        Grid->crp->fac_st =
            Afgen(Grid->crp->prm.Stems,
                  &(Grid->crp->st.Development)) + flv - Grid->crp->fac_lv;
        Grid->crp->fac_so =
            Afgen(Grid->crp->prm.Storage, &(Grid->crp->st.Development));
    }
}
