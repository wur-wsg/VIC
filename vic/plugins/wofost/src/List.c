#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function List()                                                           */
/*  Purpose: Get the value of a user provided input table                     */
/* ---------------------------------------------------------------------------*/
     
float List(TABLE_D *Table)
{
    extern dmy_struct *dmy;
    extern size_t current;
    
     while (Table) 
    { 
        if (dmy[current].month == Table->month -1 &&
            dmy[current].day == Table->day)
        {
            return Table->amount;
        }
        Table = Table->next;
    }
    
    return 0.;
}
