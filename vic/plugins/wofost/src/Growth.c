#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function Growth(float NewPlantMaterial)                                   */
/*  Purpose: Establish growth rates of the plant organs (kg ha-1 d-1) and     */
/*  rooting depth (cm)                                                        */
/* ---------------------------------------------------------------------------*/

void Growth(SimUnit *Grid, float NewPlantMaterial)
{
    extern plugin_global_param_struct plugin_global_param;
    float shoots;
    float Translocation;
    float FRTRL;
     
    /* Water stress is more severe as compared to Nitrogen stress and */
    /* partitioning will follow the original assumptions of LINTUL2   */     
        
    FRTRL = 0.;
    Translocation = 0.;
    if (Grid->crp->st.Development >= 1.)
    {
        Translocation = (Grid->crp->st.stems + Grid->crp->dst.stems) * Grid->crp->rt.Development * FRTRL;
    }
    
    Grid->crp->drt.roots = Grid->crp->st.roots * Afgen(Grid->crp->prm.DeathRateRoots, &(Grid->crp->st.Development));
    Grid->crp->rt.roots  = NewPlantMaterial * Grid->crp->fac_ro - Grid->crp->drt.roots;
	
    shoots         = NewPlantMaterial * (1-Grid->crp->fac_ro);
	    
    Grid->crp->drt.stems = Grid->crp->st.stems * Afgen(Grid->crp->prm.DeathRateStems, &(Grid->crp->st.Development));	
    Grid->crp->rt.stems  = shoots * Grid->crp->fac_st - Grid->crp->drt.stems - Translocation;
	
    Grid->crp->rt.storage = shoots * Grid->crp->fac_so + Translocation;
	
    /* Dying leave biomass is subtracted from the oldest classes */
    Grid->crp->drt.leaves = DyingLeaves(Grid); 
    Grid->crp->rt.leaves  = shoots * Grid->crp->fac_lv;
    
    /* Note that the new leave biomass is stored in the youngest leave class */
    LeaveGrowth(Grid);
    
    Grid->crp->rt.leaves  = Grid->crp->rt.leaves -  Grid->crp->drt.leaves;
	
    
    /* No Root growth if no assimilates are partitioned to the roots or if */
    /* the crop has no airducts and the roots are close to the groundwater */
    if (Grid->crp->fac_ro <= 0.0 || (!Grid->crp->prm.Airducts && Grid->ste->GroundwaterDepth - Grid->crp->st.RootDepth < 10.))
        Grid->crp->rt.RootDepth = 0.;
    else
        Grid->crp->rt.RootDepth = min(Grid->crp->prm.MaxRootingDepth - Grid->crp->st.RootDepth,
                Grid->crp->prm.MaxIncreaseRoot*plugin_global_param.wofost_steps_per_day);
}	
