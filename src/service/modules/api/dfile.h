#ifndef DFILE_H
#define DFILE_H

#include <QString>
#include <vector>

class DFile
{
public:
    explicit DFile();
    static bool isAbs(QString file);
    static bool isExisted(QString file);
    static QString dir(QString file);
    static QString base(QString file);
    static std::vector<QString> glob(const QString& pattern);
};
#endif // DFILE_H
