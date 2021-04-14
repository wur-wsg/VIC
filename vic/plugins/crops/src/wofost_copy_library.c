#include <vic_driver_image.h>
#include <plugin.h>

void
copy_wofost_table(TABLE *from,
                  TABLE *to)
{
    to->x = from->x;
    to->y = from->y;
    to->next = NULL;
}

void
copy_wofost_table_list(TABLE **from,
                       TABLE **to)
{
    TABLE *from_start, *to_start;

    to_start = NULL;
    from_start = (*from);

    while ((*from)) {
        if (to_start == NULL) {
            (*to) = to_start = malloc(sizeof(TABLE));
            check_alloc_status(to, "Memory allocation error");
        }
        else {
            (*to)->next = malloc(sizeof(TABLE));
            check_alloc_status((*to)->next, "Memory allocation error");
            (*to) = (*to)->next;
        }

        copy_wofost_table((*from), (*to));
        (*from) = (*from)->next;
    }

    (*from) = from_start;
    (*to) = to_start;
}

void
copy_wofost_table_d(TABLE_D *from,
                    TABLE_D *to)
{
    to->amount = from->amount;
    to->day = from->day;
    to->month = from->month;
    to->next = NULL;
}

void
copy_wofost_table_d_list(TABLE_D **from,
                         TABLE_D **to)
{
    TABLE_D *from_start, *to_start;

    to_start = NULL;
    from_start = (*from);

    while ((*from)) {
        if (to_start == NULL) {
            (*to) = to_start = malloc(sizeof(TABLE_D));
            check_alloc_status((*to), "Memory allocation error");
        }
        else {
            (*to)->next = malloc(sizeof(TABLE_D));
            check_alloc_status((*to), "Memory allocation error");
            (*to) = (*to)->next;
        }

        copy_wofost_table_d((*from), (*to));
        (*from) = (*from)->next;
    }

    (*from) = from_start;
    (*to) = to_start;
}

void
copy_wofost_rates(Rates *from,
                  Rates *to)
{
    to->EvapWater = from->EvapWater;
    to->EvapSoil = from->EvapSoil;
    to->Infiltration = from->Infiltration;
    to->Irrigation = from->Irrigation;
    to->Loss = from->Loss;
    to->Moisture = from->Moisture;
    to->MoistureLOW = from->MoistureLOW;
    to->Percolation = from->Percolation;
    to->RootZoneMoisture = from->RootZoneMoisture;
    to->Runoff = from->Runoff;
    to->Transpiration = from->Transpiration;
    to->WaterRootExt = from->WaterRootExt;
}

void
copy_wofost_states(States *from,
                   States *to)
{
    to->EvapWater = from->EvapWater;
    to->EvapSoil = from->EvapSoil;
    to->Infiltration = from->Infiltration;
    to->Irrigation = from->Irrigation;
    to->Loss = from->Loss;
    to->Moisture = from->Moisture;
    to->MoistureLOW = from->MoistureLOW;
    to->Percolation = from->Percolation;
    to->Rain = from->Rain;
    to->RootZoneMoisture = from->RootZoneMoisture;
    to->Runoff = from->Runoff;
    to->SurfaceStorage = from->SurfaceStorage;
    to->Transpiration = from->Transpiration;
    to->WaterRootExt = from->WaterRootExt;
}

void
copy_wofost_constants(Constants *from,
                      Constants *to)
{
    to->MaxEvapWater = from->MaxEvapWater;
    to->MoistureFC = from->MoistureFC;
    to->MoistureWP = from->MoistureWP;
    to->MoistureSAT = from->MoistureSAT;
    to->CriticalSoilAirC = from->CriticalSoilAirC;
    to->MaxPercolRTZ = from->MaxPercolRTZ;
    to->MaxPercolSubS = from->MaxPercolSubS;
    to->MaxSurfaceStorge = from->MaxSurfaceStorge;
    to->K0 = from->K0;
}

void
copy_wofost_geen(Green *from,
                 Green *to)
{
    to->age = from->age;
    to->area = from->area;
    to->weight = from->weight;
    to->next = NULL;
}

