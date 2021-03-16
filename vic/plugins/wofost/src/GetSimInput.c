#include <vic_driver_shared_image.h>
#include "plugin.h"

/* ----------------------------------------------------------------------------*/
/*  function GetMeteoInput()                                                   */
/*  Purpose: Get the names of the crop, soil, management and site files as     */
/*           as well if the simulation starting day and whether the simulation */
/*           starts at emergence (1) or sowing (0)                             */
/*-----------------------------------------------------------------------------*/

void
GetSimInput(char    *path,
            char    *cf,
            char    *mf,
            char    *start,
            int      Emergence,
            int      count,
            SimUnit *Grid)
{
    char cropfile[MAXSTRING];
    char management[MAXSTRING];

    memset(cropfile, '\0', MAXSTRING);
    memset(management, '\0', MAXSTRING);

    strncpy(cropfile, path, strlen(path));
    strncat(cropfile, cf, strlen(cf));

    strncpy(management, path, strlen(path));
    strncat(management, mf, strlen(mf));

    GetCropData(Grid->crp, cropfile);
    GetManagement(Grid->mng, management);

    Grid->file = count;             // number of elements in Grid carousel
    strpdmy(start, "%m-%d", &Grid->start);
    Grid->emergence = Emergence;      // Start the simulations at emergence (1) or at sowing (0)
    Grid->crp->Sowing = 0;
    Grid->crp->Emergence = 0;         // Crop emergence has not yet occurred
}
