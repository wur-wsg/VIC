#include <vic_driver_image.h>
#include <plugin.h>

void
initialize_wofost_table(TABLE *tbl)
{
    tbl->x = 0.;
    tbl->y = 0.;
    tbl->next = NULL;
}

void
initialize_wofost_table_d(TABLE_D *tbld)
{
    tbld->amount = 0.;
    tbld->day = 0;
    tbld->month = 0;
    tbld->next = NULL;
}

void
initialize_wofost_rates(Rates *rt)
{
    rt->EvapWater = 0.;
    rt->EvapSoil = 0.;
    rt->Infiltration = 0.;
    rt->Irrigation = 0.;
    rt->Loss = 0.;
    rt->Moisture = 0.;
    rt->MoistureLOW = 0.;
    rt->Percolation = 0.;
    rt->RootZoneMoisture = 0.;
    rt->Runoff = 0.;
    rt->Transpiration = 0.;
    rt->WaterRootExt = 0.;
}

void
initialize_wofost_states(States *st)
{
    st->EvapWater = 0.;
    st->EvapSoil = 0.;
    st->Infiltration = 0.;
    st->Irrigation = 0.;
    st->Loss = 0.;
    st->Moisture = 0.;
    st->MoistureLOW = 0.;
    st->Percolation = 0.;
    st->Rain = 0.;
    st->RootZoneMoisture = 0.;
    st->Runoff = 0.;
    st->SurfaceStorage = 0.;
    st->Transpiration = 0.;
    st->WaterRootExt = 0.;
}

void
initialize_wofost_constants(Constants *ct)
{
    ct->MaxEvapWater = 0.;
    ct->MoistureFC = 0.;
    ct->MoistureWP = 0.;
    ct->MoistureSAT = 0.;
    ct->CriticalSoilAirC = 0.;
    ct->MaxPercolRTZ = 0.;
    ct->MaxPercolSubS = 0.;
    ct->MaxSurfaceStorge = 0.;
    ct->K0 = 0.;
}

void
initialize_wofost_geen(Green *gr)
{
    gr->age = 0.;
    gr->area = 0.;
    gr->weight = 0.;
    gr->next = NULL;
}

void
initialize_wofost_growth_rates(growth_rates *rt)
{
    rt->Development = 0.;
    rt->LAIExp = 0.;
    rt->RootDepth = 0.;
    rt->leaves = 0.;
    rt->roots = 0.;
    rt->stems = 0.;
    rt->storage = 0.;
    rt->vernalization = 0.;
}

void
initialize_wofost_growth_states(growth_states *st)
{
    st->Development = 0.;
    st->Development_prev = 0.;
    st->LAI = 0.;
    st->LAIExp = 0.;
    st->RootDepth = 0.;
    st->RootDepth_prev = 0.;
    st->leaves = 0.;
    st->roots = 0.;
    st->stems = 0.;
    st->storage = 0.;
    st->vernalization = 0.;
}

void
initialize_wofost_dying_rates(dying_rates *rt)
{
    rt->leaves = 0.;
    rt->roots = 0.;
    rt->stems = 0.;
}

void
initialize_wofost_dying_states(dying_states *st)
{
    st->leaves = 0.;
    st->roots = 0.;
    st->stems = 0.;
}

void
initialize_wofost_nutrient_rates(nutrient_rates *rt)
{
    rt->roots = 0.;
    rt->stems = 0.;
    rt->leaves = 0.;
    rt->storage = 0.;
    rt->Demand_lv = 0.;
    rt->Demand_st = 0.;
    rt->Demand_ro = 0.;
    rt->Demand_so = 0.;
    rt->Supply = 0.;
    rt->Transloc = 0.;
    rt->Transloc_lv = 0.;
    rt->Transloc_st = 0.;
    rt->Transloc_ro = 0.;
    rt->Uptake = 0.;
    rt->Uptake_lv = 0.;
    rt->Uptake_st = 0.;
    rt->Uptake_ro = 0.;
    rt->death_lv = 0.;
    rt->death_st = 0.; 
}

void
initialize_wofost_nutrient_states(nutrient_states *st)
{
    st->roots = 0.;
    st->stems = 0.;
    st->leaves = 0.;
    st->storage = 0.;
    st->Max_lv = 0.;
    st->Max_st = 0.;
    st->Max_ro = 0.;
    st->Max_so = 0.;
    st->Optimum_lv = 0.;
    st->Optimum_st = 0.;
    st->Indx = 0.;
    st->Uptake = 0.;
    st->Uptake_lv = 0.;
    st->Uptake_st = 0.;
    st->Uptake_ro = 0.;
    st->death_lv = 0.;
    st->death_st = 0.;
    st->Avail = 0.;
    st->Avail_lv = 0.;
    st->Avail_st = 0.;
    st->Avail_ro = 0.;
}

