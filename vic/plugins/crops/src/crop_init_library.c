#include <vic_driver_image.h>
#include <plugin.h>

void
crop_initialize_local_structures(void)
{
    extern option_struct options;
    extern domain_struct        local_domain;
    extern SimUnit                  ***Grid;

    SimUnit                    *iGrid;
    size_t                      i;
    size_t                      j;
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < options.SNOW_BAND; j++) {

            iGrid = Grid[i][j];
            while(iGrid) {
                initialize_wofost_grid(iGrid);
                iGrid = iGrid->next;
            }
        }
    }
}
