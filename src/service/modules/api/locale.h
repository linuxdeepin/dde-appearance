#ifndef LOCALE_H
#define LOCALE_H

#include <QString>
#include <QVector>
#include <QMap>

// 本地化类
class Locale {
    struct LanguageNameCache {
        QString language;
        QVector<QString> names;
        pthread_mutex_t mutex;
    };

    struct Components {
        Components() : mask(0) {}   // 数字必须初始化
        QString language;
        QString territory;
        QString codeset;
        QString modifier;
        uint mask;
    };

public:
    QVector<QString> getLocaleVariants(const QString &locale);
    QVector<QString> getLanguageNames();
    static inline Locale *instance() {
        static Locale instance;
        return &instance;
    }

private:
    Locale();
    Locale(const Locale &);
    Locale& operator= (const Locale &);

    Components explodeLocale(QString locale);
    QString guessCategoryValue(QString categoryName);
    QString unaliasLang(QString);
    QMap<QString, QString> aliases;
    LanguageNameCache languageNames;
};

#endif
