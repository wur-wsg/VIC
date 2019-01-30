#include <vic_driver_shared_all.h>
#include "plugin.h"

void
plugin_print_global_param(void)
{
    extern plugin_global_param_struct plugin_global_param;
    
    plugin_global_param_struct *gp;
    
    gp = &plugin_global_param;
    
    fprintf(LOG_DEST, "plugin global_param:\n");
    fprintf(LOG_DEST, "\trout_steps_per_day  : %zu\n", gp->rout_steps_per_day);
    fprintf(LOG_DEST, "\trout_dt             : %.4f\n", gp->rout_dt);
}

void
plugin_print_options(void)
{
    extern plugin_option_struct plugin_options;
    
    plugin_option_struct *op;
    
    op = &plugin_options;
    
    fprintf(LOG_DEST, "plugin option:\n");
    fprintf(LOG_DEST, "\tDECOMPOSITION        : %s\n",
            op->DECOMPOSITION ? "true" : "false");
    fprintf(LOG_DEST, "\tROUTING              : %s\n",
            op->ROUTING ? "true" : "false");
    fprintf(LOG_DEST, "\tFORCE_ROUTING        : %s\n",
            op->FORCE_ROUTING ? "true" : "false");
    
    fprintf(LOG_DEST, "\tUH_LENGTH            : %d\n",
            op->UH_LENGTH);
}

void
plugin_print_parameters(void)
{
    extern plugin_parameters_struct plugin_param;
    
    plugin_parameters_struct *pa;
    
    pa = &plugin_param;
    
    fprintf(LOG_DEST, "plugin parameters:\n");
}