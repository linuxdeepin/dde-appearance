#include "scaleFactors.h"
void registerScaleFactorsMetaType()
{
    qRegisterMetaType<ScaleFactors>("ScaleFactors");
    qDBusRegisterMetaType<ScaleFactors>();
}
