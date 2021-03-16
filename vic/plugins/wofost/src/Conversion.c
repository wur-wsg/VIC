#include <vic_driver_image.h>
#include <plugin.h>

/* ----------------------------------------------------------------------------------*/
/*  function Conversion                                                              */
/*  Purpose: Convert the net assimilation products into plant dry matter kg ha-1 d-1 */
/* ----------------------------------------------------------------------------------*/

float
Conversion(SimUnit *Grid,
           float    NetAssimilation)
{
    float root, shoots;

    root = Grid->crp->fac_ro / Grid->crp->prm.ConversionRoots;
    shoots = Grid->crp->fac_st / Grid->crp->prm.ConversionStems;
    shoots += Grid->crp->fac_lv / Grid->crp->prm.ConversionLeaves;
    shoots += Grid->crp->fac_so / Grid->crp->prm.ConversionStorage;

    /* conversion */
    return NetAssimilation / (shoots * (1 - Grid->crp->fac_ro) + root);
}
