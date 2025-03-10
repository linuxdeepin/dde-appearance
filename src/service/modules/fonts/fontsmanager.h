// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FONTSMANAGER_H
#define FONTSMANAGER_H
#include <QString>
#include <QStringList>
#include <QMap>
#include <fontconfig/fontconfig.h>
#include <QSharedPointer>
#include <DConfig>
#include <QScopedPointer>
#include <QMap>

using Dtk::Core::DConfig;

class FontsManager
{
public:
    struct Family{
        QString     id;
        QString     name;
        QStringList styles;
        bool        monospace;
        bool        show;
    };
    struct FcInfo{
        char *family;
        char *familylang;
        /* char *fullname; */
        /* char *fullnamelang; */
        char *style;
        char *lang;
        char *spacing;
    };

    struct IrregularFontOverride {
        QStringList AppendLang;
    };

    using IrregularFontOverrideMap = QMap<QString, IrregularFontOverride>;
public:
    FontsManager();
    void refreshFamilyList();
    bool isFontFamily(QString value);
    bool isFontSizeValid(double size);
    bool setFamily(QString standard, QString monospace, double size);
    bool reset();
    QStringList listMonospace();
    QStringList listStandard();
    QSharedPointer<Family> getFamily(QString id);
    QVector<QSharedPointer<Family>> getFamilies(QStringList ids);
    double getFontSize();
    bool checkFontConfVersion();
    QSharedPointer<DConfig> xSetting;

private:
    QString fcFontMatch(QString family);
    static QString md5(QString src);
    static QString getStringFromUnsignedChar(unsigned char *str, unsigned int length);
    static QString fontMatch(QString family);
    static QString configContent(QString standard, QString monospace);
    bool   isFcCacheUpdate();
    FcInfo* listFontInfo (int *num);
    int appendFontinfo(FcInfo** list, FcPattern* pat, int idx);
    void freeFontInfoList(FcInfo *list, int num);
    QSharedPointer<Family> fcInfoToFamily(FcInfo* info);
    QString getCurLang();
    QString getLangFromLocale(QString locale);
    void fcInfosToFamilyTable();
    bool loadCacheFromFile(QString fileName);
    bool saveToFile();
    void loadIrregularFontOverrideMap();
private:
    QStringList                             virtualFonts;
    QString                                 filePath;
    QMap<QString,QSharedPointer<Family>>    familyMap;
    QStringList                             familyBlacklist;
    IrregularFontOverrideMap                irregularFontOverrideMap;
    QScopedArrayPointer<DConfig>            appearanceConfig;
};

#endif // FONTSMANAGER_H
