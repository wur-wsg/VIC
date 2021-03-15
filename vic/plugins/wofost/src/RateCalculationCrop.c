#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function RateCalculationCrop()                                            */
/*  Purpose: Calculate the net amount of assimilates that is available for    */
/*  crop growth and subsequently establish the crop growth rates for the      */
/*  plant organs (kg ha-1 d-1).                                               */
/* ---------------------------------------------------------------------------*/

void
RateCalculationCrop(SimUnit *Grid)
{
    float TotalAssimilation;
    float Maintenance;
    float GrossAssimilation;
    float GrossGrowth;
    float Stress;

    /* Development rate calculation */
    DevelopmentRate(Grid);

    /* Assimilation */
    GrossAssimilation = DailyTotalAssimilation(Grid);

    /* Stress: either nutrient shortage or water shortage */
    Stress = min(Grid->crp->NutrientStress, Grid->soil->WaterStress);

    /* Correction for low minimum temperatures and stress factors */
    TotalAssimilation = Stress * Correct(Grid, GrossAssimilation);

    /* Respiration */
    Maintenance = RespirationRef(Grid, TotalAssimilation);

    /* Conversion */
    GrossGrowth = Conversion(Grid, TotalAssimilation - Maintenance);

    /* Growth of roots, stems, leaves and storage organs */
    Growth(Grid, GrossGrowth);

    NutrientLoss(Grid);

    CropNutrientRates(Grid);
}
