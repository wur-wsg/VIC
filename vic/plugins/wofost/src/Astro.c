#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------*/
/*  function Astro()                                                    */
/*  Purpose: Calculation of the astronomical parameters used in Wofost  */
/*                                                                      */
/*  Originally written in Fortran by:                                   */
/*         Daniel van Kraalingen, April 1991                            */
/*         revised Allard de Wit, January 2011                          */
/* ---------------------------------------------------------------------*/

void
Astro(SimUnit *Grid)
{
    float Declination;
    float SolarConstant;
    float AOB;
    float AOB2;
    float DSinB;
    float FractionDiffuseRad;
    float tmp_min;

    float day_fl = Grid->met->MeteoDay;

    if (fabs(Grid->met->Latitude) > 90.) {
        log_err("Latitude is > 90 or < -90");
    }

    /* We start at Day= 1, we do not use Day = 0 */
    Declination =
        -asin(sin(23.45 * RAD_PER_DEG) *
              cos(2. * CONST_PI * (day_fl + 10.) / 365.));
    SolarConstant = 1370. * (1. + 0.033 * cos(2. * CONST_PI * (day_fl) / 365.));

    Grid->met->SinLD = sin(RAD_PER_DEG * Grid->met->Latitude) *
                       sin(Declination);
    Grid->met->CosLD = cos(RAD_PER_DEG * Grid->met->Latitude) *
                       cos(Declination);
    AOB = Grid->met->SinLD / Grid->met->CosLD;
    tmp_min = min(1, AOB);
    AOB = max(-1, tmp_min);

    /* Astronomical day length */
    tmp_min = min(24., 12.0 * (1. + 2. * asin(AOB) / CONST_PI));
    Grid->met->Daylength = max(0, tmp_min);

    /* Photoactive day length */
    AOB2 = (-sin(-4.0 * RAD_PER_DEG) + Grid->met->SinLD) / Grid->met->CosLD;
    tmp_min = min(1, AOB2);
    AOB2 = max(-1, tmp_min);
    tmp_min = min(24., 12.0 * (1. + 2. * asin(AOB2) / CONST_PI));
    Grid->met->PARDaylength = max(0, tmp_min);

    /* Integrals of sine of solar height */
    if (AOB <= 1.0) {
        DSinB = 3600. *
                (Grid->met->Daylength * Grid->met->SinLD + (24. / CONST_PI) *
                 Grid->met->CosLD * sqrt(1. - AOB * AOB));
        Grid->met->DSinBE = 3600. *
                            (Grid->met->Daylength *
                             (Grid->met->SinLD + 0.4 *
                              (Grid->met->SinLD * Grid->met->SinLD +
                               Grid->met->CosLD *
                               Grid->met->CosLD * 0.5)) +
                             12. * Grid->met->CosLD *
                             (2. + 3. * 0.4 * Grid->met->SinLD) *
                             sqrt(1. - AOB * AOB) / CONST_PI);
    }
    else {
        DSinB = 3600. * (Grid->met->Daylength * Grid->met->SinLD);
        Grid->met->DSinBE = 3600. *
                            (Grid->met->Daylength *
                             (Grid->met->SinLD + 0.4 *
                              (Grid->met->SinLD * Grid->met->SinLD +
                               Grid->met->CosLD *
                               Grid->met->CosLD * 0.5)));
    }

    /*  Extraterrestrial radiation and atmospheric transmission */
    Grid->met->AngotRadiation = SolarConstant * DSinB;
    Grid->met->AtmosphTransm = Grid->met->Radiation / Grid->met->AngotRadiation;

    if (Grid->met->AtmosphTransm > 0.75) {
        FractionDiffuseRad = 0.23;
    }

    if (Grid->met->AtmosphTransm <= 0.75 && Grid->met->AtmosphTransm > 0.35) {
        FractionDiffuseRad = 1.33 - 1.46 * Grid->met->AtmosphTransm;
    }

    if (Grid->met->AtmosphTransm <= 0.35 && Grid->met->AtmosphTransm > 0.07) {
        FractionDiffuseRad = 1. - 2.3 * pow((Grid->met->AtmosphTransm - 0.07),
                                            2.);
    }

    if (Grid->met->AtmosphTransm < 0.07) {
        FractionDiffuseRad = 1.0;
    }

    Grid->met->DiffRadPP = 0.5 * FractionDiffuseRad * Grid->met->AtmosphTransm *
                           SolarConstant;
}
