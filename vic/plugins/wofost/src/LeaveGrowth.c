#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function LeaveGrowth(float LAIExp, float NewLeaves)                       */
/*  Purpose: Calculation of the daily leaves growth rate, the results are     */
/*           stored in the Grid->crp->properties linked list                       */
/* ---------------------------------------------------------------------------*/

void
LeaveGrowth(SimUnit *Grid)
{
    float  GrowthExpLAI;
    float  GrowthSourceLimited;
    float  SpecLeafArea;
    float  Stress;
    float  DTeff;
    float  tmp_min;

    Green *Leave = NULL;
    Green *LeaveProperties = NULL;

    /* Specific Leaf area(m2/g), as dependent on NPK stress */
    SpecLeafArea =
        Afgen(Grid->crp->prm.SpecificLeaveArea, &(Grid->crp->st.Development)) *
        exp(-Grid->crp->prm.NutrientStessSLA *
            (1. - Grid->crp->NPK_Indx));

    /* Leave area not to exceed exponential growth */
    if (Grid->crp->st.LAIExp < 6 && Grid->crp->rt.leaves > 0.) {
        /* Growth during juvenile stage */
        if (Grid->crp->st.Development < 0.2 && Grid->crp->st.LAI < 0.75) {
            Stress = Grid->soil->WaterStress * exp(
                -Grid->crp->prm.NitrogenStressLAI * (1. - Grid->crp->NPK_Indx));
        }
        else {
            Stress = 1.;
        }

        /* Effective temperature for leave growth */
        DTeff = max(0., Grid->met->Temp - Grid->crp->prm.TempBaseLeaves);

        /* Correction for nutrient stress */
        GrowthExpLAI = Grid->crp->st.LAIExp * Grid->crp->prm.RelIncreaseLAI *
                       DTeff * Stress;

        /* Source limited leaf area increase */
        GrowthSourceLimited = Grid->crp->rt.leaves * SpecLeafArea;

        /* Sink-limited leaf area increase */
        tmp_min = min(GrowthExpLAI, GrowthSourceLimited);
        SpecLeafArea = tmp_min / Grid->crp->rt.leaves;
    }
    else {
        GrowthExpLAI = 0.;
    }


    Leave = malloc(sizeof(Green));
    Leave->weight = Grid->crp->rt.leaves;
    Leave->age = 0.;
    Leave->area = SpecLeafArea;
    Leave->next = NULL;

    /* Store the start address */
    LeaveProperties = Grid->crp->LeaveProperties;

    /* Loop until the last element in the list */
    while (Grid->crp->LeaveProperties->next != NULL) {
        Grid->crp->LeaveProperties = Grid->crp->LeaveProperties->next;
    }

    /* Add new element to the list */
    Grid->crp->LeaveProperties->next = Leave;

    /* Restore the start address */
    Grid->crp->LeaveProperties = LeaveProperties;

    Grid->crp->rt.LAIExp = GrowthExpLAI;
}
