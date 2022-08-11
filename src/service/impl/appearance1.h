#ifndef APPEARANCE1_H
#define APPEARANCE1_H

#include "appearancemanager.h"
#include "../../DBus/types/scaleFactors.h"
#include <QObject>

class Appearance1 : public QObject {
    Q_OBJECT

public:
    Appearance1(QObject *parent = nullptr);
    ~Appearance1();

public: // PROPERTIES
    Q_PROPERTY(QString Background READ background)
    QString background() const;

    Q_PROPERTY(double FontSize READ fontSize WRITE setFontSize)
    double fontSize() const;
    void setFontSize(double value);

    Q_PROPERTY(QString GtkTheme READ gtkTheme)
    QString gtkTheme() const;

    Q_PROPERTY(QString IconTheme READ iconTheme)
    QString iconTheme() const;

    Q_PROPERTY(QString CursorTheme READ cursorTheme)
    QString cursorTheme() const;

    Q_PROPERTY(QString MonospaceFont READ monospaceFont)
    QString monospaceFont() const;

    Q_PROPERTY(double Opaticy READ opaticy WRITE setOpaticy)
    double opaticy() const;
    void setOpaticy(double value);

    Q_PROPERTY(QString QtActiveColor READ qtActiveColor WRITE setQtActiveColor)
    QString qtActiveColor() const;
    void setQtActiveColor(const QString &value);

    Q_PROPERTY(QString StandarFont READ standarFont)
    QString standarFont() const;

    Q_PROPERTY(QString WallpaperSlideShow READ wallpaperSlideShow WRITE setWallpaperSlideShow)
    QString wallpaperSlideShow() const;
    void setWallpaperSlideShow(const QString &value);

    Q_PROPERTY(QString WallpaperURls READ wallpaperURls)
    QString wallpaperURls() const;

    Q_PROPERTY(int WindowRadius READ windowRadius WRITE setWindowRadius)
    int windowRadius() const;
    void setWindowRadius(int value);

public Q_SLOTS: // METHODS
    void Delete(const QString &ty, const QString &name);
    double GetScaleFactor();
    ScaleFactors GetScreenScaleFactors();
    QString GetWallpaperSlideShow(const QString &monitorName);
    QString List(const QString &ty);
    void Reset();
    void SetMonitorBackground(const QString &monitorName, const QString &imageGile);
    void SetScaleFactor(double scale);
    void SetScreenScaleFactors(ScaleFactors scaleFactors);
    void SetWallpaperSlideShow(const QString &monitorName, const QString &slideShow);
    QString Show(const QString &ty, const QStringList &names);
    QString Thumbnail(const QString &ty, const QString &name);
    void Set(const QString &ty,const QString &value);
    void handleChangeSignal(const QString& type, const QString& value);
    void handleRefreshedSignal(const QString &type);

    void SetCurrentWorkspaceBackground(const QString &uri);
    QString GetCurrentWorkspaceBackground();
    void SetCurrentWorkspaceBackgroundForMonitor(const QString &uri, const QString &strMonitorName);
    QString GetCurrentWorkspaceBackgroundForMonitor(const QString &strMonitorName);
    void SetWorkspaceBackgroundForMonitor(const int &index, const QString &strMonitorName, const QString &uri);
    QString GetWorkspaceBackgroundForMonitor(const int &index,const QString &strMonitorName);

Q_SIGNALS: // SIGNALS
    void Changed(const QString &ty, const QString &value);
    void Refreshed(const QString &type);

public:
    QSharedPointer<AppearanceManager> appearanceManager;
};

#endif
