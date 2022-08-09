#ifndef SYNCCONFIG_H
#define SYNCCONFIG_H
#include <QString>
#include <QDBusInterface>


class SyncConfig : public QObject
{
    Q_OBJECT
public:
    SyncConfig(QString name, QString path);
    virtual QByteArray Get();
    virtual void Set(QByteArray);
    void registerConfig();
    virtual ~SyncConfig() =default;

private:
    void handleNameOwnerChanged(QString name, QString oldOwner, QString newOwner);

private:
    QString         name;
    QString         path;
    QDBusInterface  orgDbusInterface;
    QDBusInterface  syncInterface;
};

#endif // SYNCCONFIG_H
