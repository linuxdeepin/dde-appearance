// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "deepinwmfaker.h"

#include <QDBusMessage>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDBusInterface>
#include <QProcess>
#include <QStringList>
#include <QFileInfo>
#include <QMetaEnum>
#include <QDBusConnectionInterface>
#include <mutex>

#include <KF6/KConfigCore/KConfigGroup>
#include <KF6/KConfigCore/KSharedConfig>
#include <KF6/KGlobalAccel/KGlobalAccel>
#include <KF6/KWindowSystem/KWindowSystem>
#include <KF6/KWindowSystem/KWindowEffects>
#include <KWindowInfo>
#include <KX11Extras>

#include <DConfig>

#ifndef DISABLE_DEEPIN_WM
#define DconfigBackgroundUri "Background_Uris"
#define GsettingsZoneRightUp "rightUp"
#define GsettingsZoneRightDown "rightDown"
#define GsettingsZoneLeftDown "leftDown"
#define GsettingsZoneLeftUp "leftUp"
#endif // DISABLE_DEEPIN_WM

#define DeepinWMConfigName "deepinwmrc"
#define DeepinWMGeneralGroupName "General"
#define DeepinWMWorkspaceBackgroundGroupName "WorkspaceBackground"

#define KWinConfigName "kwinrc"
#define KWinCloseWindowGroupName "Script-closewindowaction"
#define KWinRunCommandGroupName "Script-runcommandaction"

#define GlobalAccelComponentName "kwin"
#define GlobalAccelComponentDisplayName "KWin"

// kwin dbus
#define KWinDBusService "org.kde.KWin"
#define KWinDBusPath "/KWin"
#define KWinDBusInterface "org.kde.KWin"

#define KWinDBusCompositorInterface "org.kde.kwin.Compositing"
#define KWinDBusCompositorPath "/Compositor"
const char defaultFirstBackgroundUri[] = "file:///usr/share/wallpapers/deepin/desktop.jpg";
const char defaultSecondBackgroundUri[] = "francesco-ungaro-1fzbUyzsHV8-unsplash";

//default cursor size :24
#define DEFAULTCURSORSIZE 24

const char fallback_background_name[] = "file:///usr/share/backgrounds/default_background.jpg";
const QStringList kwin_active_effects = {"blur", "scissor"};

//using org::kde::KWin;

// deepin-wm's accel as Key
static QMap<QString, QString> AllDeepinWMKWinAccelsMap {
    { "switchToWorkspace1", "Switch to Desktop 1" },
    { "switchToWorkspace2", "Switch to Desktop 2" },
    { "switchToWorkspace3", "Switch to Desktop 3" },
    { "switchToWorkspace4", "Switch to Desktop 4" },
    { "switchToWorkspace5", "Switch to Desktop 5" },
    { "switchToWorkspace6", "Switch to Desktop 6" },
    { "switchToWorkspace7", "Switch to Desktop 7" },
    { "switchToWorkspace8", "Switch to Desktop 8" },
    { "switchToWorkspace9", "Switch to Desktop 9" },
    { "switchToWorkspace10", "Switch to Desktop 10" },
    { "switchToWorkspace11", "Switch to Desktop 11" },
    { "switchToWorkspace12", "Switch to Desktop 12" },
    { "switchToWorkspaceLeft", "Switch to Previous Desktop" },
    { "switchToWorkspaceRight", "Switch to Next Desktop" },
    { "switchGroup", "Walk Through Windows of Current Application" },
    { "switchGroupBackward", "Walk Through Windows of Current Application (Reverse)" },
    { "switchApplications", "Walk Through Windows" },
    { "switchApplicationsBackward", "Walk Through Windows (Reverse)" },
    { "showDesktop", "Show Desktop" },
    { "activateWindowMenu", "Window Operations Menu" },
    { "toggleFullscreen", "Window Fullscreen" },
    { "toggleMaximized", "Window Maximize" },
    { "toggleAbove", "Toggle Window Raise/Lower" },
    { "maximize", "Window Absolute Maximize" },
    { "unmaximize", "Window Unmaximize" },
    { "minimize", "Window Minimize" },
    { "close", "Window Close" },
    { "beginMove", "Window Move" },
    { "beginResize", "Window Resize" },
    { "toggleToLeft", "Window Quick Tile Left" },
    { "toggleToRight", "Window Quick Tile Right" },
    { "moveToWorkspace1", "Window to Desktop 1" },
    { "moveToWorkspace2", "Window to Desktop 2" },
    { "moveToWorkspace3", "Window to Desktop 3" },
    { "moveToWorkspace4", "Window to Desktop 4" },
    { "moveToWorkspace5", "Window to Desktop 5" },
    { "moveToWorkspace6", "Window to Desktop 6" },
    { "moveToWorkspace7", "Window to Desktop 7" },
    { "moveToWorkspace8", "Window to Desktop 8" },
    { "moveToWorkspace9", "Window to Desktop 9" },
    { "moveToWorkspace10", "Window to Desktop 10" },
    { "moveToWorkspace11", "Window to Desktop 11" },
    { "moveToWorkspace12", "Window to Desktop 12" },
    { "moveToWorkspaceLeft", "Window to Previous Desktop" },
    { "moveToWorkspaceRight", "Window to Next Desktop" },
    { "maximizeVertically", "Window Maximize Vertical" },
    { "maximizeHorizontally", "Window Maximize Horizontal" },
    { "exposeAllWindows", "ExposeAll" },
    { "exposeWindows", "Expose" },
    { "previewWorkspace", "ShowMultitasking" },
    { "viewZoomIn", "view_zoom_in" },
    { "viewZoomOut", "view_zoom_out" },
    { "viewActualSize", "view_actual_size" },
};

