#include "utils.h"
#include "../common/commondefine.h"

#include <QFile>
#include <QMutex>
#include <QDir>
#include <QUrl>
#include <pwd.h>
#include <unistd.h>

utils::utils()
{

}

bool utils::WriteStringToFile(QString filename, QString content)
{
    if (filename.length() == 0) {
        return false;
    }
    QMutex writeMutex = QMutex();
    writeMutex.lock();
    QString swapFile = filename + "/.swap";
    QDir dir(swapFile);
    if (!dir.mkpath(swapFile)) {
        return false;
    }

    QFile file(swapFile);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    file.write(content.toLatin1(), content.length());
    file.close();

    if (!file.rename(filename))
        return false;

    writeMutex.unlock();

    return true;
}

bool utils::isURI(QString uri)
{
    if(uri.indexOf("://") != -1)
        return true;

    return false;
}

bool utils::isDir(QString path)
{
    QDir qdir(path);
    if (!qdir.exists(path)) {
//        qInfo() << "no dir";
        return false;
    }

    return true;
}

bool utils::isFilesInDir(QVector<QString> files, QString dir)
{
    if(!isDir(dir)) {
        return false;
    }

    for (auto file : files) {
        QString tmp = dir + "/" + file;
        QFile qfile(tmp);
        if (!qfile.exists()) {
            return false;
        }
    }

    return true;
}

bool utils::isFileExists(QString filename)
{
    QString path = utils::deCodeURI(filename);
    if(QFile::exists(filename)) {
        return true;
    }

    return false;
}

QString utils::deCodeURI(QString uri)
{
    QString path;
    if(isURI(uri)) {
        QUrl Url(uri);
        path = Url.path();
    } else {
        path = uri;
    }

    return path;
}

QString utils::enCodeURI(QString content, QString scheme)
{
    QString path;
    if(isURI(content)) {
        path = deCodeURI(content);
    } else {
        path = content;
    }
    return scheme + path;
}

QString utils::GetUserHomeDir()
{
    const char *dir = getenv(HOME);

    if (dir) {
        return dir;
    }

    struct passwd *user = getpwuid(getuid());
    if (user) {
        return user->pw_dir;
    }

    return "";
}

QString utils::GetUserDataDir()
{
    // default $HOME/.local/share
    QString userDataDir = getUserDir("XDG_DATA_HOME");

    if (userDataDir.isEmpty()) {
        userDataDir = GetUserHomeDir();
        if (!userDataDir.isEmpty()) {
            userDataDir += "/.local/share";
        }
    }
    return userDataDir;
}

QString utils::GetUserConfigDir()
{
    // default $HOME/.config
    QString userConfigDir = getUserDir("XDG_CONFIG_HOME");

    if (userConfigDir.isEmpty()) {
        userConfigDir = GetUserHomeDir();
        if (!userConfigDir.isEmpty()) {
            userConfigDir += "/.config";
        }

    }
    return userConfigDir;
}

QString utils::getUserDir(const char *envName)
{
    const char *envDir = getenv(envName);
    if (envDir == nullptr) {
        return "";
    }

    if (!QDir::isAbsolutePath(envDir)) {
        return "";
    }

    return envDir;
}

QVector<QString> utils::GetSystemDataDirs()
{
    QVector<QString> systemDir = getSystemDirs("XDG_DATA_DIRS");

    if (systemDir.empty()) {
        systemDir.push_back("/usr/local/share");
        systemDir.push_back("usr/share");
    }

    return systemDir;
}

QVector<QString> utils::GetSystemConfigDirs()
{
    QVector<QString> systemDir = getSystemDirs("XDG_CONFIG_DIRS");

    if (systemDir.empty()) {
        systemDir.push_back("/etc/xdg");
    }

    return systemDir;
}

QVector<QString> utils::getSystemDirs(const char *envName)
{
    QVector<QString> dirVector;
    const char *envDir = getenv(envName);
    if (envDir == nullptr) {
        return dirVector;
    }

    QString tempDirs(envDir);
    auto tempList= tempDirs.split(":");

    for (auto iter:tempList) {
        if (QDir::isAbsolutePath(iter)) {
            dirVector.push_back(iter);
        }
    }

    return dirVector;
}

