#include "wallpaperscheduler.h"
#include "../modules/api/utils.h"
#include "../modules/common/commondefine.h"

#include <QDebug>
#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

WallpaperScheduler::WallpaperScheduler(BGCHANGEFUNC func)
    :stopScheduler(false)
{
    this->bgChangeFunc =func;
    connect(&changeTimer, SIGNAL(timeout()), this, SLOT(handleDetectSysClockTimeOut()));
}

void WallpaperScheduler::setInterval(QString monitorSpace, qint64 interval)
{
    this->monitorSpace = monitorSpace;
    this->interval =    interval;
    stopScheduler =false;
    QDateTime curr = QDateTime::currentDateTimeUtc();

    qint64 elapsed = lastSetBgTime.secsTo(curr);

    if(elapsed < interval)
    {
        changeTimer.start(static_cast<int>(interval - elapsed));
    }else {
        handleChangeTimeOut();
    }
}

void WallpaperScheduler::setLastChangeTime(QDateTime date)
{
    lastSetBgTime = date;
}

void WallpaperScheduler::stop()
{
    stopScheduler = true;
}

void WallpaperScheduler::handleChangeTimeOut()
{
    QDateTime curr = QDateTime::currentDateTimeUtc();
    if(bgChangeFunc)
    {
        bgChangeFunc(this->monitorSpace,curr);
    }

    if(!stopScheduler)
    {
        changeTimer.start(static_cast<int>(interval));
    }
}


WallpaperLoop::WallpaperLoop()
    :fileChange(true)
    ,rander(QRandomGenerator::global())
{

}

QStringList WallpaperLoop::getShowed()
{
    return showedList;
}

QStringList WallpaperLoop::getNotShowed()
{
    QStringList retList;
    if(fileChange)
    {
       QVector<Background> bgs = backgrounds.listBackground();
       for(auto iter : bgs)
       {
           allList.push_back(utils::deCodeURI(iter.getId()));
       }
    }

    for(auto iter : allList)
    {
        if(showedList.indexOf(iter) == -1)
        {
            retList.push_back(iter);
        }
    }
    return retList;
}

QString WallpaperLoop::getNext()
{
    mutex.lock();
    QString next = getNextShow();
    if(!next.isEmpty())
    {
        mutex.unlock();
        return next;
    }

    if(!allList.isEmpty())
    {
        reset();
        next = getNext();
    }

    return next;
}

QString WallpaperLoop::getNextShow()
{
    QStringList notShowList = getNotShowed();
    if(notShowList.empty())
    {
        return "";
    }

    int index = rander->bounded(notShowList.size());
    QString nextWallpaper = notShowList[index];

    showedList.push_back(notShowList[index]);

    return nextWallpaper;
}

void WallpaperLoop::reset()
{
    showedList.clear();
}

void WallpaperLoop::addToShow(QString file)
{
    file = utils::deCodeURI(file);

    mutex.lock();
    showedList.push_back(file);
    mutex.unlock();
}

void WallpaperLoop::notifyFileChange()
{
    mutex.lock();
    fileChange = true;
    mutex.unlock();
}

WallpaperLoopConfigManger::WallpaperLoopConfigManger()
{

}

WallpaperLoopConfigManger::WallpaperLoopConfigMap WallpaperLoopConfigManger::loadWSConfig(QString fileName)
{
    wallpaperLoopConfigMap.clear();

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<fileName<<" open fail";
        return wallpaperLoopConfigMap;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(),&err);
    if(err.error != QJsonParseError::NoError)
    {
        qDebug()<<fileName<<" parse fail";
        return wallpaperLoopConfigMap;
    }

    QJsonObject obj = doc.object();

    for(auto key :obj.keys())
    {
        WallpaperLoopConfig config;
        wallpaperLoopConfigMap[key]=config;

        QJsonObject wlConfigObj = obj[key].toObject();

        for(auto wlConfigObjKey :wlConfigObj.keys())
        {
            if(wlConfigObjKey == "LastChange")
            {
                wallpaperLoopConfigMap[key].lastChange = QDateTime::fromString(wlConfigObj[key].toString(), "yyyy-MM-dd hh:mm:ss");
            }

            if(wlConfigObjKey == "Showed")
            {
                QJsonArray arr = wlConfigObj[key].toArray();
                for(auto iter :arr)
                {
                    wallpaperLoopConfigMap[key].showedList.push_back(iter.toString());
                }
            }
        }
    }

    return wallpaperLoopConfigMap;
}

void WallpaperLoopConfigManger::setShowed(QString monitorSpace, QStringList showedList)
{
    if(wallpaperLoopConfigMap.count(monitorSpace) == 0)
    {
        wallpaperLoopConfigMap[monitorSpace] = WallpaperLoopConfig();
    }
    wallpaperLoopConfigMap[monitorSpace].showedList = showedList;
}

void WallpaperLoopConfigManger::setLastChange(QString monitorSpace, QDateTime date)
{
    if(wallpaperLoopConfigMap.count(monitorSpace) == 0)
    {
        wallpaperLoopConfigMap[monitorSpace] = WallpaperLoopConfig();
    }
    wallpaperLoopConfigMap[monitorSpace].lastChange = date;
}

bool WallpaperLoopConfigManger::save(QString fileName)
{
    QJsonDocument doc;
    QJsonObject obj;
    for(auto wallPaperConfig : wallpaperLoopConfigMap.toStdMap())
    {
        QJsonObject config;
        config["LastChange"] = wallPaperConfig.second.lastChange.toString();

        QJsonArray showedArr;
        for(auto showedFile : wallPaperConfig.second.showedList)
        {
            showedArr.push_back(showedArr);
        }
        config["Showed"] = showedArr;

        obj[wallPaperConfig.first] = config;
    }

    doc.setObject(obj);
    QByteArray text = doc.toJson(QJsonDocument::Compact);

    QDir dir(fileName.left(fileName.lastIndexOf("/")));
    if(!dir.exists())
    {
         if(!dir.mkpath(dir.path()))
         {
             qDebug()<< "mkpath"<<dir.path()<<"fail";
             return false;
         }
    }

    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        qDebug()<<fileName<<" open fail";
        return false;
    }

    file.write(text);
    file.close();

    return true;
}

bool WallpaperLoopConfigManger::isValidWSPolicy(QString policy)
{
    if(policy == WSPOLICYLOGIN || policy == WSPOLICYWAKEUP || policy.isEmpty())
    {
        return true;
    }
    bool ok;
    policy.toUInt(&ok);

    return ok;
}
