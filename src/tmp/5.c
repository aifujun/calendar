typedef struct
{
    double A;
    double B;
    double C;
} VSOP87_COEFFICIENT;

/*数据来源是：《天文计算》 附录II*/
// 计算太阳黄经周期项
extern const VSOP87_COEFFICIENT Earth_L0[64];
extern const VSOP87_COEFFICIENT Earth_L1[34];
extern const VSOP87_COEFFICIENT Earth_L2[20];
extern const VSOP87_COEFFICIENT Earth_L3[ 7];
extern const VSOP87_COEFFICIENT Earth_L4[ 3];
extern const VSOP87_COEFFICIENT Earth_L5[ 1];



// 计算太阳黄纬周期项
extern const VSOP87_COEFFICIENT Earth_B0[ 5];
extern const VSOP87_COEFFICIENT Earth_B1[ 2];
extern const VSOP87_COEFFICIENT Earth_B2[ 4];
extern const VSOP87_COEFFICIENT Earth_B3[ 4];
extern const VSOP87_COEFFICIENT Earth_B4[ 1];

// 计算日地经距离周期项
extern const VSOP87_COEFFICIENT Earth_R0[40];
extern const VSOP87_COEFFICIENT Earth_R1[10];
extern const VSOP87_COEFFICIENT Earth_R2[ 6];
extern const VSOP87_COEFFICIENT Earth_R3[ 2];
extern const VSOP87_COEFFICIENT Earth_R4[ 1];



double CalcSunEclipticLongitudeEC(double dt);
double CalcPeriodicTerm(const VSOP87_COEFFICIENT *coff, int count, double dt);
double CalcSunEclipticLatitudeEC(double dt);
double AdjustSunEclipticLongitudeEC(double dt, double longitude, double latitude);
double CalcEarthLongitudeNutation(double dt);
void GetEarthNutationParameter(double dt, double *D, double *M, double *Mp, double *F, double *Omega);
double CalcEarthObliquityNutation(double dt);
double CalcSunEarthRadius(double dt);
double GetSunEclipticLongitudeEC(double jde);
double CalculateSolarTerms(int year, int angle);
double CalculateSolarTermsNewton(int year, int angle);


double CalcSunEclipticLongitudeEC(double dt) {
    double L0 = CalcPeriodicTerm(Earth_L0, sizeof(Earth_L0) / sizeof(VSOP87_COEFFICIENT), dt);
    double L1 = CalcPeriodicTerm(Earth_L1, sizeof(Earth_L1) / sizeof(VSOP87_COEFFICIENT), dt);
    double L2 = CalcPeriodicTerm(Earth_L2, sizeof(Earth_L2) / sizeof(VSOP87_COEFFICIENT), dt);
    double L3 = CalcPeriodicTerm(Earth_L3, sizeof(Earth_L3) / sizeof(VSOP87_COEFFICIENT), dt);
    double L4 = CalcPeriodicTerm(Earth_L4, sizeof(Earth_L4) / sizeof(VSOP87_COEFFICIENT), dt);
    double L5 = CalcPeriodicTerm(Earth_L5, sizeof(Earth_L5) / sizeof(VSOP87_COEFFICIENT), dt);

    double L = (((((L5 * dt + L4) * dt + L3) * dt + L2) * dt + L1) * dt + L0) / 100000000.0;

    /*地心黄经 = 日心黄经 + 180度*/
    return (Mod360Degree(Mod360Degree(L / RADIAN_PER_ANGLE) + 180.0));
}

double CalcPeriodicTerm(const VSOP87_COEFFICIENT *coff, int count, double dt) {

    double val = 0.0;

    for(int i = 0; i < count; i++) {
        val += (coff[i].A * cos((coff[i].B + coff[i].C * dt)));
    }

    return val;
}

double CalcSunEclipticLatitudeEC(double dt) {

    double B0 = CalcPeriodicTerm(Earth_B0, sizeof(Earth_B0) / sizeof(VSOP87_COEFFICIENT), dt);
    double B1 = CalcPeriodicTerm(Earth_B1, sizeof(Earth_B1) / sizeof(VSOP87_COEFFICIENT), dt);
    double B2 = CalcPeriodicTerm(Earth_B2, sizeof(Earth_B2) / sizeof(VSOP87_COEFFICIENT), dt);
    double B3 = CalcPeriodicTerm(Earth_B3, sizeof(Earth_B3) / sizeof(VSOP87_COEFFICIENT), dt);
    double B4 = CalcPeriodicTerm(Earth_B4, sizeof(Earth_B4) / sizeof(VSOP87_COEFFICIENT), dt);

    double B = (((((B4 * dt) + B3) * dt + B2) * dt + B1) * dt + B0) / 100000000.0;

    /*地心黄纬 = －日心黄纬*/
    return -(B / RADIAN_PER_ANGLE);
}

double AdjustSunEclipticLongitudeEC(double dt, double longitude, double latitude) {
    //T是儒略世纪数
    double T = dt * 10;
    double dbLdash = longitude - 1.397 * T - 0.00031 * T * T;

    // 转换为弧度
    dbLdash *= RADIAN_PER_ANGLE;

    return (-0.09033 + 0.03916 * (cos(dbLdash) + sin(dbLdash)) * tan(latitude * RADIAN_PER_ANGLE)) / 3600.0;
}

