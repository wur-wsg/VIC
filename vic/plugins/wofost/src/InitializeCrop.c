#include <vic_driver_image.h>
#include <plugin.h>

/*---------------------------------------------------*/
/* function EmergenceCrop                            */
/* Purpose: determine if crop emergence has occurred */
/*---------------------------------------------------*/

int
EmergenceCrop(SimUnit *Grid,
              int      Emergence)
{
    float DeltaTempSum;

    /*  Emergence has not taken place yet */
    if (!Emergence) {
        /* Start counting TSumEmergence one day after sowing */
        if (Grid->crp->Sowing == 1) {
            Grid->crp->Sowing = 2;
        }
        else {
            DeltaTempSum = limit(0,
                                 Grid->crp->prm.TempEffMax - Grid->crp->prm.TempBaseEmergence,
                                 Grid->met->Temp -
                                 Grid->crp->prm.TempBaseEmergence);
            Grid->crp->TSumEmergence += DeltaTempSum;
            if (Grid->crp->TSumEmergence >= Grid->crp->prm.TSumEmergence) {
                Emergence = 1;
            }
        }
    }
    else {
        Grid->crp->Sowing = 2;
    }
    return Emergence;
}

/* ----------------------------------------------------------*/
/*  function InitializeCrop                                  */
/*  Purpose: Set the initial crop state and leave variables  */
/*  ---------------------------------------------------------*/

void
InitializeCrop(SimUnit *Grid)
{
    float FractionRoots;
    float FractionShoots;
    float InitialShootWeight;
    float LAIEmergence;

    /* Initialize the crop states */
    Grid->crp->st.Development = Grid->crp->prm.InitialDVS;

    FractionRoots = Afgen(Grid->crp->prm.Roots, &(Grid->crp->st.Development));
    FractionShoots = 1 - FractionRoots;
    InitialShootWeight = Grid->crp->prm.InitialDryWeight * FractionShoots;

    Grid->crp->st.roots = Grid->crp->prm.InitialDryWeight * FractionRoots;
    Grid->crp->st.RootDepth = Grid->crp->prm.InitRootingDepth;
    Grid->crp->st.stems = InitialShootWeight *
                          Afgen(Grid->crp->prm.Stems,
                                &(Grid->crp->st.Development));
    Grid->crp->st.leaves = InitialShootWeight *
                           Afgen(Grid->crp->prm.Leaves,
                                 &(Grid->crp->st.Development));
    Grid->crp->st.storage = InitialShootWeight * Afgen(Grid->crp->prm.Storage,
                                                       &(Grid->crp->st.
                                                         Development));

    /* Adapt the maximum rooting depth */
    Grid->crp->prm.MaxRootingDepth = max(Grid->crp->prm.InitRootingDepth,
                                         Grid->crp->prm.MaxRootingDepth);

    /* Calculate the LAI at emergence */
    LAIEmergence = Grid->crp->st.leaves * Afgen(
        Grid->crp->prm.SpecificLeaveArea, &(Grid->crp->st.Development));

    Grid->crp->st.LAIExp = LAIEmergence;

    Grid->crp->st.LAI = LAIEmergence + Grid->crp->st.stems *
                        Afgen(Grid->crp->prm.SpecificStemArea,
                              &(Grid->crp->st.Development)) +
                        Grid->crp->st.storage * Grid->crp->prm.SpecificPodArea;

    /* Initialize the leaves */
    Grid->crp->LeaveProperties = malloc(sizeof(Green));
    Grid->crp->LeaveProperties->age = 0.;
    Grid->crp->LeaveProperties->weight = Grid->crp->st.leaves;
    Grid->crp->LeaveProperties->area = Afgen(Grid->crp->prm.SpecificLeaveArea,
                                             &(Grid->crp->st.Development));
    Grid->crp->LeaveProperties->next = NULL;

    /* Emergence true */
    Grid->crp->Emergence = 1;
    Grid->crp->GrowthDay = 1;

    /* No dead material */
    Grid->crp->dst.leaves = 0.;
    Grid->crp->dst.stems = 0.;
    Grid->crp->dst.roots = 0.;

    /* No vernalization yet */
    Grid->crp->st.vernalization = 0.;
}
