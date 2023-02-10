// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPEARANCE1_H
#define APPEARANCE1_H

#include "scaleFactors.h"

#include <QObject>

class QDBusMessage;
class Appearance1Thread;
class Appearance1 : public QObject
{
    Q_OBJECT

public:
    Appearance1(QObject *parent = nullptr);
    ~Appearance1();

public: // PROPERTIES
    Q_PROPERTY(QString Background READ background)
    QString background() const;

    Q_PROPERTY(QString CursorTheme READ cursorTheme)
    QString cursorTheme() const;

    Q_PROPERTY(double FontSize READ fontSize WRITE setFontSize)
    double fontSize() const;
    void setFontSize(double value);

    Q_PROPERTY(QString GlobalTheme READ globalTheme)
    QString globalTheme() const;

    Q_PROPERTY(QString GtkTheme READ gtkTheme)
    QString gtkTheme() const;

    Q_PROPERTY(QString IconTheme READ iconTheme)
    QString iconTheme() const;

    Q_PROPERTY(QString MonospaceFont READ monospaceFont)
    QString monospaceFont() const;

    Q_PROPERTY(double Opacity READ opacity WRITE setOpacity)
    double opacity() const;
    void setOpacity(double value);

    Q_PROPERTY(QString QtActiveColor READ qtActiveColor WRITE setQtActiveColor)
    QString qtActiveColor() const;
    void setQtActiveColor(const QString &value);

    Q_PROPERTY(QString StandardFont READ standardFont)
    QString standardFont() const;

    Q_PROPERTY(QString WallpaperSlideShow READ wallpaperSlideShow WRITE setWallpaperSlideShow)
    QString wallpaperSlideShow() const;
    void setWallpaperSlideShow(const QString &value);

    Q_PROPERTY(QString WallpaperURls READ wallpaperURls)
    QString wallpaperURls() const;

    Q_PROPERTY(int WindowRadius READ windowRadius WRITE setWindowRadius)
    int windowRadius() const;
    void setWindowRadius(int value);

public Q_SLOTS: // METHODS
    void Delete(const QString &ty, const QString &name, const QDBusMessage &message);
    QString GetCurrentWorkspaceBackground(const QDBusMessage &message);
    QString GetCurrentWorkspaceBackgroundForMonitor(const QString &strMonitorName, const QDBusMessage &message);
    double GetScaleFactor(const QDBusMessage &message);
    ScaleFactors GetScreenScaleFactors(const QDBusMessage &message);
    QString GetWallpaperSlideShow(const QString &monitorName, const QDBusMessage &message);
    QString GetWorkspaceBackgroundForMonitor(const int &index, const QString &strMonitorName, const QDBusMessage &message);
    QString List(const QString &ty, const QDBusMessage &message);
    void Reset(const QDBusMessage &message);
    void Set(const QString &ty, const QString &value, const QDBusMessage &message);
    void SetCurrentWorkspaceBackground(const QString &uri, const QDBusMessage &message);
    void SetCurrentWorkspaceBackgroundForMonitor(const QString &uri, const QString &strMonitorName, const QDBusMessage &message);
    void SetMonitorBackground(const QString &monitorName, const QString &imageGile, const QDBusMessage &message);
    void SetScaleFactor(double scale, const QDBusMessage &message);
    void SetScreenScaleFactors(ScaleFactors scaleFactors, const QDBusMessage &message);
    void SetWallpaperSlideShow(const QString &monitorName, const QString &slideShow, const QDBusMessage &message);
    void SetWorkspaceBackgroundForMonitor(const int &index, const QString &strMonitorName, const QString &uri, const QDBusMessage &message);
    QString Show(const QString &ty, const QStringList &names, const QDBusMessage &message);
    QString Thumbnail(const QString &ty, const QString &name, const QDBusMessage &message);
Q_SIGNALS: // SIGNALS
    void Changed(const QString &ty, const QString &value);
    void Refreshed(const QString &type);

public:
    QScopedPointer<Appearance1Thread> appearance1Thread;
};

#endif
