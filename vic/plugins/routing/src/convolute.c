#include <vic_driver_image.h>
#include <routing.h>

void
convolute(double  quantity,
     double *uh,
     double *discharge,
     size_t  length,
     size_t  offset)
{
    size_t i;

    for (i = 0; i < length; i++) {
        discharge[offset + i] += quantity * uh[i];
    }
}