static const QMap<QString, QString> WaylandDeepinWMKWinAccelsMap {
    { "launcher" , "Launcher"},
    { "terminal" , "Terminal"},
    { "deepinScreenRecorder", "Screen Recorder"},
    { "lockScreen",            "Lock screen"},
    { "showDock",              "Show/Hide the dock"},
    { "logout",                 "Shutdown interface"},
    { "terminalQuake",         "Terminal Quake Window"},
    { "screenshot",             "Screenshot"},
    { "screenshotFullscreen",  "Full screenshot"},
    { "screenshotWindow",      "Window screenshot"},
    { "screenshotDelayed",     "Delay screenshot"},
    { "fileManager",           "File manager"},
    { "disableTouchpad",       "Disable Touchpad"},
    { "wmSwitcher",            "Switch window effects"},
    { "turnOffScreen",        "Fast Screen Off"},
    { "systemMonitor",         "System Monitor"},
    { "colorPicker",           "Deepin Picker"},
    { "aiAssistant",           "Desktop AI Assistant"},
    { "textToSpeech",         "Text to Speech"},
    { "speechToText",         "Speech to Text"},
    { "clipboard",              "Clipboard"},
    { "translation",            "Translation"},
    { "messenger",              "Messenger"},
    { "save",                   "Save"},
    { "new",                    "New"},
    { "wakeUp",                "WakeUp"},
    { "audioRewind",           "AudioRewind"},
    { "audioMute",             "VolumeMute"},
    { "monBrightnessUp",      "MonBrightnessUp"},//monBrightnessUp
    { "wlan",                   "WLAN"},
    { "audioMedia",            "AudioMedia"},
    { "reply",                  "Reply"},
    { "favorites",              "Favorites"},
    { "audioPlay",             "AudioPlay"},
    { "audioMicMute",         "AudioMicMute"},
    { "audioPause",            "AudioPause"},
    { "audioStop",             "AudioStop"},
    { "powerOff",              "PowerOff"},
    { "documents",              "Documents"},
    { "game",                   "Game"},
    { "search",                 "Search"},
    { "audioRecord",           "AudioRecord"},
    { "display",                "Display"},
    { "reload",                 "Reload"},
    { "explorer",               "Explorer"},
    { "calculator",             "Calculator"},
    { "calendar",               "Calendar"},
    { "forward",                "Forward"},
    { "cut",                    "Cut"},
    { "monBrightnessDown",    "MonBrightnessDown"},
    { "copy",                   "Copy"},
    { "tools",                  "Tools"},
    { "audioRaiseVolume",     "VolumeUp"},
    { "mediaClose",            "Media Close"},
    { "www",                    "WWW"},
    { "homePage",              "HomePage"},
    { "sleep",                  "Sleep"},
    { "audioLowerVolume",     "VolumeDown"},
    { "audioPrev",             "AudioPrev"},
    { "audioNext",             "AudioNext"},
    { "paste",                  "Paste"},
    { "open",                   "Open"},
    { "send",                   "Send"},
    { "myComputer",            "MyComputer"},
    { "mail",                   "Mail"},
    { "adjustBrightness",      "BrightnessAdjust"},
    { "logOff",                "LogOff"},
    { "pictures",               "Pictures"},
    { "terminal",               "Terminal"},
    { "video",                  "Video"},
    { "music",                  "Music"},
    { "appLeft",               "ApplicationLeft"},
    { "appRight",              "ApplicationRight"},
    { "meeting",                "Meeting"},
    { "switchMonitors",        "Switch monitors"},
    { "capslock",               "Capslock"},
    { "numlock",                "Numlock"},
    { "notificationCenter",    "Notification Center"},
    { "screenshotOcr",    "ScreenshotOcr"},
    { "screenshotScroll",    "ScreenshotScroll"},
    { "globalSearch",    "Global Search"},
};


static const QStringList NotConfigurationAction = {
    "Launcher",
    "Terminal",
    "Screen Recorder",
    "Lock screen",
    "Show/Hide the dock",
    "Shutdown interface",
    "Terminal Quake Window",
    "Screenshot",
    "Full screenshot",
    "Window screenshot",
    "Delay screenshot",
    "File manager",
    "Disable Touchpad",
    "Switch window effects",
    "Fast Screen Off",
    "System Monitor",
    "Deepin Picker",
    "Desktop AI Assistant",
    "Text to Speech",
    "Speech to Text",
    "Clipboard",
    "Translation",
    "Messenger",
    "Save",
    "New",
    "WakeUp",
    "AudioRewind",
    "VolumeMute",
    "MonBrightnessUp",
    "WLAN",
    "AudioMedia",
    "Reply",
    "Favorites",
    "AudioPlay",
    "AudioMicMute",
    "AudioPause",
    "AudioStop",
    "PowerOff",
    "Documents",
    "Game",
    "Search",
    "AudioRecord",
    "Display",
    "Reload",
    "Explorer",
    "Calculator",
    "Calendar",
    "Forward",
    "Cut",
    "MonBrightnessDown",
    "Copy",
    "Tools",
    "VolumeUp",
    "Media Close",
    "WWW",
    "HomePage",
    "Sleep",
    "VolumeDown",
    "AudioPrev",
    "AudioNext",
    "Paste",
    "Open",
    "Send",
    "MyComputer",
    "Mail",
    "BrightnessAdjust",
    "LogOff",
    "Pictures",
    "Terminal",
    "Video",
    "Music",
    "ApplicationLeft",
    "ApplicationRight",
    "Meeting",
    "Switch monitors",
    "Capslock",
    "Numlock",
    "Notification Center",
    "ScreenshotOcr",
    "ScreenshotScroll",
    "Global Search",
};

static const QMap<QString, QString> SpecialKeyMap = {
    {"minus", "-"}, {"equal", "="}, {"brackertleft", "["}, {"breckertright", "]"},
    {"backslash", "\\"}, {"semicolon", ";"}, {"apostrophe", "'"}, {"comma", ","},
    {"period", "."}, {"slash", "/"}, {"grave", "`"},
};

static const QMap<QString, QString> SpecialRequireShiftKeyMap = {
    {"exclam", "!"}, {"at", "@"}, {"numbersign", "#"}, {"dollar", "$"},
    {"percent", "%"}, {"asciicircum", "^"}, {"ampersand", "&"}, {"asterisk", "*"},
    {"parenleft", "("}, {"parenright", ")"}, {"underscore", "_"}, {"plus", "+"},
    {"braceleft", "{"}, {"braceright", "}"}, {"bar", "|"}, {"colon", ":"},
    {"quotedbl", "\""}, {"less", "<"}, {"greater", ">"}, {"question", "?"},
    {"asciitilde", "~"}
};

std::mutex WORKSPACE_COUNT_GUARD;

