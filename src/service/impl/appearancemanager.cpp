#include <fstream>
#include <regex>
#include <xcb/randr.h>
#include <xcb/xcb.h>
#include <cstdlib>
#include <QRegExp>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <pwd.h>
#include <QDBusArgument>
#include <QTimer>
#include <QTimeZone>
#include <stdio.h>

#include "appearancemanager.h"
#include "../modules/api/utils.h"
#include "../modules/common/commondefine.h"
#include "../modules/api/keyfile.h"

#include "../modules/api/locale.h"
#include "../modules/api/sunrisesunset.h"
#include "../modules/dconfig/phasewallpaper.h"

const QString wallpaperJsonPath = QString("%1/.cache/deepin/dde-appearance/").arg(getenv("HOME"));

AppearanceManager::AppearanceManager(QObject *parent)
    : QObject(parent)
    , settingDconfig(APPEARANCESCHEMA)
    , wmInterface(new QDBusInterface("com.deepin.wm",
                                     "/com/deepin/wm", "com.deepin.wm",
                                     QDBusConnection::sessionBus()))
    , displayInterface(new QDBusInterface("com.deepin.daemon.Display",
                                          "/com/deepin/daemon/Display",
                                          "com.deepin.daemon.Display",
                                          QDBusConnection::sessionBus()))
    , xSettingsInterface(new QDBusInterface("com.deepin.SessionManager",
                                            "/com/deepin/XSettings",
                                            "com.deepin.XSettings",
                                            QDBusConnection::sessionBus()))
    , timeDateInterface(new QDBusInterface("org.freedesktop.timedate1",
                                           "/org/freedesktop/timedate1",
                                           "org.freedesktop.timedate1",
                                           QDBusConnection::systemBus()))
    , sessionTimeDateInterface(new QDBusInterface("com.deepin.SessionManager",
                                                  "/com/deepin/XSettings",
                                                  "com.deepin.XSettings",
                                                  QDBusConnection::sessionBus()))
    , imageBlurInterface(new QDBusInterface("com.deepin.daemon.Accounts",
                                            "/com/deepin/daemon/ImageBlur",
                                            "com.deepin.daemon.ImageBlur",
                                            QDBusConnection::systemBus()))
    , imageEffectInterface(new QDBusInterface("com.deepin.daemon.ImageEffect",
                                              "/com/deepin/daemon/ImageEffect",
                                              "com.deepin.daemon.ImageEffect",
                                              QDBusConnection::sessionBus()))

    , subthemes(new Subthemes())
    , backgrounds(new Backgrounds())
    , fontsManager(new FontsManager())
    , cursorChangeHandler(new CursorChangeHandler)
    , fsnotify(new Fsnotify())
    , detectSysClockTimer(this)
    , themeAutoTimer(this)
{
    fontSize = 0;
    opaticy = 0;
    windowRadius = 0;
    longitude = 0;
    latitude = 0;
    ntpTimeId = 0;
    timeUpdateTimeId = 0;
    locationValid = false;
    nid = 0;

    if(QGSettings::isSchemaInstalled(XSETTINGSSCHEMA))\
    {
        xSetting = QSharedPointer<QGSettings>(new QGSettings(XSETTINGSSCHEMA));
    }

    if(QGSettings::isSchemaInstalled(WRAPBGSCHEMA))\
    {
        wrapBgSetting = QSharedPointer<QGSettings>(new QGSettings(WRAPBGSCHEMA));
    }

    if(QGSettings::isSchemaInstalled(XSETTINGSSCHEMA))\
    {
        gnomeBgSetting = QSharedPointer<QGSettings>(new QGSettings(GNOMEBGSCHEMA));
    }

    if(wrapBgSetting)
    {
        background = wrapBgSetting->get(GSKEYBACKGROUND).toString();
    }

    if (settingDconfig.isValid()) {
        gtkTheme        = settingDconfig.value(GSKEYGTKTHEME).toString();
        iconTheme       = settingDconfig.value(GSKEYICONTHEM).toString();
        cursorTheme     = settingDconfig.value(GSKEYCURSORTHEME).toString();
        standardFont    = settingDconfig.value(GSKEYFONTSTANDARD).toString();
        monospaceFont   = settingDconfig.value(GSKEYFONTMONOSPACE).toString();
        fontSize        = settingDconfig.value(GSKEYFONTSIZE).toDouble();
        opaticy         = settingDconfig.value(GSKEYOPACITY).toDouble();
        wallpaperSlideShow = settingDconfig.value(GSKEYWALLPAPERSLIDESHOW).toString();
        wallpaperURls   = settingDconfig.value(GSKEYWALLPAPERURIS).toString();
    }

    if(xSetting)
    {
        windowRadius  = xSetting->get(GSKEYDTKWINDOWRADIUS).toInt();
        qtActiveColor = xSetting->get(GSKEYQTACTIVECOLOR).toString();
    }

    qtActiveColor = qtActiveColorToHexColor();

    init();
}

AppearanceManager::~AppearanceManager()
{

}

bool AppearanceManager::init()
{
    qInfo()<<"init";
    initUserObj();
    initCurrentBgs();

    xcb_connection_t *conn = xcb_connect(nullptr, nullptr);
    xcb_randr_query_version_reply_t* v = xcb_randr_query_version_reply(conn, xcb_randr_query_version(conn, 1, 1), nullptr);
    if (v == nullptr) {
        qWarning()<<"xcb_randr_query_version_reply faile";
        return false;
    }

    bool bSuccess = QDBusConnection::sessionBus().connect(wmInterface->service(),
                                                          wmInterface->path(), wmInterface->interface(),
                                                          SIGNAL(WorkspaceCountChanged(int)),
                                                          this, SLOT(handleWmWorkspaceCountChanged(int)));
    if (!bSuccess) {

    }

    bSuccess = QDBusConnection::sessionBus().connect(wmInterface->service(),
                                                     wmInterface->path(), wmInterface->interface(),
                                                     SIGNAL(WorkspaceSwithched(int, int)),
                                                     this, SLOT(handleWmWorkspaceSwithched(int)));
    if (!bSuccess) {

    }

    bSuccess = QDBusConnection::sessionBus().connect(xSettingsInterface->service(),
                                                     xSettingsInterface->path(),
                                                     xSettingsInterface->interface(),
                                                     SIGNAL(SetScaleFactorStarted(int, int)),
                                                     this, SLOT(handleSetScaleFactorStarted(int)));
    if (!bSuccess) {

    }

    bSuccess = QDBusConnection::sessionBus().connect(xSettingsInterface->service(),
                                                     xSettingsInterface->path(),
                                                     xSettingsInterface->interface(),
                                                     SIGNAL(SetScaleFactorDone(int, int)),
                                                     this, SLOT(handleSetScaleFactorDone(int)));
    if (!bSuccess) {

    }

    QDBusConnection::sessionBus().connect("com.deepin.daemon.Display",
                                          "/com/deepin/daemon/Display",
                                          "org.freedesktop.DBus.Properties",
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this, SLOT(handleDisplayChanged(QDBusMessage)));
    updateMonitorMap();

    //todo NewConfig

    if (wallpaperURls.isEmpty()) {
        updateNewVersionData();
    }

    zone = timeDateInterface->property("Timezone").toString();

    // to l, err := time.LoadLocation(zone)

    QDBusConnection::sessionBus().connect(timeDateInterface->service(),
                                          timeDateInterface->path(),
                                          "org.freedesktop.DBus.Properties",
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this, SLOT(handleTimezoneChanged(QDBusMessage)));

    loadDefaultFontConfig();

    bSuccess = QDBusConnection::sessionBus().connect(sessionTimeDateInterface->service(),
                                                     sessionTimeDateInterface->path(),
                                                     sessionTimeDateInterface->interface(),
                                                     SIGNAL(TimeUpdate()),
                                                     this, SLOT(handleTimeUpdate()));
    if (!bSuccess) {

    }

    QDBusConnection::sessionBus().connect(timeDateInterface->service(),
                                          timeDateInterface->path(),
                                          "org.freedesktop.DBus.Properties",
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this, SLOT(handleNTPChanged(QDBusMessage)));

    QVector<QSharedPointer<Theme>> iconList = subthemes->listIconThemes();
    bool bFound = false;

    for (auto theme : iconList) {
        if (theme->getId() == iconTheme) {
            bFound = true;
            break;
        }
    }
    if (!bFound) {
        setIconTheme(DEFAULTICONTHEME);
        doSetIconTheme(DEFAULTICONTHEME);
    }

    QVector<QSharedPointer<Theme>> cursorList = subthemes->listCursorThemes();
    bFound = false;
    for (auto theme : cursorList) {
        if (theme->getId() == cursorTheme) {
            bFound = true;
            break;
        }
    }
    if (!bFound) {
        setCursorTheme(DEFAULTCURSORTHEME);
        doSetCursorTheme(DEFAULTCURSORTHEME);
    }

    cursorChangeHandler->start();

    connect(fsnotify.data(), SIGNAL(themeFileChange(QString)), this, SLOT(handlethemeFileChange(QString)));

    connect(xSetting.data(), SIGNAL(changed(const QString&)), this, SLOT(handleXsettingDConfigChange(QString)));

    connect(&settingDconfig, SIGNAL(valueChanged(const QString&)), this, SLOT(handleSettingDConfigChange(QString)));

    connect(wrapBgSetting.data(), SIGNAL(changed(const QString)), this, SLOT(handleWrapBgDConfigChange(QString)));

    connect(gnomeBgSetting.data(), SIGNAL(changed(const QString)), this, SLOT(handleGnomeBgDConfigChange(QString)));

    connect(&detectSysClockTimer, SIGNAL(timeout()), this, SLOT(handleDetectSysClockTimeOut()));

    return true;
}

