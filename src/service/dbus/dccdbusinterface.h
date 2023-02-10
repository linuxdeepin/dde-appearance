// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DCCDBUSINTERFACE_H
#define DCCDBUSINTERFACE_H

#include <QDBusAbstractInterface>

class DCCDBusInterfacePrivate;

class DCCDBusInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    explicit DCCDBusInterface(const QString &service, const QString &path, const QString &interface, const QDBusConnection &connection, QObject *parent);
    virtual ~DCCDBusInterface() override;

    bool serviceValid() const;
    QString suffix() const;
    void setSuffix(const QString &suffix);

    QVariant property(const char *propname);
    void setProperty(const char *propname, const QVariant &value);

Q_SIGNALS:
    void serviceValidChanged(const bool valid) const;

private:
    QScopedPointer<DCCDBusInterfacePrivate> d_ptrDCCDBusInterface;
    Q_DECLARE_PRIVATE_D(d_ptrDCCDBusInterface, DCCDBusInterface)
    Q_DISABLE_COPY(DCCDBusInterface)
};

#endif // DCCDBUSINTERFACE_H