DeepinWMFaker::DeepinWMFaker(QObject* appearance)
    : QObject(appearance)
    , m_windowSystem(KWindowSystem::self())
    , m_deepinWMConfig(new KConfig(DeepinWMConfigName, KConfig::CascadeConfig))
    , m_deepinWMGeneralGroup(new KConfigGroup(m_deepinWMConfig->group(DeepinWMGeneralGroupName)))
    , m_deepinWMWorkspaceBackgroundGroup(new KConfigGroup(m_deepinWMConfig->group(DeepinWMWorkspaceBackgroundGroupName)))
    , m_kwinConfig(new KConfig(KWinConfigName, KConfig::CascadeConfig))
    , m_kwinCloseWindowGroup(new KConfigGroup(m_kwinConfig->group(KWinCloseWindowGroupName)))
    , m_kwinRunCommandGroup(new KConfigGroup(m_kwinConfig->group(KWinRunCommandGroupName)))
    , m_globalAccel(KGlobalAccel::self())
    , m_previewWinMiniPair(QPair<uint, bool>(-1, false))
{
    m_workspaceount =  QDBusInterface(KWinDBusService, "/VirtualDesktopManager", "org.kde.KWin.VirtualDesktopManager").property("count").value<int>();
    m_isPlatformX11 = isX11Platform();
    m_appearance = QSharedPointer<DConfig>(DConfig::create("org.deepin.dde.appearance", "org.deepin.dde.appearance"));
    if (!m_appearance || !m_appearance->isValid()) {
        qWarning() << "appearance dconfig is not vaild";
        exit(-1);
    }
#ifndef DISABLE_DEEPIN_WM
    m_currentDesktop = m_kwinConfig->group("Workspace").readEntry<int>("CurrentDesktop", 1);

    connect(KX11Extras::self(), &KX11Extras::currentDesktopChanged, this, [this] (int to) {
        Q_EMIT WorkspaceSwitched(m_currentDesktop, to);
        m_currentDesktop = to;
    });
    connect(KX11Extras::self(), &KX11Extras::numberOfDesktopsChanged, this, &DeepinWMFaker::workspaceCountChanged);
//    connect(m_appearance.data(), SIGNAL(valueChanged(const QString &)),this,SLOT(DeepinWMFaker::onGsettingsDDEAppearanceChanged));
    connect(m_appearance.data(), SIGNAL(valueChanged(const QString &)), this, SLOT(onGsettingsDDEAppearanceChanged(const QString &)));
#endif // DISABLE_DEEPIN_WM

    QDBusConnection::sessionBus().connect(KWinDBusService, KWinDBusCompositorPath, KWinDBusCompositorInterface,
                                          "compositingToggled", "b", this, SLOT(wmCompositingEnabledChanged(bool)));
    QDBusConnection::sessionBus().connect(KWinDBusService, KWinDBusPath, KWinDBusInterface, "MultitaskStateChanged", this, SLOT(SlotUpdateMultitaskStatus(bool)));

    // 迁移旧的标题栏主题插件配置
    KConfigGroup decoration_group(m_kwinConfig, "org.kde.kdecoration2");

    if (decoration_group.readEntry("library", QString()) == "org.kde.kwin.aurorae") {
        const QString &theme = decoration_group.readEntry("theme", QString());

        // 自动将旧的主题更新为新的插件配置项
        if (theme == "__aurorae__svg__deepin") {
            SetDecorationDeepinTheme("light");
        } else if (theme == "__aurorae__svg__deepin-dark") {
            SetDecorationDeepinTheme("dark");
        }
    }

    auto e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));

    if (XDG_SESSION_TYPE != QLatin1String("x11")) {
        for (auto iter = WaylandDeepinWMKWinAccelsMap.begin(); iter != WaylandDeepinWMKWinAccelsMap.end(); iter++) {
            AllDeepinWMKWinAccelsMap.insert(iter.key(), iter.value());
        }
    }

    QDBusConnection::sessionBus().connect(QStringLiteral("org.deepin.dde.Appearance1"),
                                          QStringLiteral("/org/deepin/dde/Appearance1"),
                                          QStringLiteral("org.deepin.dde.Appearance1"),
                                          QStringLiteral("Changed"),
                                          this,
                                          SLOT(handleThemeChanged(QString, QString)));

    QDBusConnection::sessionBus().connect(QStringLiteral(KWinDBusService),
                                          QStringLiteral("/VirtualDesktopManager"),
                                          QStringLiteral("org.kde.KWin.VirtualDesktopManager"),
                                          QStringLiteral("countChanged"),
                                          this,
                                          SLOT(handleWorkspaceCountChanged(quint32)));
}

void DeepinWMFaker::handleWorkspaceCountChanged(quint32 count)
{
    std::lock_guard<std::mutex> guard(WORKSPACE_COUNT_GUARD);
    m_workspaceount = count;
}

void DeepinWMFaker::handleThemeChanged(const QString& key, const QString &value)
{
    if (key == "gtk") {
        SetDecorationDeepinTheme(value.contains("dark") ? "dark" : "light");
    }
}

DeepinWMFaker::~DeepinWMFaker()
{
    delete m_deepinWMConfig;
    delete m_deepinWMGeneralGroup;
    delete m_deepinWMWorkspaceBackgroundGroup;
    delete m_kwinConfig;
    delete m_kwinCloseWindowGroup;
    delete m_kwinRunCommandGroup;
}

bool DeepinWMFaker::compositingEnabled() const
{
    QStringList active_effects = QDBusInterface("org.kde.KWin", "/Effects", "org.kde.kwin.Effects").property("activeEffects").toStringList();
    bool enable = true;
    for (const QString &effect : kwin_active_effects) {
        if (!active_effects.contains(effect)) {
            enable = false;
            break;
        }
    }
    return enable;
}

bool DeepinWMFaker::compositingPossible() const
{
    return QDBusInterface(KWinDBusService, KWinDBusCompositorPath, KWinDBusCompositorInterface).property("compositingPossible").toBool();
}

bool DeepinWMFaker::compositingAllowSwitch() const
{
    if (qgetenv("KWIN_COMPOSE").startsWith("N"))
        return false;

    if (QDBusInterface(KWinDBusService, KWinDBusCompositorPath, KWinDBusCompositorInterface)
            .property("platformRequiresCompositing").toBool()) {
        return false;
    }

    return m_kwinConfig->group("Compositing").readEntry("AllowSwitch", true);
}

bool DeepinWMFaker::zoneEnabled() const
{
    bool enable_closewindow = m_kwinCloseWindowGroup->readEntry("Enabled", QVariant(true)).toBool();

    if (enable_closewindow)
        return true;

    return m_kwinRunCommandGroup->readEntry("Enabled", QVariant(true)).toBool();
}

QString DeepinWMFaker::cursorTheme() const
{
    KConfigGroup mousecfg(KSharedConfig::openConfig("kcminputrc", KConfig::NoGlobals), "Mouse");
    const QString themeName = mousecfg.readEntry("cursorTheme", "default");

    return themeName;
}

