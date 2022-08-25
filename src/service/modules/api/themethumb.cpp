#include "themethumb.h"
#include "../api/dfile.h"
#include "../common/commondefine.h"

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>

const int gtkVersion = 0;
const int cursorVersion = 1;
const int iconVersion = 1;
const int width = 320;
const int height = 70;
const int baseCursorSize = 24;
const int baseCursorPadding = 12;
const int baseIconPadding  = 4;
const int baseIconSize = 48;

static QVector<QStringList> presentCursors {
   {"left_ptr"},
   {"left_ptr_watch"},
   {"x-cursor", "X_cursor"},
   {"hand2", "hand1"},
   {"grab", "grabbing", "closedhand"},
   {"fleur", "move"},
   {"sb_v_double_arrow"},
   {"sb_h_double_arrow"},
   {"watch", "wait"}
};

static QVector<QStringList> presentIcons {
    // file manager:
    {"dde-file-manager", "system-file-manager"},
    // music player:
    {"deepin-music", "banshee", "amarok", "deadbeef", "clementine", "rhythmbox"},
    // image viewer:
    {"deepin-image-viewer", "eog", "gthumb", "gwenview", "gpicview", "showfoto", "phototonic"},
    // media/video player:
    {"deepin-movie", "media-player", "totem", "smplayer", "vlc", "dragonplayer", "kmplayer"},
    // web browser:
    {"google-chrome", "firefox", "chromium", "opera", "internet-web-browser", "browser"},
    // system settings:
    {"preferences-system"},
    // text editor:
//    {"accessories-text-editor", "text-editor", "gedit", "kedit", "xfce-edit"},
    // terminal:
//    {"deepin-terminal", "utilities-terminal", "terminal", "gnome-terminal", "xfce-terminal", "terminator", "openterm"},
};

QString getScaleDir()
{
    double scaleFactor = getScaleFactor();

    return QString("X%1").arg(scaleFactor);
}

QString getTypeDir(QString type0, int version)
{
    return QString("%1-v%2").arg(type0).arg(version);
}

void init()
{
    removeUnusedScaleDirs();
    removeAllTypesOldVersionDirs();
}

void removeUnusedScaleDirs()
{
    QString cacheDir = g_get_user_cache_dir();
    cacheDir+="/deepin/dde-api/theme_thumb";
    removeUnusedDirs(cacheDir+"/X*", getScaleDir());
}

void removeAllTypesOldVersionDirs()
{
    QString scaleDir = getScaleDir();
    removeOldVersionDirs(scaleDir, "gtk", gtkVersion);
    removeOldVersionDirs(scaleDir, "cursor", cursorVersion);
    removeOldVersionDirs(scaleDir, "icon", iconVersion);
}

void removeOldVersionDirs(QString scaleDir, QString type0, int version)
{
    QString cacheDir = g_get_user_cache_dir();
    cacheDir+="/deepin/dde-api/theme_thumb";
    QString pattern = cacheDir + "/" + scaleDir + "/" + type0 + "/-v*";
    QString usedDir = getTypeDir(type0, version);
    removeUnusedDirs(pattern, usedDir);
}

void removeUnusedDirs(QString pattern, QString userDir)
{
    std::vector<QString> dirs = DFile::glob(pattern);
    if(dirs.empty()){
        return;
    }

    for(auto dir:dirs){
        if(DFile::base(dir)!=userDir){
            remove(dir.toStdString().c_str());
        }
    }
}

bool checkScaleFactor()
{
    double scaleFactor = getScaleFactor();
    if (scaleFactor <= 0)
        return false;
    return true;
}

double getScaleFactor()
{
    double scaleFactor =0;
    QDBusInterface xSettingsInterface("com.deepin.SessionManager",
                                                "/com/deepin/XSettings",
                                                "com.deepin.XSettings",
                                                QDBusConnection::sessionBus());

    if(xSettingsInterface.isValid())
    {
        QDBusMessage message = xSettingsInterface.call("GetScaleFactor");
        if(message.type() != QDBusMessage::ErrorMessage)
        {
            scaleFactor = message.arguments().first().toDouble();
        }
    }

    return scaleFactor;
}

QString getCursor(QString id, QString descFile)
{
    if (!checkScaleFactor()) {
        qInfo() << "scaleFactor <= 0";
        return "";
    }

    QString out = prepareOutputPath("cursor", id, cursorVersion);
    if (shouldGenerateNewCursor(descFile, out)) {
        return "";
    }

    double scaleFactor = getScaleFactor();

    if(!genCursor(descFile,width,height,scaleFactor, out))
    {
        return "";
    }

    return out;
}

bool genCursor(QString descFile,int width,int height,double scaleFactor,QString out)
{
    QString dirPath = descFile.left(descFile.lastIndexOf("/")) +"/cursors";

    int iconSize = static_cast<int>(baseCursorSize * scaleFactor);
    int padding  = static_cast<int>(baseCursorPadding * scaleFactor);
    width        = static_cast<int>(width * scaleFactor);
    height       = static_cast<int>(height * scaleFactor);

    qDebug()<<"dirPath : "<<dirPath;
    QVector<QImage*> images = getCursors(dirPath,iconSize);
    QImage image = CompositeImages(images,width,height,iconSize,padding);

    if(image.save(out))
    {

    }

    return true;
}

