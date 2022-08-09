#include "scanner.h"
#include "../api/utils.h"
#include "../common/commondefine.h"

Scanner::Scanner(QObject *parent)
    :QObject (parent)
{

}

Scanner::~Scanner()
{

}

bool Scanner::isHidden(QString file, QString ty)
{
    KeyFile keyFile;
    keyFile.loadFile(file);

    bool hidden = false;

    if(ty == TYPEGTK) {
        hidden = keyFile.getBool("Desktop Entry","Hidden");
    } else if (ty == TYPEICON || ty == TYPECURSOR) {
        hidden = keyFile.getBool("Icon Theme","Hidden");
    }

    return hidden;
}

QString Scanner::query(QString uri)
{
    QString path = utils::deCodeURI(uri);
    QString mime = queryThemeMime(path);
    if(!mime.isEmpty())
    {
        return mime;
    }

    return doQueryFile(path);
}

QString Scanner::queryThemeMime(QString file)
{
    bool gtk = gtkTheme(file);
    if(gtk) {
        return MIMETYPEGTK;
    }

    bool icon = iconTheme(file);
    if(icon) {
        return MIMETYPEICON;
    }

    bool cursor = cursorTheme(file);
    if(cursor) {
        return MIMETYPECURSOR;
    }

    return "";
}

QString Scanner::doQueryFile(QString file)
{
    GError **err = nullptr;
    if (!utils::isFileExists(file)) {
        return "";
    }

    GFile *g_file = g_file_new_for_path(file.toLatin1().data());
    const QString attributes = "standard::content-type";

    GFileInfo *fileinfo = g_file_query_info(g_file, attributes.toLatin1().data(), GFileQueryInfoFlags(G_FILE_QUERY_INFO_NONE), nullptr ,err);
    if(err != nullptr){
        qInfo() << "g_file_query_info failed" << __FUNCTION__ << __LINE__;
        return "";
    }

    const char *attribute = g_file_info_get_attribute_string(fileinfo, attributes.toLatin1().data());
    QString attributeString(attribute);
    qInfo() << "attributeString" << attributeString << __FUNCTION__ << __LINE__;

    return attributeString;
}

bool Scanner::gtkTheme(QString file)
{
    return utils::isFilesInDir({"gtk-2.0","gtk-3.0","metacity-1"}, file.left(file.lastIndexOf("/")));
}

bool Scanner::iconTheme(QString file)
{
    KeyFile keyfile;
    keyfile.loadFile(file);
    if(!keyfile.loadFile(file)) {
        return false;
    }


    if(keyfile.getStr("Icon Theme","Directories").isEmpty()) {
        return false;
    }

    return true;
}

bool Scanner::cursorTheme(QString file)
{
    QString dir = file.left(file.lastIndexOf("/"));
    QString tmp = dir + "/cursors" + "/left_ptr";
    QString mime = doQueryFile(tmp);

    if(mime.isEmpty() || mime != MIMETYPEXCURSOR) {
        return false;
    }

    return true;
}

bool Scanner::isGtkTheme(QString uri)
{
    if(uri.size() == 0)
        return false;

    QString ty = query(uri);

    qInfo() << "isGtkTheme" << uri << ty << __FUNCTION__ << __LINE__;
    return ty == MIMETYPEGTK;
}

bool Scanner::isIconTheme(QString uri)
{
    if(uri.size() == 0)
        return false;

    QString ty = query(uri);
    qInfo() << "isIconTheme" << uri << ty << __FUNCTION__ << __LINE__;

    return ty == MIMETYPEICON;
}

bool Scanner::isCursorTheme(QString uri)
{
    if(uri.size() == 0)
        return false;

    QString ty = query(uri);
    qInfo() << "isCursorTheme" << uri << ty << __FUNCTION__ << __LINE__;

    return ty == MIMETYPECURSOR;
}

QVector<QString> Scanner::listSubDir(QString path)
{
    QVector<QString> subDirs;

    if(!utils::isDir(path)) {
        return subDirs;
    }

    QDir dir(path);
    QFileInfoList filenames = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);

    for(auto filename : filenames) {
        qInfo() << "filename = " << filename.filePath() << __FUNCTION__ << __LINE__;
        subDirs .push_back(filename.fileName());
    }

    for(auto sub : subDirs) {
        qInfo() << "sub = " << sub << __FUNCTION__ << __LINE__;
    }

    qInfo() << "sub.size = " << subDirs.size() << __FUNCTION__ << __LINE__;

    return subDirs;
}

QVector<QString> Scanner::doListTheme(QString uri, QString ty, Fn fn)
{
    QString path = utils::deCodeURI(uri);

    qInfo() << "path = " << path << __FUNCTION__ << __LINE__;
    QVector<QString> subDirs = listSubDir(path);
    if(!subDirs.size()) {
        qInfo() << "subDirs is empty";
    }

    QVector<QString> themes;
    for(auto subDir : subDirs) {
        QString tmp;
        if(ty == TYPECURSOR) {
            tmp = path + "/" +subDir + "/cursor.theme";
        } else {
            tmp = path + "/" + subDir + "/index.theme";
        }
        qInfo() << "subDir = " << subDir << __FUNCTION__ << __LINE__;

        if(!fn(tmp) || isHidden(tmp, ty))
            continue;
        qInfo() << "subDir = " << subDir << __FUNCTION__ << __LINE__;
        themes.push_back(tmp);
    }

    return themes;
}

QVector<QString> Scanner::listGtkTheme(QString uri)
{
    qInfo() << "listGtkTheme" << uri << __FUNCTION__ << __LINE__;
    Fn fn = std::bind(&Scanner::isGtkTheme, this,std::placeholders::_1);
    return doListTheme(uri, TYPEGTK, fn);
}

QVector<QString> Scanner::listIconTheme(QString uri)
{
    Fn fn = std::bind(&Scanner::isIconTheme, this,std::placeholders::_1);
    return doListTheme(uri, TYPEICON, fn);
}

QVector<QString> Scanner::listCursorTheme(QString uri)
{
    Fn fn = std::bind(&Scanner::isCursorTheme, this,std::placeholders::_1);
    return doListTheme(uri, TYPECURSOR, fn);
}