int DeepinWMFaker::cursorSize() const
{
    KConfigGroup mousecfg(KSharedConfig::openConfig("kcminputrc", KConfig::NoGlobals), "Mouse");
    bool ok = false;
    // The default Xcursor theme size of deepin-kwin is 24
    int themeSize = mousecfg.readEntry("cursorSize", QString("24")).toInt(&ok);

    return ok ? themeSize : 24;
}

#ifndef DISABLE_DEEPIN_WM
QString DeepinWMFaker::getWorkspaceBackgroundOfDeepinWM(const int index) const
{
    return m_appearance->value(DconfigBackgroundUri).toStringList().value(index - 1);
}

void DeepinWMFaker::setWorkspaceBackgroundForDeepinWM(const int index, const QString &uri) const
{
    QStringList all_wallpaper = m_appearance->value(DconfigBackgroundUri).toStringList();

    // 当设置的工作区编号大于列表长度时，先填充数据
    if (index > all_wallpaper.size()) {
        all_wallpaper.reserve(index);

        for (int i = all_wallpaper.size(); i < index; ++i) {
            all_wallpaper.append(QString());
        }
    }

    all_wallpaper[index - 1] = uri;
    // 将壁纸设置同步到 deepin-wm
    m_appearance->setValue(DconfigBackgroundUri, all_wallpaper);
}
#endif // DISABLE_DEEPIN_WM

QString DeepinWMFaker::GetWorkspaceBackground(const int index) const
{
    if (!m_transientBackgroundUri.isEmpty() && index == GetCurrentWorkspaceInner()) {
        return m_transientBackgroundUri;
    }

    const QString &uri = getWorkspaceBackground(index);

#ifndef DISABLE_DEEPIN_WM
    // fellback
    if (uri.isEmpty()) {
        return getWorkspaceBackgroundOfDeepinWM(index);
    }
#endif // DISABLE_DEEPIN_WM

    return uri;
}

void DeepinWMFaker::SetWorkspaceBackground(const int index, const QString &uri)
{
    m_transientBackgroundUri.clear();
    setWorkspaceBackground(index, uri);
#ifndef DISABLE_DEEPIN_WM
    m_deepinWMBackgroundUri.clear();
    setWorkspaceBackgroundForDeepinWM(index, uri);
#endif // DISABLE_DEEPIN_WM
}

QString DeepinWMFaker::GetCurrentWorkspaceBackground() const
{
    return GetWorkspaceBackground(GetCurrentWorkspaceInner());
}

void DeepinWMFaker::SetCurrentWorkspaceBackground(const QString &uri)
{
    SetWorkspaceBackground(GetCurrentWorkspaceInner(), uri);
}

QString DeepinWMFaker::GetWorkspaceBackgroundForMonitor(const int index,const QString &strMonitorName) const
{
    auto message = this->message();
    setDelayedReply(true);
    message.setDelayedReply(true);

    QDBusInterface interface("org.deepin.dde.Appearance1", "/org/deepin/dde/Appearance1", "org.deepin.dde.Appearance1");

    QDBusPendingCall reply = interface.asyncCallWithArgumentList("GetWorkspaceBackgroundForMonitor", {index, strMonitorName});

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, message, reply, watcher]{
        if (reply.isError()) {
           QDBusConnection::sessionBus().send(message.createErrorReply(reply.error()));
        }
        QDBusReply<QString> result = reply;
        QDBusConnection::sessionBus().send(message.createReply(result.value()));
        watcher->deleteLater();
    });
    return QString();
}

void DeepinWMFaker::SetWorkspaceBackgroundForMonitor(const int index, const QString &strMonitorName, const QString &uri)
{
    m_transientBackgroundUri.clear();
    setWorkspaceBackgroundForMonitor( index,strMonitorName,uri );
}

QString DeepinWMFaker::GetCurrentWorkspaceBackgroundForMonitor(const QString &strMonitorName)
{
    setDelayedReply(true);
    return GetWorkspaceBackgroundForMonitor(m_currentDesktop, strMonitorName);
}
void DeepinWMFaker::SetCurrentWorkspaceBackgroundForMonitor(const QString &uri, const QString &strMonitorName)
{
    SetWorkspaceBackgroundForMonitor(GetCurrentWorkspaceInner(), strMonitorName, uri );
}

void DeepinWMFaker::SetTransientBackground(const QString &uri)
{
    int current = GetCurrentWorkspaceInner();

    m_transientBackgroundUri = uri;
#ifndef DISABLE_DEEPIN_WM
    if (m_transientBackgroundUri.isEmpty()) {
        quitTransientBackground();
    } else {
        m_deepinWMBackgroundUri = getWorkspaceBackgroundOfDeepinWM(current);
        setWorkspaceBackgroundForDeepinWM(current, uri);
    }
#endif // DISABLE_DEEPIN_WM

    Q_EMIT WorkspaceBackgroundChanged(current, uri);
}

void DeepinWMFaker::SetTransientBackgroundForMonitor(const QString &uri, const QString &strMonitorName)
{
     int current = KX11Extras::currentDesktop();

     m_transientBackgroundUri = uri;
     Q_EMIT WorkspaceBackgroundChangedForMonitor( current,strMonitorName,uri );
}

#ifndef DISABLE_DEEPIN_WM
void DeepinWMFaker::ChangeCurrentWorkspaceBackground(const QString &uri)
{
    SetCurrentWorkspaceBackground(uri);
}
#endif // DISABLE_DEEPIN_WM

int DeepinWMFaker::GetCurrentWorkspaceInner() const
{
    QDBusReply<int> reply = QDBusInterface(KWinDBusService, KWinDBusPath, KWinDBusInterface).call("currentDesktop");
    return reply.value();
}

int DeepinWMFaker::GetCurrentWorkspace() const
{
    auto message = this->message();
    setDelayedReply(true);
    QDBusPendingCall reply = QDBusInterface(KWinDBusService, KWinDBusPath, KWinDBusInterface).asyncCall("currentDesktop");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, message, reply, watcher]{
       if (reply.isError()) {
          QDBusConnection::sessionBus().send(message.createErrorReply(reply.error()));
       }
       QDBusReply<int> result = reply;
       QDBusConnection::sessionBus().send(message.createReply(result.value()));
       watcher->deleteLater();
    });
    return 1;
}

int DeepinWMFaker::WorkspaceCount() const
{
    std::lock_guard<std::mutex> guard(WORKSPACE_COUNT_GUARD);
    return m_workspaceount;
}

