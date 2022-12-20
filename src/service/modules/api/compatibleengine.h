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
#ifndef COMPATIBLEENGINE_H
#define COMPATIBLEENGINE_H

#include <QIconEngine>

class CompatibleEnginePrivate;
class CompatibleEngine : public QIconEngine
{
public:
    CompatibleEngine(QString name);

    virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
    virtual QIconEngine *clone() const override;
    void virtual_hook(int id, void *data) override;

private:
    QScopedPointer<CompatibleEnginePrivate> d_ptrCompatibleEngine;
    Q_DECLARE_PRIVATE_D(d_ptrCompatibleEngine, CompatibleEngine)
    Q_DISABLE_COPY(CompatibleEngine)
};

#endif // COMPATIBLEENGINE_H
