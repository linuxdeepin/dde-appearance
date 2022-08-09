#ifndef THEMESApi_H
#define THEMESApi_H

#include "../subthemes/scanner.h"
#include "keyfile.h"

#include <QObject>
#include <QMutex>
#include <QDBusInterface>
#include <string>
#include <vector>
#include <QGSettings>

class ThemesApi : public QObject
{
    Q_OBJECT

    struct gtk2ConfInfo {
        QString key;
        QString value;
    };

public:
    ThemesApi(QObject *parent = nullptr);
    ~ThemesApi();

    bool isThemeInList(QString theme, QVector<QString> list);
    QVector<QString> listGtkTheme();
    QVector<QString> listIconTheme();
    QVector<QString> listCursorTheme();
    QVector<QString> doListTheme(QVector<QString> local, QVector<QString> sys, QString type);
    QVector<QString> scanThemeDirs(QVector<QString> dirs, QString type);
    QVector<QString> mergeThemeList(QVector<QString> src, QVector<QString> target);
    bool setWMTheme(QString name);
    bool setGtkTheme(QString name);
    bool setIconTheme(QString name);
    bool setCursorTheme(QString name);
    QString getThemePath(QString name, QString ty, QString key);
    void setGtk2Theme(QString name);
    void setGtk2Icon(QString name);
    void setGtk2Cursor(QString name);
    void setGtk2Prop(QString key, QString value, QString file);
    void gtk2FileReader(QString file);
    QSharedPointer<gtk2ConfInfo> getGtk2ConfInfo(QString key);
    void addGtk2ConfInfo(QString key, QString value);
    void gtk2FileWriter(QString file);
    void setGtk3Theme(QString name);
    void setGtk3Icon(QString name);
    void setGtk3Cursor(QString name);
    void setGtk3Prop(QString key, QString value, QString file);
    bool isGtk3PropEqual(QString key, QString value,KeyFile& keyfile);
    void doSetGtk3Prop(QString key,QString value, QString file, KeyFile& keyfile);
    bool setQTTheme();
    bool setQt4Theme(QString config);
    bool setDefaultCursor(QString name);
    void setGtkCursor(QString name);
    void setQtCursor(QString name);
    void setWMCursor(QString name);

private:
    QString getGtk2ConfFile();
    QString getGtk3ConfFile();
private:
    QSharedPointer<Scanner>                         scanner;
    QMutex                                          gtk2Mutex;
    QMutex                                          gtk3Mutex;
    QVector<QSharedPointer<gtk2ConfInfo>>           gtk2ConfInfos;
    QSharedPointer<QDBusInterface>                  wmDbusInterface;
    QSharedPointer<QGSettings>                      xSetting;
    QSharedPointer<QGSettings>                      metacitySetting;
    QSharedPointer<QGSettings>                      wmSetting;
    QSharedPointer<QGSettings>                      interfaceSetting;
};

#endif // THEMESApi_H
