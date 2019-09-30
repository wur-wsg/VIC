#include <vic_driver_image.h>
#include <plugin.h>

void header(FILE *fp)
{
    fprintf(fp,"                              Date   Date   DVS       WLV        WST          WSO         WRT         LAI     WSTRESS   NNI        PNI        KNI        NPKI  \n");
}

void Output(SimUnit *Grid, FILE *fp)
{    
    fprintf(fp,"%7.2f,%7.2f,%4zu,%4zu,%4zu,%4d, %4d, %7.5f,%11.5f,%11.5f,%11.5f,%11.4f,%10.4f,%7.2f,%10.5f,%10.5f,%10.5f,%10.5f\n",
        Grid->vic->Latitude,
        Grid->vic->Longitude,
        Grid->vic->veg_class,
        Grid->vic->snow_band,
        Grid->vic->crop_class,
        Grid->vic->MeteoYear,
        Grid->vic->MeteoDay,
        Grid->crp->st.Development,
        Grid->crp->st.leaves,
        Grid->crp->st.stems,
        Grid->crp->st.storage,
        Grid->crp->st.roots,
        Grid->crp->st.LAI,
        Grid->vic->WaterStressAvg / Grid->crp->GrowthDay,
        Grid->crp->N_st.Indx,
        Grid->crp->P_st.Indx,
        Grid->crp->K_st.Indx,
        Grid->crp->NPK_Indx);
}

