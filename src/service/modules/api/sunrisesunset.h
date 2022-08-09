#ifndef SUNRISESUNSET_H
#define SUNRISESUNSET_H

#include <QTime>

class SunriseSunset
{
public:
    SunriseSunset();
    static bool getSunriseSunset(double latitude, double longitude,
                                 double utcOffset, QDateTime date, QDateTime& sunrise,QDateTime& sunset);
private:
    static bool checkLatitude(double latitude);
    static bool checkLongitude(double longitude);
    static bool checkUtcOffset(double utcOffset);
    static bool checkDate(QDateTime date);
    static qint64 diffDays(QDateTime date1, QDateTime date2);
    static QVector<double> createSecondsNormalized(int seconds);
    static QVector<double> calcJulianDay(qint64 numDays, const QVector<double>& secondsNorm, double utcOffset);
    static QVector<double> calcJulianCentury(const QVector<double>& julianDay);
    static QVector<double> calcGeomMeanLongSun(const QVector<double>& julianCentury);
    static QVector<double> calcGeomMeanAnomSun(const QVector<double>& julianCentury);
    static QVector<double> calcEccentEarthOrbit(const QVector<double>& julianCentury);
    static QVector<double> calcSunEqCtr(const QVector<double>& julianCentury,const QVector<double>& geomMeanAnomSun);
    static QVector<double> calcSunTrueLong(const QVector<double>& sunEqCtr,const QVector<double>& geomMeanLongSun);
    static QVector<double> calcSunAppLong(const QVector<double>& sunTrueLong,const QVector<double>& julianCentury);
    static QVector<double> calcMeanObliqEcliptic(const QVector<double>& julianCentury);
    static QVector<double> calcObliqCorr(const QVector<double>& meanObliqEcliptic,const QVector<double>& julianCentury);
    static QVector<double> calcSunDeclination(const QVector<double>& obliqCorr,const QVector<double>& sunAppLong);
    static QVector<double> calcEquationOfTime(const QVector<double>& multiFactor,const QVector<double>& geomMeanLongSun,
                                              const QVector<double>& eccentEarthOrbit,const QVector<double>& geomMeanAnomSun);
    static QVector<double> calcHaSunrise(double latitude, QVector<double> sunDeclination);
    static QVector<double> calcSolarNoon(double longitude, QVector<double> equationOfTime, double utcOffset);
    static double deg2rad(double degrees);
    static double rad2deg(double radians);
    static QVector<double> toAbs(QVector<double> input);
    static int minIndex(QVector<double> input);
    static int round(double value);
};

#endif // SUNRISESUNSET_H
