#include <vic_driver_image.h>
#include <plugin.h>

void
crop_initialize_local_structures(void)
{
    extern option_struct options;
    extern domain_struct        local_domain;
    extern SimUnit                  ****Grid;
    extern veg_con_map_struct        *veg_con_map;

    SimUnit                    *iGrid;
    size_t                      i;
    size_t                      j;
    size_t                      k;
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < veg_con_map[i].nv_active; j++) {
            for(k = 0; k < options.SNOW_BAND; k++) {

                iGrid = Grid[i][j][k];
                while(iGrid) {
                    initialize_wofost_grid(iGrid);
                    iGrid = iGrid->next;
                }
            }
        }
    }
}
