#include "subthemes.h"
#include "impl/appearancemanager.h"
#include "../api/themethumb.h"
#include "../common/commondefine.h"
#include <QDir>
#include <QThread>
#include <QDebug>

DCORE_USE_NAMESPACE

const QString gsKeyExcludedIcon = "Excluded_Icon_Themes";

Subthemes::Subthemes(AppearanceManager *parent)
    : QObject()
    , themeApi(new ThemesApi(parent))
    , thread(new QThread(this))
{
    moveToThread(thread);
    thread->start(QThread::LowestPriority);
    connect(this, &Subthemes::requestGlobalThumbnail, this, &Subthemes::createGlobalThumbnail, Qt::QueuedConnection);
    connect(this, &Subthemes::requestGtkThumbnail, this, &Subthemes::createGtkThumbnail, Qt::QueuedConnection);
    connect(this, &Subthemes::requestIconThumbnail, this, &Subthemes::createIconThumbnail, Qt::QueuedConnection);
    connect(this, &Subthemes::requestCursorThumbnail, this, &Subthemes::createCursorThumbnail, Qt::QueuedConnection);

    refreshGtkThemes();
    refreshIconThemes();
    refreshCursorThemes();
    refreshGlobalThemes();
    gtkThumbnailMap["deepin"] ="light";
    gtkThumbnailMap["deepin-dark"] ="dark";
    gtkThumbnailMap["deepin-auto"] ="auto";

    for (auto &&theme : cursorThemes) {
        getCursorThumbnail(theme->getId());
    }
    for (auto &&theme : iconThemes) {
        getIconThumbnail(theme->getId());
    }
}

Subthemes::~Subthemes()
{
    thread->quit();
    thread->wait();
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
        KeyFile keyFile(',');
        keyFile.loadFile(info->getPath()+"/index.theme");
        info->setName(keyFile.getLocaleStr("Icon Theme","Name"));
        info->setComment(keyFile.getLocaleStr("Icon Theme","Comment"));
        iconThemes.push_back(info);
    }
}

void Subthemes::refreshCursorThemes()
{
    cursorThemes = getThemes(themeApi->listCursorTheme());
    for(auto &&info:cursorThemes) {
        KeyFile keyFile(',');
        keyFile.loadFile(info->getPath()+"/cursor.theme");
        info->setName(keyFile.getLocaleStr("Icon Theme","Name"));
        info->setComment(keyFile.getLocaleStr("Icon Theme","Comment"));
    }
}

void Subthemes::refreshGlobalThemes()
{
    globalThemes.clear();
//    // 路径同图标主题
//    QVector<QString> themePaths = themeApi->listIconTheme();
//    // 增加自定义主题
//    themePaths.append("");
    QVector<QSharedPointer<Theme>> infos = getThemes(themeApi->listGlobalTheme());

//    QStringList blacklist;
//    DConfig dConfig(APPEARANCESCHEMA);
//    if(dConfig.isValid()){
//        blacklist = dConfig.value(gsKeyExcludedIcon).toStringList();
//    }

    for (auto &&info : infos) {
        KeyFile keyFile(',');
        keyFile.loadFile(info->getPath()+"/index.theme");
        info->setName(keyFile.getLocaleStr("Deepin Theme","Name"));
        info->setComment(keyFile.getLocaleStr("Deepin Theme","Comment"));
        QStringList example =keyFile.getStrList("Deepin Theme","Example");

        for (QList<QString>::iterator i = example.begin(); i != example.end(); ++i) {
            QFileInfo file(*i);
            if (file.isRelative()) {
                QDir themefile(info->getPath());
                file.setFile(themefile, *i);
                *i = file.absoluteFilePath();
            }
        }
        info->setExample(example.join(','));
        info->setHasDark(keyFile.containKey("Deepin Theme","DarkTheme"));
        globalThemes.push_back(info);
    }
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

QVector<QSharedPointer<Theme> > Subthemes::listGlobalThemes()
{
    if(globalThemes.empty()) {
        refreshGlobalThemes();
    }

    return globalThemes;
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

bool Subthemes::isGlobalTheme(QString id)
{
    for(auto iter : globalThemes)
    {
        if(iter->getId() == id)
        {
            return true;
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

bool Subthemes::setGlobalTheme(QString id)
{
    return themeApi->setGlobalTheme(id);
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

QString Subthemes::getGlobalThumbnail(QString id)
{
    QSharedPointer<Theme> theme;
    for(auto iter: globalThemes)
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
    return getGlobal(id,path);
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
    if (!checkScaleFactor()) {
        qInfo() << "scaleFactor <= 0";
        return "";
    }

    QString out = prepareOutputPath("gtk", id, cursorVersion);
    Q_EMIT requestGtkThumbnail(path,out);
    return out;
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

    QString path = theme->getPath();
    if (!checkScaleFactor()) {
        qInfo() << "scaleFactor <= 0";
        return "";
    }

    QString out = prepareOutputPath("icon", id, iconVersion);
    Q_EMIT requestIconThumbnail(path,out);
    return out;
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

    QString path = theme->getPath();

    if (!checkScaleFactor()) {
        qInfo() << "scaleFactor <= 0";
        return "";
    }

    QString out = prepareOutputPath("cursor", id, cursorVersion);
    Q_EMIT requestCursorThumbnail(path,out);
    return out;
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

void Subthemes::createGlobalThumbnail(const QString path, const QString filename)
{
    CreateGlobalThumbnail(path, filename);
}

void Subthemes::createGtkThumbnail(const QString path, const QString filename)
{
    CreateGtkThumbnail(path, filename);
}

void Subthemes::createIconThumbnail(const QString path, const QString filename)
{
    CreateIconThumbnail(path, filename);
}

void Subthemes::createCursorThumbnail(const QString path, const QString filename)
{
    CreateCursorThumbnail(path, filename);
}