bool genIcon(QString theme,int width,int height,double scaleFactor,QString out)
{
    int iconSize = static_cast<int>(baseIconSize * scaleFactor);
    int padding  = static_cast<int>(baseIconPadding * scaleFactor);
    width        = static_cast<int>(width * scaleFactor);
    height       = static_cast<int>(height * scaleFactor);

    QVector<QImage*> images = getIcons(theme, iconSize);
    QImage image = CompositeImages(images,width,height,iconSize,padding);

    bool saveRtn = image.save(out);
    int imagesLen = images.size();
    for (int i = 0; i < imagesLen; ++i) {
        if (images[i] != NULL) {
            delete images[i];
            images[i] = NULL;
        }
    }

    images.clear();
    return saveRtn;
}

bool genGtk(QString name,int width,int height,double scaleFactor,QString dest)
{
    double gdkWinScalingFactor = 1.0;

    if (scaleFactor > 1.7) {
        // 根据 startdde 的逻辑，此种条件下 gtk 窗口放大为 2 倍
        gdkWinScalingFactor = 2.0;
    }

    width = static_cast<int>(width* scaleFactor / gdkWinScalingFactor);
    height = static_cast<int>(height* scaleFactor / gdkWinScalingFactor);

    QString cmd = "/usr/lib/deepin-api/gtk-thumbnailer";
    QStringList args{"-theme", name,
                "-dest", dest,
                "-width", QString::number(width),
                "-height", QString::number(height),
                "-force"};

    QProcess process;
    process.setProgram(cmd);
    process.setArguments(args);
    process.start();

    return true;
}

QVector<QImage*> getCursors(QString dir, int size)
{
    qDebug()<<"dir :"<<dir;
    QVector<QImage*> images;
    for(auto cursors : presentCursors)
    {
        for(auto iter : cursors)
        {
            bool bAdd = true;
            QFile file(dir + "/" + iter);
            QImage* image = loadXCursor(dir + "/" + iter,size);
            if(!image)
            {
                continue;
            }

            for(auto tempImage: images)
            {
                if(tempImage->cacheKey() == image->cacheKey())
                {
                    bAdd = false;
                }
            }

            if(bAdd)
            {
                if(image->width() != size)
                {
                    image->scaled(size,image->height());
                }
                images.push_back(image);
                break;
            }
        }

    }

    return images;
}

char* chooseIcon(const char *theme_name, const char **icon_names, int size)
{
    if (!gtk_init_check(NULL, NULL)) {
        return NULL;
    }
    GtkIconTheme *icon_theme = gtk_icon_theme_new();
    if (icon_theme == NULL) {
        qWarning() << "call gtk_icon_theme_new failed: rtn null.";
        return NULL;
    }
    gtk_icon_theme_set_custom_theme(icon_theme, theme_name);

    GtkIconInfo* icon_info = gtk_icon_theme_choose_icon(icon_theme, icon_names, size, GTK_ICON_LOOKUP_FORCE_SVG);
    if (icon_info == NULL ) {
        g_object_unref(icon_theme);
        return NULL;
    }
    const char* filename = gtk_icon_info_get_filename(icon_info);
    if (filename == NULL) {
        qWarning() << "call gtk_icon_info_get_filename failed: rtn null.";
        return NULL;
    }
    char* filename_dup = g_strdup(filename);
    if (filename_dup == NULL) {
        qWarning() << "call g_strdup failed: rtn null.";
        return NULL;
    }

    g_object_unref(icon_info);
    g_object_unref(icon_theme);
    return filename_dup;
}

