#include <vic_driver_image.h>
#include <plugin.h>

/* ----------------------------------------------------------------------------*/
/*  function GetDevelopmentStage()                                             */
/*  Purpose: Calculation of the development stage as function of the effective */
/*  daily temperature, day length and the vernalization                        */
/*-----------------------------------------------------------------------------*/

void
DevelopmentRate(SimUnit *Grid)
{
    float VernalizationFactor;

    if (Grid->crp->st.Development < 1.) {
        Grid->crp->rt.Development = Afgen(Grid->crp->prm.DeltaTempSum,
                                          &Grid->met->Temp) /
                                    Grid->crp->prm.TempSum1;
        if (Grid->crp->prm.IdentifyAnthesis == 1 ||
            Grid->crp->prm.IdentifyAnthesis == 2) {
            Grid->crp->rt.Development = Grid->crp->rt.Development *
                                        limit(0., 1.,
                                              (Grid->met->PARDaylength -
                                               Grid->crp->prm.CriticalDaylength) / (
                                                  Grid->crp->prm.
                                                  OptimumDaylength -
                                                  Grid->crp->prm.
                                                  CriticalDaylength));
        }

        /* Vernalization takes place */
        if (Grid->crp->prm.IdentifyAnthesis == 2) {
            Grid->crp->rt.vernalization = insw(Grid->crp->st.Development - 0.3, Afgen(
                                                   Grid->crp->prm.
                                                   VernalizationRate,
                                                   &Grid->met->Temp), 0.);
            VernalizationFactor = limit(0., 1.,
                                        (Grid->crp->st.vernalization -
                                         Grid->crp->prm.BaseVernRequirement) /
                                        (Grid->crp->prm.SatVernRequirement -
                                         Grid->crp->prm.BaseVernRequirement));

            Grid->crp->rt.Development = Grid->crp->rt.Development * insw(
                Grid->crp->st.Development - 0.3, VernalizationFactor, 1.);
        }
    }
    else {
        Grid->crp->rt.Development = Afgen(Grid->crp->prm.DeltaTempSum,
                                          &Grid->met->Temp) /
                                    Grid->crp->prm.TempSum2;
    }
}
