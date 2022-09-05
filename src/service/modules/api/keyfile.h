#ifndef KEYFILE_H
#define KEYFILE_H

#include <QMap>
#include <QVector>
#include <QFile>

typedef QMap<QString, QString> KeyMap;
typedef QMap<QString, KeyMap> MainKeyMap;

// 解析ini、desktop文件类
class KeyFile
{
public:
    explicit KeyFile(char separtor = ';');
    ~KeyFile();

    bool getBool(const QString &section, const QString &key, bool defaultValue = false);

    QString getStr(const QString &section, const QString &key, QString defaultValue = "");
    bool containKey(const QString &section, const QString &key);
    QString getLocaleStr(const QString &section, const QString &key, QString defaultLocale = "");
    QStringList getStrList(const QString &section, const QString &key);
    QStringList getLocaleStrList(const QString &section, const QString &key, QString defaultLocale = "");

    void setKey(const QString &section, const QString &key, const QString &value);
    bool saveToFile(const QString &filePath);
    bool loadFile(const QString &filePath);
    QStringList getMainKeys();


    // for test
    void print();

private:
    MainKeyMap mainKeyMap; // section -> key : value
    QString filePath;
    QFile fp;
    bool modified;
    char listSeparator;
};

#endif // KEYFILE_H