void AppearanceManager::deleteThermByType(const QString &ty, const QString &name)
{
    if (ty.compare(TYPEGTK) == 0) {
        subthemes->deleteGtkTheme(name);
    } else if (ty.compare(TYPEICON) == 0) {
        subthemes->deleteIconTheme(name);
    } else if (ty.compare(TYPECURSOR) == 0) {
        subthemes->deleteCursorTheme(name);
    } else if (ty.compare(TYPEBACKGROUND) == 0) {
        backgrounds->deleteBackground(name);
    } else {
        //todo log
    }
}
void AppearanceManager::handleWmWorkspaceCountChanged(int count)
{
    QStringList bgs = settingDconfig.value(GSKEYBACKGROUNDURIS).toStringList();

    if (bgs.size() < count) {
        QVector<Background> allBgs = backgrounds->listBackground();

        int addCount = count - bgs.count();
        for (int i = 0; i < addCount; i++) {
            int index = rand() % allBgs.count();

            bgs.push_back(allBgs[index].getId());
        }

        settingDconfig.setValue(GSKEYBACKGROUNDURIS, bgs);
    } else if (bgs.size() > count) {
        bgs = bgs.mid(0, count);
        settingDconfig.setValue(GSKEYBACKGROUNDURIS, bgs);
    }

    doUpdateWallpaperURIs();
}

void AppearanceManager::handleWmWorkspaceSwithched(int from, int to)
{
    if (userInterface) {
        userInterface->call("SetCurrentWorkspace", to);
    }
}

void AppearanceManager::handleSetScaleFactorStarted()
{
    QString body = tr("Setting display scaling");
    QString summary = tr("Display scaling");

    QDBusInterface notificationsInterface("org.freedesktop.Notifications",
                                          "/org/freedesktop/Notifications",
                                          "com.deepin.dde.Notification",
                                          QDBusConnection::sessionBus());

    QDBusMessage message = notificationsInterface.call("Notify", "dde-control-center", nid,
                                                       "dialog-window-scale",
                                                       summary, body, "", "", 0);

    if (message.type() == QDBusMessage::ErrorMessage) {
        return;
    }

    nid = message.arguments().first().toUInt();
}

void AppearanceManager::handleSetScaleFactorDone()
{
    QString body = tr("Setting display scaling");
    QString summary = tr("Display scaling");
    QStringList options{"_logout", tr("Log Out Now"), "_later", tr("Later")};
    QMap<QString, QVariant> optionMap;
    optionMap["x-deepin-action-_logout"] = "dbus-send,--type=method_call,--dest=com.deepin.SessionManager,"
                                           "/com/deepin/SessionManager,com.deepin.SessionManager.RequestLogout";
    optionMap["x-deepin-action-_later"] = "";
    int expireTimeout = 15 * 1000;

    QDBusInterface notificationsInterface("org.freedesktop.Notifications",
                                          "/org/freedesktop/Notifications",
                                          "com.deepin.dde.Notification",
                                          QDBusConnection::sessionBus());

    QDBusMessage message = notificationsInterface.call("Notify", "dde-control-center", nid,
                                                       "dialog-window-scale",
                                                       summary, body, options,
                                                       optionMap, expireTimeout);

    if (message.type() == QDBusMessage::ErrorMessage) {
        return;
    }

    nid = message.arguments().first().toUInt();
}

void AppearanceManager::handleDisplayChanged(QDBusMessage mes)
{
    QList<QVariant> arguments = mes.arguments();
    if (3 != arguments.count()) {
        return;
    }
    QString interfaceName = mes.arguments().at(0).toString();
    if (interfaceName == "com.deepin.daemon.Display") {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            if (keys.at(i) == "Monitors" || keys.at(i) == "Primary") {
                updateMonitorMap();
            }
        }
    }
}

void AppearanceManager::handleTimezoneChanged(QDBusMessage mes)
{
    QList<QVariant> arguments = mes.arguments();
    if (3 != arguments.count()) {
        return;
    }
    QString interfaceName = mes.arguments().at(0).toString();
    if (interfaceName == timeDateInterface->interface()) {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            if (keys.at(i) == "Timezone") {
                QString value = changedProps.value(keys.at(i)).toString();
                if (coordinateMap.count(value) == 1) {
                    latitude = coordinateMap[value].latitude;
                    longitude = coordinateMap[value].longitude;
                }
                zone = value;
                // todo l, err := time.LoadLocation(zone)

                if (gtkTheme == AUTOGTKTHEME) {
                    autoSetTheme(latitude, longitude);
                    resetThemeAutoTimer();
                }
            }
        }
    }
}

void AppearanceManager::handleTimeNTPChanged(QDBusMessage mes)
{
    QList<QVariant> arguments = mes.arguments();
    if (3 != arguments.count()) {
        return;
    }
    QString interfaceName = mes.arguments().at(0).toString();
    if (interfaceName == timeDateInterface->interface()) {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            if (keys.at(i) == "NTP") {
                handleTimeUpdate();
            }
        }
    }
}

void AppearanceManager::handleTimeUpdate()
{
    timeUpdateTimeId = this->startTimer(2000);
}

void AppearanceManager::handleNTPChanged()
{
    ntpTimeId = this->startTimer(2000);
}

void AppearanceManager::handlethemeFileChange(QString theme)
{
    if (theme == TYPEBACKGROUND) {
        backgrounds->notifyChanged();
        for (auto iter : wsLoopMap) {
            iter->notifyFileChange();
        }
    } else if (theme == TYPEGTK) {
        // todo <-time.After(time.Millisecond * 700)
        subthemes->refreshGtkThemes();
        Q_EMIT Refreshed(TYPEGTK);
    } else if (theme == TYPEICON) {
        // todo <-time.After(time.Millisecond * 700)
        subthemes->refreshIconThemes();
        subthemes->refreshCursorThemes();
        Q_EMIT Refreshed(TYPEICON);
        Q_EMIT Refreshed(TYPECURSOR);
    }
}

