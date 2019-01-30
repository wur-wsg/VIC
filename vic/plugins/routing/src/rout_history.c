#include <vic_driver_image.h>
#include <plugin.h>

bool
rout_history(int           varid,
             unsigned int *aggtype)
{
    if (varid == N_OUTVAR_TYPES + OUT_STREAM_MOIST) {
        (*aggtype) = AGG_TYPE_END;
        return true;
    }

    return false;
}
