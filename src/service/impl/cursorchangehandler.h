#ifndef CURSORCHANGEHANDER_H
#define CURSORCHANGEHANDER_H

#include <QThread>

class CursorChangeHandler : public QThread
{
public:
    CursorChangeHandler();
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
