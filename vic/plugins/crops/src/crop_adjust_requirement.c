#include <vic_driver_image.h>
#include <plugin.h>

/******************************************
* @brief    Adjust irrigation requirements if crop doesn't grow
******************************************/
void
crop_adjust_requirement(size_t iCell)
{
    extern global_param_struct  global_param;
    extern option_struct        options;
    extern irr_con_map_struct  *irr_con_map;
    extern irr_con_struct     **irr_con;
    extern irr_var_struct    ***irr_var;
    extern soil_con_struct     *soil_con;
    extern veg_con_struct     **veg_con;
    extern crop_con_map_struct *crop_con_map;
    extern SimUnit           ***Grid;
    extern dmy_struct          *dmy;
    extern size_t               current;

    bool                        isCrop;
    bool                        isCultivated;
    bool                        isOffset;
    int                         plantDay;
    int                         currentDay;
    double                      veg_fract;
    double                      area_fract;

    irr_con_struct             *cirr_con;
    irr_var_struct             *cirr_var;
    soil_con_struct            *csoil_con;
    veg_con_struct             *cveg_con;
    SimUnit                    *cgrid;

    int                         veg_class;
    size_t                      iBand;
    size_t                      iIrr;
    size_t                      crop_class;

    csoil_con = &(soil_con[iCell]);

    for (iIrr = 0; iIrr < irr_con_map[iCell].ni_active; iIrr++) {
        cirr_con = &(irr_con[iCell][iIrr]);
        cveg_con = &(veg_con[iCell][cirr_con->veg_index]);

        veg_fract = cveg_con->Cv;
        veg_class = cveg_con->veg_class;

        if (veg_fract > 0) {
            for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
                cirr_var = &(irr_var[iCell][iIrr][iBand]);

                area_fract = csoil_con->AreaFract[iBand];

                if (area_fract > 0) {
                    if (cirr_var->flag_req) {
                        cgrid = Grid[iCell][iBand];

                        isCrop = false;
                        isCultivated = false;
                        isOffset = false;
                        while (cgrid) {
                            crop_class = cgrid->met->crop_class;

                            if (crop_con_map[iCell].veg_class[crop_class] ==
                                veg_class) {
                                isCrop = true;
                                
                                if (cgrid->cultivating > 0) {
                                    isCultivated = true;
                                }
                                
                                plantDay = cgrid->start.day_in_year;
                                currentDay = dmy[current].day_in_year;
                                if(plantDay < currentDay){
                                    plantDay += DAYS_PER_YEAR + 
                                            leap_year(dmy[current].year, 
                                                      global_param.calendar);
                                }
                                if(plantDay - currentDay < cirr_con->offset){
                                    isOffset = true;
                                }
                            }

                            cgrid = cgrid->next;
                        }

                        if (isCrop && !isCultivated && !isOffset) {
                            cirr_var->flag_req = false;
                        }
                    }
                }
            }
        }
    }
}
