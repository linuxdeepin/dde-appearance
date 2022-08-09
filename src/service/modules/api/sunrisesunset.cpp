#include "sunrisesunset.h"

#include <QDebug>
#include <QtMath>

SunriseSunset::SunriseSunset()
{

}

bool SunriseSunset::getSunriseSunset(double latitude, double longitude,double utcOffset, QDateTime date, QDateTime& sunrise,QDateTime& sunset)
{
    if(!checkLatitude(latitude))
    {
        qDebug()<<"checkLatitude failure";
        return false;
    }

    if(!checkLongitude(longitude))
    {
        qDebug()<<"checkLongitude failure";
        return false;
    }

    if(!checkUtcOffset(utcOffset))
    {
        qDebug()<<"checkUtcOffset failure";
        return false;
    }

    if(!checkDate(date))
    {
        qDebug()<<"checkDate failure";
        return false;
    }

    QDateTime since = QDateTime::fromString(QString("18991230000000"),"yyyyMMddHHmmss");
    qint64 numDays = diffDays(since,date);

    int seconds = 24 * 60 * 60;

    QVector<double> secondsNorm =createSecondsNormalized(seconds);

    QVector<double> julianDay = calcJulianDay(numDays,secondsNorm,utcOffset);

    QVector<double> julianCentury = calcJulianCentury(julianDay);

    QVector<double> geomMeanLongSun = calcGeomMeanLongSun(julianCentury);

    QVector<double> geomMeanAnomSun = calcGeomMeanAnomSun(julianCentury);

    QVector<double> eccentEarthOrbit = calcEccentEarthOrbit(julianCentury);

    QVector<double> sunEqCtr = calcSunEqCtr(julianCentury, geomMeanAnomSun);

    QVector<double> sunTrueLong = calcSunTrueLong(sunEqCtr, geomMeanLongSun);

    QVector<double> sunAppLong = calcSunAppLong(sunTrueLong, julianCentury);

    QVector<double> meanObliqEcliptic = calcMeanObliqEcliptic(julianCentury);

    QVector<double> obliqCorr = calcObliqCorr(meanObliqEcliptic, julianCentury);

    QVector<double> sunDeclination = calcSunDeclination(obliqCorr, sunAppLong);

    QVector<double> multiFactor;
    for (int index=0;index<obliqCorr.size();index++){
        double temp = tan(deg2rad(obliqCorr[index]/2.0)) * tan(deg2rad(obliqCorr[index]/2.0));
        multiFactor.push_back(temp);
    }

    QVector<double> equationOfTime = calcEquationOfTime(multiFactor, geomMeanLongSun, eccentEarthOrbit, geomMeanAnomSun);

    QVector<double> haSunrise = calcHaSunrise(latitude, sunDeclination);

    QVector<double> solarNoon = calcSolarNoon(longitude, equationOfTime, utcOffset);

    QVector<double> tempSunrise;
    QVector<double> tempSunset;

    for (int index=0;index<solarNoon.size();index++) {
        tempSunrise.push_back(solarNoon[index] - round(haSunrise[index]*4.0*60.0)-(seconds)*secondsNorm[index]);
        tempSunset.push_back(solarNoon[index] + round(haSunrise[index]*4.0*60.0) - seconds*secondsNorm[index]);
    }

    // Get the sunrise and sunset in seconds
    int sunriseSeconds = minIndex(toAbs(tempSunrise));
    int sunsetSeconds = minIndex(toAbs(tempSunset));

    QDateTime  currTime =QDateTime::currentDateTime();
    sunrise = currTime.addSecs(sunriseSeconds);
    sunset = currTime.addSecs(sunsetSeconds);
}

bool SunriseSunset::checkLatitude(double latitude)
{
    if(latitude < -90.0 || latitude > 90.0)
    {
        return false;
    }

    return true;
}

bool SunriseSunset::checkLongitude(double longitude)
{
    if(longitude < -180.0 || longitude > 180.0)
    {
        return false;
    }
    return true;
}

bool SunriseSunset::checkUtcOffset(double utcOffset)
{
    if(utcOffset < -12.0 || utcOffset > 14.0)
    {
        return false;
    }
    return true;
}

bool SunriseSunset::checkDate(QDateTime date)
{
    QDateTime minDate = QDateTime::fromString(QString("19000101000000"),"yyyyMMddHHmmss");
    QDateTime maxDate = QDateTime::fromString(QString("22000101000000"),"yyyyMMddHHmmss");

    if(date<minDate || date>maxDate)
    {
        return false;
    }
    return  true;
}

qint64 SunriseSunset::diffDays(QDateTime date1, QDateTime date2)
{
    return  date1.daysTo(date2);
}

