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
#include "appearanceproperty.h"
#include "modules/common/commondefine.h"

#include <QDBusMessage>
#include <QDBusConnection>

void AppearancePropertiesChanged(const QString &property, const QVariant &value)
{
    QVariantMap properties;
    properties.insert(property, value);

    QList<QVariant> arguments;
    arguments.push_back(AppearanceInterface);
    arguments.push_back(properties);
    arguments.push_back(QStringList());

    QDBusMessage msg = QDBusMessage::createSignal(AppearancePath, "org.freedesktop.DBus.Properties", "PropertiesChanged");
    msg.setArguments(arguments);
    APPEARANCEDBUS.send(msg);
}

AppearanceProperty::AppearanceProperty()
    : background("Background")
    , cursorTheme("CursorTheme")
    , fontSize("FontSize")
    , globalTheme("GlobalTheme")
    , gtkTheme("GtkTheme")
    , iconTheme("IconTheme")
    , monospaceFont("MonospaceFont")
    , opacity("Opacity")
    , qtActiveColor("QtActiveColor")
    , standardFont("StandardFont")
    , wallpaperSlideShow("WallpaperSlideShow")
    , wallpaperURls("WallpaperURls")
    , windowRadius("WindowRadius")
{
    fontSize.init(0.0);
    opacity.init(0.0);
    windowRadius.init(0);
}
