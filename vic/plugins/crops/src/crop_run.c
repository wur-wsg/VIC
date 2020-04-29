#include <vic_driver_image.h>
#include <plugin.h>

void
crop_reset_meteo(size_t iCell)
{
    extern option_struct options;
    extern SimUnit ***Grid;
    
    size_t iBand;
    
    SimUnit *cgrid;

    for(iBand = 0; iBand < options.SNOW_BAND; iBand ++) {
        cgrid = Grid[iCell][iBand];
        
        while(cgrid){
            cshift(cgrid->met->Tmin, 1, DAYS_PER_WEEK, 1, -1);

            cgrid->met->Tmin[0] = 9999;
            cgrid->met->Tmax = -9999;
            cgrid->met->Temp = 0.0;
            cgrid->met->DayTemp = 0.0;
            cgrid->met->Radiation = 0.0;
            cgrid->met->CO2 = 0.0;
            cgrid->met->PotEvaptrans = 0.0;

            cgrid = cgrid->next;
        }
    }
}

void
crop_register_meteo(size_t iCell)
{
    extern global_param_struct global_param;
    extern soil_con_struct *soil_con;
    extern option_struct options;
    extern force_data_struct *force;
    extern crop_force_struct *crop_force;
    extern all_vars_struct *all_vars;
    extern veg_con_map_struct *veg_con_map;
    extern size_t NR;
    
    size_t iBand;
    size_t crop_class;
    size_t veg_class;
    size_t iVeg;
    
    SimUnit *cgrid;
    cell_data_struct *ccell_data;

    for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
        cgrid = Grid[iCell][iBand];

        while(cgrid){
            crop_class = cgrid->met->crop_class;
            veg_class = crop_con_map[iCell].veg_class[crop_class];
            iVeg = veg_con_map[iCell].vidx[veg_class];
            ccell_data = &(all_vars[iCell].cell[iVeg][iBand]);

            cgrid->met->CO2 += crop_force[iCell].CO2 * 
                    (plugin_global_param.wofost_steps_per_day / global_param.atmos_steps_per_day);
            cgrid->met->Radiation += force[iCell].shortwave[NR] * SEC_PER_DAY * 
                    (plugin_global_param.wofost_steps_per_day / global_param.atmos_steps_per_day);
            cgrid->met->PotEvaptrans += ccell_data->pot_evap / MM_PER_CM * 
                    (plugin_global_param.wofost_steps_per_day / global_param.atmos_steps_per_day);

            cgrid->met->Tmin[0] = min(cgrid->met->Tmin[0], force[iCell].air_temp[NR] + soil_con[iCell].Tfactor[iBand]);
            cgrid->met->Tmax = max(cgrid->met->Tmax, force[iCell].air_temp[NR] + soil_con[iCell].Tfactor[iBand]);
            cgrid->met->Temp = 0.5 * (cgrid->met->Tmin[0] + cgrid->met->Tmax);
            cgrid->met->DayTemp = 0.5 * (cgrid->met->Temp + cgrid->met->Tmax);
            
            cgrid = cgrid->next;
        }
    }
}

void
crop_reset_soil(size_t iCell)
{
    extern option_struct options;
    extern SimUnit ***Grid;
    
    size_t iBand;
    
    SimUnit *cgrid;

    for(iBand = 0; iBand < options.SNOW_BAND; iBand ++) {
        cgrid = Grid[iCell][iBand];
        
        while(cgrid){
            cgrid->soil->WaterStress = 0.0;

            cgrid = cgrid->next;
        }
    }
}

void
crop_register_soil(size_t iCell)
{
    extern global_param_struct global_param;
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern veg_con_map_struct *veg_con_map;
    
    size_t iBand;
    size_t crop_class;
    size_t veg_class;
    size_t iVeg;
    
    SimUnit *cgrid;

    for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
        cgrid = Grid[iCell][iBand];

        while(cgrid){
            crop_class = cgrid->met->crop_class;
            veg_class = crop_con_map[iCell].veg_class[crop_class];
            iVeg = veg_con_map[iCell].vidx[veg_class];
            
            if (plugin_options.WOFOST_PIRR) {
                cgrid->soil->WaterStress += 1. / 
                        global_param.atmos_steps_per_day / plugin_global_param.wofost_steps_per_day;
            } else {
                cgrid->soil->WaterStress += all_vars[iCell].cell[iVeg][iBand].water_stress / 
                        global_param.atmos_steps_per_day / plugin_global_param.wofost_steps_per_day;
            }

            cgrid = cgrid->next;
        }
    }
}

bool
crop_run_flag(void)
{
    extern global_param_struct global_param;
    extern dmy_struct *dmy;
    extern size_t current;
    
    if (current > 0 &&
            dmy[current].dayseconds == 
            SEC_PER_DAY - (SEC_PER_DAY / global_param.model_steps_per_day)) {
        return true;
    } else {
        return false;
    }
}

bool
crop_reset_flag(void)
{
    extern dmy_struct *dmy;
    extern size_t current;
    
    if (current == 0 ||
            dmy[current].dayseconds == 0) {
        return true;
    } else {
        return false;
    }
}

void
crop_run(size_t iCell)
{
    extern global_param_struct global_param;
    extern option_struct options;
    extern crop_con_map_struct *crop_con_map;
    extern veg_con_map_struct *veg_con_map;
    extern veg_con_struct **veg_con;
    extern SimUnit ***Grid;
    extern dmy_struct *dmy;
    extern size_t current;
    
    size_t iBand;
    size_t crop_class;
    size_t veg_class;
    size_t iVeg;
    
    SimUnit *cgrid;
    
    if(crop_reset_flag()){
        crop_reset_meteo(iCell);
        crop_reset_soil(iCell);
    }
    crop_register_meteo(iCell);
    crop_register_soil(iCell);
    
    if(crop_run_flag()){
        for(iBand = 0; iBand < options.SNOW_BAND; iBand ++) {
            cgrid = Grid[iCell][iBand];

            while(cgrid){
                crop_class = cgrid->met->crop_class;
                veg_class = crop_con_map[iCell].veg_class[crop_class];
                iVeg = veg_con_map[iCell].vidx[veg_class];
                
                cgrid->met->MeteoDay = dmy[current].day_in_year;
                cgrid->met->MeteoYear = dmy[current].year;

                if(cgrid->met->MeteoDay <= 0){
                    cgrid->met->MeteoDay += DAYS_PER_YEAR + leap_year(cgrid->met->MeteoYear - 1, global_param.calendar);
                }

                wofost_run(cgrid);
                
                /* Check crop coverage */
                if (cgrid->growing == 1) {
                    if (crop_con_map[iCell].Cc[crop_class][dmy[current].month - 1] <= 0) {
                        log_err("Crop %zu coverage is <= 0 [%.4f] but "
                                "growing is true for cell %zu",
                                crop_class,
                                crop_con_map[iCell].Cc[crop_class][dmy[current].month - 1],
                                iCell);
                    }
                    if (veg_con[iCell][iVeg].Cv <= 0) {
                        log_err("Crop %zu vegetation %zu coverage is <= 0 [%.4f] but "
                                "growing is true for cell %zu",
                                crop_class,
                                veg_class,
                                veg_con[iCell][iVeg].Cv,
                                iCell);
                    }
                }

                cgrid = cgrid->next;
            }
        }
    }
}