QVector<double> SunriseSunset::createSecondsNormalized(int seconds)
{
    QVector<double> ret;

    for(int i=0;i<seconds;i++)
    {
        double temp = i/(seconds-1);
        ret.push_back(temp);
    }
    return ret;
}

QVector<double> SunriseSunset::calcJulianDay(qint64 numDays, const QVector<double>& secondsNorm, double utcOffset)
{
    QVector<double> julianDay;

    for(int index =0;index<secondsNorm.length();index++)
    {
        double temp = numDays + 2415018.5 + secondsNorm[index] - utcOffset/24.0;
        julianDay.push_back(temp);
    }
    return julianDay;
}

QVector<double> SunriseSunset::calcJulianCentury(const QVector<double>& julianDay)
{
    QVector<double> julianCentury;

    for(int index =0;index<julianDay.length();index++)
    {
        double temp = (julianDay[index] - 2451545.0) / 36525.0;
        julianCentury.push_back(temp);
    }
    return julianCentury;
}

QVector<double> SunriseSunset::calcGeomMeanLongSun(const QVector<double>& julianCentury)
{
    QVector<double> geomMeanLongSun;

    for(int index =0;index<julianCentury.length();index++)
    {
        double temp = 280.46646 + julianCentury[index]*(36000.76983+julianCentury[index]*0.0003032);
        temp = static_cast<int>(temp) % 360;
        geomMeanLongSun.push_back(temp);
    }
    return geomMeanLongSun;
}

QVector<double> SunriseSunset::calcGeomMeanAnomSun(const QVector<double>& julianCentury)
{
    QVector<double> geomMeanAnomSun;

    for(int index =0;index<julianCentury.length();index++)
    {
        double temp = 2357.52911 + julianCentury[index]*(35999.05029-0.0001537*julianCentury[index]);
        geomMeanAnomSun.push_back(temp);
    }
    return geomMeanAnomSun;
}

QVector<double> SunriseSunset::calcEccentEarthOrbit(const QVector<double>& julianCentury)
{
    QVector<double> eccentEarthOrbit;

    for(int index =0;index<julianCentury.length();index++)
    {
        double temp = 0.016708634 - julianCentury[index]*(0.000042037+0.0000001267*julianCentury[index]);
        eccentEarthOrbit.push_back(temp);
    }
    return eccentEarthOrbit;
}

QVector<double> SunriseSunset::calcSunEqCtr(const QVector<double>& julianCentury,const QVector<double>& geomMeanAnomSun)
{
    QVector<double> sunEqCtr;
    if(julianCentury.length()!=geomMeanAnomSun.length())
    {
        return sunEqCtr;
    }

    for(int index =0;index<julianCentury.length();index++)
    {
        double temp = sin(deg2rad(geomMeanAnomSun[index]))*(1.914602-julianCentury[index]*(0.004817+0.000014*julianCentury[index])) +
                sin(deg2rad(2*geomMeanAnomSun[index]))*(0.019993-0.000101*julianCentury[index]) +
                sin(deg2rad(3*geomMeanAnomSun[index]))*0.000289;

        sunEqCtr.push_back(temp);
    }
    return sunEqCtr;
}

QVector<double> SunriseSunset::calcSunTrueLong(const QVector<double>& sunEqCtr,const QVector<double>& geomMeanLongSun)
{
    QVector<double> sunTrueLong;
    if(sunEqCtr.length()!=geomMeanLongSun.length())
    {
        return sunEqCtr;
    }

    for(int index =0;index<sunEqCtr.length();index++)
    {
        double temp = sunEqCtr[index] + geomMeanLongSun[index];
        sunTrueLong.push_back(temp);
    }
    return sunEqCtr;
}

QVector<double> SunriseSunset::calcSunAppLong(const QVector<double>& sunTrueLong,const QVector<double>& julianCentury)
{
    QVector<double> sunAppLong;
    if(sunTrueLong.length()!=julianCentury.length())
    {
        return sunAppLong;
    }

    for(int index =0;index<sunTrueLong.length();index++)
    {
        double temp = sunTrueLong[index] - 0.00569 - 0.00478*sin(deg2rad(125.04-1934.136*julianCentury[index]));
        sunAppLong.push_back(temp);
    }
    return sunAppLong;
}

QVector<double> SunriseSunset::calcMeanObliqEcliptic(const QVector<double>& julianCentury)
{
    QVector<double> meanObliqEcliptic;

    for(int index =0;index<julianCentury.length();index++)
    {
        double temp = 23.0 + (26.0+(21.448-julianCentury[index]*(46.815+julianCentury[index]*(0.00059-julianCentury[index]*0.001813)))/60.0)/60.0;
        meanObliqEcliptic.push_back(temp);
    }
    return meanObliqEcliptic;
}

