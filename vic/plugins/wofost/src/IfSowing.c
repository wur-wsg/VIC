#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function void IfSowing    ()                                              */
/*  Purpose: Checks whether sowing has occurred. Note that if the emergence   */
/*           flag is set to 1 the crop simulation starts the next day. If it  */
/*           is set to 0 the Emergence date has to be established.            */
/* ---------------------------------------------------------------------------*/

void
IfSowing(SimUnit   *Grid,
         dmy_struct*start)
{
    extern dmy_struct *dmy;
    extern size_t      current;

    if (dmy[current].month == start->month &&
        dmy[current].day == start->day) {
        Grid->crp->Sowing = 1;
    }
}
