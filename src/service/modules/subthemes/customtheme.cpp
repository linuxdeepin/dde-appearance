// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "customtheme.h"

#include "modules/api/keyfile.h"
#include "modules/api/utils.h"
#include "modules/common/commondefine.h"
#include "impl/appearancemanager.h"
#include "theme.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>

const QString CUSTOMTHEMEPATH =
        QString("%1/dde-appearance/deepin-themes/custom").arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));

#define THEMEFILE "index.theme"

CustomTheme::CustomTheme(QObject *parent)
    : QObject(parent)
    , m_customTheme(new KeyFile(','))
{
    openCustomTheme();
}

void CustomTheme::updateValue(const QString &type, const QString &value, const QString &oldTheme, const QVector<QSharedPointer<Theme>> &globalThemes)
{
    static const QMap<QString, QString> typekeyMap = { { TYPEWALLPAPER, "Wallpaper" },
                                                       { TYPEGREETERBACKGROUND, "LockBackground" },
                                                       { TYPEICON, "IconTheme" },
                                                       { TYPECURSOR, "CursorTheme" },
                                                       { TYPEGTK, "AppTheme" },
                                                       { TYPESTANDARDFONT, "StandardFont" },
                                                       { TYPEMONOSPACEFONT, "MonospaceFont" },
                                                       { TYPEFONTSIZE, "FontSize" },
                                                       { TYPEACTIVECOLOR, "ActiveColor" },
                                                       { TYPEDOCKOPACITY, "DockOpacity" },
                                                       { TYPWINDOWRADIUS, "WindowRadius" },
                                                       { TYPEWINDOWOPACITY, "WindowOpacity" } };
    if (!typekeyMap.contains(type))
        return;

    enum GolbalThemeMode {
        Light = 1,
        Dark = 2,
        Auto = 3,
    };

    QString themeId = oldTheme;
    QString modeStr;
    if (themeId.endsWith(".light")) {
        themeId.chop(6);
        modeStr = ".light";
    } else if (oldTheme.endsWith(".dark")) {
        themeId.chop(5);
        modeStr = ".dark";
    }
    if (themeId != "custom") {
        QString themePath;
        for (auto iter : globalThemes) {
            if (iter->getId() == themeId) {
                themePath = iter->getPath();
                break;
            }
        }
        copyTheme(themeId, themePath, typekeyMap.values());
        Q_EMIT updateToCustom(modeStr);
    }

    if (type == TYPEACTIVECOLOR) {
        QStringList colors = value.split(',');
        m_customTheme->setKey("DefaultTheme", typekeyMap.value(type), colors.value(0));
        m_customTheme->setKey("DarkTheme", typekeyMap.value(type), colors.value(1));
        saveCustomTheme();
        return;
    }

    // 自定义主题深浅色配置统一
    m_customTheme->setKey("DefaultTheme", typekeyMap.value(type), value);
    m_customTheme->setKey("DarkTheme", typekeyMap.value(type), value);
    saveCustomTheme();
}

void CustomTheme::openCustomTheme()
{
    QDir root = QDir::root();
    root.cd(CUSTOMTHEMEPATH);
    m_customTheme->loadFile(root.absoluteFilePath(THEMEFILE));
    m_customTheme->removeSection("Deepin Theme");
    m_customTheme->setKey("Deepin Theme", "Name", "Custom");
    m_customTheme->setKey("Deepin Theme", "DefaultTheme", "DefaultTheme");
    m_customTheme->setKey("Deepin Theme", "DarkTheme", "DarkTheme");
    m_customTheme->setKey("Deepin Theme", "Example", "/usr/share/dde-appearance/custom.png");
}

void CustomTheme::saveCustomTheme()
{
    QDir root = QDir::root();
    root.mkpath(CUSTOMTHEMEPATH);
    root.cd(CUSTOMTHEMEPATH);
    m_customTheme->saveToFile(root.absoluteFilePath(THEMEFILE));
}

void CustomTheme::copyTheme(const QString themeId, const QString &themePath, const QStringList &keys)
{
    if (themePath.isEmpty())
        return;
    KeyFile theme(',');
    theme.loadFile(themePath + "/index.theme");
    QString defaultTheme = theme.getStr("Deepin Theme", "DefaultTheme");
    if (defaultTheme.isEmpty())
        return;
    QString darkTheme = theme.getStr("Deepin Theme", "DarkTheme");
    if (darkTheme.isEmpty())
        darkTheme = defaultTheme;

    const QStringList fileItem = { "Wallpaper", "LockBackground" };
    auto setKey = [fileItem, themePath, this](const QString &section, const QString &key, const QString &oldValue) {
        QString value = oldValue;
        if (fileItem.contains(key)) {
            value = utils::deCodeURI(value);
            QFileInfo fileInfo(value);
            if (!fileInfo.isAbsolute()) {
                value = themePath + "/" + value;
            }
        }
        m_customTheme->setKey(section, key, value);
    };

    // 预先构建 override 映射
    QHash<QString, QString> defaultThemeOverrides;
    QHash<QString, QString> darkThemeOverrides;
    
    const auto& overrideList = AppearanceManager::getGlobalOverride().value(themeId);
    for (const auto &overrideItem : overrideList) {
        if (overrideItem.section == "DefaultTheme") {
            defaultThemeOverrides[overrideItem.key] = overrideItem.value;
        } else if (overrideItem.section == "DarkTheme") {
            darkThemeOverrides[overrideItem.key] = overrideItem.value;
        }
    }

    for (auto &&key : keys) {
        QString defaultValue = defaultThemeOverrides.value(key);
        if (defaultValue.isEmpty()) {
            defaultValue = theme.getStr(defaultTheme, key);
        }
        setKey("DefaultTheme", key, defaultValue);

        QString darkValue = darkThemeOverrides.value(key);
        if (darkValue.isEmpty()) {
            darkValue = theme.getStr(darkTheme, key, theme.getStr(defaultTheme, key));
        }
        setKey("DarkTheme", key, darkValue);
    }
}
