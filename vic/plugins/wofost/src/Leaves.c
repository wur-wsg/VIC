#include <vic_driver_image.h>
#include <plugin.h>

/* ------------------------------------------------------------------------*/
/*  function DyingLeaves()                                                 */
/*  Purpose: To compute the amount of dying leaves as a result of shading, */
/*  (i.e. high LAI), stress or age                                         */
/* ------------------------------------------------------------------------*/

float
DyingLeaves(SimUnit *Grid)
{
    float  tiny = 0.001;
    float  Death, Death1, Death2, DeathStress, DeathAge;
    float  CriticalLAI;
    Green *wipe;

    /* Dying rate of leaves due to water stress or high LAI */
    CriticalLAI = 3.2 /
                  Afgen(Grid->crp->prm.KDiffuseTb,
                        &(Grid->crp->st.Development));
    Death1 = Grid->crp->st.leaves * (1. - Grid->soil->WaterStress) *
             Grid->crp->prm.MaxRelDeathRate;
    Death2 = Grid->crp->st.leaves *
             limit(0., 0.03,
                   0.03 * (Grid->crp->st.LAI - CriticalLAI) / CriticalLAI);
    Death = max(Death1, Death2);

    /* Death rate increase due to nutrient shortage */
    Death += Grid->crp->st.leaves * Grid->crp->prm.DyingLeaves_NPK_Stress *
             (1. - Grid->crp->NPK_Indx);

    if (Death < tiny) {
        Death = 0;
    }

    DeathStress = Death;

    if (Grid->crp->LeaveProperties != NULL) {
        /* Oldest leave classes are at the beginning of the list */
        while (Death > Grid->crp->LeaveProperties->weight) {
            Death = Death - Grid->crp->LeaveProperties->weight;
            wipe = Grid->crp->LeaveProperties;
            Grid->crp->LeaveProperties = Grid->crp->LeaveProperties->next;
            free(wipe);
        }
    }


    DeathAge = 0; // Amount of death leaves due to ageing
    if (Grid->crp->LeaveProperties != NULL) {
        Grid->crp->LeaveProperties->weight =
            Grid->crp->LeaveProperties->weight - Death;

        while (Grid->crp->LeaveProperties != NULL &&
               Grid->crp->LeaveProperties->age > Grid->crp->prm.LifeSpan) {
            wipe = Grid->crp->LeaveProperties;
            DeathAge += Grid->crp->LeaveProperties->weight;
            Grid->crp->LeaveProperties = Grid->crp->LeaveProperties->next;
            free(wipe);
        }
    }

    /* Return the amount of death leaves due to stress and due to ageing */
    return (DeathStress + DeathAge);
}
