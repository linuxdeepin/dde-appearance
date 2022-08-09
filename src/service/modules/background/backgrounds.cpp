#include "backgrounds.h"
#include "../api/utils.h"
#include "../common/commondefine.h"

#include <QDBusInterface>
#include <pwd.h>
#include <QDBusReply>

QStringList Backgrounds::systemWallpapersDir = {"/usr/share/wallpapers/deepin"};
QStringList Backgrounds::uiSupportedFormats = {"jpeg", "png", "bmp", "tiff", "gif"};

Backgrounds::Backgrounds(QObject *parent)
    : QObject(parent)
    , backgroundsMu(new QMutex())
    , fsChanged(false)
{
    init();
}

Backgrounds::~Backgrounds()
{

}

bool Backgrounds::deleteBackground(const QString& uri)
{
    QVector<Background>::iterator iter=backgrounds.begin();

    while (iter!=backgrounds.end()) {
        if((*iter).getId() == uri){
            (*iter).Delete();
            iter = backgrounds.erase(iter);
            return true;
        }else {
            iter++;
        }
    }

    return false;
}

void Backgrounds::init()
{
    QString configPath = g_get_user_config_dir();
    customWallpapersConfigDir = configPath +"/deepin/dde-daemon/appearance/custom-wallpapers";
    QDir qdir;
    if(!qdir.exists(customWallpapersConfigDir))
    {
        qdir.mkdir(customWallpapersConfigDir);
        qInfo() << "mkdir: " << customWallpapersConfigDir;
    }

    refreshBackground();
}

QStringList Backgrounds::listDirs()
{
    QStringList result;
    result.append(systemWallpapersDir);
    result.append(customWallpapersConfigDir);
    return result;
}

void Backgrounds::refreshBackground()
{
    QStringList files = getCustomBgFiles();
    for (auto file : files) {
        qInfo() << "custom = " << file << __FUNCTION__ << __LINE__;
        Background bg;
        bg.setId(utils::enCodeURI(file, SCHEME_FILE));
        bg.setDeletable(true);
        backgrounds.push_back(bg);
    }

    files = getSysBgFIles();
    for (auto file : files) {
        qInfo() << "system = " << file << __FUNCTION__ << __LINE__;
        Background bg;
        bg.setId(utils::enCodeURI(file, SCHEME_FILE));
        bg.setDeletable(true);
        backgrounds.push_back(bg);
    }
    fsChanged=false;
}

void Backgrounds::sortByTime(QFileInfoList listFileInfo)
{
    qSort(listFileInfo.begin(), listFileInfo.end(), [ = ](const QFileInfo &f1, QFileInfo &f2){
        return  f1.lastModified().toTime_t() < f2.lastModified().toTime_t();
    });
}



QStringList Backgrounds::getSysBgFIles()
{
    QStringList files;
    for (auto dir : systemWallpapersDir) {
        files.append(getBgFilesInDir(dir));
    }
    return files;
}

QStringList Backgrounds::getCustomBgFiles()
{
    QStringList ret;
    QDBusInterface daemon("com.deepin.daemon.Daemon","/com/deepin/daemon/Daemon","com.deepin.daemon.Daemon",QDBusConnection::systemBus());

     struct passwd *user = getpwuid(getuid());
     if(user==nullptr)
     {
         return ret;
     }

     QDBusReply<QStringList> reply = daemon.call("GetCustomWallPapers",user->pw_name);
     if(!reply.isValid())
     {
         return ret;
     }
     ret=reply.value();

     return ret;
}

QStringList Backgrounds::getCustomBgFilesInDir(QString dir)
{
    QStringList wallpapers;
    QDir qdir(dir);
    if (!qdir.exists())
        return wallpapers;

    QFileInfoList fileInfoList = qdir.entryInfoList(QDir::NoSymLinks);
    sortByTime(fileInfoList);

    for (auto info : fileInfoList) {
        if (info.isDir()) {
            continue;
        }
        if (!isBackgroundFile(info.path())) {
            continue;
        }
        wallpapers.append(info.path());
    }

    return wallpapers;
}

QStringList Backgrounds::getBgFilesInDir(QString dir)
{
    QStringList walls;

    QDir qdir(dir);
    if (!qdir.exists())
        return walls;

    QFileInfoList fileInfoList = qdir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    for (auto file : fileInfoList) {
        if (!isBackgroundFile(file.filePath())) {
            continue;
        }
        walls.append(file.filePath());
    }

    return walls;
}

bool Backgrounds::isFileInDirs(QString file, QStringList dirs)
{
    for (auto dir : dirs) {
        QFileInfo qfile(file);
        if (qfile.dir() == dir)
            return true;
    }

    return false;
}

bool Backgrounds::isBackgroundFile(QString file)
{
    file = utils::deCodeURI(file);

    QString format = FormatPicture::getPictureType(file);
    if (format == "") {
        return false;
    }

    if (uiSupportedFormats.contains(format)) {
        return true;
    }

    return false;
}

QVector<Background> Backgrounds::listBackground()
{
    backgroundsMu->lock();

    if (backgrounds.length() == 0 && fsChanged)
        refreshBackground();

    backgroundsMu->unlock();
    return backgrounds;
}

void Backgrounds::notifyChanged()
{
    backgroundsMu->lock();
    fsChanged = true;
    backgroundsMu->unlock();
}

QString Backgrounds::prepare(QString file)
{
    QString tempFile = utils::deCodeURI(file);
    if(isFileInDirs(tempFile,systemWallpapersDir))
    {
        return tempFile;
    }

    // todo
    return prepare(file);
}

