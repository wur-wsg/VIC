#include <vic_driver_shared_image.h>
#include "plugin.h"

/* ----------------------------------------------------------------------------*/
/*  function GetMeteoInput()                                                   */
/*  Purpose: Get the names of the crop, soil, management and site files as     */
/*           as well if the simulation starting day and whether the simulation */
/*           starts at emergence (1) or sowing (0)                             */
/*-----------------------------------------------------------------------------*/

void GetSimInputSingle(char *path,
        char *cf, 
        char *mf, 
        char *start, 
        int Emergence, 
        int count,
        SimUnit *Grid)
{
    char cropfile[MAXSTRING];
    char management[MAXSTRING];
    
    memset(cropfile,'\0',MAXSTRING);
    memset(management,'\0',MAXSTRING);

    strncpy(cropfile, path, strlen(path));
    strncat(cropfile, cf, strlen(cf));

    strncpy(management, path, strlen(path));
    strncat(management, mf, strlen(mf));

    GetCropData(Grid->crp, cropfile); 
    GetManagement(Grid->mng, management);

    if (strlen(start) >= MAXSTRING) {
        log_err("file length (%zu) is larger than maximum (%d)", strlen(start), MAXSTRING);
    }
    strpdmy(start, "%d-%m-0000", &Grid->start); // Starting string month day of the simulations 

    Grid->file  = count;            // number of elements in Grid carousel
    Grid->emergence = Emergence;      // Start the simulations at emergence (1) or at sowing (0)                
    Grid->crp->Sowing = 0;
    Grid->crp->Emergence = 0;         // Crop emergence has not yet occurred
}   