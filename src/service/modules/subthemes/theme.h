#ifndef THEMES_H
#define THEMES_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
//#include <X11/Xcursor/Xcursor.h>

class Theme
{
public:
    Theme(QString id, QString filePath, bool deletable);
    Theme(QString fileName);

    bool    Delete();
    virtual ~Theme();
    QString getId();
    QString getPath();
    bool getDeleteable();

private:
    QString id;
    QString filePath;
    bool    deletable;
};

#endif // THEMES_H
