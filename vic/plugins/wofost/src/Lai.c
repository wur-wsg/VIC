#include <vic_driver_image.h>
#include <plugin.h>

/* -----------------------------------------------------------------------*/
/*  function LeaveAreaIndex()                                             */
/*  Purpose: Calculation of the LAI of the LeaveProperties ha ha-1 struct */
/* -----------------------------------------------------------------------*/
float
LeaveAreaIndex(SimUnit *Grid)
{
    float  LAISum = 0.;
    Green *LeaveProperties;

    /* Store the initial address */
    LeaveProperties = Grid->crp->LeaveProperties;

    /* Loop until the last element in the list */
    while (Grid->crp->LeaveProperties) {
        LAISum += Grid->crp->LeaveProperties->weight *
                  Grid->crp->LeaveProperties->area;
        Grid->crp->LeaveProperties = Grid->crp->LeaveProperties->next;
    }

    /* Return to the initial address */
    Grid->crp->LeaveProperties = LeaveProperties;

    /* Return Green Area Index which will be used as LAI */
    return (LAISum + Grid->crp->st.stems *
            Afgen(Grid->crp->prm.SpecificStemArea,
                  &(Grid->crp->st.Development)) +
            Grid->crp->st.storage * Grid->crp->prm.SpecificPodArea);
}
