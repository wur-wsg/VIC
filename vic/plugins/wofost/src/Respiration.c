#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function RespirationRef(float TotalAssimilation)                          */
/*  Purpose: Calculation of the crop respiration rate (kg ha-1 d-1). Note     */
/*  that the respiration rate can not exceed the total assimilation rate.     */
/* ---------------------------------------------------------------------------*/

float
RespirationRef(SimUnit *Grid,
               float    TotalAssimilation)
{
    float respiration;
    float TempRef = 25.;
    float tmp_min;

    respiration = Grid->crp->prm.RelRespiLeaves * Grid->crp->st.leaves;
    respiration += Grid->crp->prm.RelRespiStorage * Grid->crp->st.storage;
    respiration += Grid->crp->prm.RelRespiRoots * Grid->crp->st.roots;
    respiration += Grid->crp->prm.RelRespiStems * Grid->crp->st.stems;
    respiration *=
        Afgen(Grid->crp->prm.FactorSenescence, &(Grid->crp->st.Development));
    respiration *= pow(Grid->crp->prm.Q10, 0.1 * (Grid->met->Temp - TempRef));

    /* respiration can not exceed the assimilation */
    tmp_min = min(respiration, TotalAssimilation);
    return (tmp_min);
}
