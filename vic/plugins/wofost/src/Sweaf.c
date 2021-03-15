#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------*/
/*  function sweaf()                                              */
/*  Purpose: Calculates the Soil Water Easily Available Fraction  */
/*                                                                */
/* Chapter 20 in documentation WOFOST Version 4.1 (1988)          */
/*                                                                */
/*    The fraction of easily available soil water between         */
/*    field capacity and wilting point is a function of the       */
/*    potential evapotranspiration rate (for a closed canopy)     */
/*    in cm/day, ET0, and the crop group number, CGNR (from       */
/*    1 (=drought-sensitive) to 5 (=drought-resistent)). The      */
/*    function SWEAF describes this relationship given in tabular */
/*    form by Doorenbos & Kassam (1979) and by Van Keulen & Wolf  */
/*    (1986; p.108, table 20).                                    */
/*    Original fortran version: D.M. Jansen and C.A. van Diepen,  */
/*    October 1986.                                               */
/* ---------------------------------------------------------------*/
float
sweaf(float CropGroupNumber,
      float PotEvapotransp)
{
    float sweaf;
    sweaf = 1. / (0.76 + 1.5 * PotEvapotransp) - (5. - CropGroupNumber) * 0.10;
    if (CropGroupNumber < 3.) {
        sweaf = sweaf + (PotEvapotransp - 0.6) /
                (CropGroupNumber * (CropGroupNumber + 3.));
    }
    return limit(0.10, 0.95, sweaf);
}
