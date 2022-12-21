#ifndef FSNOTIFY_H
#define FSNOTIFY_H

#include <QObject>
#include <QDir>
#include <QFileSystemWatcher>
#include <QDateTime>

#include "modules/background/backgrounds.h"

class QTimer;
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
    void watchGlobalDirs();
    void watchDirs(QStringList dirs);
    bool hasEventOccurred(QString name, QStringList lists);

public Q_SLOTS:
    void onFileChanged(const QString &path);
    void onTimeOut();

Q_SIGNALS:
    void themeFileChange(QString theme);

private:
    QStringList gtkDirs;
    QStringList iconDirs;
    QStringList bgDirs;
    qint64 prevTimestamp;
    QSharedPointer<QFileSystemWatcher> fileWatcher;
    QSharedPointer<Backgrounds> backgrounds;
    QTimer *timer;
    QSet<QString> changedThemes;
};

#endif // FSNOTIFY_H