void AppearanceManager::handleXsettingDConfigChange(QString key)
{
    if (key != GSKEYQTACTIVECOLOR) {
        return;
    }

    QString value = qtActiveColorToHexColor();

    if (qtActiveColor != value) {
        qtActiveColor = value;
        Q_EMIT Changed("QtActiveColor", value);
    }
}

void AppearanceManager::handleSettingDConfigChange(QString key)
{
    QString type;
    QString value;
    bool bSuccess = false;
    if (key == GSKEYGTKTHEME) {
        type = TYPEGTK;
        value = settingDconfig.value(key).toString();
        bSuccess = doSetGtkTheme(value);
        updateThemeAuto(value == AUTOGTKTHEME);
    } else if (key == GSKEYICONTHEM) {
        type = TYPEICON;
        value = settingDconfig.value(key).toString();
        bSuccess = doSetIconTheme(value);
    } else if (key == GSKEYCURSORTHEME) {
        type = TYPECURSOR;
        value = settingDconfig.value(key).toString();
        bSuccess = doSetCursorTheme(value);
    } else if (key == GSKEYFONTSTANDARD) {
        type = TYPESTANDARDFONT;
        value = settingDconfig.value(key).toString();
        bSuccess = doSetStandardFont(value);
    } else if (key == GSKEYFONTMONOSPACE) {
        type = TYPEMONOSPACEFONT;
        value = settingDconfig.value(key).toString();
        bSuccess = doSetMonospaceFont(value);
    } else if (key == GSKEYFONTSIZE) {
        type = TYPEFONTSIZE;
        double size = settingDconfig.value(key).toDouble();
        bSuccess = doSetFonts(size);
        value = QString::number(size);
    } else if (key == GSKEYBACKGROUNDURIS) {
        type = TYPEBACKGROUND;
        desktopBgs = settingDconfig.value(key).toStringList();
        userInterface->call("SetDesktopBackgrounds", desktopBgs);
        value = desktopBgs.join(";");
    } else if (key == GSKEYWALLPAPERSLIDESHOW) {
        QString policy = settingDconfig.value(key).toString();
        updateWSPolicy(policy);
    } else if (key == DCKEYALLWALLPAPER) {
        QVariant value = settingDconfig.value(key);
        QDir dir;
        if (!dir.exists(wallpaperJsonPath)) {
            bool res = dir.mkpath(wallpaperJsonPath);
            if (!res) {
                qWarning() << QString("mkdir %1 failed.").arg(wallpaperJsonPath);
                return;
            }
        }

        QFile file(wallpaperJsonPath + "config.json");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream textStream(&file);
            textStream << QString(QJsonDocument::fromVariant(value).toJson());
            textStream.flush();
            file.close();
        } else {
            qWarning() <<QString("%1 error.").arg(wallpaperJsonPath);
        }
    } else {
        return;
    }

    if (!bSuccess) {
        qDebug() << "set " << key << "fail";
    }

    if (!type.isEmpty()) {
        Q_EMIT Changed("QtActiveColor", value);
    }

}

void AppearanceManager::handleWrapBgDConfigChange(QString key)
{
    if (key != GSKEYBACKGROUND) {
        return;
    }

    QString value = wrapBgSetting->get(key).toString();
    bool bSuccess = doSetBackground(value);
    if (!bSuccess) {
        return;
    }

    if (wsLoopMap.count(curMonitorSpace) != 0) {
        wsLoopMap[curMonitorSpace]->addToShow(value);
    }
}

void AppearanceManager::handleGnomeBgDConfigChange(QString key)
{
    if (key != GSKEYBACKGROUND) {
        return;
    }

    QString value = gnomeBgSetting->get(key).toString();
    bool bSuccess = doSetBackground(value);
    if (!bSuccess) {
        return;
    }

    if (wsLoopMap.count(curMonitorSpace) != 0) {
        wsLoopMap[curMonitorSpace]->addToShow(value);
    }
}

void AppearanceManager::handleDetectSysClockTimeOut()
{

    qint64 now = QDateTime::currentSecsSinceEpoch();
    qint64 diff = now - detectSysClockStartTime - 60;
    if (diff > -2 && diff < 2) {
        if (locationValid) {
            autoSetTheme(latitude, longitude);
            resetThemeAutoTimer();
        }
        detectSysClockStartTime = QDateTime::currentSecsSinceEpoch();
        detectSysClockTimer.start(60 * 1000);
    }
}

void AppearanceManager::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timeUpdateTimeId || event->timerId() == ntpTimeId) {
        if (locationValid) {
            autoSetTheme(latitude, longitude);
            resetThemeAutoTimer();
        }

        killTimer(event->timerId());
    }
}
// 设置gsetting

void AppearanceManager::setFontSize(double value)
{
    if (settingDconfig.isValid() && !qFuzzyCompare(value, fontSize)) {
        settingDconfig.setValue(GSKEYFONTSIZE, value);
        fontSize = value;
    }
}

void AppearanceManager::setGtkTheme(QString value)
{
    if (settingDconfig.isValid() && value != gtkTheme) {
        settingDconfig.setValue(GSKEYGTKTHEME, value);
        gtkTheme = value;
    }
}

void AppearanceManager::setIconTheme(QString value)
{
    if (settingDconfig.isValid() && value != iconTheme) {
        settingDconfig.setValue(GSKEYICONTHEM, value);
        iconTheme = value;
    }
}

void AppearanceManager::setCursorTheme(QString value)
{
    if (settingDconfig.isValid() && value != cursorTheme) {
        settingDconfig.setValue(GSKEYCURSORTHEME, value);
        cursorTheme = value;
    }
}

void AppearanceManager::setStandarFont(QString value)
{
    if (settingDconfig.isValid() && value != standardFont) {
        settingDconfig.setValue(GSKEYFONTSTANDARD, value);
        standardFont = value;
    }
}

void AppearanceManager::setMonospaceFont(QString value)
{
    if (settingDconfig.isValid() && value != monospaceFont) {
        settingDconfig.setValue(GSKEYFONTMONOSPACE, value);
        monospaceFont = value;
    }
}

void AppearanceManager::setWindowRadius(int value)
{
    if (value != windowRadius && xSetting) {
        xSetting->set(GSKEYWALLPAPERURIS, value);
        windowRadius = value;
    }
}

void AppearanceManager::setOpaticy(double value)
{
    if (settingDconfig.isValid() && !qFuzzyCompare(value, opaticy)) {
        settingDconfig.setValue(GSKEYOPACITY, value);
        opaticy = value;
    }
}

void AppearanceManager::setQtActiveColor(const QString &value)
{
    if ( value != qtActiveColor && xSetting) {
        xSetting->set(GSKEYQTACTIVECOLOR, value);
        qtActiveColor = value;
    }
}

bool AppearanceManager::setWallpaperSlideShow(const QString &value)
{
    if (value == wallpaperSlideShow) {
        return true;
    }
    if (!settingDconfig.isValid()) {
        return false;
    }
    qInfo()<<"value: "<<value;
    qInfo()<<"value: GSKEYWALLPAPERSLIDESHOW"<<settingDconfig.value(GSKEYWALLPAPERSLIDESHOW);
    settingDconfig.setValue(GSKEYWALLPAPERSLIDESHOW, value);
    wallpaperSlideShow = value;

    return true;
}

