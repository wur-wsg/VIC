#include <plugin.h>

void
plugin_initialize_options(void)
{
    extern plugin_option_struct plugin_options;

    plugin_options.DECOMPOSITION = RANDOM_DECOMPOSITION;
    plugin_options.ROUTING = false;
    plugin_options.UH_LENGTH = 0;
    plugin_options.FORCE_ROUTING = false;
}

void
plugin_initialize_global(void)
{
    extern plugin_global_param_struct plugin_global_param;

    size_t i;
    
    plugin_global_param.rout_steps_per_day = 0;
    plugin_global_param.rout_dt = 0;
    for(i = 0; i < PLUGIN_N_FORCING_TYPES; i++){
        plugin_global_param.force_steps_per_year[i] = 0;
        plugin_global_param.force_dt[i] = 0;
        plugin_global_param.forceyear[i] = 0;
        plugin_global_param.forcemonth[i] = 1;
        plugin_global_param.forceday[i] = 1;
        plugin_global_param.forcesec[i] = 0;
        plugin_global_param.forceskip[i] = 0;
        plugin_global_param.forcerun[i] = true;
        plugin_global_param.forceoffset[i] = 0;
        plugin_global_param.forcefreq[i] = 0;
    }
}

void
plugin_initialize_parameters(void)
{
    extern plugin_parameters_struct plugin_param;

    /* Unused variables */
    UNUSED(plugin_param);
}

void
plugin_initialize_filenames(void)
{
    extern plugin_filenames_struct plugin_filenames;
    
    size_t i;
    
    snprintf(plugin_filenames.routing.nc_filename, MAXSTRING, "%s", MISSING_S);
    snprintf(plugin_filenames.decomposition.nc_filename, MAXSTRING, "%s", MISSING_S);
    
    for(i = 0; i < PLUGIN_N_FORCING_TYPES; i++){
        snprintf(plugin_filenames.forcing[i].nc_filename, MAXSTRING, "%s", MISSING_S);
        snprintf(plugin_filenames.f_path_pfx[i], MAXSTRING, "%s", MISSING_S);
        snprintf(plugin_filenames.f_varname[i], MAXSTRING, "%s", MISSING_S);
    }
}

void
plugin_initialize_global_structures(void)
{
    plugin_initialize_options();
    plugin_initialize_global();
    plugin_initialize_parameters();
    plugin_initialize_filenames();
}
