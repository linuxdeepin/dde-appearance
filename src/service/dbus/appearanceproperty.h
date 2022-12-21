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
#ifndef APPEARANCEPROPERTY_H
#define APPEARANCEPROPERTY_H

#include <QVariant>

void AppearancePropertiesChanged(const QString &property, const QVariant &value);

template<typename T>
class DBusProperty
{
public:
    explicit DBusProperty(QString name)
        : m_name(name) { }
    inline void init(const T &initValue)
    {
        m_data = initValue;
    }

    inline T operator=(const T &newValue)
    {
        if (m_data != newValue) {
            m_data = newValue;
            AppearancePropertiesChanged(m_name, QVariant::fromValue(m_data));
        }
        return newValue;
    }
    inline T data() const
    {
        return m_data;
    }
    inline operator T() const
    {
        return m_data;
    }
    inline const T *operator->() const
    {
        return &m_data;
    }

private:
    T m_data;
    QString m_name;
};

class AppearanceProperty
{
public:
    explicit AppearanceProperty();

public:
    DBusProperty<QString> background;
    DBusProperty<QString> cursorTheme;
    DBusProperty<double> fontSize;
    DBusProperty<QString> globalTheme;
    DBusProperty<QString> gtkTheme;
    DBusProperty<QString> iconTheme;
    DBusProperty<QString> monospaceFont;
    DBusProperty<double> opacity;
    DBusProperty<QString> qtActiveColor;
    DBusProperty<QString> standardFont;
    DBusProperty<QString> wallpaperSlideShow;
    DBusProperty<QString> wallpaperURls;
    DBusProperty<int> windowRadius;
};

#endif // APPEARANCEPROPERTY_H