bool AppearanceManager::setWallpaperURls(const QString &value)
{
    if (value == wallpaperURls) {
        return true;
    }
    if (!settingDconfig.isValid()) {
        return false;
    }

    settingDconfig.setValue(GSKEYWALLPAPERURIS, value);
    wallpaperURls = value;

    return true;
}
QString AppearanceManager::qtActiveColorToHexColor()
{
    QStringList fields = qtActiveColor.split(",");
    if (fields.size() != 4)
        return "";

    uint16_t array[4];
    for (int i = 0; i < 4; i++) {
        QString field = fields[i];
        array[i] = uint16_t(field.toInt());
    }

    uint8_t byteArr[4];
    for (int i = 0; i < 4; i++) {
        byteArr[i] = uint8_t(float(array[i]) / float(UINT16_MAX) * float(UINT8_MAX));
    }
    char buf[512];
    if (byteArr[3] == 255) {
        sprintf(buf, "#%02X%02X%02X", byteArr[0], byteArr[1], byteArr[2]);
    } else {
        sprintf(buf, "#%02X%02X%02X%02X", byteArr[0], byteArr[1], byteArr[2], byteArr[3]);
    }

    return buf;
}

void AppearanceManager::initCoordinate()
{
    QString context;
    QFile file(ZONEPATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QRegExp pattern{"^#`"};

    while (!file.atEnd()) {
        QString line = file.readLine();
        if (line.length() == 0) {
            continue;
        }

        if (pattern.exactMatch(line)) {
            continue;
        }

        QStringList strv = line.split("\t");
        if (strv.size() < 3) {
            continue;
        }

        iso6709Parsing(strv[2], strv[1]);
    }
}

void AppearanceManager::initUserObj()
{
    qInfo()<<"initUserObj";
    struct passwd *pw = getpwuid(getuid());
    if (pw == nullptr) {
        return;
    }

    QDBusInterface accountInter("com.deepin.daemon.Accounts",
                                "/com/deepin/daemon/Accounts",
                                "com.deepin.daemon.Accounts",
                                QDBusConnection::systemBus());

    if (!accountInter.isValid()) {
        return;
    }

    QDBusMessage    message = accountInter.call("FindUserById", QString::number(pw->pw_uid));
    if (message.type() == QDBusMessage::ErrorMessage) {
        qWarning()<<message.errorMessage();
        return;
    }
    QString userPath = message.arguments().first().toString();

    userInterface = QSharedPointer<QDBusInterface>(new QDBusInterface("com.deepin.daemon.Accounts",
                                                                      userPath,
                                                                      "com.deepin.daemon.Accounts.User",
                                                                      QDBusConnection::systemBus()));

    if (!userInterface->isValid()) {
        return;
    }

    QStringList userBackgrounds = userInterface->property("DesktopBackgrounds").toStringList();

    QStringList gsBackgrounds = settingDconfig.value(GSKEYBACKGROUNDURIS).toStringList();
    for (auto iter : gsBackgrounds) {
        if (userBackgrounds.indexOf(iter) == -1) {
            userInterface->call("SetDesktopBackgrounds", gsBackgrounds);
            break;
        }
    }
}

void AppearanceManager::initCurrentBgs()
{
    qInfo()<<"initCurrentBgs";
    desktopBgs = settingDconfig.value(GSKEYBACKGROUNDURIS).toStringList();

    greeterBg = userInterface->property("GreeterBackground").toString();
}

void AppearanceManager::initWallpaperSlideshow()
{
    loadWSConfig();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(wallpaperSlideShow.toLatin1(), &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "parse wallpaperSlideShow: " << wallpaperSlideShow << ",fail";
        return;
    }

    QVariantMap tempMap = doc.object().toVariantMap();
    for (auto iter : tempMap.toStdMap()) {
        if (wsSchedulerMap.count(iter.first) != 1) {
            QSharedPointer<WallpaperScheduler> wallpaperScheduler(new WallpaperScheduler(std::bind(
                                                                                             &AppearanceManager::autoChangeBg, this, std::placeholders::_1, std::placeholders::_2)));
            wsSchedulerMap[iter.first] = wallpaperScheduler;
        }

        if (wsLoopMap.count(iter.second.toString()) != 1) {
            wsLoopMap[iter.first] = QSharedPointer<WallpaperLoop>(new WallpaperLoop());
        }

        if (WallpaperLoopConfigManger::isValidWSPolicy(iter.second.toString())) {
            if (iter.second.toString() == WSPOLICYLOGIN) {
                bool bSuccess = changeBgAfterLogin(iter.first);
                if(!bSuccess)
                {
                    qWarning()<<"failed to change background after login";
                }
            } else {
                uint sec = iter.second.toString().toUInt();
                if (wsSchedulerMap.count(iter.first) == 1) {
                    wsSchedulerMap[iter.first]->setInterval(iter.first, sec);
                }
            }
        }
    }

}

void AppearanceManager::updateMonitorMap()
{
    QString primary = displayInterface->property("Primary").toString();

    QDBusMessage    message = displayInterface->call("ListOutputNames");
    if (message.type() == QDBusMessage::ErrorMessage) {
        return;
    }
    QStringList monitorList = message.arguments().first().toStringList();

    for (int i = 0; i < monitorList.size(); i++) {
        if (monitorList[i] == primary) {
            monitorMap[monitorList[i]] = "Primary";
        } else {
            monitorMap[monitorList[i]] = "Subsidiary" + QString::number(i);
        }
    }
}

void AppearanceManager::iso6709Parsing(QString city, QString coordinates)
{
    QRegExp pattern("(\+|-)\d+\.?\d*");

    QVector<QString> resultVet;

    int pos = 0;
    while ((pos = pattern.indexIn(coordinates, pos)) != -1 && resultVet.size() <= 2) {
        resultVet.push_back(coordinates.mid(pos, pattern.matchedLength()));
        pos += pattern.matchedLength();
    }

    if (resultVet.size() < 2) {
        return;
    }

    resultVet[0] = resultVet[0].mid(0, 3) + "." + resultVet[0].mid(3, resultVet[0].size());
    resultVet[1] = resultVet[1].mid(0, 4) + "." + resultVet[1].mid(4, resultVet[1].size());

    coordinate cdn;

    cdn.latitude  = resultVet[0].toDouble();
    cdn.longitude = resultVet[1].toDouble();

    coordinateMap[city] = cdn;
}

void AppearanceManager::doUpdateWallpaperURIs()
{
    QMap<QString, QString> monitorWallpaperUris;

    QDBusMessage message = wmInterface->call("WorkspaceCount");
    int  workspaceCount = message.arguments().first().toInt();

    message = wmInterface->call("ListOutputNames");
    QStringList  monitorList = message.arguments().first().toStringList();

    for (int i = 0; i < monitorList.length(); i++) {
        for (int idx = 1; idx <= workspaceCount; idx++) {
            message = wmInterface->call("GetWorkspaceBackgroundForMonitor", idx, monitorList[i]);
            if (message.type() == QDBusMessage::ErrorMessage) {
                continue;
            }

            QString wallpaperUri = message.arguments().first().toString();

            QString key;
            if (monitorMap.count(monitorList[i]) != 0) {
                key.sprintf("%s##%d", monitorMap[monitorList[i]].toLatin1().data(), idx);
            } else {
                key.sprintf("%s##%d", "", idx);
            }

            monitorWallpaperUris[key] = wallpaperUri;
        }
    }

    setPropertyWallpaperURIs(monitorWallpaperUris);
}

void AppearanceManager::setPropertyWallpaperURIs(QMap<QString, QString> monitorWallpaperUris)
{
    QJsonDocument doc;
    QJsonObject monitorObj;
    for (auto iter : monitorWallpaperUris.toStdMap()) {
        monitorObj.insert(iter.first, iter.second);
    }

    doc.setObject(monitorObj);
    QString wallPaperUriVal = doc.toJson(QJsonDocument::Compact);

    settingDconfig.setValue(GSKEYWALLPAPERURIS, wallPaperUriVal);
}

void AppearanceManager::updateNewVersionData()
{
    QString primaryMonitor;
    for (auto item : monitorMap.toStdMap()) {
        if (item.second == "Primary") {
            primaryMonitor = item.first;
        }
    }
    int workspaceCount = wmInterface->call("WorkspaceCount").arguments().first().toInt();
    QJsonDocument doc = QJsonDocument::fromJson(wallpaperSlideShow.toLatin1());
    QJsonObject wallPaperSlideObj;
    if (!doc.isEmpty()) {
        for (int i = 1 ; i <= workspaceCount; i++) {
            QString key = QString("%1##%2").arg(primaryMonitor).arg(i);
            wallPaperSlideObj.insert(key, wallpaperSlideShow);
        }

        QJsonDocument tempDoc(wallPaperSlideObj);

        if (!setWallpaperSlideShow(tempDoc.toJson(QJsonDocument::Compact))) {
            return;
        }
    }

    QJsonObject wallpaperURIsObj;
    for (auto item : monitorMap.toStdMap()) {
        for (int i = 1; i <= workspaceCount; i++) {
            QDBusMessage message = wmInterface->call("GetWorkspaceBackgroundForMonitor", i, item.first);
            if (message.type() == QDBusMessage::ErrorMessage) {
                continue;
            }
            QString key = QString("%1##%2").arg(item.second).arg(i);
            wallpaperURIsObj.insert(key, message.arguments().first().toString());
        }
    }

    QJsonDocument tempDoc(wallpaperURIsObj);
    setWallpaperURls(tempDoc.toJson(QJsonDocument::Compact));
}

void AppearanceManager::autoSetTheme(double latitude, double longitude)
{
    QDateTime curr = QDateTime::currentDateTimeUtc();
    curr.setTimeZone(QTimeZone(zone.toLatin1()));

    double utcOffset = curr.toSecsSinceEpoch() / 3600.0;

    QDateTime sunrise, sunset;
    bool bSuccess = SunriseSunset::getSunriseSunset(latitude, longitude, utcOffset, curr.toUTC(), sunrise, sunset);
    if (!bSuccess) {
        return;
    }

    QString themeName;
    curr = QDateTime::currentDateTimeUtc();
    if (sunrise.secsTo(curr) >= 0 && curr.secsTo(sunset) >= 0) {
        themeName = "deepin";
    } else {
        themeName = "deepin-dark";
    }

    if (gtkTheme != themeName) {
        doSetGtkTheme(themeName);
    }
}

void AppearanceManager::resetThemeAutoTimer()
{
    if (!locationValid) {
        qDebug() << "location is invalid";
        return;
    }

    QDateTime curr = QDateTime::currentDateTimeUtc();
    curr.setTimeZone(QTimeZone(zone.toLatin1()));
    QDateTime changeTime = getThemeAutoChangeTime(curr, latitude, longitude);

    qint64 interval = curr.secsTo(changeTime);
    themeAutoTimer.start(static_cast<int>(interval));
    qDebug() << "change theme after:" << interval;
}

void AppearanceManager::loadDefaultFontConfig()
{
    QFile file(DEFAULTFONTCONFIGFILE);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "json error!" << jsonError.errorString();
        return;
    }

    QVariantMap rootMap = doc.object().toVariantMap();
    for (auto item : rootMap.toStdMap()) {
        AppearanceManager::fontConfigItem fontConfig;
        QVariantMap itemMap = item.second.toJsonObject().toVariantMap();
        if (!itemMap.value("Standard").toString().isEmpty()) {
            fontConfig.Standard = itemMap.value("Standard").toString();
        }

        if (!itemMap.value("Mono").toString().isEmpty()) {
            fontConfig.Monospace = itemMap.value("Mono").toString();
        }

        defaultFontConfigMap[item.first] = fontConfig;
    }
}

