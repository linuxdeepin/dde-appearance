#ifndef SUBTHEMES_
#define SUBTHEMES_

#include "scanner.h"
#include "theme.h"
#include "../api/themes.h"

#include <QObject>
#include <vector>
#include <DConfig>

DCORE_USE_NAMESPACE

class Subthemes : public QObject
{
    Q_OBJECT
public:
    Subthemes(QObject *parent = nullptr);
    virtual ~Subthemes();

    void refreshGtkThemes();
    void refreshIconThemes();
    void refreshCursorThemes();
    QVector<QSharedPointer<Theme>> listGtkThemes();
    QVector<QSharedPointer<Theme>> listIconThemes();
    QVector<QSharedPointer<Theme>> listCursorThemes();
    bool deleteGtkTheme(const QString& name);
    bool deleteIconTheme(const QString& name);
    bool deleteCursorTheme(const QString& name);
    bool isGtkTheme(QString id);
    bool isIconTheme(QString id);
    bool isCursorTheme(QString id);
    bool setGtkTheme(QString id);
    bool setIconTheme(QString id);
    bool setCursorTheme(QString id);
    QString getGtkThumbnail(QString id);
    QString getIconThumbnail(QString id);
    QString getCursorThumbnail(QString id);
    QVector<QSharedPointer<Theme>> getThemes(QVector<QString> files);
    bool isDeletable(QString file);
    bool isItemInList(QString item, QVector<QString> lists);
    QString getBasePath(QString filename);
    QMap<QString,QString>& getGtkThumbnailMap();

private:
    QSharedPointer<ThemesApi>          themeApi;
    QVector<QSharedPointer<Theme>>     gtkThemes;
    QVector<QSharedPointer<Theme>>     iconThemes;
    QVector<QSharedPointer<Theme>>     cursorThemes;
    QMap<QString,QString>              gtkThumbnailMap;
};

#endif
