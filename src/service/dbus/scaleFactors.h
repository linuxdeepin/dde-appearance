#ifndef SCALEFACTORS_H
#define SCALEFACTORS_H

#include <QMap>
#include <QDBusMetaType>

typedef QMap<QString, double> ScaleFactors;

void registerScaleFactorsMetaType();

#endif // SCALEFACTORS_H