void AppearanceManager::getDefaultFonts(QString &standard, QString &monospace)
{
    if (defaultFontConfigMap.empty()) {
        standard = DEFAULTSTANDARDFONT;
        monospace = DEFAULTMONOSPACEFONT;
        return;
    }

    QVector<QString> languages = Locale::instance()->getLanguageNames();
    for (auto lang : languages) {
        if (defaultFontConfigMap.count(lang) != 0) {
            standard = defaultFontConfigMap[lang].Standard;
            monospace = defaultFontConfigMap[lang].Monospace;
            return;
        }
    }

    if (defaultFontConfigMap.count("en_US") == 1) {
        standard = defaultFontConfigMap["en_US"].Standard;
        monospace = defaultFontConfigMap["en_US"].Monospace;
    }
    return;
}

void AppearanceManager::updateThemeAuto(bool enable)
{
    enableDetectSysClock(enable);

    if (enable) {
        themeAutoTimer.start(0);

        QString city = timeDateInterface->property("Timezone").toString();
        if (coordinateMap.count(city) == 1) {
            latitude = coordinateMap[city].latitude;
            longitude = coordinateMap[city].longitude;
        }
        locationValid = true;
        autoSetTheme(latitude, longitude);
        resetThemeAutoTimer();
    }
}

void AppearanceManager::enableDetectSysClock(bool enable)
{
    if (enable) {
        detectSysClockTimer.start(60 * 1000);
    } else {
        detectSysClockTimer.stop();
    }
}

void AppearanceManager::updateWSPolicy(QString policy)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(policy.toLatin1(), &error);
    if (error.error != QJsonParseError::NoError) {
        return;
    }
    loadWSConfig();

    QVariantMap config = doc.object().toVariantMap();
    for (auto iter : config.toStdMap()) {
        if (wsSchedulerMap.count(iter.first) == 0) {
            QSharedPointer<WallpaperScheduler> wallpaperScheduler(new WallpaperScheduler(std::bind(
                                                                                             &AppearanceManager::autoChangeBg, this, std::placeholders::_1, std::placeholders::_2)));
            wsSchedulerMap[iter.first] = wallpaperScheduler;
        }

        if (wsLoopMap.count(iter.first) == 0) {
            wsLoopMap[iter.first] = QSharedPointer<WallpaperLoop>(new WallpaperLoop());
        }

        if (curMonitorSpace == iter.first && WallpaperLoopConfigManger::isValidWSPolicy(policy)) {
            bool bOk;
            int nSec = policy.toInt(&bOk);
            if (bOk) {
                wsSchedulerMap[iter.first]->setLastChangeTime(QDateTime::currentDateTime());
                wsSchedulerMap[iter.first]->setInterval(iter.first, nSec);
                saveWSConfig(iter.first, QDateTime::currentDateTime());
            } else {
                wsSchedulerMap[iter.first]->stop();
            }
        }
    }
}

void AppearanceManager::loadWSConfig()
{
    WallpaperLoopConfigManger wallConfig;
    QString fileName = utils::GetUserConfigDir() + "/deepin/dde-daemon/appearance/wallpaper-slideshow.json";
    WallpaperLoopConfigManger::WallpaperLoopConfigMap  cfg = wallConfig.loadWSConfig(fileName);

    for (auto monitorSpace : cfg.keys()) {
        if (wsSchedulerMap.count(monitorSpace) == 0) {
            QSharedPointer<WallpaperScheduler> wallpaperScheduler(new WallpaperScheduler(std::bind(&AppearanceManager::autoChangeBg, this, std::placeholders::_1, std::placeholders::_2)));
            wsSchedulerMap[monitorSpace] = wallpaperScheduler;
        }

        wsSchedulerMap[monitorSpace] ->setLastChangeTime(cfg[monitorSpace].lastChange);

        if (wsLoopMap.count(monitorSpace) == 0) {
            wsLoopMap[monitorSpace] = QSharedPointer<WallpaperLoop>(new WallpaperLoop());
        }

        for (auto file : cfg[monitorSpace].showedList) {
            wsLoopMap[monitorSpace]->addToShow(file);
        }
    }

}