void DeepinWMFaker::SetCurrentWorkspace(const int index)
{
    // 切换工作区时关闭壁纸预览
    quitTransientBackground();
    QDBusInterface(KWinDBusService, KWinDBusPath, KWinDBusInterface).call("setCurrentDesktop", index);
}

void DeepinWMFaker::NextWorkspace()
{
    // loopback support
//    int current = m_windowSystem->currentDesktop();
//    ++current < m_windowSystem->numberOfDesktops() ? current : loopback ? 0 : --current;
//    SetCurrentWorkspace(current);

   SetCurrentWorkspace(GetCurrentWorkspaceInner() + 1);
}

void DeepinWMFaker::PreviousWorkspace()
{
    // loopback support
//    int current = m_windowSystem->currentDesktop();
//    --current >= 0 ? current : loopback ? --(m_windowSystem->numberOfDesktops()) : 0;
//    SetCurrentWorkspace(current);

    SetCurrentWorkspace(GetCurrentWorkspaceInner() - 1);
}

/*!
 * [ { "Id":"...", "Accels":["...", "..."] }, {...} ]
 */
QString DeepinWMFaker::GetAllAccels() const
{
    QJsonArray allAccelsArray;
    for (auto it = AllDeepinWMKWinAccelsMap.constBegin(); it != AllDeepinWMKWinAccelsMap.constEnd(); ++it) {
        QJsonObject accelObj;
        accelObj.insert("Id", it.key());
        accelObj.insert("Accels", QJsonArray::fromStringList(GetAccel(it.key())));
        accelObj.insert("Default", QJsonArray::fromStringList(GetDefaultAccel(it.key())));
        allAccelsArray.append(accelObj);
    }

    return QJsonDocument(allAccelsArray).toJson(QJsonDocument::Compact);
}

/*!
 * \brief DeepinWMFaker::GetAccel
 * \param id The deepin wm accel name
 * \return
 */
QStringList DeepinWMFaker::GetAccel(const QString &id) const
{
    if (id.isEmpty()) {
        return QStringList();
    }

    const QString &kId = AllDeepinWMKWinAccelsMap.value(id);
    if (kId.isEmpty()) {
        return QStringList();
    }

    const QList<QKeySequence> &seqList = m_globalAccel->globalShortcut(GlobalAccelComponentName, kId);
    if (seqList.isEmpty()) {
        return QStringList();
    }

    QStringList accelList;
    for (const QKeySequence &seq : seqList) {
        accelList.append(transToDaemonAccelStr(seq.toString()));
    }

    return accelList;
}

static QMap<QString, QList<QKeySequence>> getShoutcutListFromKDEConfigFile()
{
    // 认为系统配置文件中存储的快捷键为默认值
    KConfig kglobalshortcutsrc("/etc/xdg/kglobalshortcutsrc");
    KConfigGroup kwin(&kglobalshortcutsrc, GlobalAccelComponentName);

    if (!kwin.isValid())
        return {};

    const QStringList key_list = kwin.keyList();
    QMap<QString, QList<QKeySequence>> result;

    for (const QString &str : key_list) {
        auto value_list = kwin.readEntry(str, QStringList());

        if (value_list.isEmpty())
            continue;

        QList<QKeySequence> ks_list;

        // 多个快捷键是以制表符为分隔符
        for (const QString &key : value_list.first().split("\t")) {
            QKeySequence ks(key);

            if (!ks.isEmpty()) {
                ks_list << ks;
            }
        }

        result[str] = ks_list;
    }

    return result;
}

QStringList DeepinWMFaker::GetDefaultAccel(const QString &id) const
{
    if (id.isEmpty()) {
        return QStringList();
    }

    const QString &kId = AllDeepinWMKWinAccelsMap.value(id);
    if (kId.isEmpty()) {
        return QStringList();
    }

    static auto shortcutMap = getShoutcutListFromKDEConfigFile();
    const QList<QKeySequence> &seqList = shortcutMap.value(kId);
    if (seqList.isEmpty()) {
        return QStringList();
    }

    QStringList accelList;
    for (const QKeySequence &seq : seqList) {
        accelList.append(transToDaemonAccelStr(seq.toString()));
    }

    return accelList;
}

/*!
 * { "Id":"...", "Accels":["...", "..."] }
 */
bool DeepinWMFaker::SetAccel(const QString &data)
{
    if (data.isEmpty()) {
        return false;
    }

    const QJsonDocument &jsonDoc = QJsonDocument::fromJson(data.toUtf8());
    if (jsonDoc.isEmpty()) {
        return false;
    }

    bool result = true;

    const QJsonObject &jsonObj = jsonDoc.object();
    QString kId = AllDeepinWMKWinAccelsMap.value(jsonObj.value("Id").toString());
    if (kId.isEmpty()) {
        kId = jsonObj.value("Id").toString();
        qDebug() << "Info: add a new shortcut for kId:"<<kId;
    }
    QAction *action = accelAction(kId);
    if (!action) {
        return false;
    }

    QList<QKeySequence> accelList;
    const QJsonArray &accelArray = jsonObj.value("Accels").toArray();
    for (const QJsonValue &jsonValue : accelArray) {
        const QString &accelStr = jsonValue.toString();

        qDebug() << "transFromDaemonAccelStr:" << transFromDaemonAccelStr(accelStr);

    QKeySequence seq;
    if(transFromDaemonAccelStr(accelStr).contains("Qt::Key_")){
            bool isOk = false;
            QMetaEnum metaEnum = QMetaEnum::fromType<Qt::Key>();
            int iRet = metaEnum.keyToValue(accelStr.toStdString().c_str(),&isOk);
            if (iRet <0 || !isOk){
                qDebug() << "metaEnum err:" << accelStr;
                return false;
            }
            Qt::Key keycode = Qt::Key(iRet);
            seq = QKeySequence(keycode);
        }else{
            seq = QKeySequence(transFromDaemonAccelStr(accelStr));
        }

        if (seq.isEmpty()) {
             qDebug() << "WARNING: got an empty key sequence for accel string:" << accelStr;
        }

        if(!qgetenv("WAYLAND_DISPLAY").isEmpty()) {
            m_globalAccel->stealShortcutSystemwide(seq);
        }

        accelList.append(seq);
    }

    // using setGlobalShortcat() only can set a new accel,
    // it will not override the exist global accel just change the default accel
    if (!m_globalAccel->setShortcut(action, accelList, KGlobalAccel::NoAutoloading)) {
        // qDebug() << "WARNING: set accel failed for" << kId << "with accels:" << accelList;
        result = false;
    }

    m_accelIdActionMap.insert(kId, action);

    return result;
}

