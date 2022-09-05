#ifndef APPEARANCEMANAGER_H
#define APPEARANCEMANAGER_H

#include "fsnotify.h"
#include "../modules/subthemes/subthemes.h"
#include "../modules/fonts/fontsmanager.h"
#include "../../dbus/types/scaleFactors.h"
#include "cursorchangehandler.h"
#include "wallpaperscheduler.h"

#include <QObject>
#include <QDebug>
#include <DConfig>
#include <QDBusInterface>
#include <QMap>
#include <QTimerEvent>
#include <QTimer>
#include <QGSettings>

class Appearance1;
class CustomTheme;

class AppearanceManager : public QObject
{
    Q_OBJECT
    struct coordinate{
        double latitude;
        double longitude;
    };

    struct fontConfigItem{
        QString Standard;
        QString Monospace;
    };

public:
    explicit AppearanceManager(QObject *parent = nullptr);
    ~AppearanceManager();

public:
    bool init();
    void deleteThermByType(const QString &ty, const QString &name);
    void setFontSize(double value);
    void setGlobalTheme(QString value);
    void setGtkTheme(QString value);
    void setIconTheme(QString value);
    void setCursorTheme(QString value);
    void setStandardFont(QString value);
    void setMonospaceFont(QString value);
    void setWindowRadius(int value);
    void setOpacity(double value);
    void setQtActiveColor(const QString &value);
    bool setWallpaperSlideShow(const QString &value);
    bool setWallpaperURls(const QString &value);
    bool doSetFonts(double size);
    bool doSetGlobalTheme(QString value);
    bool doSetGtkTheme(QString value);
    bool doSetIconTheme(QString value);
    bool doSetCursorTheme(QString value);
    bool doSetStandardFont(QString value);
    bool doSetMonospaceFont(QString value);
    bool doSetBackground(QString value);
    bool doSetGreeterBackground(QString value);
    QString doGetWallpaperSlideShow(QString monitorName);
    double getScaleFactor();
    ScaleFactors getScreenScaleFactors();
    bool setScaleFactor(double scale);
    bool setScreenScaleFactors(ScaleFactors scaleFactors);
    QString doList(QString type);
    QString doShow(const QString& type, const QStringList& names);
    void doResetSettingBykeys(QStringList keys);
    void doResetFonts();
    void doSetByType(const QString& type,const QString& value);
    QString doSetMonitorBackground(const QString& monitorName,const QString& imageGile);
    bool doSetWallpaperSlideShow(const QString &monitorName,const QString &wallpaperSlideShow);
    bool doSetWsLoop(const QString& monitorName,const QString& file);
    QString doThumbnail(const QString &type,const QString &name);
    void doSetCurrentWorkspaceBackground(const QString &uri);
    QString doGetCurrentWorkspaceBackground();
    void doSetCurrentWorkspaceBackgroundForMonitor(const QString &uri, const QString &strMonitorName);
    QString doGetCurrentWorkspaceBackgroundForMonitor(const QString &strMonitorName);
    void doSetWorkspaceBackgroundForMonitor(const int &index, const QString &strMonitorName, const QString &uri);
    QString doGetWorkspaceBackgroundForMonitor(const int &index,const QString &strMonitorName);

    inline QString getBackground() {return background; }
    inline double getFontSize() {return fontSize; }
    inline QString getGLobalTheme() {return globalTheme; }
    inline QString getGtkTheme() {return gtkTheme; }
    inline QString getIconTheme() {return iconTheme; }
    inline QString getCursorTheme() {return cursorTheme; }
    inline QString getMonospaceFont() {return monospaceFont; }
    inline double getOpacity() {return opacity; }
    inline QString getQtActiveColor() {return qtActiveColor; }
    inline QString getStandardFont() {return standardFont; }
    inline QString getWallpaperSlideShow() {return wallpaperSlideShow; }
    inline QString getWallpaperURls() {return wallpaperURls;}
    inline int getWindowRadius() {return windowRadius; }
    inline QString getGreetBg() {return greeterBg; }
    inline QMap<QString,QString>& getMonitor() {return monitorMap; }
    inline QSharedPointer<QDBusInterface> getWmDbusInterface(){return wmInterface;}
    void timerEvent( QTimerEvent *event) override;

public Q_SLOTS:
    void handleWmWorkspaceCountChanged(int count);
    void handleWmWorkspaceSwithched(int from,int to);
    void handleSetScaleFactorStarted();
    void handleSetScaleFactorDone();
    void handleDisplayChanged(QDBusMessage mes);
    void handleTimezoneChanged(QDBusMessage mes);
    void handleTimeNTPChanged(QDBusMessage mes);
    void handleTimeUpdate();
    void handleNTPChanged();
    void handlethemeFileChange(QString theme);
    void handleXsettingDConfigChange(QString key);
    void handleSettingDConfigChange(QString key);
    void handleWrapBgDConfigChange(QString key);
    void handleGnomeBgDConfigChange(QString key);
    void handleDetectSysClockTimeOut();
    void handleUpdateToCustom(const QString &mode);

private:
    QString qtActiveColorToHexColor(const QString &activeColor) const;
    QString hexColorToQtActiveColor(const QString &hexColor) const;
    void initCoordinate();
    void initUserObj();
    void initCurrentBgs();
    void initWallpaperSlideshow();
    void updateMonitorMap();
    void iso6709Parsing(QString city, QString coordinates);
    void doUpdateWallpaperURIs();
    void setPropertyWallpaperURIs(QMap<QString,QString> monitorWallpaperUris);
    void updateNewVersionData();
    void autoSetTheme(double latitude, double longitude);
    void resetThemeAutoTimer();
    void loadDefaultFontConfig();
    void getDefaultFonts(QString& standard,QString& monospace);
    void updateThemeAuto(bool enable);
    void enableDetectSysClock(bool enable);
    void updateWSPolicy(QString policy);
    void loadWSConfig();
    QDateTime getThemeAutoChangeTime(QDateTime date, double latitude, double longitude);