QDateTime AppearanceManager::getThemeAutoChangeTime(QDateTime date, double latitude, double longitude)
{
    QDateTime curr = QDateTime::currentDateTimeUtc();
    curr.setTimeZone(QTimeZone(zone.toLatin1()));

    double utcOffset = curr.toSecsSinceEpoch() / 3600.0;

    QDateTime sunrise, sunset;
    bool bSuccess = SunriseSunset::getSunriseSunset(latitude, longitude, utcOffset, curr.toUTC(), sunrise, sunset);
    if (!bSuccess) {
        return QDateTime();
    }
    if (curr.secsTo(sunrise) <= 0) {
        return sunrise;
    }

    if (curr.secsTo(sunset) <= 0) {
        return sunset;
    }

    curr = curr.addDays(1);

    bSuccess = SunriseSunset::getSunriseSunset(latitude, longitude, utcOffset, curr.toUTC(), sunrise, sunset);
    if (!bSuccess) {
        return QDateTime();
    }

    return sunrise;
}

bool AppearanceManager::doSetFonts(double size)
{
    if (!fontsManager->isFontSizeValid(size)) {
        return false;
    }

    bool bSuccess = fontsManager->setFamily(standardFont, monospaceFont, size);
    if (!bSuccess) {
        return false;
    }

    QDBusMessage message = xSettingsInterface->call("SetString", "Qt/FontPointSize", QString::number(size));
    if (message.type() == QDBusMessage::ErrorMessage) {
        return false;
    }

    return setDQtTheme({QTKEYFONTSIZE}, {QString::number(size)});
}

bool AppearanceManager::doSetGtkTheme(QString value)
{
    if (value == AUTOGTKTHEME) {
        return true;
    }

    if (!subthemes->isGtkTheme(value)) {
        return false;
    }
    QString ddeKWinTheme;
    if (value == DEEPIN) {
        ddeKWinTheme = "light";
    } else if (value == DEEPINDARK) {
        ddeKWinTheme = "dark";
    }

    if (!ddeKWinTheme.isEmpty()) {
        wmInterface->call("SetDecorationDeepinTheme", ddeKWinTheme);
    }
    return subthemes->setGtkTheme(value);
}

bool AppearanceManager::doSetIconTheme(QString value)
{
    if (!subthemes->isIconTheme(value)) {
        return false;
    }

    if (!subthemes->setIconTheme(value)) {
        return false;
    }

    return setDQtTheme({QTKEYICON}, {value});
}

bool AppearanceManager::doSetCursorTheme(QString value)
{
    if (!subthemes->isCursorTheme(value)) {
        return false;
    }

    return  subthemes->setCursorTheme(value);
}

bool AppearanceManager::doSetStandardFont(QString value)
{
    if (!fontsManager->isFontFamily(value)) {
        return false;
    }
    QString tmpMonoFont = monospaceFont;
    QStringList fontList = fontsManager->listMonospace();
    if (!fontList.isEmpty()) {
        tmpMonoFont = fontList[0];
    }

    if (!fontsManager->setFamily(value, tmpMonoFont, fontSize)) {
        return false;
    }

    QDBusMessage message = xSettingsInterface->call("SetString", "Qt/FontName", value);
    if (message.type() == QDBusMessage::ErrorMessage) {
        return false;
    }

    return setDQtTheme({QTKEYFONT}, {value});
}

bool AppearanceManager::doSetMonospaceFont(QString value)
{
    if (!fontsManager->isFontFamily(value)) {
        return false;
    }
    QString tmpStandardFont = standardFont;
    QStringList fontList = fontsManager->listStandard();
    if (!fontList.isEmpty()) {
        tmpStandardFont = fontList[0];
    }

    if (!fontsManager->setFamily(tmpStandardFont, value, fontSize)) {
        return false;
    }

    QDBusMessage message = xSettingsInterface->call("SetString", "Qt/MonoFontName", value);
    if (message.type() == QDBusMessage::ErrorMessage) {
        return false;
    }

    return setDQtTheme({QTKEYMONOFONT}, {value});
}

bool AppearanceManager::doSetBackground(QString value)
{
    if (!backgrounds->isBackgroundFile(value)) {
        return false;
    }

    QString file = backgrounds->prepare(value);

    QString uri = utils::enCodeURI(file, SCHEME_FILE);

    QDBusMessage message = wmInterface->call("ChangeCurrentWorkspaceBackground", uri);
    if (message.type() == QDBusMessage::ErrorMessage) {
        return false;
    }

    imageBlurInterface->call("Get", file);

    imageEffectInterface->call("Get", "", file);

    return true;
}

bool AppearanceManager::doSetGreeterBackground(QString value)
{
    value = utils::enCodeURI(value, SCHEME_FILE);
    greeterBg = value;

    QDBusMessage message = userInterface->call("SetGreeterBackground", value);
    if (message.type() == QDBusMessage::ErrorMessage) {
        return false;
    }

    return true;
}

QString AppearanceManager::doGetWallpaperSlideShow(QString monitorName)
{
    if (!wmInterface->isValid()) {
        return "";
    }

    QDBusMessage message = wmInterface->call("GetCurrentWorkspace");
    if (message.type() == QDBusMessage::ErrorMessage) {
        return "";
    }

    QJsonDocument doc = QJsonDocument::fromJson(wallpaperSlideShow.toLatin1());
    QVariantMap tempMap = doc.object().toVariantMap();

    QString key = QString("%1##%2").arg(monitorName).arg(message.arguments().first().toInt());

    if (tempMap.count(key) == 1) {
        return tempMap[key].toString();
    }

    return "";
}

double AppearanceManager::getScaleFactor()
{
    if (!xSettingsInterface->isValid()) {
        return 0;
    }

    QDBusMessage message = xSettingsInterface->call("GetScaleFactor");
    if (message.type() == QDBusMessage::ErrorMessage) {
        return 0;
    }

    return message.arguments().first().toDouble();
}

ScaleFactors AppearanceManager::getScreenScaleFactors()
{
    ScaleFactors retScaleFactors;
    if (!xSettingsInterface->isValid()) {
        return retScaleFactors;
    }

    QDBusMessage message = xSettingsInterface->call("GetScreenScaleFactors");
    if (message.type() == QDBusMessage::ErrorMessage) {
        qDebug() << message.errorMessage();
        return retScaleFactors;
    }

    retScaleFactors = qdbus_cast<ScaleFactors>(message.arguments().at(0).value<QDBusArgument>());

    return retScaleFactors;
}

bool AppearanceManager::setScaleFactor(double scale)
{
    if (!xSettingsInterface->isValid()) {
        return false;
    }

    QDBusMessage message = xSettingsInterface->call("SetScaleFactor", scale);
    if (message.type() == QDBusMessage::ErrorMessage) {
        return false;
    }

    return true;
}

bool AppearanceManager::setScreenScaleFactors(ScaleFactors scaleFactors)
{
    if (!xSettingsInterface->isValid()) {
        return false;
    }

    QMap<QString, QVariant> tempMap;
    for (auto iter : scaleFactors.toStdMap()) {
        tempMap[iter.first] = iter.second;
    }

    QDBusMessage message = xSettingsInterface->call("SetScreenScaleFactors", tempMap);
    if (message.type() == QDBusMessage::ErrorMessage) {
        return false;
    }

    return true;
}