double CalcEarthLongitudeNutation(double dt) {

    double T = dt * 10;
    double D, M, Mp, F, Omega;
    double result = 0.0;

    GetEarthNutationParameter(dt, &D, &M, &Mp, &F, &Omega);

    for(int i = 0; i < sizeof(nutation) / sizeof(nutation[0]); i++) {
        double sita = nutation[i].D * D + nutation[i].M * M + nutation[i].Mp * Mp + nutation[i].F * F + nutation[i].omega * Omega;

        result += (nutation[i].sine1 + nutation[i].sine2 * T ) * sin(sita);
    }

    /*先乘以章动表的系数 0.0001，然后换算成度的单位*/
    return result * 0.0001 / 3600.0;
}

void GetEarthNutationParameter(double dt, double *D, double *M, double *Mp, double *F, double *Omega) {
    /*T是从J2000起算的儒略世纪数*/
    double T = dt * 10;
    double T2 = T * T;
    double T3 = T2 * T;

    /*平距角（如月对地心的角距离）*/
    *D = 297.85036 + 445267.111480 * T - 0.0019142 * T2 + T3 / 189474.0;

    /*太阳（地球）平近点角*/
    *M = 357.52772 + 35999.050340 * T - 0.0001603 * T2 - T3 / 300000.0;

    /*月亮平近点角*/
    *Mp = 134.96298 + 477198.867398 * T + 0.0086972 * T2 + T3 / 56250.0;

    /*月亮纬度参数*/
    *F = 93.27191 + 483202.017538 * T - 0.0036825 * T2 + T3 / 327270.0;

    /*黄道与月亮平轨道升交点黄经*/
    *Omega = 125.04452 - 1934.136261 * T + 0.0020708 * T2 + T3 / 450000.0;
}

double CalcEarthObliquityNutation(double dt) {
    /*T是从J2000起算的儒略世纪数*/
    double T = dt * 10;
    double D,M,Mp,F,Omega;

    GetEarthNutationParameter(dt, &D, &M, &Mp, &F, &Omega);

    double result = 0.0;

    for(int i = 0; i < sizeof(nutation) / sizeof(nutation[0]); i++) {
        double sita = nutation[i].D * D + nutation[i].M * M + nutation[i].Mp * Mp + nutation[i].F * F + nutation[i].omega * Omega;
        result += (nutation[i].cosine1 + nutation[i].cosine2 * T ) * cos(sita);
    }

    /*先乘以章动表的系数 0.001，然后换算成度的单位*/
    return result * 0.0001 / 3600.0;
}

double CalcSunEarthRadius(double dt) {

    double R0 = CalcPeriodicTerm(Earth_R0, sizeof(Earth_R0) / sizeof(VSOP87_COEFFICIENT), dt);
    double R1 = CalcPeriodicTerm(Earth_R1, sizeof(Earth_R1) / sizeof(VSOP87_COEFFICIENT), dt);
    double R2 = CalcPeriodicTerm(Earth_R2, sizeof(Earth_R2) / sizeof(VSOP87_COEFFICIENT), dt);
    double R3 = CalcPeriodicTerm(Earth_R3, sizeof(Earth_R3) / sizeof(VSOP87_COEFFICIENT), dt);
    double R4 = CalcPeriodicTerm(Earth_R4, sizeof(Earth_R4) / sizeof(VSOP87_COEFFICIENT), dt);

    return (((((R4 * dt) + R3) * dt + R2) * dt + R1) * dt + R0) / 100000000.0;
}

double GetSunEclipticLongitudeEC(double jde) {
    /*儒略千年数*/
    double dt = (jde - JD2000) / 365250.0;

    // 计算太阳的地心黄经
    double longitude = CalcSunEclipticLongitudeEC(dt);

    // 计算太阳的地心黄纬
    double latitude = CalcSunEclipticLatitudeEC(dt) * 3600.0;

    // 修正精度
    longitude += AdjustSunEclipticLongitudeEC(dt, longitude, latitude);

    // 修正天体章动
    longitude += CalcEarthLongitudeNutation(dt);

    // 修正光行差
    /*太阳地心黄经光行差修正项是: -20".4898/R*/
    longitude -= (20.4898 / CalcSunEarthRadius(dt)) / (20 * PI);

    return longitude;
}

double CalculateSolarTerms(int year, int angle) {

    double lJD, rJD;
    double solarTermsJD = 0.0;
    double longitude = 0.0;

    EstimateSTtimeScope(year, angle, lJD, rJD); /*估算迭代起始时间区间*/

    do {
        solarTermsJD = ((rJD - lJD) * 0.618) + lJD;
        longitude = GetSunEclipticLongitudeECDegree(solarTermsJD);
        /* 对黄经0度迭代逼近时，由于角度360度圆周性，估算黄经值可能在(345,360]和[0,15)两个区间，如果值落入前一个区间，需要进行修正 */
        longitude = ((angle == 0) && (longitude > 345.0)) ? longitude - 360.0 : longitude;
        (longitude > (double)angle) ? rJD = solarTermsJD : lJD = solarTermsJD;
    } while((rJD - lJD) > 0.0000001);

    return solarTermsJD;
}

double CalculateSolarTermsNewton(int year, int angle) {
    double JD0, JD1,stDegree,stDegreep;

    JD1 = GetInitialEstimateSolarTerms(year, angle);

    do {
        JD0 = JD1;
        stDegree = GetSunEclipticLongitudeECDegree(JD0) - angle;
        stDegreep = (GetSunEclipticLongitudeECDegree(JD0 + 0.000005) - GetSunEclipticLongitudeECDegree(JD0 - 0.000005)) / 0.00001;
        JD1 = JD0 - stDegree / stDegreep;
    } while((fabs(JD1 - JD0) > 0.0000001));

    return JD1;
}