    void autoChangeBg(QString monitorSpace, QDateTime date);
    bool changeBgAfterLogin(QString monitorSpace);
    bool setDQtTheme(QStringList key,QStringList value);
    bool saveWSConfig(QString monitorSpace, QDateTime date);
    QString marshal(const QVector<QSharedPointer<Theme>>& themes);
    QString marshal(const QVector<Background>& backgrounds);
    QString marshal(const QStringList& strs);
    QString marshal(const QVector<QSharedPointer<FontsManager::Family>>& strs);
    QString getCurrentDesktopIndex();
    void applyGlobalTheme(KeyFile &theme, const QString &themeName, const QString &defaultTheme);

    void updateCustomTheme(const QString &type, const QString &value);

Q_SIGNALS:
    void Changed(const QString &ty, const QString &value);
    void Refreshed(const QString &type);

private: // PROPERTIES
    QString background;
    double  fontSize;
    QString globalTheme;
    QString currentGlobalTheme; // 当前主题，globalTheme+.light/.dark
    QString gtkTheme;
    QString iconTheme;
    QString cursorTheme;
    QString monospaceFont;
    double  opacity;
    QString qtActiveColor;
    QString standardFont;
    QString wallpaperSlideShow;
    QString wallpaperURls;
    int     windowRadius;

private:
    DConfig                         settingDconfig;
    QSharedPointer<QGSettings>      xSetting;
    QSharedPointer<QGSettings>      wrapBgSetting;
    QSharedPointer<QGSettings>      gnomeBgSetting;
    QSharedPointer<QDBusInterface>  wmInterface;
    QSharedPointer<QDBusInterface>  displayInterface;
    QSharedPointer<QDBusInterface>  xSettingsInterface;
    QSharedPointer<QDBusInterface>  userInterface;
    QSharedPointer<QDBusInterface>  timeDateInterface;
    QSharedPointer<QDBusInterface>  sessionTimeDateInterface;
    QSharedPointer<QDBusInterface>  imageBlurInterface;
    QSharedPointer<QDBusInterface>  imageEffectInterface;
    QSharedPointer<Subthemes>       subthemes;
    QSharedPointer<Backgrounds>     backgrounds;
    QSharedPointer<FontsManager>    fontsManager;
    QMap<QString,QString>           monitorMap;
    QMap<QString,coordinate>        coordinateMap;
    QMap<QString,fontConfigItem>    defaultFontConfigMap;
    double                          longitude;
    double                          latitude;
    QStringList                     desktopBgs;
    QString                         greeterBg;
    int                             timeUpdateTimeId;
    int                             ntpTimeId;
    bool                            locationValid;
    uint                            nid;
    QString                         curMonitorSpace;
    QSharedPointer<CursorChangeHandler> cursorChangeHandler;
    QSharedPointer<Fsnotify>        fsnotify;
    QString                         m_gsQtActiveColor;
    QTimer                          detectSysClockTimer;
    QTimer                          themeAutoTimer;
    qint64                          detectSysClockStartTime;
    QString                         zone;
    QMap<QString,QSharedPointer<WallpaperScheduler>> wsSchedulerMap;
    QMap<QString,QSharedPointer<WallpaperLoop>>      wsLoopMap;
    CustomTheme                     *customTheme;
    bool                            globalThemeUpdating;
};

#endif // APPEARANCEMANAGER_H
