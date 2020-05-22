#include <vic_driver_image.h>
#include <plugin.h>

void
wu_update_step_vars(size_t iCell)
{
    extern plugin_option_struct plugin_options;
    extern wu_force_struct **wu_force;
    extern wu_con_struct *wu_con;
    
    if (plugin_options.FORCE_PUMP_CAP) {
       wu_con[iCell].pumping_capacity = wu_force[iCell][0].pumping_capacity; 
    }
}
