#include "themes.h"
#include "utils.h"
#include "../common/commondefine.h"

#include <gtk/gtk.h>
#include <xcb/xcb_cursor.h>


ThemesApi::ThemesApi(QObject *parent)
    : QObject(parent)
    , scanner(new Scanner())
    , gtk2Mutex(QMutex())
    , gtk3Mutex(QMutex())
    , wmDbusInterface(new QDBusInterface("com.deepin.wm", "/com/deepin/wm",
                                         "com.deepin.wm", QDBusConnection::sessionBus()))
{
    if(QGSettings::isSchemaInstalled(XSETTINGSSCHEMA))
    {
        xSetting = QSharedPointer<QGSettings>(new QGSettings(XSETTINGSSCHEMA));
    }

    if(QGSettings::isSchemaInstalled(METACITYSCHEMA))
    {
        metacitySetting = QSharedPointer<QGSettings>(new QGSettings(METACITYSCHEMA));
    }

    if(QGSettings::isSchemaInstalled(WMSCHEMA))
    {
        wmSetting = QSharedPointer<QGSettings>(new QGSettings(WMSCHEMA));
    }

    if(QGSettings::isSchemaInstalled(INTERFACESCHEMA))
    {
        interfaceSetting = QSharedPointer<QGSettings>(new QGSettings(INTERFACESCHEMA));
    }
}

ThemesApi::~ThemesApi()
{
}

bool ThemesApi::isThemeInList(QString theme, QVector<QString> list)
{
    int index = theme.lastIndexOf("/");
    QString name = theme.mid(0,index);
    for(auto l : list) {
        index = l.lastIndexOf("/");

        if(name == l.mid(0,index))
            return true;
    }
    return false;
}

QVector<QString> ThemesApi::listGtkTheme()
{
    QVector<QString> local;
    QString home = getenv("HOME");
    local.push_back(home + "/.local/share/themes");
    local.push_back(home + "/.themes");

    QVector<QString> sys;
    sys.push_back("/usr/share/themes");

    return doListTheme(local, sys, TYPEGTK);
}

QVector<QString> ThemesApi::listIconTheme()
{
    QVector<QString> local;
    QString home = getenv("HOME");
    local.push_back(home + "/.local/share/icons");
    local.push_back(home + "/.icons");

    QVector<QString> sys;
    sys.push_back("/usr/share/icons");

    return doListTheme(local, sys, TYPEICON);
}

QVector<QString> ThemesApi::listCursorTheme()
{
    QVector<QString> local;
    QString home = getenv("HOME");
    local.push_back(home + "/.local/share/icons");
    local.push_back(home + "/.icons");

    QVector<QString> sys;
    sys.push_back("/usr/share/icons");

    return doListTheme(local, sys, TYPECURSOR);
}

QVector<QString> ThemesApi::doListTheme(QVector<QString> local, QVector<QString> sys, QString type)
{
    QVector<QString> lists = scanThemeDirs(local, type);
    QVector<QString> syslists = scanThemeDirs(sys, type);

    qInfo() << "doListTheme" << lists.size() << syslists.size() << __FUNCTION__ << __LINE__;

    return mergeThemeList(lists, syslists);
}

QVector<QString> ThemesApi::scanThemeDirs(QVector<QString> dirs, QString type)
{
    QVector<QString> lists;
    for(auto dir : dirs) {
        QVector<QString> tmp;
        if(type == TYPEGTK)
        {
            tmp = scanner->listGtkTheme(dir);
        }else if (type == TYPECURSOR) {
            tmp = scanner->listCursorTheme(dir);
        }else if (type == TYPEICON){
            tmp = scanner->listIconTheme(dir);
        }else {
            break;
        }

        lists.append(tmp);
    }

    return lists;
}

QVector<QString> ThemesApi::mergeThemeList(QVector<QString> src, QVector<QString> target)
{
    qInfo() << "mergeThemeList";
    if(target.size() == 0) {
        return src;
    }

    for(auto t: target) {
        if(isThemeInList(t, src)) {
            continue;
        }
        src.push_back(t);
    }

    return src;
}

bool ThemesApi::setWMTheme(QString name)
{ 
    if(metacitySetting)
    {
        metacitySetting->set("theme",name);
    }

    if(!wmSetting)
    {
        return false;
    }

    wmSetting->set("theme", name);

    return true;
}

bool ThemesApi::setGtkTheme(QString name)
{
    if(!scanner->isGtkTheme(getThemePath(name, TYPEGTK, "themes"))) {
        qWarning() << "isGtkTheme failed";
        return false;
    }

    setGtk2Theme(name);

    setGtk3Theme(name);

    if(!xSetting)
    {
        return false;
    }
    QString old = xSetting->get(XSKEYTHEME).toString();
    if (old == name) {
        qWarning() << "getXSettingsValue failed";
        return false;
    }

    xSetting->set(XSKEYTHEME,name);

    if (!setWMTheme(name)) {
        xSetting->set(XSKEYTHEME,old);
        qWarning() << "setWMTheme failed";
        return false;
    }

    if (!setQTTheme()) {
        xSetting->set(XSKEYTHEME,old);
        setWMTheme(old);
        qWarning() << "setQTTheme failed";
        return false;
    }

    return true;
}

