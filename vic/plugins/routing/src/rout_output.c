#include <vic_driver_image.h>
#include <plugin.h>

void
rout_set_output_met_data_info(void)
{
    extern metadata_struct out_metadata[];
    
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].varname, "OUT_DISCHARGE");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].long_name, "discharge");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].standard_name, "discharge");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].units, "m3/s");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].description, "discharge at the cell outflow point");

    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].varname, "OUT_STREAM_MOIST");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].long_name, "stream_storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].standard_name, "stream storage");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].units, "mm");
    strcpy(out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].description, "moisture storage in stream flow");

    out_metadata[N_OUTVAR_TYPES + OUT_DISCHARGE].nelem = 1;
    out_metadata[N_OUTVAR_TYPES + OUT_STREAM_MOIST].nelem = 1;
}

void
rout_history(int           varid,
             unsigned int *agg_type)
{
    switch (varid) {
    case  N_OUTVAR_TYPES + OUT_STREAM_MOIST:
        (*agg_type) = AGG_TYPE_END;
    case  N_OUTVAR_TYPES + OUT_DISCHARGE:
        (*agg_type) = AGG_TYPE_AVG;
    }
}

void
rout_put_data(size_t iCell)
{
    extern global_param_struct global_param;
    extern domain_struct local_domain;
    extern rout_var_struct *rout_var;
    extern double        ***out_data;

    out_data[iCell][N_OUTVAR_TYPES + OUT_STREAM_MOIST][0] = rout_var[iCell].stream * 
            global_param.dt / local_domain.locations[iCell].area *
            MM_PER_M;
    out_data[iCell][N_OUTVAR_TYPES + OUT_DISCHARGE][0] = rout_var[iCell].discharge;
}