void
copy_wofost_green_list(Green **from,
                       Green **to)
{
    Green *from_start, *to_start;

    to_start = NULL;
    from_start = (*from);

    while ((*from)) {
        if (to_start == NULL) {
            (*to) = to_start = malloc(sizeof(Green));
            check_alloc_status((*to), "Memory allocation error");
        }
        else {
            (*to)->next = malloc(sizeof(Green));
            check_alloc_status((*to), "Memory allocation error");
            (*to) = (*to)->next;
        }

        copy_wofost_geen((*from), (*to));
        (*from) = (*from)->next;
    }

    (*from) = from_start;
    (*to) = to_start;
}

void
copy_wofost_growth_rates(growth_rates *from,
                         growth_rates *to)
{
    to->Development = from->Development;
    to->LAIExp = from->LAIExp;
    to->RootDepth = from->RootDepth;
    to->leaves = from->leaves;
    to->roots = from->roots;
    to->stems = from->stems;
    to->storage = from->storage;
    to->vernalization = from->vernalization;
}

void
copy_wofost_growth_states(growth_states *from,
                          growth_states *to)
{
    to->Development = from->Development;
    to->Development_prev = from->Development_prev;
    to->LAI = from->LAI;
    to->LAIExp = from->LAIExp;
    to->RootDepth = from->RootDepth;
    to->RootDepth_prev = from->RootDepth_prev;
    to->leaves = from->leaves;
    to->roots = from->roots;
    to->stems = from->stems;
    to->storage = from->storage;
    to->vernalization = from->vernalization;
}

void
copy_wofost_dying_rates(dying_rates *from,
                        dying_rates *to)
{
    to->leaves = from->leaves;
    to->roots = from->roots;
    to->stems = from->stems;
}

void
copy_wofost_dying_states(dying_states *from,
                         dying_states *to)
{
    to->leaves = from->leaves;
    to->roots = from->roots;
    to->stems = from->stems;
}

void
copy_wofost_nutrient_rates(nutrient_rates *from,
                           nutrient_rates *to)
{
    to->roots = from->roots;
    to->stems = from->stems;
    to->leaves = from->leaves;
    to->storage = from->storage;
    to->Demand = from->Demand;
    to->Demand_lv = from->Demand_lv;
    to->Demand_st = from->Demand_st;
    to->Demand_ro = from->Demand_ro;
    to->Demand_so = from->Demand_so;
    to->Supply = from->Supply;
    to->Transloc = from->Transloc;
    to->Transloc_lv = from->Transloc_lv;
    to->Transloc_st = from->Transloc_st;
    to->Transloc_ro = from->Transloc_ro;
    to->Uptake = from->Uptake;
    to->Uptake_lv = from->Uptake_lv;
    to->Uptake_st = from->Uptake_st;
    to->Uptake_ro = from->Uptake_ro;
    to->death_lv = from->death_lv;
    to->death_st = from->death_st;
}

void
copy_wofost_nutrient_states(nutrient_states *from,
                            nutrient_states *to)
{
    to->roots = from->roots;
    to->stems = from->stems;
    to->leaves = from->leaves;
    to->storage = from->storage;
    to->Demand = from->Demand;
    to->Demand_lv = from->Demand_lv;
    to->Demand_st = from->Demand_st;
    to->Demand_ro = from->Demand_ro;
    to->Demand_so = from->Demand_so;
    to->Max_lv = from->Max_lv;
    to->Max_st = from->Max_st;
    to->Max_ro = from->Max_ro;
    to->Max_so = from->Max_so;
    to->Optimum_lv = from->Optimum_lv;
    to->Optimum_st = from->Optimum_st;
    to->Indx = from->Indx;
    to->Uptake = from->Uptake;
    to->Uptake_lv = from->Uptake_lv;
    to->Uptake_st = from->Uptake_st;
    to->Uptake_ro = from->Uptake_ro;
    to->death_lv = from->death_lv;
    to->death_st = from->death_st;
    to->Avail = from->Avail;
    to->Avail_lv = from->Avail_lv;
    to->Avail_st = from->Avail_st;
    to->Avail_ro = from->Avail_ro;
}

