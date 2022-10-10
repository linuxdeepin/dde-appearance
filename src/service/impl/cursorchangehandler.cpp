#include "cursorchangehandler.h"

#include <QDebug>
#include <gtk/gtk.h>

int     CursorChangeHandler::sigId=0;
bool    CursorChangeHandler::endFlag=false;

CursorChangeHandler::CursorChangeHandler(QObject *parent)
    : QThread(parent)
{
    gtk_init(nullptr,nullptr);
    bRun=false;
}

CursorChangeHandler::~CursorChangeHandler()
{
    stop();
}

void CursorChangeHandler::start()
{
    bRun=true;
    QThread::start();
}

void CursorChangeHandler::run()
{
    while (bRun) {
        msleep(1000);
        handleGtkCursorChange();
    }
}

void CursorChangeHandler::stop()
{
    bRun=false;
    gtk_main_quit();
}

void CursorChangeHandler::endCursorChangeHandler()
{
    endFlag =true;
}

void CursorChangeHandler::handleGtkCursorChange()
{
    if(sigId > 0)
    {
        qDebug()<<"Cursor changed handler has running";
        return;
    }

    GtkSettings* gtkSetting = gtk_settings_get_default();
    sigId = static_cast<int>(g_signal_connect(gtkSetting,"notify::gtk-cursor-theme-name",updateGtkCursor, nullptr));

    if(sigId < 0)
    {
        qWarning()<<"Connect gtk cursor changed failed!";
        return;
    }

    gtk_main();
}

void CursorChangeHandler::updateGtkCursor()
{
    if(endFlag)
    {
        return;
    }

    GdkCursor* cursor = gdk_cursor_new_for_display(gdk_display_get_default(),GDK_LEFT_PTR);
    gdk_window_set_cursor(gdk_get_default_root_window(), cursor);
    g_object_unref(G_OBJECT(cursor));
}