/*!
 * \brief DeepinWMFaker::RemoveAccel
 * \param id The deepin wm accel name
 * \return
 */
void DeepinWMFaker::RemoveAccel(const QString &id)
{
    if (id.isEmpty()) {
        return;
    }

    const QString &kId = AllDeepinWMKWinAccelsMap.value(id);
    if (kId.isEmpty()) {
        return;
    }

    const bool contains = m_accelIdActionMap.contains(kId);

    QAction *action = accelAction(kId);
    if (!action) {
        return;
    }

    // remove will failed if the action is not handled by KGlobalAccel
    if (!contains) {
        m_globalAccel->setShortcut(
                    action, m_globalAccel->globalShortcut(GlobalAccelComponentName, kId));
    }

    m_globalAccel->removeAllShortcuts(action);

    m_accelIdActionMap.remove(kId);
    action->deleteLater();
}

static WId previewingController = 0;
void DeepinWMFaker::PreviewWindow(uint xid)
{
    QDBusInterface interface_kwin(KWinDBusService, KWinDBusPath);

    interface_kwin.call("previewWindows", QVariant::fromValue(QList<uint>({xid})));

    if (interface_kwin.lastError().type() == QDBusError::NoError) {
        return;
    } // else 兼容非deepin-kwin的环境

    // 只允许同时预览一个窗口
    if (previewingController) {
        return;
    }

    // 使用kwin自带的预览特效
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered("org.kde.KWin.HighlightWindow")) {
        //if (KWindowEffects::isEffectAvailable(KWindowEffects::HighlightWindows)) {
        // ###(zccrs): 按道理讲 previewingController 应该为dock的预览展示窗口（发起预览请求的窗口）
        // 不过，dde-dock中不支持此种用法，而且对kwin接口的调用仅仅是fallback，因此直接将xid作为预览请求的controller窗口
        //previewingController = xid;
        //KWindowEffects::highlightWindows(previewingController, {xid});
        callHighlightWindows({QString::number(xid)});
        return;
    }

    // FIXME: preview window should not change the order of windows

    // qDebug() << "winid" << xid;
    // qDebug() << "windows" << m_windowSystem->windows();
    // qDebug() << "order" << m_windowSystem->stackingOrder();
    // qDebug() << "contains" << m_windowSystem->hasWId(xid);

    KX11Extras::forceActiveWindow(xid);
    m_previewWinMiniPair.first = xid;
    m_previewWinMiniPair.second = false;

    KWindowInfo info(xid, NET::WMState | NET::XAWMState);
    if (info.valid()) {
        m_previewWinMiniPair.second = info.isMinimized();
    }

    // qDebug() << "preview" << m_previewWinMiniPair;
}

void DeepinWMFaker::CancelPreviewWindow()
{
    QDBusInterface interface_kwin(KWinDBusService, KWinDBusPath);

    interface_kwin.call("quitPreviewWindows");

    if (interface_kwin.lastError().type() == QDBusError::NoError) {
        return;
    } // else 兼容非deepin-kwin的环境

    // 退出kwin自带的预览特效
    if (previewingController) {
        //KWindowEffects::highlightWindows(previewingController, {});
        callHighlightWindows({QString::number(previewingController)});
        previewingController = 0;
        return;
    }

    // FIXME: same as above
    if (KX11Extras::windows().contains(m_previewWinMiniPair.first)) {
        if (m_previewWinMiniPair.second) {
            // m_windowSystem->minimizeWindow(m_previewWinMiniPair.first);
            // using this way to minimize a window without animation
            KX11Extras::setState(m_previewWinMiniPair.first, NET::Hidden);
            return;
        }
        // TODO, in kf6, lowerWindow is deprecated and be removed,and the source code lowerWindow implementation is empty
        // https://github.com/KDE/kwindowsystem/blob/kf5/src/kwindowsystem.cpp
        //m_windowSystem->lowerWindow(m_previewWinMiniPair.first);
    }
}

void DeepinWMFaker::PerformAction(int type)
{
    switch (type) {
    case wmActionShowWorkspace:
        ShowWorkspace();
        break;
    case wmActionToggleMaximize:
        ToggleActiveWindowMaximize();
        break;
    case wmActionMinimize:
        MinimizeActiveWindow();
        break;
    case wmActionShowWindow:
        ShowWindow();
        break;
    case wmActionShowAllWindow:
        ShowAllWindow();
        break;
    default:
        break;
    }
}

void DeepinWMFaker::BeginToMoveActiveWindow()
{
    Q_EMIT BeginToMoveActiveWindowChanged();
}

void DeepinWMFaker::SwitchApplication(bool backward)
{
    Q_EMIT SwitchApplicationChanged(backward);
}

void DeepinWMFaker::TileActiveWindow(uint side)
{
    Q_EMIT TileActiveWindowChanged(side);
}

void DeepinWMFaker::ToggleActiveWindowMaximize()
{
    Q_EMIT ToggleActiveWindowMaximizeChanged();
}

void DeepinWMFaker::MinimizeActiveWindow()
{
    KX11Extras::minimizeWindow(KX11Extras::activeWindow());
}

void DeepinWMFaker::SetDecorationTheme(const QString &type, const QString &name)
{
    m_kwinConfig->group("org.kde.kdecoration2").writeEntry("theme", QVariant());
    m_kwinConfig->group("org.kde.kdecoration2").writeEntry("library", "com.deepin.chameleon");
    m_kwinConfig->group("deepin-chameleon").writeEntry("theme", type + "/" + name);

    syncConfigForKWin();

    Q_EMIT DecorationThemeChanged();
}

void DeepinWMFaker::SetDecorationDeepinTheme(const QString &name)
{
    SetDecorationTheme(name, "deepin");
}

void DeepinWMFaker::enableEffect(bool on)
{
    QDBusInterface interface("org.kde.KWin", "/Effects", "org.kde.kwin.Effects");
    for (const QString& name : kwin_active_effects) {
        if (on)
            interface.call("toggleEffect", name);
        else
            interface.call("unloadEffect", name);
    }
}

void DeepinWMFaker::setCompositingEnabled(bool on)
{
    if (!compositingAllowSwitch()) {
        return;
    }

    if (compositingEnabled() == on) {
        return;
    }

    enableEffect(on);

    if (on)
        Q_EMIT ResumeCompositorChanged(1);
    else
        Q_EMIT SuspendCompositorChanged(1);

    // !on 时说明再关闭窗口特效，关闭特效往往都能成功，因此不再需要判断是否成功（KWin中给出值时有些延迟，导致未能及时获取到值）
    if (!on || compositingEnabled() == on)
        Q_EMIT compositingEnabledChanged(on);
}

