// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./dbus/deepinwmfaker.h"
#include "wmadaptor.h"

#include <DLog>

#include <QGuiApplication>

int main (int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("deepin");
    app.setApplicationName("org.deepin.dde.deepinwmfaker");
    const QString sessionType = qEnvironmentVariable("XDG_SESSION_TYPE");
    if (sessionType == "wayland") {
        qWarning() << "dde-fakewm does not support session type:" << sessionType;
        return -1;
    }
    DeepinWMFaker faker;
    WmAdaptor wmAdaptor(&faker);
    bool registerWmServiceSuccessed = QDBusConnection::sessionBus().registerService("com.deepin.wm");
    bool registerWmObjectSuccessed = QDBusConnection::sessionBus().registerObject("/com/deepin/wm", "com.deepin.wm", &faker);
    if (!registerWmServiceSuccessed || !registerWmObjectSuccessed) {
        qWarning() << "wm dbus service already registered";
        return -1;
    }
    return app.exec();
}
