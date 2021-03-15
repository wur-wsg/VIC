#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function NutrientLoss                                                     */
/*  Purpose: To calculate nutrient loss rate of dying of roots, stems leaves  */
/*           and storage organs (kg N ha-1 d-1)                               */
/* ---------------------------------------------------------------------------*/

void
NutrientLoss(SimUnit *Grid)
{
    Grid->crp->N_rt.death_lv = Grid->crp->prm.N_ResidualFrac_lv *
                               Grid->crp->drt.leaves;
    Grid->crp->N_rt.death_st = Grid->crp->prm.N_ResidualFrac_st *
                               Grid->crp->drt.stems;
    Grid->crp->N_rt.death_ro = Grid->crp->prm.N_ResidualFrac_ro *
                               Grid->crp->drt.roots;

    Grid->crp->P_rt.death_lv = Grid->crp->prm.P_ResidualFrac_lv *
                               Grid->crp->drt.leaves;
    Grid->crp->P_rt.death_st = Grid->crp->prm.P_ResidualFrac_st *
                               Grid->crp->drt.stems;
    Grid->crp->P_rt.death_ro = Grid->crp->prm.P_ResidualFrac_ro *
                               Grid->crp->drt.roots;

    Grid->crp->K_rt.death_lv = Grid->crp->prm.K_ResidualFrac_lv *
                               Grid->crp->drt.leaves;
    Grid->crp->K_rt.death_st = Grid->crp->prm.K_ResidualFrac_st *
                               Grid->crp->drt.stems;
    Grid->crp->K_rt.death_ro = Grid->crp->prm.K_ResidualFrac_ro *
                               Grid->crp->drt.roots;
}