void
initialize_wofost_parameters(Parameters *prm)
{
    prm->Roots = NULL;
    prm->Stems = NULL;
    prm->Leaves = NULL;
    prm->Storage = NULL;
    prm->VernalizationRate = NULL;
    prm->DeltaTempSum = NULL;
    prm->SpecificLeaveArea = NULL;
    prm->SpecificStemArea = NULL;
    prm->KDiffuseTb = NULL;
    prm->EFFTb = NULL;
    prm->MaxAssimRate = NULL;
    prm->FactorAssimRateTemp = NULL;
    prm->FactorGrossAssimTemp = NULL;
    prm->FactorSenescence = NULL;
    prm->DeathRateStems = NULL;
    prm->DeathRateRoots = NULL;
    prm->CO2AMAXTB = NULL;
    prm->CO2EFFTB = NULL;
    prm->CO2TRATB = NULL;
    prm->N_MaxLeaves = NULL;
    prm->P_MaxLeaves = NULL;
    prm->K_MaxLeaves = NULL;
    
    prm->TempBaseEmergence          = 0.;
    prm->TempEffMax          = 0.;
    prm->TSumEmergence          = 0.;
    prm->IdentifyAnthesis          = 0;
    prm->OptimumDaylength          = 0.;
    prm->CriticalDaylength          = 0.;
    prm->SatVernRequirement          = 0.;
    prm->BaseVernRequirement          = 0.;
    prm->TempSum1          = 0.;
    prm->TempSum2          = 0.;
    prm->InitialDVS          = 0.;
    prm->DevelopStageHarvest          = 0.;
    prm->InitialDryWeight          = 0.;
    prm->RelIncreaseLAI          = 0.;
    prm->SpecificPodArea          = 0.;
    prm->LifeSpan          = 0.;
    prm->TempBaseLeaves          = 0.;
    prm->ConversionLeaves          = 0.;
    prm->ConversionStorage          = 0.;
    prm->ConversionRoots          = 0.;
    prm->ConversionStems          = 0.;
    prm->Q10          = 0.;
    prm->RelRespiLeaves          = 0.;
    prm->RelRespiStorage          = 0.;
    prm->RelRespiRoots          = 0.;
    prm->RelRespiStems          = 0.;
    prm->MaxRelDeathRate          = 0.;
    prm->CorrectionTransp          = 0.;
    prm->CropGroupNumber          = 0.;
    prm->Airducts          = 0.;
    prm->InitRootingDepth          = 0.;
    prm->MaxIncreaseRoot          = 0.;
    prm->MaxRootingDepth          = 0.;
    prm->DyingLeaves_NPK_Stress          = 0.;
    prm->DevelopmentStageNLimit          = 0.;
    prm->DevelopmentStageNT          = 0.;
    prm->FracTranslocRoots          = 0.;
    prm->Opt_N_Frac          = 0.;
    prm->Opt_P_Frac          = 0.;
    prm->Opt_K_Frac          = 0.;
    prm->N_MaxRoots          = 0.;
    prm->P_MaxRoots          = 0.;
    prm->K_MaxRoots          = 0.;
    prm->N_MaxStems          = 0.;
    prm->P_MaxStems          = 0.;
    prm->K_MaxStems          = 0.; 
    prm->NitrogenStressLAI = 0.;
    prm->NLUE = 0.;
    prm->Max_N_storage          = 0.;
    prm->Max_P_storage          = 0.;
    prm->Max_K_storage          = 0.;
    prm->N_lv_partitioning          = 0.;
    prm->NutrientStessSLA          = 0.;
    prm->N_ResidualFrac_lv          = 0.;
    prm->N_ResidualFrac_st          = 0.;
    prm->N_ResidualFrac_ro          = 0.;
    prm->P_ResidualFrac_lv          = 0.;
    prm->P_ResidualFrac_st          = 0.;
    prm->P_ResidualFrac_ro          = 0.;
    prm->K_ResidualFrac_lv          = 0.;
    prm->K_ResidualFrac_st          = 0.;
    prm->K_ResidualFrac_ro          = 0.;
    prm->TCNT          = 0.;
    prm->TCPT          = 0.;
    prm->TCKT          = 0.;
    prm->N_fixation                 = 0.;
    prm->MaxHeight                  = 0.;
    prm->RGL                        = 0.;
    prm->RadiationAttenuation       = 0.;
    prm->WindAttenuation            = 0.;
    prm->TrunkRatio                 = 0.;
    prm->Albedo                     = 0.;
    prm->MinStomatalResistance      = 0.;
    prm->MaxArchitecturalResistance = 0.;
}