QString AppearanceManager::doList(QString type)
{
    if (type == TYPEGTK) {
        QVector<QSharedPointer<Theme>> gtks = subthemes->listGtkThemes();

        QVector<QSharedPointer<Theme>>::iterator iter = gtks.begin();
        while (iter != gtks.end()) {
            if ((*iter)->getId().startsWith("deepin")) {
                iter++;
            } else {
                iter = gtks.erase(iter);
            }
        }

        QSharedPointer<Theme> theme(new Theme(AUTOGTKTHEME, "", false));
        gtks.push_back(theme);

        return marshal(gtks);
    } else if (type == TYPEICON) {
        return marshal(subthemes->listIconThemes());
    } else if (type == TYPECURSOR) {
        return marshal(subthemes->listCursorThemes());
    } else if (type == TYPEBACKGROUND) {
        return marshal(backgrounds->listBackground());
    } else if (type == TYPESTANDARDFONT) {
        return marshal(fontsManager->listStandard());
    } else if (type == TYPEMONOSPACEFONT) {
        return marshal(fontsManager->listMonospace());
    }

    return "";
}

QString AppearanceManager::doShow(const QString &type, const QStringList &names)
{
    if (type == TYPEGTK) {
        QVector<QSharedPointer<Theme>> gtks = subthemes->listGtkThemes();

        QVector<QSharedPointer<Theme>>::iterator iter = gtks.begin();
        while (iter != gtks.end()) {
            if (names.indexOf((*iter)->getId())!=-1 ||(*iter)->getId() == AUTOGTKTHEME) {
                iter++;
            } else {
                iter = gtks.erase(iter);
            }
        }
        return marshal(gtks);
    } else if (type == TYPEICON) {
        QVector<QSharedPointer<Theme>> icons = subthemes->listIconThemes();

        QVector<QSharedPointer<Theme>>::iterator iter = icons.begin();
        while (iter != icons.end()) {
            if (names.indexOf((*iter)->getId())!=-1) {
                iter++;
            } else {
                iter = icons.erase(iter);
            }
        }
        return marshal(icons);
    } else if (type == TYPECURSOR) {
        QVector<QSharedPointer<Theme>> cursor = subthemes->listCursorThemes();

        QVector<QSharedPointer<Theme>>::iterator iter = cursor.begin();
        while (iter != cursor.end()) {
            if (names.indexOf((*iter)->getId())!=-1) {
                iter++;
            } else {
                iter = cursor.erase(iter);
            }
        }
        return marshal(cursor);
    } else if (type == TYPEBACKGROUND) {
        QVector<Background> background = backgrounds->listBackground();

        QVector<Background>::iterator iter = background.begin();
        while (iter != background.end()) {
            if (names.indexOf(iter->getId())!=-1) {
                iter++;
            } else {
                iter = background.erase(iter);
            }
        }
        return marshal(background);
    } else if (type == TYPESTANDARDFONT) {
        return marshal(fontsManager->getFamilies(names));
    } else if (type == TYPEMONOSPACEFONT) {
        return marshal(fontsManager->getFamilies(names));
    }

    return "";
}

void AppearanceManager::doResetSettingBykeys(QStringList keys)
{
    QStringList keyList = settingDconfig.keyList();
    for (auto item : keys) {
        if (keyList.contains(item)) {
            continue;
        }
        settingDconfig.reset(item);
    }
}

void AppearanceManager::doResetFonts()
{
    QString defaultStandardFont, defaultMonospaceFont;
    getDefaultFonts(defaultStandardFont, defaultMonospaceFont);

    if (defaultStandardFont != standardFont) {
        setStandarFont(defaultStandardFont);
    }

    if (defaultMonospaceFont != monospaceFont) {
        setMonospaceFont(defaultMonospaceFont);
    }

    bool bSuccess = fontsManager->setFamily(defaultStandardFont, defaultMonospaceFont, fontSize);
    if (!bSuccess) {
        return;
    }

    fontsManager->checkFontConfVersion();
}

void AppearanceManager::doSetByType(const QString &type, const QString &value)
{
    if (type == TYPEGTK) {
        if (value == gtkTheme) {
            return;
        }

        if (doSetGtkTheme(value)) {
            setGtkTheme(value);
        }
    } else if (type == TYPEICON) {
        if (value == iconTheme) {
            return;
        }

        if (doSetIconTheme(value)) {
            setIconTheme(value);
        }
    } else if (type == TYPECURSOR) {
        if (value == cursorTheme) {
            return;
        }

        if (doSetCursorTheme(value)) {
            setCursorTheme(value);
        }
    } else if (type == TYPEBACKGROUND) {
        bool bSuccess = doSetBackground(value);
        if (bSuccess && wsLoopMap.count(curMonitorSpace) == 1) {
            wsLoopMap[curMonitorSpace]->addToShow(value);
        }
    } else if (type == TYPEGREETERBACKGROUND) {
        doSetGreeterBackground(value);
    } else if (type == TYPESTANDARDFONT) {
        if (standardFont == value) {
            return;
        }
        bool bSuccess = doSetStandardFont(value);
        if (bSuccess) {
            setStandarFont(value);
        }
    } else if (type == TYPEMONOSPACEFONT) {
        if (monospaceFont == value) {
            return;
        }
        bool bSuccess = doSetMonospaceFont(value);
        if (bSuccess) {
            setMonospaceFont(value);
        }
    } else if (type == TYPEFONTSIZE) {
        double size = value.toDouble();
        if (fontSize > size - 0.01 && fontSize < size + 0.01) {
            return;
        }
        bool bSuccess = doSetFonts(size);
        if (bSuccess) {
            setFontSize(size);
        }
    } else {
        return;
    }
}

QString AppearanceManager::doSetMonitorBackground(const QString &monitorName, const QString &imageGile)
{
    if (!backgrounds->isBackgroundFile(imageGile)) {
        return "";
    }

    QString file = backgrounds->prepare(imageGile);

    QString uri = utils::enCodeURI(file, SCHEME_FILE);

    QDBusMessage message = wmInterface->call("SetCurrentWorkspaceBackgroundForMonitor", uri, monitorName);
    if (message.type() == QDBusMessage::ErrorMessage) {
        return "";
    }

    doUpdateWallpaperURIs();

    imageBlurInterface->call("Get", file);

    imageEffectInterface->call("Get", "", file);

    return file;
}

QString AppearanceManager::doThumbnail(const QString &type, const QString &name)
{
    if (type == TYPEGTK) {
        QMap<QString, QString> gtkThumbnailMap = subthemes->getGtkThumbnailMap();
        if (gtkThumbnailMap.count(name) == 1) {
            return "/usr/share/dde-daemon/appearance/" + gtkThumbnailMap[name] + ".svg";
        }
        return  subthemes->getGtkThumbnail(name);
    } else if (type == TYPEICON) {
        return subthemes->getIconThumbnail(name);
    } else if (type == TYPECURSOR) {
        return subthemes->getCursorThumbnail(name);
    } else {
        return QString("invalid type: %1").arg(type);
    }
}

bool AppearanceManager::doSetWallpaperSlideShow(const QString &monitorName, const QString &wallpaperSlideShow)
{
    QDBusMessage message = wmInterface->call("GetCurrentWorkspace");
    if (message.type() == QDBusMessage::ErrorMessage) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(wallpaperSlideShow.toLatin1());
    QJsonObject cfgObj = doc.object();

    QString key = QString("%1##%2").arg(monitorName).
            arg(message.arguments().first().toString());

    cfgObj[key] = wallpaperSlideShow;

    QJsonDocument docTmp;
    docTmp.setObject(cfgObj);
    QString value = docTmp.toJson(QJsonDocument::Compact);

    setWallpaperSlideShow(value);

    curMonitorSpace = key;

    return true;
}

