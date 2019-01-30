#include <stddef.h>
#include <stdbool.h>

void
size_t_swap(size_t  i,
           size_t  j,
           size_t *array)
{
    size_t hold;

    hold = array[i];
    array[i] = array[j];
    array[j] = hold;
}

void
int_swap(size_t i,
         size_t j,
         int   *array)
{
    int hold;

    hold = array[i];
    array[i] = array[j];
    array[j] = hold;
}

void
double_swap(size_t  i,
            size_t  j,
            double *array)
{
    double hold;

    hold = array[i];
    array[i] = array[j];
    array[j] = hold;
}

void
size_t_sort(size_t *array,
           size_t *cost,
           size_t  Nelements,
           bool    acending)
{
    size_t i, j;
    size_t tmp_cost[Nelements];

    for (i = 0; i < Nelements; i++) {
        tmp_cost[i] = cost[i];
    }

    if (acending) {
        for (i = 0; i < Nelements - 1; i++) {
            for (j = 0; j < Nelements - i - 1; j++) {
                if (tmp_cost[j] > tmp_cost[j + 1]) {
                    size_t_swap(j, j + 1, array);
                    size_t_swap(j, j + 1, tmp_cost);
                }
            }
        }
    }
    else {
        for (i = 0; i < Nelements - 1; i++) {
            for (j = 0; j < Nelements - i - 1; j++) {
                if (tmp_cost[j] < tmp_cost[j + 1]) {
                    size_t_swap(j, j + 1, array);
                    size_t_swap(j, j + 1, tmp_cost);
                }
            }
        }
    }
}

void
size_t_sort2(size_t *array,
            int    *cost,
            size_t  Nelements,
            bool    acending)
{
    size_t i, j;
    int    tmp_cost[Nelements];

    for (i = 0; i < Nelements; i++) {
        tmp_cost[i] = cost[i];
    }

    if (acending) {
        for (i = 0; i < Nelements - 1; i++) {
            for (j = 0; j < Nelements - i - 1; j++) {
                if (tmp_cost[j] > tmp_cost[j + 1]) {
                    size_t_swap(j, j + 1, array);
                    int_swap(j, j + 1, tmp_cost);
                }
            }
        }
    }
    else {
        for (i = 0; i < Nelements - 1; i++) {
            for (j = 0; j < Nelements - i - 1; j++) {
                if (tmp_cost[j] < tmp_cost[j + 1]) {
                    size_t_swap(j, j + 1, array);
                    int_swap(j, j + 1, tmp_cost);
                }
            }
        }
    }
}

void
double_flip(double *array,
            size_t  Nelements)
{
    size_t i, end;

    end = Nelements - 1;
    for (i = 0; i < Nelements / 2; i++) {
        double_swap(i, end, array);
        end--;
    }
}