QImage* loadIcon(const QString &theme, const QStringList &iconNames, const int &size)
{
    int iconNamesLen = iconNames.length();
    const char** tmp = new const char*[iconNamesLen + 1];
    for (int i = 0; i < iconNamesLen; ++i) {
        tmp[i] = new char[iconNames[i].toLatin1().length()];
        memcpy(const_cast<char*>(tmp[i]), iconNames[i].toStdString().c_str(), iconNames[i].toLatin1().length() + 1);
    }

    tmp[iconNamesLen] = nullptr;
    QString fileName = chooseIcon(theme.toStdString().c_str(), tmp, size);

    for (int i = 0; i < iconNamesLen; ++i) {
        if (tmp[i]) {
            delete[] tmp[i];
        }
    }
    if (tmp) {
        delete[] tmp;
    }

    if (fileName == "") {
        qWarning() << QString("choose Icon [%1] failed: ").arg(fileName);
        return nullptr;
    }

    QImage* image = new QImage(fileName);

    if (size != image->width()) {
        auto tmp = image;
        image = new QImage(image->scaled(size, 0, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        delete tmp;
    }

    return image;
}

QVector<QImage*> getIcons(QString theme, int size)
{
    QVector<QImage*> images;
    for(const auto &icons : qAsConst(presentIcons)) {
        bool bAdd = true;
        QImage* image = loadIcon(theme, icons, size);
        if(!image) {
            continue;
        }

        for(auto tempImage: images) {
            if(tempImage->cacheKey() == image->cacheKey()) {
                bAdd = false;
                break;
            }
        }

        if(bAdd) {
            if(image->width() != size) {
                image->scaled(size,image->height());
            }
            images.push_back(image);
        }
    }

    return images;
}

QImage CompositeImages(QVector<QImage*> images, int width, int height, int iconSize,int padding)
{
    QImage image(width,height,QImage::Format_RGBA8888);
    if(images.isEmpty())
    {
        return image;
    }

    while (images.size()>9) {
        images.pop_back();
    }
    QColor bmpBackA(254,254,254,0);
    for(int i=0;i< image.width();++i)
    {
        for(int j=0;j<image.height();++j)
        {
            image.setPixelColor(i,j,bmpBackA);
        }

    }
    int spaceW = width - iconSize*images.size();
    int x = (spaceW-(images.size() -1)*padding) / 2;
    int y = (height - iconSize) / 2;

    QPainter painter;
    painter.begin(&image);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);

    int i=0;

    for(auto iter : images)
    {
        if(!iter->save("/home/uos/file"+QString::number(i++)+".png"))
        {
            qDebug()<<"save faile";
        }
        qDebug()<<i<<" :"<<(*iter).bits();
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(x,y,*iter);
        x+=iconSize +padding;
    }
    painter.end();

    return image;
}

QImage* loadXCursor(QString fileName, int size)
{
    XcursorImage* xcursorImage = XcursorFilenameLoadImage(fileName.toStdString().c_str(),size);
    if(xcursorImage == nullptr)
    {
        return nullptr;
    }
    QImage* image =fromXCurorImageToQImage(xcursorImage);
    delete xcursorImage;

    return image;
}

QImage* fromXCurorImageToQImage(XcursorImage* image)
{
    // golang 原代码 pixels := (*[1 << 12]C.XcursorPixel)(unsafe.Pointer(img.pixels))[:n:n]
    XcursorPixel* tempPixel = &image->pixels[0];
    QImage* qImage =new QImage(static_cast<int>(image->width),static_cast<int>(image->height),QImage::Format_RGBA8888);

    int i=0;
    for(int y=0; y<static_cast<int>(image->height); y++)
    {
        for(int x=0; x<static_cast<int>(image->width); x++)
        {
            int alpha = tempPixel[i] >> 24;
            int red = (tempPixel[i] >> 16) & 0xff;
            int green = (tempPixel[i] >>8) & 0xff;
            int blue = tempPixel[i] & 0xff;
            QColor color(red,green,blue,alpha);
            qImage->setPixelColor(x,y,color);
            i++;
        }
    }

    return qImage;
}


QString getGtk(QString id, QString descFile)
{
    if (!checkScaleFactor()) {
        qInfo() << "scaleFactor <= 0";
        return "";
    }

    QString out = prepareOutputPath("gtk", id, cursorVersion);
    if (shouldGenerateNewCursor(descFile, out)) {
        return "";
    }

    double scaleFactor = getScaleFactor();

    if(!genGtk(id,width,height,scaleFactor,out))
    {
        return "";
    }

    return out;
}

QString getIcon(QString id, QString descFile)
{
    if (!checkScaleFactor()) {
        qInfo() << "scaleFactor <= 0";
        return "";
    }

    QString out = prepareOutputPath("icon", id, iconVersion);
    if (shouldGenerateNewCursor(descFile, out)) {
        return "";
    }

    double scaleFactor = getScaleFactor();

    if(!genIcon(id, width, height, scaleFactor, out)) {
        return "";
    }

    return out;
}

bool shouldGenerateNew(QString descFile, QString out)
{
    struct stat descFileStat;
    struct stat outFileStat;

    int res = stat(descFile.toStdString().c_str(),&descFileStat);
    if(res==-1){
        return false;
    }

    struct timespec descFileTime = descFileStat.st_ctim;
    int descFileTimestamp= clock_gettime(CLOCK_REALTIME,&descFileTime);

    res = stat(out.toStdString().c_str(),&outFileStat);
    if(res==-1){
        return false;
    }

    struct timespec outFileTime = descFileStat.st_ctim;
    int outFileTimestamp= clock_gettime(CLOCK_REALTIME,&outFileTime);

    if(descFileTimestamp>outFileTimestamp){
        return true;
    }

    return false;
}

bool shouldGenerateNewCursor(QString descFile, QString out)
{
    QString dir = DFile::dir(descFile);
    descFile = dir+"/"+"cursors"+"/"+"left_ptr";

    return shouldGenerateNew(descFile,out);
}

QString prepareOutputPath(QString type0, QString id, int version)
{
    QString scaleDir = getScaleDir();
    QString typeDir = getTypeDir(type0,version);

    QString cacheDir = g_get_user_cache_dir();
    cacheDir+="/deepin/dde-api/theme_thumb";
    QString dirPath = cacheDir +"/"+scaleDir+"/"+typeDir;

    QDir dir;
    if(!dir.mkpath(dirPath))
    {
        return "";
    }

    return dirPath+"/"+id+".png";
}