// 2D效果下不支持显示工作区、显示应用程序所有窗口、显示所有窗口等功能，此处调用对话框告知用户
bool DeepinWMFaker::maybeShowWarningDialog()
{
    bool enable = QDBusInterface(KWinDBusService, KWinDBusCompositorPath, KWinDBusCompositorInterface).property("active").toBool();
    if (!enable) {
        return QProcess::startDetached("/usr/lib/deepin-daemon/dde-warning-dialog");
    }

    return false;
}

void DeepinWMFaker::callHighlightWindows(const QStringList &windowIds)
{
    QDBusInterface interface("org.kde.KWin.HighlightWindow", "/org/kde/KWin/HighlightWindow",
        "org.kde.KWin.HighlightWindow", QDBusConnection::sessionBus());
    if (interface.isValid()) {
        QDBusReply<void> reply = interface.call("highlightWindows", QVariant::fromValue(windowIds));
        if (reply.isValid()) {
            qDebug() << "Successfully called highlightWindows with" << windowIds;
        } else {
            qWarning() << "Failed to call highlightWindows:" << reply.error().message();
        }
    } else {
        qWarning() << "Failed to call highlightWindows:" << interface.lastError().message();
    }
}

void DeepinWMFaker::ShowAllWindow()
{
    if (maybeShowWarningDialog())
        return;

    Q_EMIT ShowAllWindowChanged();
}

void DeepinWMFaker::ShowWindow()
{
    if (maybeShowWarningDialog())
        return;

    Q_EMIT ShowWindowChanged();
}

void DeepinWMFaker::ShowWorkspace()
{
    if (maybeShowWarningDialog())
        return;

    Q_EMIT ShowWorkspaceChanged();
}

void DeepinWMFaker::setZoneEnabled(bool zoneEnabled)
{
    m_kwinCloseWindowGroup->writeEntry("Enabled", zoneEnabled);
    m_kwinRunCommandGroup->writeEntry("Enabled", zoneEnabled);
    syncConfigForKWin();
}

static bool updateCursorConfig()
{
    if (!KSharedConfig::openConfig("kcminputrc", KConfig::NoGlobals)->sync()) {
        return false;
    }

    auto message = QDBusMessage::createSignal(QStringLiteral("/KGlobalSettings"),
                                              QStringLiteral("org.kde.KGlobalSettings"),
                                              QStringLiteral("notifyChange"));

    // 添加 notify 类型参数 (ChangeCursor = 5)
    message << 5;
    // 添加任意 int 参数，未被使用
    message << 0;

    return QDBusConnection::sessionBus().send(message);
}

void DeepinWMFaker::setCursorTheme(QString cursorTheme)
{
    KConfigGroup mousecfg(KSharedConfig::openConfig("kcminputrc", KConfig::NoGlobals), "Mouse");
    mousecfg.writeEntry("cursorTheme", cursorTheme);
    updateCursorConfig();
}

void DeepinWMFaker::setCursorSize(int cursorSize)
{
    KConfigGroup mousecfg(KSharedConfig::openConfig("kcminputrc", KConfig::NoGlobals), "Mouse");
    mousecfg.writeEntry("cursorSize", QString::number(cursorSize));
    updateCursorConfig();
}

#ifndef DISABLE_DEEPIN_WM
void DeepinWMFaker::SwitchToWorkspace(bool backward)
{
    backward ? PreviousWorkspace() : NextWorkspace();
}

void DeepinWMFaker::PresentWindows(const QList<uint> &xids)
{
    if (xids.isEmpty())
        return;
    if (m_isPlatformX11) {
        QList<WId> windows;

        for (uint w : xids)
            windows << w;

        // TODO kf6 has no method to present windows
        //KWindowEffects::presentWindows(windows.first(), windows);
    } else {
        QDBusInterface Interface("org.kde.KWin",
                                 "/org/kde/KWin/PresentWindows",
                                "org.kde.KWin.PresentWindows",
                                QDBusConnection::sessionBus());
        QStringList strList;
        for (uint w : xids)
            strList << QString::number(w);
        Interface.call("PresentWindows",strList);
    }
}

// TODO(zccrs): 开启/禁用热区
void DeepinWMFaker::EnableZoneDetected(bool enabled)
{
    setZoneEnabled(enabled);
}
#endif

/*!
 * \brief DeepinWMFaker::accelAction
 * \param accelKid The KWin accel name
 * \return
 */
QAction *DeepinWMFaker::accelAction(const QString accelKid) const
{
    if (accelKid.isEmpty()) {
        // qDebug() << "ERROR: obtain action from an empty accel id";
        return nullptr;
    }

    QAction *action = m_accelIdActionMap.value(accelKid, nullptr);
    if (action) {
        return action;
    }

    // pass empty string to the constructor means to not change the Accel Friendly Name
    action = new QAction("");
    action->setObjectName(accelKid);
    action->setProperty("componentName", GlobalAccelComponentName);
    action->setProperty("componentDisplayName", GlobalAccelComponentDisplayName);
    //NOTE: this is from KGlobalAccel
    //
    //a isConfigurationAction shortcut combined with NoAutoloading will
    //make it a foreign shortcut, which triggers a dbus signal sent to
    //kglobalaccel when changed. this gives KWin the chance to listen for
    //the externally shortcut changes and and allow effects to respond.
    action->setProperty("isConfigurationAction", !NotConfigurationAction.contains(accelKid));

    return action;
}

QString DeepinWMFaker::transFromDaemonAccelStr(const QString &accelStr) const
{
    if (accelStr.isEmpty()) {
        return accelStr;
    }

    QString str(accelStr);

    str.remove("<")
            .replace(">", "+")
            .replace("Control", "Ctrl")
            .replace("Super", "Meta");

    for (auto it = SpecialKeyMap.constBegin(); it != SpecialKeyMap.constEnd(); ++it) {
        QString origin(str);
        str.replace(it.key(), it.value());
        if (str != origin) {
            return str;
        }
    }

    for (auto it = SpecialRequireShiftKeyMap.constBegin(); it != SpecialRequireShiftKeyMap.constEnd(); ++it) {
        QString origin(str);
        str.replace(it.key(), it.value());
        if (str != origin) {
            return str.remove("Shift+");
        }
    }

    return str;
}

