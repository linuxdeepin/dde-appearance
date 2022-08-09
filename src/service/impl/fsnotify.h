#ifndef FSNOTIFY_H
#define FSNOTIFY_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QFileSystemWatcher>
#include <QDateTime>

#include "modules/background/backgrounds.h"

class Fsnotify : public QObject
{
    Q_OBJECT
public:
    Fsnotify(QObject *parent = nullptr);
    ~Fsnotify();

public:
    void watchGtkDirs();
    void watchIconDirs();
    void watchBgDirs();
    void watchDirs(QStringList dirs);
    bool hasEventOccurred(QString name, QStringList lists);

Q_SIGNALS:
    void themeFileChange(QString theme);
private:
    QStringList gtkDirs;
    QStringList iconDirs;
    QStringList bgDirs;
    qint64 prevTimestamp;
    QSharedPointer<QFileSystemWatcher>  fileWatcher;
    QSharedPointer<Backgrounds>         backgrounds;
};

#endif // FSNOTIFY_H
