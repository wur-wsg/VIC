#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function IntegrationGrid->crp                                                  */
/*  Purpose: Establish the crop state variables by integration of the crop    */
/*  rate variables and update the crop leave classes                          */
/* ---------------------------------------------------------------------------*/

void
IntegrationCrop(SimUnit *Grid)
{
    float  PhysAgeing;
    Green *LeaveProperties;

    Grid->crp->st.roots += Grid->crp->rt.roots;
    Grid->crp->st.stems += Grid->crp->rt.stems;
    Grid->crp->st.leaves += Grid->crp->rt.leaves;
    Grid->crp->st.storage += Grid->crp->rt.storage;
    Grid->crp->st.LAIExp += Grid->crp->rt.LAIExp;

    Grid->crp->st.RootDepth_prev = Grid->crp->st.RootDepth;
    Grid->crp->st.RootDepth += Grid->crp->rt.RootDepth;

    /* Calculate the developmentstage */
    Grid->crp->st.Development_prev = Grid->crp->st.Development;
    if (Grid->crp->st.Development < 1.) {
        Grid->crp->st.Development += Grid->crp->rt.Development;
        if (Grid->crp->st.Development > 1.) {
            Grid->crp->st.Development = 1.;
        }
    }
    else {
        Grid->crp->st.Development += Grid->crp->rt.Development;
    }

    /* Calculate the amount of death material kg ha-1 */
    Grid->crp->dst.roots += Grid->crp->drt.roots;
    Grid->crp->dst.stems += Grid->crp->drt.stems;
    Grid->crp->dst.leaves += Grid->crp->drt.leaves;

    /* Calculate vernalization state in case the switch is set */
    if (Grid->crp->prm.IdentifyAnthesis == 2) {
        Grid->crp->st.vernalization += Grid->crp->rt.vernalization;
    }

    /* Establish the age increase */
    PhysAgeing =
        max(0.,
            (Grid->met->Temp - Grid->crp->prm.TempBaseLeaves) /
            (35. - Grid->crp->prm.TempBaseLeaves));

    /* Store the initial address */
    LeaveProperties = Grid->crp->LeaveProperties;

    /* Update the leave age for each age class */
    while (Grid->crp->LeaveProperties->next) {
        Grid->crp->LeaveProperties->age += PhysAgeing;
        Grid->crp->LeaveProperties = Grid->crp->LeaveProperties->next;
    }

    /* Return to beginning of the linked list */
    Grid->crp->LeaveProperties = LeaveProperties;
}