bool ThemesApi::setIconTheme(QString name)
{
    if(!scanner->isIconTheme(getThemePath(name, TYPEICON, "icons"))) {
        qWarning() << "isIconTheme failed";
        return false;
    }

    setGtk2Icon(name);

    setGtk3Icon(name);

    if(!xSetting)
    {
        return false;
    }
    QString old = xSetting->get(XSKEYICONTHEME).toString();
    if (old == name) {
        return false;
    }

    xSetting->set(XSKEYICONTHEME,name);

    return true;
}

bool ThemesApi::setCursorTheme(QString name)
{
    if(!scanner->isCursorTheme(getThemePath(name, TYPECURSOR, "icons"))) {
        qWarning() << "isCursorTheme failed";
        return false;
    }

    setGtk2Cursor(name);

    setGtk3Cursor(name);

    setDefaultCursor(name);

    if(!xSetting)
    {
        return false;
    }

    QString old = xSetting->get(XSKEYCURSORNAME).toString();
    if (old == name) {
        return false;
    }

    xSetting->set(XSKEYCURSORNAME,name);

    setQtCursor(name);
    setGtkCursor(name);
    setWMCursor(name);

    return true;
}

QString ThemesApi::getThemePath(QString name, QString ty, QString key)
{
    QVector<QString> dirs;
    QString home = getenv("HOME");
    dirs.push_back(home + "/.local/share/" + key);
    dirs.push_back(home + "." + key);
    dirs.push_back("usr/lical/share/" + key);
    dirs.push_back("/usr/share/" + key);

    for(auto dir : dirs) {
        QString tmp = dir + "/" + name;

        if(!utils::isFileExists(tmp)) {
            continue;
        }

        if (ty == TYPEGTK || ty == TYPEICON) {
            return utils::enCodeURI(tmp + "/index.theme", SCHEME_FILE);
        } else if (ty == TYPECURSOR) {
            return utils::enCodeURI(tmp + "/cursor.theme", SCHEME_FILE);
        }
    }

    return "";
}

void ThemesApi::setGtk2Theme(QString name)
{
    setGtk2Prop("gtk-theme-name", "\""+name+"\"", getGtk2ConfFile());
}

void ThemesApi::setGtk2Icon(QString name)
{
    setGtk2Prop("gtk-icon-theme-name", "\""+name+"\"", getGtk2ConfFile());
}

void ThemesApi::setGtk2Cursor(QString name)
{
    setGtk2Prop("gtk-cursor-theme-name", "\""+name+"\"", getGtk2ConfFile());
}

void ThemesApi::setGtk2Prop(QString key, QString value, QString file)
{
    QMutexLocker gtk2MutexLocker(&gtk2Mutex);
    gtk2FileReader(file);
    QString info = getGtk2ConfInfo(key);
    if (info.isEmpty()) {
        addGtk2ConfInfo(key, value);
    } else {
        if (info == value) {
            return;
        }
        addGtk2ConfInfo(key, value);
    }

    gtk2FileWriter(file);
}