void
copy_wofost_parameters(Parameters *from,
                       Parameters *to)
{
    copy_wofost_table_list(&from->Roots, &to->Roots);
    copy_wofost_table_list(&from->Stems, &to->Stems);
    copy_wofost_table_list(&from->Leaves, &to->Leaves);
    copy_wofost_table_list(&from->Storage, &to->Storage);
    copy_wofost_table_list(&from->VernalizationRate, &to->VernalizationRate);
    copy_wofost_table_list(&from->DeltaTempSum, &to->DeltaTempSum);
    copy_wofost_table_list(&from->SpecificLeaveArea, &to->SpecificLeaveArea);
    copy_wofost_table_list(&from->SpecificStemArea, &to->SpecificStemArea);
    copy_wofost_table_list(&from->KDiffuseTb, &to->KDiffuseTb);
    copy_wofost_table_list(&from->EFFTb, &to->EFFTb);
    copy_wofost_table_list(&from->MaxAssimRate, &to->MaxAssimRate);
    copy_wofost_table_list(&from->FactorAssimRateTemp,
                           &to->FactorAssimRateTemp);
    copy_wofost_table_list(&from->FactorGrossAssimTemp,
                           &to->FactorGrossAssimTemp);
    copy_wofost_table_list(&from->FactorSenescence, &to->FactorSenescence);
    copy_wofost_table_list(&from->DeathRateStems, &to->DeathRateStems);
    copy_wofost_table_list(&from->DeathRateRoots, &to->DeathRateRoots);
    copy_wofost_table_list(&from->CO2AMAXTB, &to->CO2AMAXTB);
    copy_wofost_table_list(&from->CO2EFFTB, &to->CO2EFFTB);
    copy_wofost_table_list(&from->CO2TRATB, &to->CO2TRATB);
    copy_wofost_table_list(&from->N_MaxLeaves, &to->N_MaxLeaves);
    copy_wofost_table_list(&from->P_MaxLeaves, &to->P_MaxLeaves);
    copy_wofost_table_list(&from->K_MaxLeaves, &to->K_MaxLeaves);

    to->TempBaseEmergence = from->TempBaseEmergence;
    to->TempEffMax = from->TempEffMax;
    to->TSumEmergence = from->TSumEmergence;
    to->IdentifyAnthesis = from->IdentifyAnthesis;
    to->OptimumDaylength = from->OptimumDaylength;
    to->CriticalDaylength = from->CriticalDaylength;
    to->SatVernRequirement = from->SatVernRequirement;
    to->BaseVernRequirement = from->BaseVernRequirement;
    to->TempSum1 = from->TempSum1;
    to->TempSum2 = from->TempSum2;
    to->InitialDVS = from->InitialDVS;
    to->DevelopStageHarvest = from->DevelopStageHarvest;
    to->InitialDryWeight = from->InitialDryWeight;
    to->RelIncreaseLAI = from->RelIncreaseLAI;
    to->SpecificPodArea = from->SpecificPodArea;
    to->LifeSpan = from->LifeSpan;
    to->TempBaseLeaves = from->TempBaseLeaves;
    to->ConversionLeaves = from->ConversionLeaves;
    to->ConversionStorage = from->ConversionStorage;
    to->ConversionRoots = from->ConversionRoots;
    to->ConversionStems = from->ConversionStems;
    to->Q10 = from->Q10;
    to->RelRespiLeaves = from->RelRespiLeaves;
    to->RelRespiStorage = from->RelRespiStorage;
    to->RelRespiRoots = from->RelRespiRoots;
    to->RelRespiStems = from->RelRespiStems;
    to->CritLAIFactor = from->CritLAIFactor;
    to->MaxRelDeathRate = from->MaxRelDeathRate;
    to->CorrectionTransp = from->CorrectionTransp;
    to->CropGroupNumber = from->CropGroupNumber;
    to->Airducts = from->Airducts;
    to->InitRootingDepth = from->InitRootingDepth;
    to->MaxIncreaseRoot = from->MaxIncreaseRoot;
    to->MaxRootingDepth = from->MaxRootingDepth;
    to->DyingLeaves_NPK_Stress = from->DyingLeaves_NPK_Stress;
    to->DevelopmentStageNLimit = from->DevelopmentStageNLimit;
    to->DevelopmentStageNT = from->DevelopmentStageNT;
    to->FracTranslocRoots = from->FracTranslocRoots;
    to->Opt_N_Frac = from->Opt_N_Frac;
    to->Opt_P_Frac = from->Opt_P_Frac;
    to->Opt_K_Frac = from->Opt_K_Frac;
    to->N_MaxRoots = from->N_MaxRoots;
    to->P_MaxRoots = from->P_MaxRoots;
    to->K_MaxRoots = from->K_MaxRoots;
    to->N_MaxStems = from->N_MaxStems;
    to->P_MaxStems = from->P_MaxStems;
    to->K_MaxStems = from->K_MaxStems;
    to->NitrogenStressLAI = from->NitrogenStressLAI;
    to->NLUE = from->NLUE;
    to->Max_N_storage = from->Max_N_storage;
    to->Max_P_storage = from->Max_P_storage;
    to->Max_K_storage = from->Max_K_storage;
    to->N_lv_partitioning = from->N_lv_partitioning;
    to->NutrientStessSLA = from->NutrientStessSLA;
    to->N_ResidualFrac_lv = from->N_ResidualFrac_lv;
    to->N_ResidualFrac_st = from->N_ResidualFrac_st;
    to->N_ResidualFrac_ro = from->N_ResidualFrac_ro;
    to->P_ResidualFrac_lv = from->P_ResidualFrac_lv;
    to->P_ResidualFrac_st = from->P_ResidualFrac_st;
    to->P_ResidualFrac_ro = from->P_ResidualFrac_ro;
    to->K_ResidualFrac_lv = from->K_ResidualFrac_lv;
    to->K_ResidualFrac_st = from->K_ResidualFrac_st;
    to->K_ResidualFrac_ro = from->K_ResidualFrac_ro;
    to->TCNT = from->TCNT;
    to->TCPT = from->TCPT;
    to->TCKT = from->TCKT;
    to->N_fixation = from->N_fixation;
    to->MaxHeight = from->MaxHeight;
    to->RGL = from->RGL;
    to->RadiationAttenuation = from->RadiationAttenuation;
    to->WindAttenuation = from->WindAttenuation;
    to->TrunkRatio = from->TrunkRatio;
    to->Albedo = from->Albedo;
    to->MinStomatalResistance = from->MinStomatalResistance;
    to->MaxArchitecturalResistance = from->MaxArchitecturalResistance;
    to->Fcanopy = from->Fcanopy;
    to->TranslocationFrac = from->TranslocationFrac;
}

