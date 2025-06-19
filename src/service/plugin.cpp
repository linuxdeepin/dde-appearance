// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "appearance1adaptor.h"
#include "dbus/appearance1.h"
#include "modules/common/commondefine.h"
#include <QCoreApplication>

static Appearance1 *appearance = nullptr;
// QDBusConnection *pluginDbus = nullptr;

extern "C" int DSMRegister(const char *name, void *data)
{
    Q_UNUSED(name);
    Q_UNUSED(data);
    
    // TODO: deepin-service-manager 传递进来的dbus在后面使用会无效，因此暂时采用QDBusConnection::sessionBus()
    // (void)data;
    // pluginDbus = static_cast<QDBusConnection*>(data);
    appearance = new Appearance1();

    auto *translator = new QTranslator(appearance);
    QString languagePath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                  QString("plugin-dde-appearance/translations"),
                                                  QStandardPaths::LocateDirectory);
    bool translatorLoaded = translator->load(languagePath+"/dde-appearance_" + QLocale::system().name());
    if (!translatorLoaded) {
        qWarning() << "Failed to load translator for locale:" << QLocale::system().name();
    }
    qApp->installTranslator(translator);

    new Appearance1Adaptor(appearance);
    bool appearanceRegister = APPEARANCEDBUS.registerService(APPEARANCE_SERVICE);
    bool appearanceObjectRegister = APPEARANCEDBUS.registerObject(APPEARANCE_PATH, APPEARANCE_INTERFACE, appearance);

    if (!appearanceRegister ||!appearanceObjectRegister) {
        qWarning() << "appearance dbus object already registered";
        return -1;
    }
    return 0;
}

// 该函数用于资源释放
// 非常驻插件必须实现该函数，以防内存泄漏
extern "C" int DSMUnRegister(const char *name, void *data)
{
    Q_UNUSED(name);
    Q_UNUSED(data);
    
    if (appearance) {
        appearance->deleteLater();
    }
    appearance = nullptr;
    return 0;
}
