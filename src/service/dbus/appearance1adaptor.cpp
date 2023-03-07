// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "appearance1adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class Appearance1Adaptor
 */

Appearance1Adaptor::Appearance1Adaptor(Appearance1 *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

Appearance1Adaptor::~Appearance1Adaptor()
{
    // destructor
}

QString Appearance1Adaptor::background() const
{
    // get the value of property Cackground
    return parent()->background();
}

QString Appearance1Adaptor::cursorTheme() const
{
    // get the value of property CursorTheme
    return parent()->cursorTheme();
}

double Appearance1Adaptor::fontSize() const
{
    // get the value of property FontSize
    return parent()->fontSize();
}

void Appearance1Adaptor::setFontSize(double value)
{
    // set the value of property FontSize
    parent()->setFontSize(value);
}

QString Appearance1Adaptor::globalTheme() const
{
    // get the value of property GlobalTheme
    return parent()->globalTheme();
}

QString Appearance1Adaptor::gtkTheme() const
{
    // get the value of property GtkTheme
    return parent()->gtkTheme();
}

QString Appearance1Adaptor::iconTheme() const
{
    // get the value of property IconTheme
    return parent()->iconTheme();
}

QString Appearance1Adaptor::monospaceFont() const
{
    // get the value of property MonospaceFont
    return parent()->monospaceFont();
}

double Appearance1Adaptor::opacity() const
{
    // get the value of property Opacity
    return parent()->opacity();
}

void Appearance1Adaptor::setOpacity(double value)
{
    // set the value of property Opacity
    parent()->setOpacity(value);
}

QString Appearance1Adaptor::qtActiveColor() const
{
    // get the value of property QtActiveColor
    return parent()->qtActiveColor();
}

void Appearance1Adaptor::setQtActiveColor(const QString &value)
{
    // set the value of property QtActiveColor
    parent()->setQtActiveColor(value);
}

QString Appearance1Adaptor::standardFont() const
{
    // get the value of property StandardFont
    return parent()->standardFont();
}

QString Appearance1Adaptor::wallpaperSlideShow() const
{
    // get the value of property WallpaperSlideShow
    return parent()->wallpaperSlideShow();
}

void Appearance1Adaptor::setWallpaperSlideShow(const QString &value)
{
    // set the value of property WallpaperSlideShow
    parent()->setWallpaperSlideShow(value);
}

QString Appearance1Adaptor::wallpaperURls() const
{
    // get the value of property WallpaperURls
    return parent()->wallpaperURls();
}

int Appearance1Adaptor::windowRadius() const
{
    // get the value of property WindowRadius
    return parent()->windowRadius();
}

void Appearance1Adaptor::setWindowRadius(int value)
{
    // set the value of property WindowRadius
    parent()->setWindowRadius(value);
}

void Appearance1Adaptor::Delete(const QString &ty, const QString &name, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.Delete
    parent()->Delete(ty, name, message);
}

QString Appearance1Adaptor::GetCurrentWorkspaceBackground(const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.GetCurrentWorkspaceBackground
    return parent()->GetCurrentWorkspaceBackground(message);
}

QString Appearance1Adaptor::GetCurrentWorkspaceBackgroundForMonitor(const QString &strMonitorName, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.GetCurrentWorkspaceBackgroundForMonitor
    return parent()->GetCurrentWorkspaceBackgroundForMonitor(strMonitorName, message);
}

double Appearance1Adaptor::GetScaleFactor(const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.GetScaleFactor
    return parent()->GetScaleFactor(message);
}

ScaleFactors Appearance1Adaptor::GetScreenScaleFactors(const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.GetScreenScaleFactors
    return parent()->GetScreenScaleFactors(message);
}

QString Appearance1Adaptor::GetWallpaperSlideShow(const QString &monitorName, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.GetWallpaperSlideShow
    return parent()->GetWallpaperSlideShow(monitorName, message);
}

QString Appearance1Adaptor::GetWorkspaceBackgroundForMonitor(const int &index, const QString &strMonitorName, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.GetWorkspaceBackgroundForMonitor
    return parent()->GetWorkspaceBackgroundForMonitor(index, strMonitorName, message);
}

QString Appearance1Adaptor::List(const QString &ty, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.List
    return parent()->List(ty, message);
}

void Appearance1Adaptor::Reset(const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.Reset
    parent()->Reset(message);
}

void Appearance1Adaptor::Set(const QString &ty, const QString &value, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.Set
    parent()->Set(ty, value, message);
}

void Appearance1Adaptor::SetCurrentWorkspaceBackground(const QString &uri, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.SetCurrentWorkspaceBackground
    parent()->SetCurrentWorkspaceBackground(uri, message);
}

void Appearance1Adaptor::SetCurrentWorkspaceBackgroundForMonitor(const QString &uri, const QString &strMonitorName, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.SetCurrentWorkspaceBackgroundForMonitor
    parent()->SetCurrentWorkspaceBackgroundForMonitor(uri, strMonitorName, message);
}

void Appearance1Adaptor::SetMonitorBackground(const QString &monitorName, const QString &imageGile, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.SetMonitorBackground
    parent()->SetMonitorBackground(monitorName, imageGile, message);
}

void Appearance1Adaptor::SetScaleFactor(double scale, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.SetScaleFactor
    parent()->SetScaleFactor(scale, message);
}

void Appearance1Adaptor::SetScreenScaleFactors(ScaleFactors scaleFactor, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.SetScreenScaleFactors
    parent()->SetScreenScaleFactors(scaleFactor, message);
}

void Appearance1Adaptor::SetWallpaperSlideShow(const QString &monitorName, const QString &slideShow, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.SetWallpaperSlideShow
    parent()->SetWallpaperSlideShow(monitorName, slideShow, message);
}

void Appearance1Adaptor::SetWorkspaceBackgroundForMonitor(const int &index, const QString &strMonitorName, const QString &uri, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.SetWorkspaceBackgroundForMonitor
    parent()->SetWorkspaceBackgroundForMonitor(index, strMonitorName, uri, message);
}

QString Appearance1Adaptor::Show(const QString &ty, const QStringList &names, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.Show
    return parent()->Show(ty, names, message);
}

QString Appearance1Adaptor::Thumbnail(const QString &ty, const QString &name, const QDBusMessage &message)
{
    // handle method call org.deepin.dde.Appearance1.Thumbnail
    return parent()->Thumbnail(ty, name, message);
}