void
copy_wofost_plant(Plant *from,
                  Plant *to)
{
    to->DaysOxygenStress = from->DaysOxygenStress;
    to->Emergence = from->Emergence;
    to->CultivateDay = from->CultivateDay;
    to->GrowthDay = from->GrowthDay;
    to->NPK_Indx = from->NPK_Indx;
    to->NutrientStress = from->NutrientStress;
    to->Sowing = from->Sowing;
    to->TSumEmergence = from->TSumEmergence;
    to->fac_lv = from->fac_lv;
    to->fac_ro = from->fac_ro;
    to->fac_so = from->fac_so;
    to->fac_st = from->fac_st;

    copy_wofost_parameters(&from->prm, &to->prm);
    copy_wofost_growth_rates(&from->rt, &to->rt);
    copy_wofost_growth_states(&from->st, &to->st);
    copy_wofost_dying_rates(&from->drt, &to->drt);
    copy_wofost_dying_states(&from->dst, &to->dst);
    copy_wofost_nutrient_rates(&from->N_rt, &to->N_rt);
    copy_wofost_nutrient_states(&from->N_st, &to->N_st);
    copy_wofost_nutrient_rates(&from->K_rt, &to->K_rt);
    copy_wofost_nutrient_states(&from->K_st, &to->K_st);
    copy_wofost_nutrient_rates(&from->P_rt, &to->P_rt);
    copy_wofost_nutrient_states(&from->P_st, &to->P_st);

    copy_wofost_green_list(&from->LeaveProperties, &to->LeaveProperties);
}