QString DeepinWMFaker::transToDaemonAccelStr(const QString &accelStr) const
{
    if (accelStr.isEmpty()) {
        return accelStr;
    }

    QString str(accelStr);

    str.replace("Shift+", "<Shift>")
            .replace("Ctrl+", "<Control>")
            .replace("Alt+", "<Alt>")
            .replace("Meta+", "<Super>")
            .replace("Backtab", "Tab");

    for (auto it = SpecialKeyMap.constBegin(); it != SpecialKeyMap.constEnd(); ++it) {
        if (it.value() == str.at(str.length() - 1)) {
            str.chop(1);
            return str.append(it.key());
        }
    }

    for (auto it = SpecialRequireShiftKeyMap.constBegin(); it != SpecialRequireShiftKeyMap.constEnd(); ++it) {
        if (it.value() == str.at(str.length() - 1)) {
            str.chop(1);
            str = str.append(it.key());
            if (!str.contains("<Shift>")) {
                str = str.prepend("<Shift>");
            }
            return str;
        }
    }

    return str;
}

QString DeepinWMFaker::getWorkspaceBackground(const int index) const
{
    return m_deepinWMWorkspaceBackgroundGroup->readEntry(QString::number(index));
}

void DeepinWMFaker::setWorkspaceBackground(const int index, const QString &uri)
{
    m_deepinWMWorkspaceBackgroundGroup->writeEntry(QString::number(index), uri);

    Q_EMIT WorkspaceBackgroundChanged(index, uri);

}

QString DeepinWMFaker::getWorkspaceBackgroundForMonitor(const int index, const QString &strMonitorName) const
{
    return  m_deepinWMConfig->group("WorkspaceBackground").readEntry( QString("%1%2%3").arg(index).arg("@" ,strMonitorName)) ;
}
void DeepinWMFaker::setWorkspaceBackgroundForMonitor(const int index, const QString &strMonitorName, const QString &uri) const
{
    QDBusInterface interface("org.deepin.dde.Appearance1", "/org/deepin/dde/Appearance1", "org.deepin.dde.Appearance1");
    interface.call("SetWorkspaceBackgroundForMonitor", index, strMonitorName, uri);

    m_deepinWMWorkspaceBackgroundGroup->writeEntry(QString("%1%2%3").arg(index).arg("@" ,strMonitorName), uri);
    m_deepinWMConfig->sync();

#ifndef DISABLE_DEEPIN_WM
    QStringList allWallpaper = m_appearance->value(DconfigBackgroundUri).toStringList();

    if (index > allWallpaper.size()) {
        allWallpaper.reserve(index);

        for (int i = allWallpaper.size(); i < index; ++i) {
            allWallpaper.append(QString());
        }
    }

    allWallpaper[index - 1] = uri;
    m_appearance->setValue(DconfigBackgroundUri, allWallpaper);
#endif // DISABLE_DEEPIN_WM
}

bool DeepinWMFaker::isX11Platform()
{
    QString strCmd = "loginctl show-session $(loginctl | grep $(whoami) | awk '{print $1}') -p Type";
    QProcess p;
    p.start("bash", QStringList() <<"-c" << strCmd);
    p.waitForFinished();
    QString result = p.readAllStandardOutput();
    if (result.replace("\n", "").contains("Type=x11")) {
        return  true;
    } else {
        return  false;
    }
}

void DeepinWMFaker::quitTransientBackground()
{
    if (!m_transientBackgroundUri.isEmpty()) {
        m_transientBackgroundUri.clear();

        Q_EMIT WorkspaceBackgroundChanged(GetCurrentWorkspaceInner(), GetCurrentWorkspaceBackground());
    }

#ifndef DISABLE_DEEPIN_WM
    if (!m_deepinWMBackgroundUri.isEmpty()) {
        // 在退出预览时不同步deepin-wm的设置
        QSignalBlocker blocker(m_appearance.data());
        Q_UNUSED(blocker)
        setWorkspaceBackgroundForDeepinWM(GetCurrentWorkspaceInner(), m_deepinWMBackgroundUri);
        m_deepinWMBackgroundUri.clear();
    }
#endif // DISABLE_DEEPIN_WM
}

#ifndef DISABLE_DEEPIN_WM
void DeepinWMFaker::onGsettingsDDEAppearanceChanged(const QString &key)
{
    if (QLatin1String(DconfigBackgroundUri) == key) {
        const QStringList &uris = m_appearance->value(DconfigBackgroundUri).toStringList();

        for (int i = 0; i < uris.count(); ++i) {
            const QString &uri = uris.at(i);

            // 从 deepin-wm 中同步壁纸设置
            if (uri != getWorkspaceBackground(i + 1)) {
                setWorkspaceBackground(i + 1, uri);
            }
        }

        // 更新值
        if (!m_deepinWMBackgroundUri.isEmpty()) {
            m_deepinWMBackgroundUri = uris.value(GetCurrentWorkspaceInner());
        }
    }
}

static void setBorderActivate(KConfigGroup *group, int value, bool remove)
{
    const QString &activate = "BorderActivate";
    QStringList list = group->readEntry(activate).split(",");
    const QString &v = QString::number(value);

    if (remove) {
        list.removeAll(v);
    } else if (!list.contains(v)) {
        list.append(v);
    } else {
        return;
    }

    group->writeEntry(activate, list.join(","));
}

void DeepinWMFaker::syncConfigForKWin()
{
    // 同步配置到文件
    m_kwinConfig->sync();
    // 通知kwin重新加载配置文件
    QDBusInterface(KWinDBusService, KWinDBusPath).call("reconfigure");
}

void DeepinWMFaker::updateCursorConfig()
{
    if (!::updateCursorConfig() && calledFromDBus()) {
        auto error = QDBusConnection::sessionBus().lastError();

        if (error.type() == QDBusError::NoError) {
            error = QDBusError(QDBusError::Failed, "Failed on sync kcminputrc");
        }

        setDelayedReply(true);
        connection().send(QDBusMessage::createError(error));
    }
}
#endif // DISABLE_DEEPIN_WM

bool DeepinWMFaker::GetMultiTaskingStatus()
{
    return m_isMultitaskingActived;
}

void DeepinWMFaker::SetMultiTaskingStatus(bool isActive)
{
    m_isMultitaskingActived = isActive;
}

void DeepinWMFaker::SlotUpdateMultitaskStatus(bool isActive)
{
    m_isMultitaskingActived = isActive;
}

bool DeepinWMFaker::GetIsShowDesktop()
{
    return m_isShowDesktop;
}

void DeepinWMFaker::SetShowDesktop(bool isShowDesktop)
{
    m_isShowDesktop = isShowDesktop;
}