bool AppearanceManager::doSetWsLoop(const QString &monitorName, const QString &file)
{
    QDBusMessage message = wmInterface->call("GetCurrentWorkspace");
    if (message.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "call GetCurrentWorkspace fail";
        return false;
    }

    int index = message.arguments().first().toInt();
    QString monitor = QString("%1##%2").arg(monitorName).arg(index);
    if (wsLoopMap.count(monitor) == 1) {
        wsLoopMap[monitor]->addToShow(file);
    }

    return true;
}

QString AppearanceManager::getCurrentDesktopIndex()
{
    QDBusMessage message = wmInterface->call("GetCurrentWorkspace");
    if (message.type() == QDBusMessage::ErrorMessage) {
        qDebug() << message.errorMessage();
        return "";
    }

    return QString::number(message.arguments().first().toInt());
}

void AppearanceManager::doSetCurrentWorkspaceBackground(const QString &uri)
{
    QString strIndex = getCurrentDesktopIndex();
    if (strIndex == "") {
        qWarning() << "error getting current desktop index through wm";
        return;
    }

    PhaseWallPaper::setWallpaperUri(strIndex, "", uri);
    return;
}

QString AppearanceManager::doGetCurrentWorkspaceBackground()
{
    QString strIndex = getCurrentDesktopIndex();
    if (strIndex == "") {
        qWarning() << "error getting current desktop index through wm.";
        return "";
    }

    return PhaseWallPaper::getWallpaperUri(strIndex, "");
}

void AppearanceManager::doSetCurrentWorkspaceBackgroundForMonitor(const QString &uri, const QString &strMonitorName)
{
    QString strIndex = getCurrentDesktopIndex();
    if (strIndex == "") {
        qWarning() << "error getting current desktop index through wm";
        return;
    }

    PhaseWallPaper::setWallpaperUri(strIndex, strMonitorName, uri);
    return;
}

QString AppearanceManager::doGetCurrentWorkspaceBackgroundForMonitor(const QString &strMonitorName)
{
    QString strIndex = getCurrentDesktopIndex();
    if (strIndex == "") {
        qWarning() << "error getting current desktop index through wm.";
        return "";
    }

    return PhaseWallPaper::getWallpaperUri(strIndex, strMonitorName);
}

void AppearanceManager::doSetWorkspaceBackgroundForMonitor(const int &index, const QString &strMonitorName, const QString &uri)
{
    return PhaseWallPaper::setWallpaperUri(QString::number(index), strMonitorName, uri);
}

QString AppearanceManager::doGetWorkspaceBackgroundForMonitor(const int &index,const QString &strMonitorName)
{
    return PhaseWallPaper::getWallpaperUri(QString::number(index), strMonitorName);
}

void AppearanceManager::autoChangeBg(QString monitorSpace, QDateTime date)
{
    qDebug() << "autoChangeBg: " << monitorSpace << ", " << date;

    if (wsLoopMap.count(monitorSpace) == 0) {
        return;
    }

    QString file = wsLoopMap[monitorSpace]->getNext();
    if (file.isEmpty()) {
        qDebug() << "file is empty";
        return;
    }

    QDBusMessage message = wmInterface->call("GetCurrentWorkspace");
    if (message.type() == QDBusMessage::ErrorMessage) {
        qDebug() << message.errorMessage();
    }

    QString strIndex = QString::number(message.arguments().first().toInt());

    int index = monitorSpace.indexOf("##");
    if (index == -1) {
        qWarning() << "monitorSpace format error";
        return;
    }

    if (strIndex == monitorSpace.right(index + 2)) {
        doSetMonitorBackground(monitorSpace.left(index), file);
    }

    saveWSConfig(monitorSpace, date);
}

bool AppearanceManager::changeBgAfterLogin(QString monitorSpace)
{
    QString runDir = g_get_user_runtime_dir();

    QFile file("/proc/self/sessionid");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "open /proc/self/sessionid fail";
        return false;
    }

    QString currentSessionId = file.readAll();
    currentSessionId = currentSessionId.simplified();

    bool needChangeBg = false;
    runDir = runDir + "/dde-daemon-wallpaper-slideshow-login" + "/" + monitorSpace;
    QFile fileTemp(runDir);

    if (!file.exists()) {
        needChangeBg = true;
    } else if (!fileTemp.open(QIODevice::ReadOnly)) {
        qWarning() << "open " << runDir << " fail";
        return false;
    } else {
        if (currentSessionId != fileTemp.readAll().simplified()) {
            needChangeBg = true;
        }
    }

    if (needChangeBg) {
        autoChangeBg(monitorSpace, QDateTime::currentDateTime());
        fileTemp.write(currentSessionId.toLatin1());
    }

    file.close();
    fileTemp.close();

    return true;
}

bool AppearanceManager::setDQtTheme(QStringList key, QStringList value)
{
    if (key.length() != value.length()) {
        return false;
    }

    QString filePath = utils::GetUserConfigDir() + "/deepin" + "/qt-theme.ini";

    QFile file(filePath);
    if (!file.exists()) {
        QDir dir(filePath);
        if (!dir.exists()) {
            if (!dir.mkdir(filePath.left(filePath.lastIndexOf("/")))) {
                return false;
            }
        }

        if (!file.open(QIODevice::WriteOnly)) {
            return false;
        }
    }

    KeyFile keyfile;
    keyfile.loadFile(filePath);
    for (int i = 0; i < key.length(); i++) {
        QString temp = keyfile.getStr("Theme", key[i]);
        if (temp == value[i]) {
            continue;
        }
        keyfile.setKey("Theme", key[i], value[i]);
    }
    keyfile.saveToFile(filePath);

    return true;
}

bool AppearanceManager::saveWSConfig(QString monitorSpace, QDateTime date)
{
    WallpaperLoopConfigManger configManger;

    QString fileName = utils::GetUserConfigDir() + "/deepin/dde-daemon/appearance/wallpaper-slideshow.json";
    configManger.loadWSConfig(fileName);

    if (wsLoopMap.count(monitorSpace) != 0) {
        configManger.setShowed(monitorSpace, wsLoopMap[monitorSpace]->getShowed());
    }
    configManger.setLastChange(monitorSpace, date);

    return configManger.save(fileName);
}

QString AppearanceManager::marshal(const QVector<QSharedPointer<Theme>> &themes)
{
    QJsonDocument doc;
    QJsonArray array;
    for (auto iter : themes) {
        QJsonObject obj;
        obj.insert("Id", iter->getId());
        obj.insert("Path", iter->getPath());
        obj.insert("Deletable", iter->getDeleteable());
        array.append(obj);
    }

    doc.setArray(array);

    return doc.toJson(QJsonDocument::Compact);
}

QString AppearanceManager::marshal(const QVector<Background> &backgrounds)
{
    QJsonDocument doc;
    QJsonArray array;
    for (auto iter : backgrounds) {
        QJsonObject obj;
        obj.insert("Id", iter.getId());
        obj.insert("Deletable", iter.getDeleteable());
        array.append(obj);
    }

    doc.setArray(array);

    return doc.toJson(QJsonDocument::Compact);
}

QString AppearanceManager::marshal(const QStringList &strs)
{
    QJsonDocument doc;
    QJsonArray array;
    for (auto iter : strs) {
        array.append(iter);
    }

    doc.setArray(array);

    return doc.toJson(QJsonDocument::Compact);
}

QString AppearanceManager::marshal(const QVector<QSharedPointer<FontsManager::Family>> &strs)
{
    QJsonDocument doc;

    QJsonArray arr;
    for (auto iter : strs) {
        QJsonObject obj;
        obj["Id"] = iter->id;
        obj["Name"] = iter->id;
        obj["Styles"] = iter->id;
        obj["Styles"] = iter->id;
        obj["Show"] = iter->id;
        arr.push_back(obj);
    }

    doc.setArray(arr);
    return doc.toJson(QJsonDocument::Compact);
}