void
copy_wofost_field(Field *from,
                  Field *to)
{
    to->FlagGroundWater = from->FlagGroundWater;
    to->InfRainDependent = from->InfRainDependent;
    to->FlagDrains = from->FlagDrains;
    to->MaxSurfaceStorage = from->MaxSurfaceStorage;
    to->InitSoilMoisture = from->InitSoilMoisture;
    to->GroundwaterDepth = from->GroundwaterDepth;
    to->DD = from->DD;
    to->SoilLimRootDepth = from->SoilLimRootDepth;
    to->NotInfiltrating = from->NotInfiltrating;
    to->SurfaceStorage = from->SurfaceStorage;
    to->MaxInitSoilM = from->MaxInitSoilM;
    to->st_N_tot = from->st_N_tot;
    to->st_P_tot = from->st_P_tot;
    to->st_K_tot = from->st_K_tot;
    to->st_N_mins = from->st_N_mins;
    to->st_P_mins = from->st_P_mins;
    to->st_K_mins = from->st_K_mins;
    to->rt_N_tot = from->rt_N_tot;
    to->rt_P_tot = from->rt_P_tot;
    to->rt_K_tot = from->rt_K_tot;
    to->rt_N_mins = from->rt_N_mins;
    to->rt_P_mins = from->rt_P_mins;
    to->rt_K_mins = from->rt_K_mins;

    copy_wofost_table_list(&from->NotInfTB, &to->NotInfTB);
}

void
copy_wofost_soil(Soil *from,
                 Soil *to)
{
    to->DaysSinceLastRain = from->DaysSinceLastRain;
    to->InfPreviousDay = from->InfPreviousDay;
    to->SoilMaxRootingDepth = from->SoilMaxRootingDepth;
    to->WaterStress = from->WaterStress;

    copy_wofost_states(&from->st, &to->st);
    copy_wofost_rates(&from->rt, &to->rt);
    copy_wofost_constants(&from->ct, &to->ct);

    copy_wofost_table_list(&from->HydraulicConductivity,
                           &to->HydraulicConductivity);
    copy_wofost_table_list(&from->VolumetricSoilMoisture,
                           &to->VolumetricSoilMoisture);
}

void
copy_wofost_vic(Meteo *from,
                Meteo *to)
{
    size_t i;

    to->crop_class = from->crop_class;
    to->Latitude = from->Latitude;
    to->Longitude = from->Longitude;
    to->MeteoDay = from->MeteoDay;
    to->MeteoYear = from->MeteoYear;
    to->CycleLength = from->CycleLength;
    for (i = 0; i < DAYS_PER_WEEK; i++) {
        to->Tmin[i] = from->Tmin[i];
    }
    to->Tmax = from->Tmax;
    to->Temp = from->Temp;
    to->DayTemp = from->DayTemp;
    to->Radiation = from->Radiation;
    to->CO2 = from->CO2;
    to->PotEvaptrans = from->PotEvaptrans;
    to->AtmosphTransm = from->AtmosphTransm;
    to->AngotRadiation = from->AngotRadiation;
    to->Daylength = from->Daylength;
    to->PARDaylength = from->PARDaylength;
    to->SinLD = from->SinLD;
    to->CosLD = from->CosLD;
    to->DiffRadPP = from->DiffRadPP;
    to->DSinBE = from->DSinBE;
}

void
copy_wofost_management(Management *from,
                       Management *to)
{
    to->N_Mins = from->N_Mins;
    to->NRecoveryFrac = from->NRecoveryFrac;
    to->P_Mins = from->P_Mins;
    to->PRecoveryFrac = from->PRecoveryFrac;
    to->K_Mins = from->K_Mins;
    to->KRecoveryFrac = from->KRecoveryFrac;
    to->N_Uptake_frac = from->N_Uptake_frac;
    to->P_Uptake_frac = from->P_Uptake_frac;
    to->K_Uptake_frac = from->K_Uptake_frac;

    copy_wofost_table_d_list(&from->N_Fert_table, &to->N_Fert_table);
    copy_wofost_table_d_list(&from->P_Fert_table, &to->P_Fert_table);
    copy_wofost_table_d_list(&from->K_Fert_table, &to->K_Fert_table);
    copy_wofost_table_d_list(&from->Irrigation, &to->Irrigation);
}

void
copy_wofost_grid(SimUnit *from,
                 SimUnit *to)
{
    to->cultivating = from->cultivating;
    to->growing = from->growing;
    to->emergence = from->emergence;
    to->file = from->file;
    copy_dmy(&from->start, &to->start);
    copy_dmy(&from->end, &to->end);

    copy_wofost_plant(from->crp, to->crp);
    copy_wofost_management(from->mng, to->mng);
    copy_wofost_field(from->ste, to->ste);
    copy_wofost_soil(from->soil, to->soil);
    copy_wofost_vic(from->met, to->met);
}
