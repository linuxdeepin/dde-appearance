#ifndef FORMAT_H
#define FORMAT_H

#include <QObject>
#include <QFile>
#include <QMap>

class FormatPicture{

public:
    static QString getPictureType(QString file);

private:
    static QMap<QString,QString> typeMap;
};


#endif // FORMAT_H