void ThemesApi::gtk2FileReader(QString file)
{
    QFile qfile(file);
    if(!qfile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    while (!qfile.atEnd())
    {
        QString line = qfile.readLine();
        if(line.length()==0){
            continue;
        }



        QStringList strv = line.split(GTK2CONFDELIM);
        if(strv.size() != 2){
            continue;
        }

        addGtk2ConfInfo(strv[0], strv[1]);
    }

    qfile.close();
}

QString ThemesApi::getGtk2ConfInfo(QString key)
{
    return gtk2ConfInfos.value(key);
}

void ThemesApi::addGtk2ConfInfo(QString key, QString value)
{
    gtk2ConfInfos[key] = value;
}

void ThemesApi::gtk2FileWriter(QString file)
{
    QStringList content;
    for (auto it = gtk2ConfInfos.cbegin();it !=gtk2ConfInfos.end();it++) {
        content.append(it.key() + GTK2CONFDELIM + it.value());
    }

    QFile qfile(file);
    if (!qfile.exists()) {
        QDir dir(file.left(file.lastIndexOf("/")));
        dir.mkpath(file.left(file.lastIndexOf("/")));
        qInfo() << "mkpath" << file;
    }

    qfile.open(QIODevice::WriteOnly);
    qfile.write(content.join("\n").toLatin1());
    qfile.close();
}

void ThemesApi::setGtk3Theme(QString name)
{
    setGtk3Prop(GTK3KEYTHEME, name, getGtk3ConfFile());
}

void ThemesApi::setGtk3Icon(QString name)
{
    setGtk3Prop(GTK3KEYICON, name, getGtk3ConfFile());
}

void ThemesApi::setGtk3Cursor(QString name)
{
    setGtk3Prop(GTK3KEYCURSOR, name, getGtk3ConfFile());
}

void ThemesApi::setGtk3Prop(QString key, QString value, QString file)
{
    QMutexLocker gtk3MutexLocker(&gtk3Mutex);
    QFile qfile(file);
    if (!qfile.exists()) {
        QDir dir(file.left(file.lastIndexOf("/")));
        dir.mkpath(file.left(file.lastIndexOf("/")));
    }
    KeyFile keyfile;
    if (!keyfile.loadFile(file)) {
        return;
    }

    if (isGtk3PropEqual(key, value, keyfile)) {
        return;
    }

    doSetGtk3Prop(key, value, file, keyfile);
}

bool ThemesApi::isGtk3PropEqual(QString key, QString value, KeyFile& keyfile)
{
    QString old = keyfile.getStr(GTK3GROUPSETTINGS, key);
    return old == value;
}

void ThemesApi::doSetGtk3Prop(QString key, QString value, QString file, KeyFile& keyfile)
{
    keyfile.setKey(GTK3GROUPSETTINGS, key, value);
    keyfile.saveToFile(file);
}

bool ThemesApi::setQTTheme()
{
    QString config = g_get_user_config_dir();
    config += "/Trolltech.conf";
    return setQt4Theme(config);
}

bool ThemesApi::setQt4Theme(QString config)
{
    if (!utils::isFileExists(config)) {
        return false;
    }

    KeyFile keyfile;
    keyfile.loadFile(config);

    QString value = keyfile.getStr("Qt", "style");
    if (value == "GTK+")
       return true;

    if (config.length() == 0)
       return false;

    QFile file(config);
    if (file.exists()) {
       QDir dir(config.left(config.lastIndexOf("/")));
       if (!dir.mkpath(config.left(config.lastIndexOf("/"))))
           return false;
    }

    keyfile.setKey("Qt", "style", "GTK+");

    return keyfile.saveToFile(config);
}

bool ThemesApi::setDefaultCursor(QString name)
{
    QString file = utils::GetUserHomeDir() + "/.icons/default/index.theme";
    if (utils::isFileExists(file)) {
        QDir qdir(file.left(file.lastIndexOf("/")));
        if (!qdir.mkpath(file.left(file.lastIndexOf("/")))) {
            return false;
        }
    }

    KeyFile keyfile;
    keyfile.loadFile(file);

    QString value = keyfile.getStr("Icon Theme", "Inherits");
    if (value == "GTK+")
       return true;

    if (file.length() == 0)
       return false;

    QFile qfile(file);
    if (qfile.exists()) {
       QDir dir(file.left(file.lastIndexOf("/")));
       if (!dir.mkpath(file.left(file.lastIndexOf("/"))))
           return false;
    }

    keyfile.setKey("Icon Theme", "Inherits", name);
    return keyfile.saveToFile(file);
}

void ThemesApi::setGtkCursor(QString name)
{
    QByteArray ba = name.toLatin1();
    char* date = ba.data();
    GtkSettings* s = gtk_settings_get_default();
    g_object_set(G_OBJECT(s), "gtk-cursor-theme-name", date, NULL);
}

void ThemesApi::setQtCursor(QString name)
{
    xcb_connection_t * conn;
    xcb_screen_t* screen=nullptr;
    int screen_nbr;
    xcb_screen_iterator_t iter;

    conn = xcb_connect(nullptr, &screen_nbr);

    iter = xcb_setup_roots_iterator(xcb_get_setup(conn));

    for(; iter.rem; --screen_nbr, xcb_screen_next(&iter))
    {
        if (screen_nbr == 0)
        {
            screen = iter.data;
            break;
        }
    }
    if(!screen)
    {
        qWarning()<<"get screen fail";
        return;
    }

    xcb_cursor_context_t *ctx;
    if (xcb_cursor_context_new(conn, screen, &ctx) < 0)
    {
        qWarning()<<"xcb_cursor_context_new fail";
        return;
    }

    xcb_cursor_t cid = xcb_cursor_load_cursor(ctx, name.toLatin1());

    screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    xcb_change_window_attributes(conn, screen->root, XCB_CW_CURSOR, static_cast<void*>(&cid));
    xcb_flush(conn);

    xcb_cursor_context_free(ctx);
    xcb_disconnect(conn);
}

void ThemesApi::setWMCursor(QString name)
{
    if(interfaceSetting)
    {
        interfaceSetting->set("cursorTheme",name);
    }

    if(wmDbusInterface)
    {
        wmDbusInterface->setProperty("cursorTheme",QString(name));
    }
}

QString ThemesApi::getGtk2ConfFile()
{
    QString path = utils::GetUserHomeDir();
    path= path + "/.gtkrc-2.0";

    return path;
}

QString ThemesApi::getGtk3ConfFile()
{
    QString path = utils::GetUserHomeDir();
    path= path + "/.config/gtk-3.0/settings.ini";

    return path;
}
