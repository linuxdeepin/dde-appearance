#include "syncconfig.h"
#include "../common/commondefine.h"

SyncConfig::SyncConfig(QString name, QString path)
    :name(name)
    ,path(path)
    ,orgDbusInterface("org.freedesktop.DBus",
                      "/org/freedesktop/DBus",
                      "org.freedesktop.DBus",
                      QDBusConnection::sessionBus())
    ,syncInterface("com.deepin.sync.Daemon",
                   "/com/deepin/sync/Daemon",
                   "com.deepin.sync.Daemon",
                   QDBusConnection::sessionBus())
{
     QDBusConnection::sessionBus().connect(orgDbusInterface.service(),
                                                     orgDbusInterface.path(),
                                                     orgDbusInterface.interface(),
                                                     SIGNAL(NameOwnerChanged(QString,QString,QString)),
                                                     this, SLOT(handleNameOwnerChanged(QString,QString,QString)));
}

QByteArray SyncConfig::Get()
{
    return nullptr;
}

void SyncConfig::Set(QByteArray)
{

}

void SyncConfig::registerConfig()
{
    if(!orgDbusInterface.isValid())
    {
        return;
    }

    QDBusMessage  message = orgDbusInterface.call("ListNames");
    if(message.type()==QDBusMessage::ErrorMessage)
    {
        return;
    }

    QStringList availableServices = message.arguments().first().toStringList();
    if(availableServices.indexOf(SYNCSERVICENAME) == -1)
    {
        return;
    }

    syncInterface.call("Register", name, path);
}

void SyncConfig::handleNameOwnerChanged(QString name, QString oldOwner, QString newOwner)
{
    if(name == SYNCSERVICENAME && !newOwner.isEmpty())
    {
        registerConfig();
    }
}
