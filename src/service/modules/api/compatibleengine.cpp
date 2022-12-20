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
#include "compatibleengine.h"

#include <DDciIcon>
#include <QFileInfo>

class CompatibleEnginePrivate
{
public:
    CompatibleEnginePrivate(CompatibleEngine *parent, QString name)
        : q_ptr(parent)
        , m_name(name)
    {
        QString dciFile = "/usr/share/dsg/icons/" + QIcon::themeName() + "/" + m_name + ".dci";
        if (QFileInfo::exists(dciFile)) {
            m_dicon = DTK_GUI_NAMESPACE::DDciIcon(dciFile);
        } else {
            m_icon = QIcon::fromTheme(m_name);
        }
    }

public:
    CompatibleEngine *q_ptr;
    Q_DECLARE_PUBLIC(CompatibleEngine)

    QString m_name;
    DTK_GUI_NAMESPACE::DDciIcon m_dicon;
    QIcon m_icon;
};

CompatibleEngine::CompatibleEngine(QString name)
    : QIconEngine()
    , d_ptrCompatibleEngine(new CompatibleEnginePrivate(this, name))
{
}

void CompatibleEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    Q_D(CompatibleEngine);
    if (!d->m_dicon.isNull()) {
        d->m_dicon.paint(painter, rect, 1.0, DTK_GUI_NAMESPACE::DDciIcon::Light);
    } else if (!d->m_icon.isNull()) {
        d->m_icon.paint(painter, rect, Qt::AlignCenter, mode, state);
    }
}

QIconEngine *CompatibleEngine::clone() const
{
    Q_D(const CompatibleEngine);
    return new CompatibleEngine(d->m_name);
}

void CompatibleEngine::virtual_hook(int id, void *data)
{
    Q_D(CompatibleEngine);
    switch (id) {
    case QIconEngine::IsNullHook:
        *((bool *)(data)) = d->m_dicon.isNull() && d->m_icon.isNull();
        break;
    }
}
