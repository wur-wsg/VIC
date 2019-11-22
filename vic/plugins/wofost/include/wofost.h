#include <vic_def.h>

#ifndef WOFOST_H
#define WOFOST_H

#define NR_VARIABLES_CRP	74
#define NR_TABLES_CRP   	22
#define NR_VARIABLES_SITE       12
#define NR_TABLES_SITE          1
#define NR_VARIABLES_SOIL       12
#define NR_VARIABLES_SOIL_USED  6
#define NR_TABLES_SOIL          2
#define NR_VARIABLES_MANAGEMENT 9
#define NR_TABLES_MANAGEMENT    4
#define NUMBER_OF_TABLES        31

typedef struct TBL {
	float x;
	float y;
	struct TBL *next;
	} TABLE;
        
typedef struct TBLD {
	int month;
	int day;
        float amount;
	struct TBLD *next;
	} TABLE_D;

typedef struct CONSTANTS {
        float MaxEvapWater;
        float MoistureFC;
        float MoistureWP;
        float MoistureSAT;
        float CriticalSoilAirC;
        float MaxPercolRTZ;
        float MaxPercolSubS;
        float MaxSurfaceStorge;
        float K0;
        } Constants;

typedef struct PARAMETERS {
          /** Tables for the Crop simulations **/
        TABLE *Roots;
        TABLE *Stems;
        TABLE *Leaves;
        TABLE *Storage;

        TABLE *VernalizationRate;
        TABLE *DeltaTempSum;
        TABLE *SpecificLeaveArea;
        TABLE *SpecificStemArea;
        TABLE *KDiffuseTb;
        TABLE *EFFTb;
        TABLE *MaxAssimRate; 
        TABLE *FactorAssimRateTemp;
        TABLE *FactorGrossAssimTemp;
        TABLE *FactorSenescence;
        TABLE *DeathRateStems;
        TABLE *DeathRateRoots; 
        
        /** Tables to account for the atmospheric CO2 concentration **/
        TABLE *CO2AMAXTB;
        TABLE *CO2EFFTB;
        TABLE *CO2TRATB;

        /** Tables for the maximum nutrient content in leaves as a function of DVS **/
        TABLE *N_MaxLeaves;
        TABLE *P_MaxLeaves;
        TABLE *K_MaxLeaves;

        /** Static Variables  **/
        /**  Emergence  **/
        float TempBaseEmergence;
        float TempEffMax;
        float TSumEmergence;                      	     

        /**  Phenology  **/
        int   IdentifyAnthesis; 
        float OptimumDaylength;	            
        float CriticalDaylength;
        float SatVernRequirement;
        float BaseVernRequirement;
        float TempSum1;       
        float TempSum2; 
        float InitialDVS;
        float DevelopStageHarvest;

        /** Initial Values  **/
        float InitialDryWeight;
        float RelIncreaseLAI;

        /**  Green Area  **/
        float SpecificPodArea;
        float LifeSpan;
        float TempBaseLeaves;

        /** Conversion assimilates into biomass **/
        float ConversionLeaves;
        float ConversionStorage;
        float ConversionRoots;
        float ConversionStems;
        
        /** Crop Characteristics **/
        float MaxHeight;
        float RGL;
        float RadiationAttenuation;
        float WindAttenuation;
        float TrunkRatio;
        float Albedo;
        float MinStomatalResistance;
        float MaxArchitecturalResistance;

        /** Maintenance Respiration **/
        float Q10;
        float RelRespiLeaves;
        float RelRespiStorage;
        float RelRespiRoots;
        float RelRespiStems;

        /** Death Rates  **/
        float MaxRelDeathRate;

        /** Water Use  **/
        float CorrectionTransp;
        float CropGroupNumber;
        float Airducts;

        /** Rooting **/
        float InitRootingDepth;
        float MaxIncreaseRoot;
        float MaxRootingDepth;

        /** Nutrients **/
        float DyingLeaves_NPK_Stress; 
        float DevelopmentStageNLimit; 
        float DevelopmentStageNT;
        float FracTranslocRoots;  
        float Opt_N_Frac;   
        float Opt_P_Frac;   
        float Opt_K_Frac;   
        float N_MaxRoots;   
        float N_MaxStems;   
        float P_MaxRoots;   
        float P_MaxStems;   
        float K_MaxRoots;   
        float K_MaxStems;   
        float NitrogenStressLAI;   
        float NLUE;   
        float Max_N_storage; 
        float Max_P_storage; 
        float Max_K_storage; 
        float N_lv_partitioning;  
        float NutrientStessSLA;   
        float N_ResidualFrac_lv;  
        float N_ResidualFrac_st;  
        float N_ResidualFrac_ro;  
        float P_ResidualFrac_lv;  
        float P_ResidualFrac_st;  
        float P_ResidualFrac_ro;  
        float K_ResidualFrac_lv;  
        float K_ResidualFrac_st;   
        float K_ResidualFrac_ro;   
        float TCNT;   
        float TCPT;   
        float TCKT;   
        float N_fixation; 
        } Parameters;


