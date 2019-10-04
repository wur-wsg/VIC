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
            cshift(cgrid->vic->Tmin, 1, DAYS_PER_WEEK, 1, -1);

            cgrid->vic->Tmin[0] = 9999;
            cgrid->vic->Tmax = -9999;
            cgrid->vic->Temp = 0.0;
            cgrid->vic->DayTemp = 0.0;
            cgrid->vic->Radiation = 0.0;
            cgrid->vic->CO2 = 0.0;
            cgrid->vic->WaterStress = 0.0;

            cgrid = cgrid->next;
        }
    }
}

void
crop_register_meteo(size_t iCell)
{
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

    for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
        cgrid = Grid[iCell][iBand];

        while(cgrid){
            crop_class = cgrid->vic->crop_class;
            veg_class = crop_con_map[iCell].veg_class[crop_class];
            iVeg = veg_con_map[iCell].vidx[veg_class];

            cgrid->vic->CO2 += crop_force[iCell].CO2;
            cgrid->vic->Radiation += force[iCell].shortwave[NR] * SEC_PER_DAY;
            cgrid->vic->WaterStress += all_vars[iCell].cell[iVeg][iBand].water_stress;

            cgrid->vic->Tmin[0] = min(cgrid->vic->Tmin[0], force[iCell].air_temp[NR] + soil_con[iCell].Tfactor[iBand]);
            cgrid->vic->Tmax = max(cgrid->vic->Tmax, force[iCell].air_temp[NR] + soil_con[iCell].Tfactor[iBand]);
            cgrid->vic->Temp = 0.5 * (cgrid->vic->Tmin[0] + cgrid->vic->Tmax);
            cgrid->vic->DayTemp = 0.5 * (cgrid->vic->Temp + cgrid->vic->Tmax);
        }
    }
}

void
crop_run(size_t iCell)
{
    extern global_param_struct global_param;
    extern option_struct options;
    extern SimUnit ***Grid;
    extern dmy_struct *dmy;
    extern size_t current;
    
    size_t iBand;
    
    SimUnit *cgrid;
    
    if(current == 0){
        crop_reset_meteo(iCell);
    }
    
    if(current > 0 && dmy[current].day != dmy[current - 1].day){
        for(iBand = 0; iBand < options.SNOW_BAND; iBand ++) {
            cgrid = Grid[iCell][iBand];

            while(cgrid){
                cgrid->vic->MeteoDay = dmy[current].day_in_year - 1;
                cgrid->vic->MeteoYear = dmy[current].year;

                cgrid->vic->CO2 /= global_param.atmos_steps_per_day / plugin_global_param.wofost_steps_per_day;
                cgrid->vic->Radiation /= global_param.atmos_steps_per_day / plugin_global_param.wofost_steps_per_day;
                cgrid->vic->WaterStress /= global_param.atmos_steps_per_day / plugin_global_param.wofost_steps_per_day;

                if(cgrid->vic->MeteoDay <= 0){
                    cgrid->vic->MeteoDay += DAYS_PER_JYEAR + leap_year(cgrid->vic->MeteoYear - 1, global_param.calendar);
                }

                wofost_run(cgrid);

                cgrid = cgrid->next;
            }
        }
        
        crop_reset_meteo(iCell);
    }
    
    crop_register_meteo(iCell);
}