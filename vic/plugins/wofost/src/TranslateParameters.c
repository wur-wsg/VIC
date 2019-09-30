#include <vic_driver_shared_image.h>
#include <plugin.h>

#include "wofost.h"

/* -------------------------------------------------------------------------*/
/*  function TranslateParameters()                                          */
/*  Purpose: To compute the parameters needed to run VIC                    */
/*           based on the WOFOST simulation                                 */
/* -------------------------------------------------------------------------*/
void TranslateParameters(SimUnit *Grid)
{     
    extern parameters_struct param;
    
    double height;
    
    height = Grid->crp->prm.MaxHeight * min(Grid->crp->st.Development, 1.0);
            
    Grid->vic->LAI = Grid->crp->st.LAI;
    Grid->vic->roughness = height * param.VEG_RATIO_RL_HEIGHT;
    Grid->vic->displacement = height * param.VEG_RATIO_DH_HEIGHT;
    Grid->vic->albedo = (1 - min(Grid->crp->st.LAI, 1.0)) * param.ALBEDO_BARE_SOIL +
            min(Grid->crp->st.LAI, 1.0) * Grid->crp->prm.Albedo;
    Grid->vic->overstory = height > 2 && Grid->crp->st.LAI > 1;
    Grid->vic->trunk_ratio = Grid->crp->prm.TrunkRatio;
    Grid->vic->wind_atten = Grid->crp->prm.WindAttenuation;
    Grid->vic->wind_h = Grid->crp->prm.MaxHeight + 1;
    Grid->vic->rad_atten = Grid->crp->prm.RadiationAttenuation;
    Grid->vic->RGL = Grid->crp->prm.RGL;
    Grid->vic->rmin = Grid->crp->prm.MinStomatalResistance;
    Grid->vic->rarc = Grid->crp->prm.MaxArchitecturalResistance * 
            min(Grid->crp->st.Development, 1.0);
}