typedef struct STATES {
        float EvapWater;
        float EvapSoil;
        float Infiltration;
        float Irrigation;
        float Loss;
        float Moisture;
        float MoistureLOW;
        float Percolation;
        float Rain;
        float RootZoneMoisture;
        float Runoff;
        float SurfaceStorage;
        float Transpiration;
        float WaterRootExt;
        } States;

typedef struct RATES {
        float EvapWater;
        float EvapSoil;
        float Infiltration;
        float Irrigation;
        float Loss;
        float Moisture;
        float MoistureLOW;
        float Percolation;
        float RootZoneMoisture;
        float Runoff;
        float Transpiration;
        float WaterRootExt;
        } Rates;
 
typedef struct NUTRIENT_RATES {
        float roots;
        float stems;
        float leaves;
        float storage;
        float Demand_lv;
        float Demand_st;
        float Demand_ro;
        float Demand_so;
        float Supply;
        float Transloc;
        float Transloc_lv;
        float Transloc_st;
        float Transloc_ro;
        float Uptake;
        float Uptake_lv;
        float Uptake_st;
        float Uptake_ro;
        float death_lv;
        float death_st;
        float death_ro;
        } nutrient_rates;

typedef struct NUTRIENT_STATES {
        float roots;
        float stems;
        float leaves;
        float storage;
        float Max_lv;
        float Max_st;
        float Max_ro;
        float Max_so;
        float Optimum_lv;
        float Optimum_st;
        float Indx;
        float Uptake;
        float Uptake_lv;
        float Uptake_st;
        float Uptake_ro;
        float death_lv;
        float death_st;
        float death_ro;
        float Avail;
        float Avail_lv;
        float Avail_st;
        float Avail_ro;
        
        } nutrient_states;

typedef struct GROWTH_RATES {
        float roots;
        float stems;
        float leaves;
        float LAIExp;
        float storage;
        float Development;
        float RootDepth;
        float vernalization;
} growth_rates;

typedef struct GROWTH_STATES {
        float roots;
        float stems;
        float leaves;
        float LAI;
        float LAIExp;
        float storage;
        float Development;
        float RootDepth;
        float RootDepth_prev;
        float vernalization;
        } growth_states;

typedef struct DYING_STATES {
        float roots;
        float stems;
        float leaves;
        } dying_states; 
        
typedef struct DYING_RATES {
        float roots;
        float stems;
        float leaves;
        } dying_rates; 


typedef struct GREEN {
	float weight;
	float age;
	float area;
	struct GREEN *next;
	} Green;

typedef struct PLANT {
        int Emergence;
        int Sowing;
        int GrowthDay;       
        float NPK_Indx;
        float NutrientStress;
        float DaysOxygenStress;
        float TSumEmergence;
        float fac_ro;
        float fac_lv;
        float fac_st;
        float fac_so;
        
        Parameters prm;
        
        growth_rates  rt;
        growth_states st;
        dying_rates   drt;
        dying_states  dst;
        
        nutrient_states N_st;
	nutrient_states P_st;
        nutrient_states K_st;
        
        nutrient_rates N_rt;
	nutrient_rates P_rt;
        nutrient_rates K_rt;            
        
        Green *LeaveProperties;
	} Plant;    

typedef struct SOIL {
        float DaysSinceLastRain;
        float SoilMaxRootingDepth;
        float WaterStress;
        float InfPreviousDay;
        
        /* Tables for Soil */
        TABLE *VolumetricSoilMoisture;
        TABLE *HydraulicConductivity; /* currently not used */
        
        Constants ct;
        States st;
        Rates rt;
        } Soil;

typedef struct FIELD {
        /* Water related parameters */
        float FlagGroundWater;
        float InfRainDependent;
        float FlagDrains;
        float MaxSurfaceStorage;     
        float InitSoilMoisture;
        float GroundwaterDepth;
        float DD;
        float SoilLimRootDepth;
        float NotInfiltrating;
        float SurfaceStorage;
        float MaxInitSoilM;
        
        /* Mineral states and rates */
        float st_N_tot;
        float st_P_tot;
        float st_K_tot;

        float st_N_mins;
        float st_P_mins;
        float st_K_mins;

        float rt_N_tot;
        float rt_P_tot;
        float rt_K_tot;

        float rt_N_mins;
        float rt_P_mins;
        float rt_K_mins;
        
        /** Table for the fraction of precipitation that does not infiltrate **/
        TABLE *NotInfTB;
        } Field;

