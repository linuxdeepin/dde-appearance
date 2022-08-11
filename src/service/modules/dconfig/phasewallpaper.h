/*
 * Copyright (C) 2022 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     lixi <lixi@uniontech.com>
 *
 * Maintainer: lixi <lixi@uniontech.com>
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

#ifndef PHASEWALLPAPERDCONFIG
#define PHASEWALLPAPERDCONFIG

#include <DConfig>
#include <QObject>
#include <QString>

#include "dconfigsettings.h"

DCORE_USE_NAMESPACE

inline QString generateWpIndexKey(const QString &arg1, const QString &arg2)
{
    return arg1 + "+" + arg2;
}

// Dconfig 配置类
class PhaseWallPaper : public DconfigSettings
{
public:
    explicit PhaseWallPaper();
    ~PhaseWallPaper();

public:
    static void setWallpaperUri(const QString &index, const QString &strMonitorName, const QString &uri);
    static QString getWallpaperUri(const QString &index,const QString &strMonitorName);
};

#endif // PHASEWALLPAPERDCONFIG
