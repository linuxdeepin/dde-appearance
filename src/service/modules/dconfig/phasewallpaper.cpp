#include "phasewallpaper.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

const QString appearanceProcessId = "org.deepin.dde.appearance";
const QString appearanceDconfJson = "org.deepin.dde.appearance";
const QString allWallpaperUrisKey = "All_Wallpaper_Uris";

PhaseWallPaper::PhaseWallPaper()
{
}

PhaseWallPaper::~PhaseWallPaper()
{
}

QString phaseWPType(const QString &index, const QString &strMonitorName)
{
    QString shouldGetWPType = "";
    if (index != "" && strMonitorName != "") {
        shouldGetWPType = "index+monitorName";
    } else if (index != "" && strMonitorName == "") {
        shouldGetWPType = "onlyIndex";
    } else {
        return "";
    }

    return shouldGetWPType;
}

void PhaseWallPaper::setWallpaperUri(const QString &index, const QString &strMonitorName, const QString &uri)
{
    QString wpIndexKey = generateWpIndexKey(index, strMonitorName);
    QString shouldGetWPType = phaseWPType(index, strMonitorName);
    if (shouldGetWPType == "") {
        qWarning() << QString("set wall paper type error, index [%1] strMonitorName [%2]").arg(index, strMonitorName);
        return;
    }

    QVariant v = DconfigSettings::ConfigValue(appearanceProcessId, appearanceDconfJson, allWallpaperUrisKey, "");
    if (!v.isValid()) {
        return;
    }

    bool shouldAddWPTypeInfo = true;
    QJsonArray allWallpaperUri = v.toJsonArray();

    for (QJsonArray::Iterator it1 = allWallpaperUri.begin(); it1 != allWallpaperUri.end(); ++it1) {
        QJsonObject wpTypeObj = it1[0].toObject();
        if (!wpTypeObj.contains("type")) {
            continue;
        }

        QString wpType = wpTypeObj["type"].toString();
        if (wpType != shouldGetWPType) {
            continue;
        }

        shouldAddWPTypeInfo = false;
        if (!wpTypeObj.contains("wallpaperInfo")) {
            continue;
        }

        bool shouldAddWPInfo = true;
        QJsonArray wpInfoArray = wpTypeObj["wallpaperInfo"].toArray();
        for (QJsonArray::Iterator it2 = wpInfoArray.begin(); it2 != wpInfoArray.end(); ++it2) {
            QJsonObject wpInfoObj = it2[0].toObject();
            if (!wpInfoObj.contains("uri") || !wpInfoObj.contains("wpIndex")) {
                continue;
            }

            if (wpInfoObj["wpIndex"] == wpIndexKey) {
                wpInfoObj["uri"] = uri;
                shouldAddWPInfo = false;
            } else {
                continue;
            }

            it2[0] = wpInfoObj;
            shouldAddWPTypeInfo = false;
        }

        wpTypeObj["wallpaperInfo"] = wpInfoArray;
        if (shouldAddWPInfo) {
            QJsonObject obj = {
                {"uri", uri,},
                {"wpIndex", wpIndexKey}
            };

            QJsonArray array = wpTypeObj["wallpaperInfo"].toArray();
            array.append(obj);
            wpTypeObj["wallpaperInfo"] = array;
        }
        it1[0] = wpTypeObj;
    }

    if (shouldAddWPTypeInfo) {
        QJsonObject obj1 = {
            {"uri", uri,},
            {"wpIndex", wpIndexKey}
        };
        QJsonArray array = { obj1 };
        QJsonObject obj2 = {
            {"type", shouldGetWPType},
            {"wallpaperInfo", array}
        };

        allWallpaperUri.append(obj2);
    }

    DconfigSettings::ConfigSaveValue(appearanceProcessId, appearanceDconfJson, allWallpaperUrisKey, allWallpaperUri.toVariantList());
    return;
}

QString PhaseWallPaper::getWallpaperUri(const QString &index, const QString &strMonitorName)
{
    QString wpIndexKey = generateWpIndexKey(index, strMonitorName);
    QString shouldGetWPType = phaseWPType(index, strMonitorName);
    if (shouldGetWPType == "") {
        qWarning() << QString("set wall paper type error, index [%1] strMonitorName [%2]").arg(index, strMonitorName);
        return "";
    }

    QVariant v = DconfigSettings::ConfigValue(appearanceProcessId, appearanceDconfJson, allWallpaperUrisKey, "");
    if (!v.isValid()) {
        return "";
    }

    QJsonArray allWallpaperUri = v.toJsonArray();
    for (auto wallpaper : allWallpaperUri) {
        QJsonObject wpObj = wallpaper.toObject();
        if (!wpObj.contains("type")) {
            continue;
        }

        QString wpType = wpObj["type"].toString();
        if (wpType != shouldGetWPType) {
            continue;
        }

        if (!wpObj.contains("wallpaperInfo")) {
            continue;
        }

        QJsonArray wpInfoArray = wpObj["wallpaperInfo"].toArray();
        for (auto wpInfo : wpInfoArray) {
            QJsonObject obj = wpInfo.toObject();
            if (!obj.contains("uri") || !obj.contains("wpIndex")) {
                continue;
            }

            if (obj["wpIndex"] == wpIndexKey) {
                return obj["uri"].toString();
            }
        }
    }

    return "";
}