void
initialize_wofost_plant(Plant *crp)
{
    crp->DaysOxygenStress = 0;
    crp->Emergence = 0;
    crp->CultivateDay = 0;
    crp->GrowthDay = 0;
    crp->NPK_Indx = 0.;
    crp->NutrientStress = 0.;
    crp->Sowing = 0;
    crp->TSumEmergence = 0.;
    crp->fac_lv = 0.;
    crp->fac_ro = 0.;
    crp->fac_so = 0.;
    crp->fac_st = 0.;
    
    initialize_wofost_parameters(&crp->prm);
    initialize_wofost_growth_rates(&crp->rt);
    initialize_wofost_growth_states(&crp->st);
    initialize_wofost_dying_rates(&crp->drt);
    initialize_wofost_dying_states(&crp->dst);
    initialize_wofost_nutrient_rates(&crp->N_rt);
    initialize_wofost_nutrient_states(&crp->N_st);
    initialize_wofost_nutrient_rates(&crp->K_rt);
    initialize_wofost_nutrient_states(&crp->K_st);
    initialize_wofost_nutrient_rates(&crp->P_rt);
    initialize_wofost_nutrient_states(&crp->P_st);
    
    crp->LeaveProperties = NULL;
}

void
initialize_wofost_field(Field *ste)
{
    ste->FlagGroundWater = 0.;
    ste->InfRainDependent = 0.;
    ste->FlagDrains = 0.;
    ste->MaxSurfaceStorage = 0.;
    ste->InitSoilMoisture = 0.;
    ste->GroundwaterDepth = 0.;
    ste->DD = 0.;
    ste->SoilLimRootDepth = 0.;
    ste->NotInfiltrating = 0.;
    ste->SurfaceStorage = 0.;
    ste->MaxInitSoilM = 0.;
    ste->st_N_tot = 0.;
    ste->st_P_tot = 0.;
    ste->st_K_tot = 0.;
    ste->st_N_mins = 0.;
    ste->st_P_mins = 0.;
    ste->st_K_mins = 0.;
    ste->rt_N_tot = 0.;
    ste->rt_P_tot = 0.;
    ste->rt_K_tot = 0.;
    ste->rt_N_mins = 0.;
    ste->rt_P_mins = 0.;
    ste->rt_K_mins = 0.;
    
    ste->NotInfTB = NULL;
}

void
initialize_wofost_soil(Soil *soil)
{
    soil->DaysSinceLastRain = 0.;
    soil->InfPreviousDay = 0.;
    soil->SoilMaxRootingDepth = 0.;
    soil->WaterStress = 0.;
    
    initialize_wofost_states(&soil->st);
    initialize_wofost_rates(&soil->rt);
    initialize_wofost_constants(&soil->ct);
    
    soil->HydraulicConductivity = NULL;
    soil->VolumetricSoilMoisture = NULL;
}

void
initialize_wofost_vic(Meteo *met)
{
    size_t i;
    
    met->crop_class = MISSING_USI;
    met->Latitude = 0.;
    met->Longitude = 0.;
    met->MeteoDay = 0;
    met->MeteoYear = 0;
    met->CycleLength = 0;
    for(i = 0; i < DAYS_PER_WEEK; i++) {
        met->Tmin[i] = 0.;
    }
    met->Tmax = 0.;
    met->Temp = 0.;
    met->DayTemp = 0.;
    met->Radiation = 0.;
    met->CO2 = 0.;
    met->PotEvaptrans = 0.;
    met->AtmosphTransm = 0.;
    met->AngotRadiation = 0.;
    met->Daylength = 0.;
    met->PARDaylength = 0.;
    met->SinLD = 0.;
    met->CosLD = 0.;
    met->DiffRadPP = 0.;
    met->DSinBE = 0.;
}

void
initialize_wofost_management(Management *mng)
{
    mng->N_Mins = 0.;
    mng->NRecoveryFrac = 0.;
    mng->P_Mins = 0.;
    mng->PRecoveryFrac = 0.;
    mng->K_Mins = 0.;
    mng->KRecoveryFrac = 0.;
    mng->N_Uptake_frac = 0.;
    mng->P_Uptake_frac = 0.;
    mng->K_Uptake_frac = 0.;
    mng->N_external = 0.;
    mng->P_external = 0.;
    mng->K_external = 0.;
    
    mng->N_Fert_table = NULL;
    mng->P_Fert_table = NULL;
    mng->K_Fert_table = NULL;
    mng->Irrigation = NULL;
}

void
initialize_wofost_grid(SimUnit *Grid)
{
    Grid->emergence = 0;
    Grid->file = 0;
    Grid->cultivating = 0;
    Grid->growing = 0;
    initialize_dmy(&Grid->start);
    initialize_dmy(&Grid->end);
    
    initialize_wofost_plant(Grid->crp);
    initialize_wofost_management(Grid->mng);
    initialize_wofost_field(Grid->ste);
    initialize_wofost_soil(Grid->soil);
    initialize_wofost_vic(Grid->met);
}

void
wofost_initialize_local_structures(void)
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
