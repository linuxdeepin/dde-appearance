#include "subthemes.h"
#include "../api/themethumb.h"
#include "../common/commondefine.h"
#include <QDir>
#include <QDebug>

const QString gsKeyExcludedIcon = "excluded-icon-themes";

Subthemes::Subthemes(QObject *parent)
    : QObject(parent)
    , themeApi(new ThemesApi())
{
    refreshGtkThemes();
    refreshIconThemes();
    refreshCursorThemes();
    gtkThumbnailMap["deepin"] ="light";
    gtkThumbnailMap["deepin-dark"] ="dark";
    gtkThumbnailMap["deepin-auto"] ="auto";
}

Subthemes::~Subthemes()
{

}


void Subthemes::refreshGtkThemes()
{
    gtkThemes = getThemes(themeApi->listGtkTheme());
}

void Subthemes::refreshIconThemes()
{
    iconThemes.clear();
    QVector<QSharedPointer<Theme>> infos = getThemes(themeApi->listIconTheme());

    QStringList blacklist;
    DConfig dConfig(APPEARANCESCHEMA);
    if(dConfig.isValid()){
        blacklist = dConfig.value(gsKeyExcludedIcon).toStringList();
    }

    for (auto info : infos) {
        if(blacklist.indexOf(info->getId()) != -1)
        {
            continue;
        }
        iconThemes.push_back(info);
    }
}

void Subthemes::refreshCursorThemes()
{
    cursorThemes = getThemes(themeApi->listCursorTheme());
}

QVector<QSharedPointer<Theme>> Subthemes::listGtkThemes()
{
    if(gtkThemes.empty()) {
        refreshGtkThemes();
    }

    return gtkThemes;
}

QVector<QSharedPointer<Theme>> Subthemes::listIconThemes()
{
    if(iconThemes.empty()) {
        refreshIconThemes();
    }

    return iconThemes;
}

QVector<QSharedPointer<Theme>> Subthemes::listCursorThemes()
{
    if(cursorThemes.empty()) {
        refreshCursorThemes();
    }

    return cursorThemes;
}

bool Subthemes::deleteGtkTheme(const QString& name)
{
    QVector<QSharedPointer<Theme>>::Iterator iter = gtkThemes.begin();
    while(iter != gtkThemes.end()){
        if((*iter)->getId() == name)
        {
            (*iter)->Delete();
            iter = gtkThemes.erase(iter);
            return true;
        }else {
            iter++;
        }
    }

    return false;
}

bool Subthemes::deleteIconTheme(const QString& name)
{
    QVector<QSharedPointer<Theme>>::Iterator iter = iconThemes.begin();
    while(iter != iconThemes.end()){
        if((*iter)->getId() == name)
        {
            (*iter)->Delete();
            iter = iconThemes.erase(iter);
            return true;
        }else {
            iter++;
        }
    }

    return false;
}

bool Subthemes::deleteCursorTheme(const QString& name){
    QVector<QSharedPointer<Theme>>::Iterator iter = cursorThemes.begin();
    while(iter != cursorThemes.end()){
        if((*iter)->getId() == name)
        {
            (*iter)->Delete();
            iter = cursorThemes.erase(iter);
            return true;
        }else {
            iter++;
        }
    }

    return false;
}

bool Subthemes::isGtkTheme(QString id)
{
    for(auto iter : gtkThemes)
    {
        if(iter->getId() == id)
        {
            return true;
        }
    }
    return false;
}

bool Subthemes::isIconTheme(QString id)
{
    for(auto iter : iconThemes)
    {
        if(iter->getId() == id)
        {
            return true;
        }
    }
    return false;
}

bool Subthemes::isCursorTheme(QString id)
{
    for(auto iter : cursorThemes)
    {
        if(iter->getId() == id)
        {
            return true;
        }
    }
    return false;
}

bool Subthemes::setGtkTheme(QString id)
{
    return themeApi->setGtkTheme(id);
}

bool Subthemes::setIconTheme(QString id)
{
    return themeApi->setIconTheme(id);
}

bool Subthemes::setCursorTheme(QString id)
{
    return  themeApi->setCursorTheme(id);
}

QString Subthemes::getGtkThumbnail(QString id)
{
    QSharedPointer<Theme> theme;
    for(auto iter: gtkThemes)
    {
        if(iter->getId()==id)
        {
            theme = iter;
        }
    }

    if(!theme)
    {
        return "";
    }

    QString path = theme->getPath()+"/index.theme";
    return getGtk(id,path);
}

QString Subthemes::getIconThumbnail(QString id)
{
    QSharedPointer<Theme> theme;
    for(auto iter: iconThemes)
    {
        if(iter->getId()==id)
        {
            theme = iter;
        }
    }

    if(!theme)
    {
        return "";
    }

    QString path = theme->getPath()+"/index.theme";
    return getIcon(id,path);
}

QString Subthemes::getCursorThumbnail(QString id)
{
    QSharedPointer<Theme> theme;
    for(auto iter: cursorThemes)
    {
        if(iter->getId()==id)
        {
            theme = iter;
        }
    }

    if(!theme)
    {
        return "";
    }

    QString path = theme->getPath()+"/index.theme";
    return getCursor(id,path);
}

QVector<QSharedPointer<Theme>> Subthemes::getThemes(QVector<QString> files)
{
    QVector<QSharedPointer<Theme>> infos;
    for (auto file : files) {

        QSharedPointer<Theme> theme(new Theme(file));
        infos.push_back(theme);
    }

    return infos;
}

bool Subthemes::isDeletable(QString file)
{
    QString home = getenv("HOME");


    if(file.indexOf(home)!=-1){
        return true;
    }

    return false;
}

bool Subthemes::isItemInList(QString item, QVector<QString> lists)
{
    for(auto list : lists) {
        if(item == list) {
            return true;
        }
    }

    return false;
}

QString Subthemes::getBasePath(QString filename)
{
    int index = filename.lastIndexOf("/");

    return filename.mid(0,index);
}

QMap<QString,QString>& Subthemes::getGtkThumbnailMap()
{
    return gtkThumbnailMap;
}