typedef struct MANAGEMENT {
        /** Tables for fertilizer application and recovery fraction **/
        TABLE_D *N_Fert_table;
        TABLE_D *P_Fert_table;
        TABLE_D *K_Fert_table;
        TABLE_D *Irrigation;
        
        float N_Mins;
        float NRecoveryFrac;
        float P_Mins;
        float PRecoveryFrac;
        float K_Mins; 
        float KRecoveryFrac;
        float N_Uptake_frac;
        float P_Uptake_frac;
        float K_Uptake_frac;
        } Management;

typedef struct METEO {
    /* constants */
    size_t crop_class;
    float Latitude;
    float Longitude;
    int CycleLength;

    /* variables */
    int MeteoDay;
    int MeteoYear;
    double Tmin[DAYS_PER_WEEK];
    double Tmax;
    float Temp;
    float DayTemp;
    float Radiation;
    float CO2;

    /* Derived */
    float AtmosphTransm;
    float AngotRadiation;
    float Daylength;
    float PARDaylength;
    float SinLD;
    float CosLD; 
    float DiffRadPP;
    float DSinBE;
} Meteo;
        
/* Place holder for a simulation unit */
typedef struct SIMUNIT {
        Plant *crp;
        Field *ste;
        Management *mng;
        Soil  *soil;
        Meteo *met;
        int emergence;
        int file;
        dmy_struct start;
        dmy_struct end;
        struct SIMUNIT *next;
        int growing;
        } SimUnit;
        
SimUnit ***Grid;

extern char *CropParam[MAXSTRING];
extern char *CropParam2[MAXSTRING];
extern char *SiteParam[MAXSTRING];
extern char *SiteParam2[MAXSTRING];
extern char *SoilParam[MAXSTRING];
extern char *SoilParam2[MAXSTRING];
extern char *ManageParam[MAXSTRING];
extern char *ManageParam2[MAXSTRING];

extern float ScatCoef;
extern float XGauss[];
extern float WGauss[];

void FillCropVariables();
void FillSiteVariables();
void FillSoilVariables();
void FillManageVariables();

/* General help functions */
float Afgen(TABLE *, float *);
float List(TABLE_D *);
float limit(float a, float b, float c);
float insw(float x1, float x2, float x3);
void GetSimInput(char *, char *, char *,
        int, int, SimUnit *);
void IfSowing(SimUnit *, dmy_struct*);

/* Additional functions */
void Astro(SimUnit *);
void Clean(SimUnit *);
void CleanHarvest(SimUnit *);

/* Crop growth */
void Partioning(SimUnit *);
void HeatStress(SimUnit *);
void RateCalculationCrop(SimUnit *);
void Growth(SimUnit *, float NewPlantMaterial);
void IntegrationCrop(SimUnit *);
void InitializeCrop(SimUnit *);
int EmergenceCrop(SimUnit *, int Emergence);

void DevelopmentRate(SimUnit *);
void LeaveGrowth(SimUnit *);
float DailyTotalAssimilation(SimUnit *);
float DyingLeaves(SimUnit *);
float InstantAssimilation(SimUnit *, float KDiffuse, float EFF, float AssimMax, float SinB, 
        float PARDiffuse, float PARDirect);
float LeaveAreaIndex(SimUnit *);
float Correct(SimUnit *, float Assimilation);
float RespirationRef(SimUnit *, float TotalAssimilation);
float Conversion(SimUnit *, float NetAssimilation);

/* Nutrients */
void CropNutrientRates(SimUnit *);
void InitializeNutrients(SimUnit *);
void IntegrationNutrients(SimUnit *);
void NutritionINDX(SimUnit *);
void NutrientLoss(SimUnit *);
void NutrientMax(SimUnit *);
void NutrientPartioning(SimUnit *);
void NutrientRates(SimUnit *);
void NutrientOptimum(SimUnit *);
void NutrientDemand(SimUnit *);
void SoilNutrientRates(SimUnit *);
void NutrientTranslocation(SimUnit *);
void RateCalcultionNutrients(SimUnit *);

void GetCropData(Plant *, char *);
void GetManagement(Management *, char *);

void header(FILE *);
void Output(SimUnit *, FILE *);
void RatesToZero(SimUnit *);

#endif	// WOFOST_H