QVector<double> SunriseSunset::calcObliqCorr(const QVector<double>& meanObliqEcliptic,const QVector<double>& julianCentury)
{
    QVector<double> obliqCorr;
    if(meanObliqEcliptic.length()!=julianCentury.length())
    {
        return obliqCorr;
    }

    for(int index =0;index<julianCentury.length();index++)
    {
        double temp = meanObliqEcliptic[index] + 0.00256*cos(deg2rad(125.04-1934.136*julianCentury[index]));
        obliqCorr.push_back(temp);
    }
    return obliqCorr;
}

QVector<double> SunriseSunset::calcSunDeclination(const QVector<double>& obliqCorr,const QVector<double>& sunAppLong)
{
    QVector<double> sunDeclination;
    if(obliqCorr.length()!=sunAppLong.length())
    {
        return sunDeclination;
    }

    for(int index =0;index<obliqCorr.length();index++)
    {
        double temp = rad2deg(asin(sin(deg2rad(obliqCorr[index])) * sin(deg2rad(sunAppLong[index]))));
        sunDeclination.push_back(temp);
    }
    return sunDeclination;
}

QVector<double> SunriseSunset::calcEquationOfTime(const QVector<double>& multiFactor,const QVector<double>& geomMeanLongSun,
                                              const QVector<double>& eccentEarthOrbit,const QVector<double>& geomMeanAnomSun)
{
    QVector<double> equationOfTime;
    if(multiFactor.length()!=geomMeanLongSun.length()||
       multiFactor.length()!=eccentEarthOrbit.length()||
       multiFactor.length()!=geomMeanAnomSun.length())
    {
        return equationOfTime;
    }

    for(int index =0;index<multiFactor.length();index++)
    {
        double a = multiFactor[index] * sin(2.0*deg2rad(geomMeanLongSun[index]));
        double b = 2.0 * eccentEarthOrbit[index] * sin(deg2rad(geomMeanAnomSun[index]));
        double c = 4.0 * eccentEarthOrbit[index] * multiFactor[index] * sin(deg2rad(geomMeanAnomSun[index]));
        double d = cos(2.0 * deg2rad(geomMeanLongSun[index]));
        double e = 0.5 * multiFactor[index] * multiFactor[index] * sin(4.0*deg2rad(geomMeanLongSun[index]));
        double f = 1.25 * eccentEarthOrbit[index] * eccentEarthOrbit[index] * sin(2.0*deg2rad(geomMeanAnomSun[index]));
        double temp = 4.0 * rad2deg(a-b+c*d-e-f);
        equationOfTime.push_back(temp);
    }
    return equationOfTime;
}

QVector<double> SunriseSunset::calcHaSunrise(double latitude, QVector<double> sunDeclination)
{
    QVector<double> haSunrise;
    for(int index =0;index<sunDeclination.length();index++)
    {
        double temp = rad2deg(acos(cos(deg2rad(90.833))/(cos(deg2rad(latitude))*cos(deg2rad(sunDeclination[index])))
                                   - tan(deg2rad(latitude))*tan(deg2rad(sunDeclination[index]))));
        haSunrise.push_back(temp);
    }
    return haSunrise;
}

QVector<double> SunriseSunset::calcSolarNoon(double longitude, QVector<double> equationOfTime, double utcOffset)
{
    QVector<double> solarNoon;
    for(int index =0;index<equationOfTime.length();index++)
    {
        double temp = (720.0 - 4.0*longitude - equationOfTime[index] + utcOffset*60.0) * 60.0;
        solarNoon.push_back(temp);
    }
    return solarNoon;
}

double SunriseSunset::deg2rad(double degrees)
{
    return degrees *(M_PI/180.0);
}

double SunriseSunset::rad2deg(double radians)
{
    return radians * (180.0 / M_PI);
}

QVector<double> SunriseSunset::toAbs(QVector<double> input)
{
    QVector<double> output;
    for(auto iter:input)
    {
        output.push_back(abs(iter));
    }
    return output;
}

int SunriseSunset::minIndex(QVector<double> input)
{
    if(input.empty())
    {
        return -1;
    }

    double minValue = input[0];
    int minIndex = 0;
    for(int i=1;i<input.length();i++)
    {
        if(input[i]<minValue)
        {
            minValue = input[i];
            minIndex = i;
        }
    }

    return minIndex;
}

int SunriseSunset::round(double value)
{
    if(value < 0)
    {
        return static_cast<int>(value - 0.5);
    }

    return static_cast<int>(value + 0.5);
}
