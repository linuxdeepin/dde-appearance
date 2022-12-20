#ifndef UTILS_H
#define UTILS_H
#include <QString>
#include <QVector>

class utils
{
public:
    utils();
    static bool WriteStringToFile(QString filename, QString content);
    static bool isURI(QString uri);
    static bool isDir(QString dir);
    static bool isFilesInDir(QVector<QString> files, QString dir);
    static bool isFileExists(QString filename);
    static QString deCodeURI(QString uri);
    static QString enCodeURI(QString content, QString scheme);
    static QString GetUserHomeDir();
    static QString GetUserDataDir();
    static QString GetUserConfigDir();
    static QString GetUserCacheDir();
    static QString GetUserRuntimeDir();
};

#endif // UTILS_H
