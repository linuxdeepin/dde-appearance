#include "fsnotify.h"
#include "../modules/common/commondefine.h"

Fsnotify::Fsnotify(QObject *parent)
    : QObject(parent)
    , fileWatcher(new QFileSystemWatcher())
    , backgrounds(new Backgrounds())
    , prevTimestamp(0)
{
    watchGtkDirs();
    watchIconDirs();
    watchGlobalDirs();
    watchBgDirs();
    QString tmpFilePrefix = backgrounds->getCustomWallpapersConfigDir() + "/temp-";
    connect(fileWatcher.data(),&QFileSystemWatcher::directoryChanged,this, [ = ](const QString &path) {
        if(path.startsWith(tmpFilePrefix)){
            return ;
        }
        qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qint64 tmp = timestamp - prevTimestamp;
        prevTimestamp = timestamp;

        if(tmp > 1000)
        {
            if (hasEventOccurred(path, bgDirs)){
                Q_EMIT themeFileChange(TYPEBACKGROUND);
            } else if(hasEventOccurred(path, gtkDirs)) {
                Q_EMIT themeFileChange(TYPEGTK);
            } else if(hasEventOccurred(path, iconDirs)) {
                Q_EMIT themeFileChange(TYPEICON);
            } else if (path.contains("deepin-themes")) {
                Q_EMIT themeFileChange(TYPEGLOBALTHEME);
            }
        }
    });
}

Fsnotify::~Fsnotify()
{

}

void Fsnotify::watchGtkDirs()
{
    auto home = qgetenv("HOME");
    gtkDirs.append(home + "/.local/share/themes");
    gtkDirs.append(home + "/.themes");
    gtkDirs.append("/usr/local/share/themes");
    gtkDirs.append("/usr/share/themes");
    watchDirs(gtkDirs);
}

void Fsnotify::watchIconDirs()
{
    auto home = qgetenv("HOME");
    iconDirs.append(home + "/.local/share/icons");
    iconDirs.append(home + "/.icons");
    iconDirs.append("/usr/local/share/icons");
    iconDirs.append("/usr/share/icons");
    watchDirs(iconDirs);
}

void Fsnotify::watchBgDirs()
{
    bgDirs = backgrounds->listDirs();
    watchDirs(bgDirs);
}

void Fsnotify::watchGlobalDirs()
{
    QStringList globalDirs;
    QDir home = QDir::home();
    globalDirs.append(home.absoluteFilePath(".cache/deepin/dde-appearance/deepin-themes/"));
    globalDirs.append(home.absoluteFilePath(".local/share/deepin-themes"));
    globalDirs.append(home.absoluteFilePath(".deepin-themes"));
    globalDirs.append("/usr/local/share/deepin-themes");
    globalDirs.append("/usr/share/deepin-themes");
    watchDirs(globalDirs);
}

void Fsnotify::watchDirs(QStringList dirs)
{
    QDir qdir;
    //TODO 文件创建失败 监听失败
    for(auto dir : dirs) {
        if(!qdir.exists(dir)) {
            qdir.mkpath(dir);
            qInfo() << "mkpath:" << dir;
        }
        if(!fileWatcher->addPath(dir)) {
            qInfo() << "filewatcher add path failed" << dir << __FUNCTION__;
        }
    }
}

bool Fsnotify::hasEventOccurred(QString name, QStringList lists)
{
    for(auto list : lists) {
        if(list.contains(name))
            return true;
    }
    return false;
}
