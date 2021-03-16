#include <vic_driver_image.h>
#include <plugin.h>

void
wofost_run(SimUnit *Grid)
{
    int Emergence;

    Emergence = Grid->emergence;

    /* Determine if the sowing already has occurred */
    IfSowing(Grid, &Grid->start);

    /* If sowing gas occurred than determine the emergence */
    if (Grid->crp->Sowing >= 1 && Grid->crp->Emergence == 0) {
        Grid->cultivating = 1;
        if (EmergenceCrop(Grid, Emergence)) {
            /* Initialize: set state variables */
            InitializeCrop(Grid);
            InitializeNutrients(Grid);
            Grid->growing = 1;
        }

        /* Update the number of days that the crop was cultivated */
        Grid->crp->CultivateDay++;
    }

    if (Grid->crp->Sowing >= 1 && Grid->crp->Emergence == 1) {
        if (Grid->crp->st.Development <= (Grid->crp->prm.DevelopStageHarvest) &&
            Grid->crp->CultivateDay < Grid->met->CycleLength) {
            Astro(Grid);

            /* Set the rate variables to zero */
            RatesToZero(Grid);

            /* Rate calculations */
            Partioning(Grid);
            RateCalcultionNutrients(Grid);
            RateCalculationCrop(Grid);

            /* Calculate LAI */
            Grid->crp->st.LAI = LeaveAreaIndex(Grid);

            /* State calculations */
            IntegrationCrop(Grid);
            IntegrationNutrients(Grid);

            /* Update the number of days that the crop has grown */
            Grid->crp->CultivateDay++;
            Grid->crp->GrowthDay++;

            /* External accounting*/
            Grid->mng->N_external = 0.;
            Grid->mng->P_external = 0.;
            Grid->mng->K_external = 0.;
        }
        else {
            Harvest(&(Grid->crp->LeaveProperties));
            Emergence = 0;
            Grid->cultivating = 0;
            Grid->growing = 0;
            Grid->crp->CultivateDay = 0;
            Grid->crp->TSumEmergence = 0;
            Grid->crp->Emergence = 0;
            Grid->crp->Sowing = 0;
            Grid->crp->st.LAI = 0;
        }
    }
}
