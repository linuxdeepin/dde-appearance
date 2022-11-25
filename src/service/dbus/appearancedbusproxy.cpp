/*
 * Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     caixiangrong <caixiangrong@uniontech.com>
 *
 * Maintainer: caixiangrong <caixiangrong@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "appearancedbusproxy.h"

#include <QDBusPendingReply>

AppearanceDBusProxy::AppearanceDBusProxy(QObject *parent)
    : QObject(parent)
    , m_wmInterface(new DCCDBusInterface("com.deepin.wm", "/com/deepin/wm", "com.deepin.wm", QDBusConnection::sessionBus(), this))
    , m_displayInterface(new DCCDBusInterface("org.deepin.dde.Display1", "/org/deepin/dde/Display1", "org.deepin.dde.Display1", QDBusConnection::sessionBus(), this))
    , m_xSettingsInterface(new DCCDBusInterface("org.deepin.dde.XSettings1", "/org/deepin/dde/XSettings1", "org.deepin.dde.XSettings1", QDBusConnection::sessionBus(), this))
    , m_timeDateInterface(new DCCDBusInterface("org.freedesktop.timedate1", "/org/freedesktop/timedate1", "org.freedesktop.timedate1", QDBusConnection::systemBus(), this))
    , m_sessionTimeDateInterface(new DCCDBusInterface("org.deepin.dde.Timedate1", "/org/deepin/dde/Timedate1", "org.deepin.dde.Timedate1", QDBusConnection::sessionBus(), this))
    , nid(0)
{
    registerScaleFactorsMetaType();
    m_sessionTimeDateInterface->setSuffix("Session");
}

void AppearanceDBusProxy::setUserInterface(const QString &userPath)
{
    m_userInterface = QSharedPointer<DCCDBusInterface>(new DCCDBusInterface("org.deepin.dde.Accounts1",
                                                                            userPath,
                                                                            "org.deepin.dde.Accounts1.User",
                                                                            QDBusConnection::systemBus(), this));
}

void AppearanceDBusProxy::Notify(const QString &in0, const QString &in2, const QString &summary, const QString &body, const QStringList &options, const QVariantMap &optionMap, int expireTimeout)
{
    QDBusMessage notifyMessage = QDBusMessage::createMethodCall("org.freedesktop.Notifications",
                                                                "/org/freedesktop/Notifications",
                                                                "org.freedesktop.Notifications", "Notify");
    notifyMessage << in0 << nid << in2
                  << summary << body << options
                  << optionMap << expireTimeout;
    QDBusConnection::sessionBus().callWithCallback(notifyMessage, this, SLOT(NotifyNid(uint)));
}

void AppearanceDBusProxy::NotifyNid(uint id)
{
    nid = id;
}
// wmInterface
QString AppearanceDBusProxy::cursorTheme()
{
    return qvariant_cast<QString>(m_wmInterface->property("cursorTheme"));
}

void AppearanceDBusProxy::setcursorTheme(const QString &cursorTheme)
{
    m_wmInterface->setProperty("cursorTheme", cursorTheme);
}

int AppearanceDBusProxy::WorkspaceCount()
{
    return QDBusPendingReply<int>(m_wmInterface->asyncCall(QStringLiteral("WorkspaceCount")));
}
QString AppearanceDBusProxy::GetWorkspaceBackgroundForMonitor(int index, const QString &strMonitorName)
{
    return QDBusPendingReply<QString>(m_wmInterface->asyncCall(QStringLiteral("GetWorkspaceBackgroundForMonitor"), index, strMonitorName));
}
void AppearanceDBusProxy::SetCurrentWorkspaceBackgroundForMonitor(const QString &uri, const QString &strMonitorName)
{
    m_wmInterface->asyncCall(QStringLiteral("SetCurrentWorkspaceBackgroundForMonitor"), uri, strMonitorName);
}
void AppearanceDBusProxy::SetDecorationDeepinTheme(const QString &deepinThemeName)
{
    m_wmInterface->asyncCall(QStringLiteral("SetDecorationDeepinTheme"), deepinThemeName);
}
void AppearanceDBusProxy::ChangeCurrentWorkspaceBackground(const QString &uri)
{
    m_wmInterface->asyncCall(QStringLiteral("ChangeCurrentWorkspaceBackground"), uri);
}
int AppearanceDBusProxy::GetCurrentWorkspace()
{
    return QDBusPendingReply<int>(m_wmInterface->asyncCall(QStringLiteral("GetCurrentWorkspace")));
}

void AppearanceDBusProxy::SetWorkspaceBackgroundForMonitor(int index, const QString &strMonitorName, const QString &uri)
{
    m_wmInterface->asyncCall(QStringLiteral("SetWorkspaceBackgroundForMonitor"), index, strMonitorName, uri);
}
// displayInterface
QString AppearanceDBusProxy::primary()
{
    return qvariant_cast<QString>(m_displayInterface->property("Primary"));
}

QList<QDBusObjectPath> AppearanceDBusProxy::monitors()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_displayInterface->property("Monitors"));
}

QStringList AppearanceDBusProxy::ListOutputNames()
{
    return QDBusPendingReply<QStringList>(m_displayInterface->asyncCall(QStringLiteral("ListOutputNames")));
}
// xSettingsInterface
void AppearanceDBusProxy::SetString(const QString &prop, const QString &v)
{
    m_xSettingsInterface->asyncCall(QStringLiteral("SetString"), prop, v);
}
double AppearanceDBusProxy::GetScaleFactor()
{
    return QDBusPendingReply<double>(m_xSettingsInterface->asyncCall(QStringLiteral("GetScaleFactor")));
}
void AppearanceDBusProxy::SetScaleFactor(double scale)
{
    m_xSettingsInterface->asyncCall(QStringLiteral("SetScaleFactor"), scale);
}
ScaleFactors AppearanceDBusProxy::GetScreenScaleFactors()
{
    return QDBusPendingReply<ScaleFactors>(m_xSettingsInterface->asyncCall(QStringLiteral("GetScreenScaleFactors")));
}
void AppearanceDBusProxy::SetScreenScaleFactors(const ScaleFactors &factors)
{
    m_xSettingsInterface->asyncCall(QStringLiteral("SetScreenScaleFactors"), QVariant::fromValue(factors));
}

QString AppearanceDBusProxy::FindUserById(const QString &uid)
{
    QDBusMessage accountsMessage = QDBusMessage::createMethodCall("org.deepin.dde.Accounts1", "/org/deepin/dde/Accounts1", "org.deepin.dde.Accounts1", "FindUserById");
    accountsMessage << uid;
    return QDBusPendingReply<QString>(QDBusConnection::systemBus().asyncCall(accountsMessage));
}
// userInterface
QStringList AppearanceDBusProxy::desktopBackgrounds()
{
    if (m_userInterface.isNull())
        return QStringList();
    return qvariant_cast<QStringList>(m_userInterface->property("DesktopBackgrounds"));
}

QString AppearanceDBusProxy::greeterBackground()
{
    if (m_userInterface.isNull())
        return QString();
    return qvariant_cast<QString>(m_userInterface->property("GreeterBackground"));
}

void AppearanceDBusProxy::SetCurrentWorkspace(int currentWorkspace)
{
    if (!m_userInterface.isNull())
        m_userInterface->asyncCall(QStringLiteral("SetCurrentWorkspace"), currentWorkspace);
}

void AppearanceDBusProxy::SetDesktopBackgrounds(const QStringList &val)
{
    if (!m_userInterface.isNull())
        m_userInterface->asyncCall(QStringLiteral("SetDesktopBackgrounds"), val);
}

void AppearanceDBusProxy::SetGreeterBackground(const QString &bg)
{
    if (!m_userInterface.isNull())
        m_userInterface->asyncCall(QStringLiteral("SetGreeterBackground"), bg);
}
// timeDateInterface
QString AppearanceDBusProxy::timezone()
{
    return qvariant_cast<QString>(m_timeDateInterface->property("Timezone"));
}

bool AppearanceDBusProxy::nTP()
{
    return qvariant_cast<bool>(m_timeDateInterface->property("NTP"));
}
// sessionTimeDateInterface
bool AppearanceDBusProxy::nTPSession()
{

    return qvariant_cast<bool>(m_sessionTimeDateInterface->property("NTPSession"));
}
// imageBlurInterface
void AppearanceDBusProxy::Delete(const QString &file)
{
    QDBusMessage imageBlurMessage = QDBusMessage::createMethodCall("org.deepin.dde.ImageBlur1", "/org/deepin/dde/ImageBlur1", "org.deepin.dde.ImageBlur1", "Delete");
    imageBlurMessage << file;
    QDBusConnection::systemBus().asyncCall(imageBlurMessage);
}

QString AppearanceDBusProxy::Get(const QString &file)
{
    QDBusMessage imageBlurMessage = QDBusMessage::createMethodCall("org.deepin.dde.ImageBlur1", "/org/deepin/dde/ImageBlur1", "org.deepin.dde.ImageBlur1", "Get");
    imageBlurMessage << file;
    return QDBusPendingReply<QString>(QDBusConnection::systemBus().asyncCall(imageBlurMessage));
}
// imageEffectInterface
void AppearanceDBusProxy::Delete(const QString &effect, const QString &filename)
{
    QDBusMessage imageEffectMessage = QDBusMessage::createMethodCall("org.deepin.dde.ImageEffect1", "/org/deepin/dde/ImageEffect1", "org.deepin.dde.ImageEffect1", "Delete");
    imageEffectMessage << effect << filename;
    QDBusConnection::systemBus().asyncCall(imageEffectMessage);
}

QString AppearanceDBusProxy::Get(const QString &effect, const QString &filename)
{
    QDBusMessage imageEffectMessage = QDBusMessage::createMethodCall("org.deepin.dde.ImageEffect1", "/org/deepin/dde/ImageEffect1", "org.deepin.dde.ImageEffect1", "Get");
    imageEffectMessage << effect << filename;
    QDBusConnection::systemBus().asyncCall(imageEffectMessage);

    return QDBusPendingReply<QString>(QDBusConnection::systemBus().asyncCall(imageEffectMessage));
}
// Daemon1
void AppearanceDBusProxy::DeleteCustomWallPaper(const QString &username, const QString &file)
{
    QDBusMessage daemonMessage = QDBusMessage::createMethodCall("org.deepin.dde.Daemon1", "/org/deepin/dde/Daemon1", "org.deepin.dde.Daemon1", "DeleteCustomWallPaper");
    daemonMessage << username << file;
    QDBusConnection::systemBus().asyncCall(daemonMessage);
}

QStringList AppearanceDBusProxy::GetCustomWallPapers(const QString &username)
{
    QDBusMessage daemonMessage = QDBusMessage::createMethodCall("org.deepin.dde.Daemon1", "/org/deepin/dde/Daemon1", "org.deepin.dde.Daemon1", "GetCustomWallPapers");
    daemonMessage << username;
    return QDBusPendingReply<QStringList>(QDBusConnection::systemBus().asyncCall(daemonMessage));
}

QString AppearanceDBusProxy::SaveCustomWallPaper(const QString &username, const QString &file)
{
    QDBusMessage daemonMessage = QDBusMessage::createMethodCall("org.deepin.dde.Daemon1", "/org/deepin/dde/Daemon1", "org.deepin.dde.Daemon1", "SaveCustomWallPaper");
    daemonMessage << username << file;
    return QDBusPendingReply<QString>(QDBusConnection::systemBus().asyncCall(daemonMessage));
}
