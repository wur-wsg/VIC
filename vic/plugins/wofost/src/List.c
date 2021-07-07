#include <vic_driver_image.h>
#include <plugin.h>

/* ---------------------------------------------------------------------------*/
/*  function List()                                                           */
/*  Purpose: Get the value of a user provided input table                     */
/* ---------------------------------------------------------------------------*/

float
List(TABLE_D *Table)
{
    extern dmy_struct *dmy;
    extern size_t      current;

    while (Table) {
        if (dmy[current].month == Table->month &&
            dmy[current].day == Table->day) {
            return Table->amount;
        }
        Table = Table->next;
    }

    return 0.;
}

float
List_cumsum(TABLE_D *Table, size_t n)
{
    extern dmy_struct *dmy;
    extern size_t      current;
    
    size_t i;
    
    TABLE_D *start = Table;
    float total = 0.;
    
    for(i = 0; i < n; i++){
        Table = start;
        while (Table) {
            if (dmy[current - i].month == Table->month &&
                dmy[current - i].day == Table->day) {
                total += Table->amount;
            }
            Table = Table->next;
        }
    }

    return total;
}