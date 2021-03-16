#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function Afgen()                                                          */
/*  Purpose: Get the interpolated value of a user provided input table        */
/* ---------------------------------------------------------------------------*/

float
Afgen(TABLE *Table,
      float *X)
{
    if (*X <= Table->x) {
        return Table->y;
    }

    while (Table->next) {
        if (*X >= Table->x && *X < Table->next->x) {
            return (Table->y + (*X - Table->x) *
                    (Table->next->y - Table->y) / (Table->next->x - Table->x));
        }
        Table = Table->next;
    }

    return Table->y;
}
