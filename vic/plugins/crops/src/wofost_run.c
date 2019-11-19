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
    if (Grid->crp->Sowing >= 1 && Grid->crp->Emergence == 0)
    {
        if (EmergenceCrop(Grid, Emergence))
        {                 
            /* Initialize: set state variables */
            InitializeCrop(Grid);
            InitializeNutrients(Grid);
        }  
    }

    if (Grid->crp->Sowing >= 1 && Grid->crp->Emergence == 1)
    {   
        if (Grid->crp->st.Development <= (Grid->crp->prm.DevelopStageHarvest) && Grid->crp->GrowthDay < Grid->met->CycleLength) 
        {
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

            /* Update the number of days that the crop has grown*/
            Grid->crp->GrowthDay++;

        }
        else
        {
            CleanHarvest(Grid);
            Emergence = 0;
            Grid->crp->TSumEmergence = 0;
            Grid->crp->Emergence = 0;
            Grid->crp->Sowing    = 0;
        }
    }
}