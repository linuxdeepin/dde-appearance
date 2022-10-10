#ifndef SUBTHEMES_
#define SUBTHEMES_

#include "scanner.h"
#include "theme.h"
#include "../api/themes.h"

#include <QObject>
#include <vector>
#include <DConfig>

class QThread;

class Subthemes : public QObject
{
    Q_OBJECT
public:
    Subthemes(QObject *parent = nullptr);
    virtual ~Subthemes();

    void refreshGtkThemes();
    void refreshIconThemes();
    void refreshCursorThemes();
    void refreshGlobalThemes();
    QVector<QSharedPointer<Theme>> listGtkThemes();
    QVector<QSharedPointer<Theme>> listIconThemes();
    QVector<QSharedPointer<Theme>> listCursorThemes();
    QVector<QSharedPointer<Theme>> listGlobalThemes();
    bool deleteGtkTheme(const QString& name);
    bool deleteIconTheme(const QString& name);
    bool deleteCursorTheme(const QString& name);
    bool isGlobalTheme(QString id);
    bool isGtkTheme(QString id);
    bool isIconTheme(QString id);
    bool isCursorTheme(QString id);
    bool setGlobalTheme(QString id);
    bool setGtkTheme(QString id);
    bool setIconTheme(QString id);
    bool setCursorTheme(QString id);
    QString getGlobalThumbnail(QString id);
    QString getGtkThumbnail(QString id);
    QString getIconThumbnail(QString id);
    QString getCursorThumbnail(QString id);
    QVector<QSharedPointer<Theme>> getThemes(QVector<QString> files);
    bool isDeletable(QString file);
    bool isItemInList(QString item, QVector<QString> lists);
    QString getBasePath(QString filename);
    QMap<QString,QString>& getGtkThumbnailMap();

protected Q_SLOTS:
    void createGlobalThumbnail(const QString path, const QString filename);
    void createGtkThumbnail(const QString path, const QString filename);
    void createIconThumbnail(const QString path, const QString filename);
    void createCursorThumbnail(const QString path, const QString filename);

Q_SIGNALS:
    void requestGlobalThumbnail(const QString &path, const QString &filename);
    void requestGtkThumbnail(const QString &path, const QString &filename);
    void requestIconThumbnail(const QString &path, const QString &filename);
    void requestCursorThumbnail(const QString &path, const QString &filename);

private:
    QSharedPointer<ThemesApi>          themeApi;
    QVector<QSharedPointer<Theme>>     gtkThemes;
    QVector<QSharedPointer<Theme>>     iconThemes;
    QVector<QSharedPointer<Theme>>     cursorThemes;
    QVector<QSharedPointer<Theme>>     globalThemes;
    QMap<QString,QString>              gtkThumbnailMap;
    QThread                            *thread;
};

#endif
