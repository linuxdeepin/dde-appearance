// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "appearance1adaptor.h"
#include "dbus/appearance1.h"
#include "modules/common/commondefine.h"
#include <QCoreApplication>

static Appearance1 *appearance = nullptr;
QDBusConnection *pluginDbus = nullptr;

extern "C" int DSMRegister(const char *name, void *data)
{
    (void)name;
    (void)data;
    pluginDbus = reinterpret_cast<QDBusConnection *>(data);
    QTranslator translator;
    QString languagePath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                  QString("dde-appearance/translations"),
                                                  QStandardPaths::LocateDirectory);
    translator.load(languagePath+"/dde-appearance_" + QLocale::system().name());
    qApp->installTranslator(&translator);

    appearance = new Appearance1();

    new Appearance1Adaptor(appearance);
    bool appearanceObjectRegister = pluginDbus->registerObject(APPEARANCE_PATH, APPEARANCE_INTERFACE, appearance);

    if (!appearanceObjectRegister) {
        qWarning() << "appearance dbus object already registered";
        return -1;
    }
    return 0;
}

// 该函数用于资源释放
// 非常驻插件必须实现该函数，以防内存泄漏
extern "C" int DSMUnRegister(const char *name, void *data)
{
    (void)name;
    (void)data;
    if (appearance) {
        appearance->deleteLater();
    }
    appearance = nullptr;
    return 0;
}
