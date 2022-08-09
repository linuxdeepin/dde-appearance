#ifndef THEMEFONTSYNCCONFIG_H
#define THEMEFONTSYNCCONFIG_H

#include "appearancemanager.h"
#include "../modules/api/syncconfig.h"


class ThemeFontSyncConfig : public SyncConfig
{
public:
    ThemeFontSyncConfig(QString name, QString path, QSharedPointer<AppearanceManager> appearManager);
    virtual QByteArray Get();
    virtual void Set(QByteArray);
    virtual ~ThemeFontSyncConfig(){}

private:
    QSharedPointer<AppearanceManager> appearanceManager;
};

class BackgroundSyncConfig : public SyncConfig
{
public:
    BackgroundSyncConfig(QString name, QString path, QSharedPointer<AppearanceManager> appearManager);
    virtual QByteArray Get();
    virtual void Set(QByteArray);
    virtual ~BackgroundSyncConfig(){}

private:
    QSharedPointer<AppearanceManager> appearanceManager;
};

#endif // THEMEFONTSYNCCONFIG_H
