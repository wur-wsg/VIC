#include <vic_driver_image.h>
#include <plugin.h>

/* ----------------------------------------------------------------------------*/
/*  function InstantAssimilation(float KDiffuse, float EFF, float AssimMax,    */
/*                            float SinB, float PARDiffuse, float PARDirect)   */ 
/*  Purpose: Calculation of the instant Assimilation rate as a function of     */
/*  radiation using the three point Gaussian integration method.               */
/*-----------------------------------------------------------------------------*/

float InstantAssimilation(SimUnit *Grid, float KDiffuse, float EFF, float AssimMax, float SinB, 
        float PARDiffuse, float PARDirect)
{
    extern float XGauss[];
    extern float WGauss[];
    
    int i;
    float AbsorbedRadiationDiffuse, AbsorbedRadiationTotal, AbsorbedRadiationDirect;
    float AbsorbedShadedLeaves, AbsorbedDirectLeaves;
    float AssimShadedLeaves, AssimSunlitLeaves, AssimTotal;
    float Reflection, KDirectBl, KDirectTl;
    float GrossCO2, FractionSunlitLeaves, LAIC ;
    float tmp_max;

    /* Extinction coefficients KDIF,KDIRBL,KDIRT */
    Reflection  = (1.-sqrt(1.-ScatCoef))/(1.+sqrt(1.-ScatCoef))*(2/(1+1.6*SinB));
    KDirectBl   = (0.5/SinB)*KDiffuse/(0.8*sqrt(1.-ScatCoef));
    KDirectTl   = KDirectBl*sqrt(1.-ScatCoef);

    /* Three-point Gaussian integration over LAI */
    GrossCO2  = 0.;
    for (i=0;i<3;i++)
    {
       LAIC   = Grid->crp->st.LAI*XGauss[i];
        
       /* Absorbed radiation */
       AbsorbedRadiationDiffuse = (1.-Reflection)*PARDiffuse*KDiffuse * exp(-KDiffuse * LAIC);
       AbsorbedRadiationTotal   = (1.-Reflection)*PARDirect*KDirectTl * exp(-KDirectTl * LAIC);
       AbsorbedRadiationDirect  = (1.-ScatCoef)  *PARDirect*KDirectBl * exp(-KDirectBl * LAIC);

       /* Absorbed flux in W/m2 for shaded leaves and assimilation */
       AbsorbedShadedLeaves = AbsorbedRadiationDiffuse  + AbsorbedRadiationTotal - AbsorbedRadiationDirect;
       tmp_max = max(2.0,AssimMax);
       AssimShadedLeaves    = AssimMax*(1.-exp (-AbsorbedShadedLeaves*EFF/tmp_max));

       /* Direct light absorbed by leaves perpendicular on direct */
       /* beam and assimilation of sunlit leaf area               */
       AbsorbedDirectLeaves=(1 - ScatCoef)*PARDirect/SinB;
       if (AbsorbedDirectLeaves <= 0) {
           AssimSunlitLeaves = AssimShadedLeaves;
       }
       else {
           tmp_max = max(2.0,AssimMax);
           AssimSunlitLeaves = AssimMax*(1. - (AssimMax - AssimShadedLeaves)*
              (1 - exp( -AbsorbedDirectLeaves*EFF/tmp_max))/(EFF*AbsorbedDirectLeaves));
       }

        /*  Fraction of sunlit leaf area and local assimilation rate  */ 
        FractionSunlitLeaves  = exp(-KDirectBl*LAIC);
        AssimTotal = FractionSunlitLeaves*AssimSunlitLeaves + (1. - FractionSunlitLeaves)*AssimShadedLeaves;

        /*  Integration */
        GrossCO2 += AssimTotal * WGauss[i];
    }
    
    return (GrossCO2 * Grid->crp->st.LAI);     
}


/* ----------------------------------------------------------------------------*/
/*  function DailyTotalAssimilation()                                          */ 
/*  Purpose: Calculation of the daily assimilation rate using the three point  */
/*  Gaussian integration method.                                               */
/*-----------------------------------------------------------------------------*/
float DailyTotalAssimilation(SimUnit *Grid)
{
    extern float XGauss[];
    extern float WGauss[];
    
    int i;
    float KDiffuse, EFF, Factor;
    float Hour, SinB, PAR, PARDiffuse, PARDirect, AssimMax; 
    float DailyTotalAssimilation = 0.;

    KDiffuse = Afgen(Grid->crp->prm.KDiffuseTb, &(Grid->crp->st.Development));

    EFF      = Afgen(Grid->crp->prm.EFFTb, &Grid->met->DayTemp);
    Factor   = Afgen(Grid->crp->prm.CO2EFFTB, &Grid->met->CO2);

    /* Correction for the atmospheric CO2 concentration */
    EFF      = EFF * Factor ;

    AssimMax = Afgen(Grid->crp->prm.FactorAssimRateTemp, &Grid->met->DayTemp) * 
               Afgen(Grid->crp->prm.MaxAssimRate, &(Grid->crp->st.Development)) * 
               Afgen(Grid->crp->prm.CO2AMAXTB, &Grid->met->CO2);

    if (AssimMax > 0. && Grid->crp->st.LAI > 0.)
    {
        for (i=0;i<3;i++)
        {
            Hour       = 12.0+0.5*Grid->met->Daylength*XGauss[i];
            SinB       = max (0.,Grid->met->SinLD+Grid->met->CosLD*cos(2.*CONST_PI*(Hour+12.)/24.));
            PAR        = 0.5*Grid->met->Radiation*SinB*(1.+0.4*SinB)/Grid->met->DSinBE;
            PARDiffuse = min (PAR,SinB*Grid->met->DiffRadPP);
            PARDirect  = PAR-PARDiffuse;
            DailyTotalAssimilation = DailyTotalAssimilation + 
                InstantAssimilation(Grid, KDiffuse,EFF,AssimMax,SinB,PARDiffuse,PARDirect) * WGauss[i];
        }  
    }
    return(DailyTotalAssimilation*Grid->met->Daylength);
}


/* ----------------------------------------------------------------------------*/
/*  function Correct()                                                         */ 
/*  Purpose: Correct the daily assimilation rate for low temperatures          */
/*-----------------------------------------------------------------------------*/
float Correct(SimUnit *Grid, float Assimilation)
{
    int PreviousDay;
    int Counter;
    int number = DAYS_PER_WEEK;
    float TminLowAvg = 0.;
    

    if (Grid->crp->GrowthDay < DAYS_PER_WEEK - 1)
    {
        number = Grid->crp->GrowthDay;
    }
    
    Counter = 0;
    PreviousDay = 0;
    while (PreviousDay >= 0 && Counter < number)
    {
      TminLowAvg += Grid->met->Tmin[PreviousDay++]; 
      Counter++;
    }

    TminLowAvg = TminLowAvg/Counter;
    return (Assimilation*Afgen(Grid->crp->prm.FactorGrossAssimTemp, &TminLowAvg)*30./44.);

}
