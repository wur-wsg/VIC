#include <vic_driver_image.h>
#include <plugin.h>

void
rout_put_data(size_t cur_cell)
{
    extern global_param_struct global_param;
    extern domain_struct local_domain;
    extern rout_var_struct *rout_var;
    extern double        ***out_data;

    out_data[cur_cell][N_OUTVAR_TYPES + OUT_STREAM_MOIST][0] = rout_var[cur_cell].stream * 
            global_param.dt / local_domain.locations[cur_cell].area *
            MM_PER_M;
    out_data[cur_cell][N_OUTVAR_TYPES + OUT_DISCHARGE][0] = rout_var[cur_cell].discharge;
}
