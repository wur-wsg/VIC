#include <vic_driver_image.h>
#include <plugin.h>

void
wofost_update_step_vars(size_t iCell)
{
    extern plugin_option_struct plugin_options;
    extern crop_force_struct *crop_force;
    extern crop_con_struct **crop_con;
    extern crop_con_map_struct *crop_con_map;
    
    size_t iCrop;
    size_t iFert;
    
    if (plugin_options.WOFOST_FORCE_FERT) {
        for(iCrop = 0; iCrop < crop_con_map[iCell].nc_active; iCrop++){
            for(iFert = 0; iFert < plugin_options.NFERTTIMES; iFert++){
                crop_con[iCell][iCrop].DVS_point[iFert] = crop_force[iCell].DVS_point[iCrop][iFert]; 
                crop_con[iCell][iCrop].N_amount[iFert] = crop_force[iCell].N_amount[iCrop][iFert]; 
                crop_con[iCell][iCrop].P_amount[iFert] = crop_force[iCell].P_amount[iCrop][iFert]; 
                crop_con[iCell][iCrop].K_amount[iFert] = crop_force[iCell].K_amount[iCrop][iFert]; 
            }
        }
    }
}
