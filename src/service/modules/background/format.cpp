#include "format.h"
#include <QMimeDatabase>

QMap<QString,QString> FormatPicture::typeMap{
    {"image/jpeg","jpeg"},
    {"image/bmp", "bmp"},
    {"image/png","png"},
    {"image/tiff","tiff"},
    {"image/gif","jpeg"}
};

QString FormatPicture::getPictureType(QString file)
{
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(file);
    for(auto iter : typeMap.keys())
    {
        if(mime.name().startsWith(iter))
        {
            return typeMap[iter];
        }
    }

    return "";
}
