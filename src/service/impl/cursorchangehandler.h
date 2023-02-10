// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CURSORCHANGEHANDER_H
#define CURSORCHANGEHANDER_H

#include <QThread>

class CursorChangeHandler : public QThread
{
public:
    explicit CursorChangeHandler(QObject *parent = nullptr);
    ~CursorChangeHandler();

    void start();
    void stop();
    static void endCursorChangeHandler();
protected:
    void run();

private:
    void handleGtkCursorChange();
    static void updateGtkCursor();

private:
    bool bRun;
    static int  sigId;
    static bool endFlag;
};

#endif // CURSORCHANGEHANDER_H
