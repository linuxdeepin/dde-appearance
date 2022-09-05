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
#ifndef CUSTOMTHEME_H
#define CUSTOMTHEME_H

#include <QObject>

class KeyFile;
class Theme;

class CustomTheme : public QObject
{
    Q_OBJECT
public:
    explicit CustomTheme(QObject *parent = nullptr);

    void updateValue(const QString &type, const QString &value, const QString &oldTheme, const QVector<QSharedPointer<Theme>> &globalThemes);

protected:
    void openCustomTheme();
    void saveCustomTheme();
    void copyTheme(const QString &themePath, const QStringList &keys);

Q_SIGNALS:
    void updateToCustom(const QString &mode);

private:
    KeyFile *m_customTheme;
};

#endif // CUSTOMTHEME_H
