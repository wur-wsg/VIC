#include <vic_driver_image.h>
#include <plugin.h>

void
crop_reset_meteo(size_t iCell)
{
    extern option_struct options;
    extern SimUnit ****Grid;
    extern veg_con_map_struct *veg_con_map;
    
    size_t iVeg;
    size_t iBand;
    SimUnit *iGrid;

    for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg ++){
        for(iBand = 0; iBand < options.SNOW_BAND; iBand ++) {
            
            iGrid = Grid[iCell][iVeg][iBand];
            while(iGrid){
                cshift(iGrid->vic->Tmin, 1, DAYS_PER_WEEK, 1, -1);
                
                iGrid->vic->Tmin[0] = 9999;
                iGrid->vic->Tmax = -9999;
                iGrid->vic->Temp = 0.0;
                iGrid->vic->DayTemp = 0.0;
                iGrid->vic->Radiation = 0.0;
                iGrid->vic->CO2 = 0.0;
                
                iGrid->vic->WaterStressAvg += iGrid->vic->WaterStress;
                iGrid->vic->WaterStress = 0.0;
                
                iGrid = iGrid->next;
            }
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
    extern veg_con_struct **veg_con;
    extern size_t NR;
    
    float shortwave;
    float co2;
    float water_stress;
    
    double temp;
    double stress;
    double root;
    size_t iVeg;
    size_t iBand;
    size_t iLayer;
    SimUnit *iGrid;
    
    for(iBand = 0; iBand < options.SNOW_BAND; iBand++){
        temp = force[iCell].air_temp[NR] + soil_con[iCell].Tfactor[iBand];
        shortwave = force[iCell].shortwave[NR] * SEC_PER_DAY;
        co2 = crop_force[iCell].CO2[NR];

        for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg ++){
            water_stress = 0;
            for(iLayer = 0; iLayer < options.Nlayer; iLayer++){
                stress = all_vars[iCell].cell[iVeg][iBand].layer[iLayer].water_stress;
                root = veg_con[iCell][iVeg].root[iLayer];
                
                water_stress += stress * root;
            }
            
            iGrid = Grid[iCell][iVeg][iBand];
            while(iGrid){
                iGrid->vic->CO2 += co2;
                iGrid->vic->Radiation += shortwave;
                
                //iGrid->vic->WaterStress += water_stress;
                iGrid->vic->WaterStress += 1.0; // TEMPORARY FOR TESTING
                
                iGrid->vic->Tmin[0] = min(iGrid->vic->Tmin[0], temp);
                iGrid->vic->Tmax = max(iGrid->vic->Tmax, temp);
                //iGrid->vic->Temp += temp;
                iGrid->vic->Temp = 0.5 * (iGrid->vic->Tmin[0] + iGrid->vic->Tmax);
                iGrid->vic->DayTemp = 0.5 * (iGrid->vic->Temp + iGrid->vic->Tmax);
            
                iGrid = iGrid->next;
            }
        }
    }
}

void
crop_run(size_t iCell)
{
    extern global_param_struct global_param;
    extern option_struct options;
    extern veg_con_map_struct *veg_con_map;
    extern SimUnit ****Grid;
    extern dmy_struct *dmy;
    extern size_t current;
    
    size_t iVeg;
    size_t iBand;
    SimUnit *iGrid;
    
    if(current == 0){
        crop_reset_meteo(iCell);
    }
    
    if(current > 0 && dmy[current].day != dmy[current - 1].day){
        for(iVeg = 0; iVeg < veg_con_map[iCell].nv_active; iVeg++){
            for(iBand = 0; iBand < options.SNOW_BAND; iBand ++) {
                iGrid = Grid[iCell][iVeg][iBand];

                while(iGrid){
                    iGrid->vic->MeteoDay = dmy[current].day_in_year - 1;
                    iGrid->vic->MeteoYear = dmy[current].year;
                    
                    iGrid->vic->CO2 /= global_param.atmos_steps_per_day;
                    iGrid->vic->Radiation /= global_param.atmos_steps_per_day;
                    iGrid->vic->WaterStress /= global_param.atmos_steps_per_day;
                    
                    iGrid->vic->Radiation = 26370866; // TEMPORARY FOR TESTING
                    iGrid->vic->Temp = 22; // TEMPORARY FOR TESTING
                    iGrid->vic->DayTemp = 25; // TEMPORARY FOR TESTING
                    
                    if(iGrid->vic->MeteoDay <= 0){
                        iGrid->vic->MeteoDay += DAYS_PER_JYEAR + leap_year(iGrid->vic->MeteoYear - 1, global_param.calendar);
                    }
                    
                    wofost_run(iGrid);
                    
                    iGrid = iGrid->next;
                }
            }
        }
        
        crop_reset_meteo(iCell);
    }
    
    crop_register_meteo(iCell);
}