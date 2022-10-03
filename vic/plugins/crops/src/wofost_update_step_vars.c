#include <vic_driver_image.h>
#include <plugin.h>

void
wofost_update_step_vars(size_t iCell)
{
    extern option_struct options;
    extern plugin_option_struct plugin_options;
    extern crop_force_struct   *crop_force;
    extern crop_con_struct    **crop_con;
    extern crop_con_map_struct *crop_con_map;
    extern SimUnit           ***Grid;

    SimUnit                    *iGrid;
    
    size_t                      iCrop;
    size_t                      iFert;
    size_t                      iBand;
    size_t                      l;

    if (plugin_options.WOFOST_FORCE_TSUM) {
        for (iCrop = 0; iCrop < crop_con_map[iCell].nc_active; iCrop++) {
            for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
                iGrid = Grid[iCell][iBand];
                for (l = 0; l < (size_t)iCrop; l++) {
                    iGrid = iGrid->next;
                }

                /* Determine if the sowing already has occurred */
                IfSowing(iGrid, &iGrid->start);
                
                if(iGrid->crp->Sowing != 1){
                    continue;
                }
                
                crop_con[iCell][iCrop].tsum1 =
                    crop_force[iCell].tsum1[iCrop];
                crop_con[iCell][iCrop].tsum2 =
                    crop_force[iCell].tsum2[iCrop];
            }
        }
    }
    if (plugin_options.WOFOST_FORCE_FERT) {
        for (iCrop = 0; iCrop < crop_con_map[iCell].nc_active; iCrop++) {
            for (iFert = 0; iFert < plugin_options.NFERTTIMES; iFert++) {
                for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
                    iGrid = Grid[iCell][iBand];
                    for (l = 0; l < (size_t)iCrop; l++) {
                        iGrid = iGrid->next;
                    }

                    /* Determine if the sowing already has occurred */
                    IfSowing(iGrid, &iGrid->start);

                    if(iGrid->crp->Sowing != 1){
                        continue;
                    }
                    
                    crop_con[iCell][iCrop].DVS_point[iFert] =
                        crop_force[iCell].DVS_point[iCrop][iFert];
                    crop_con[iCell][iCrop].N_amount[iFert] =
                        crop_force[iCell].N_amount[iCrop][iFert];
                    crop_con[iCell][iCrop].P_amount[iFert] =
                        crop_force[iCell].P_amount[iCrop][iFert];
                    crop_con[iCell][iCrop].K_amount[iFert] =
                        crop_force[iCell].K_amount[iCrop][iFert];
                }
            }
        }
    }
}
