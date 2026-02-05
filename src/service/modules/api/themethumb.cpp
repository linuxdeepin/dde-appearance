// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QImageReader>

#include "themethumb.h"
#include "../api/dfile.h"
#include "../api/utils.h"
#include "compatibleengine.h"
#include "keyfile.h"
#include "modules/subthemes/theme.h"

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <utility>

#include <X11/Xcursor/Xcursor.h>

#undef Bool
#undef Status
#undef Unsorted
#undef True
#undef False

const int width = 220;
const int height = 36;
const int baseCursorSize = 24;
const int baseCursorPadding = 7;
const int baseIconPadding  = 10;
const int baseIconSize = 36;

static QVector<QStringList> presentCursors {
   {"left_ptr"},
   {"left_ptr_watch"},
   {"x-cursor", "X_cursor"},
   {"hand2", "hand1"},
   {"grab", "grabbing", "closedhand"},
   {"fleur", "move"},
   {"sb_v_double_arrow"},
};

static QVector<QStringList> presentIcons {
    // file manager:
    {"dde-file-manager", "system-file-manager"},
    // music player:
    {"deepin-music", "banshee", "amarok", "deadbeef", "clementine", "rhythmbox"},
    // image viewer:
    {"deepin-image-viewer", "eog", "gthumb", "gwenview", "gpicview", "showfoto", "phototonic"},
    // web browser:
    {"org.deepin.browser", "google-chrome", "firefox", "chromium", "opera", "internet-web-browser", "browser"},
    // system settings:
    {"user-trash"},
    // text editor:
//    {"accessories-text-editor", "text-editor", "gedit", "kedit", "xfce-edit"},
    // terminal:
//    {"deepin-terminal", "utilities-terminal", "terminal", "gnome-terminal", "xfce-terminal", "terminator", "openterm"},
};
// ScaleFactor is used multiple times and has signals for modification, changed to static variable for caching
static double g_ScaleFactor = 0.0;

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
    QString cacheDir = utils::GetUserCacheDir();
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
    QString cacheDir = utils::GetUserCacheDir();
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
    return g_ScaleFactor;
}

QString getCursor(QString id, QString descFile)
{
    if (!checkScaleFactor()) {
        qInfo() << "scaleFactor <= 0";
        return "";
    }

    QString out = prepareOutputPath("cursor", id, cursorVersion);
    if (!shouldGenerateNewCursor(descFile, out)) {
        return out;
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
    QString dirPath = descFile + "/cursors";

    int iconSize = static_cast<int>(baseCursorSize * scaleFactor);
    int padding  = static_cast<int>(baseCursorPadding * scaleFactor);
    width        = static_cast<int>(width * scaleFactor);
    height       = static_cast<int>(height * scaleFactor);

    qDebug()<<"dirPath : "<<dirPath;
    QVector<QImage*> images = getCursors(dirPath,iconSize);
    QImage image = CompositeImages(images,width,height,iconSize,padding);

    // Clean up the images since we own them
    for(auto img : images) {
        delete img;
    }

    if(image.save(out))
    {

    }

    return true;
}

bool genIcon(QString theme, int width, int height, double scaleFactor, QString out)
{
    int iconSize = static_cast<int>(baseIconSize * scaleFactor);
    int padding  = static_cast<int>(baseIconPadding * scaleFactor);
    width        = static_cast<int>(width * scaleFactor);
    height       = static_cast<int>(height * scaleFactor);

    QList<QIcon> images = getIcons(theme, iconSize);

    // Use QImage instead of QPixmap
    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::transparent); // Set transparent background

    QPainter painter;
    painter.begin(&image);

    int spaceW = width - iconSize * images.size();
    int x = (spaceW - (images.size() - 1) * padding) / 2;
    int y = (height - iconSize) / 2;

    for (const auto& iter : images) {
        // Draw icon to QImage
        iter.paint(&painter, QRect(x, y, iconSize, iconSize));
        x += iconSize + padding;
    }
    painter.end();

    // Save QImage to specified path
    return image.save(out);
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
            QImage* image = loadXCursor(dir + "/" + iter, size);
            if(!image)
            {
                continue;
            }

            for(auto tempImage: images)
            {
                if(tempImage->cacheKey() == image->cacheKey())
                {
                    bAdd = false;
                    delete image;
                    break;
                }
            }

            if(bAdd)
            {
                // Perform scaling in CompositeImages to avoid quality loss from multiple scaling operations
                images.push_back(image);
                break;
            }
        }

    }

    return images;
}

QList<QIcon> getIcons(QString theme, int size)
{
    Q_UNUSED(size); // Suppress unused parameter warning
    QList<QIcon> images;
    QIcon::setThemeName(theme);
    for(const auto &icons : std::as_const(presentIcons)) {
        for (auto &&iconName:icons) {
            QIcon icon(new CompatibleEngine(iconName)); // QIcon::fromTheme(iconName); DCI support in DTK is incomplete, temporarily using CompatibleEngine for compatibility
            if (!icon.isNull()) {
                images.append(icon);
                break;
            }
        }
    }

    return images;
}

QImage CompositeImages(QVector<QImage*> images, int width, int height, int iconSize,int padding)
{
    if(images.isEmpty())
    {
        return QImage();
    }

    while (images.size()>9) {
        images.pop_back();
    }
    
    // Adjust final image size based on icon count
    double scaleFactor = getScaleFactor();
    width = iconSize * images.size() + padding * (images.size() - 1);
    width = width * scaleFactor;
    height = height * scaleFactor;

    QImage image(width, height, QImage::Format_RGBA8888);
    image.setDevicePixelRatio(scaleFactor);
    image.fill(Qt::transparent);
    
    // Calculate layout starting point based on scale factor
    int spaceW = width - iconSize * images.size() * scaleFactor;
    int x = (spaceW - (images.size() - 1) * padding * scaleFactor) / 2;
    int y = (height - iconSize * scaleFactor) / 2;

    QPainter painter;
    painter.begin(&image);
    
    // Enable all available high-quality rendering options
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    int i=0;

    for(auto iter : images)
    {
        qDebug()<<i<<" :"<<(*iter).bits();
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(x,y,*iter);
        x+=iconSize +padding;
    }
    painter.end();
    
    return image;
}

static QImage* fromXCurorImageToQImage(XcursorImage* image)
{
    // Original golang code: pixels := (*[1 << 12]C.XcursorPixel)(unsafe.Pointer(img.pixels))[:n:n]
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

QImage* loadXCursor(QString fileName, int size)
{
    XcursorImage* xcursorImage = XcursorFilenameLoadImage(fileName.toStdString().c_str(),size);
    if(xcursorImage == nullptr)
    {
        return nullptr;
    }
    QImage* image =fromXCurorImageToQImage(xcursorImage);
    XcursorImageDestroy(xcursorImage);

    return image;
}

QString getGlobal(QString id, QSharedPointer<Theme> theme, QString gtkTheme)
{
    Q_UNUSED(id); // Suppress unused parameter warning
    if (!checkScaleFactor()) {
        qInfo() << "scaleFactor <= 0";
        return "";
    }
    QStringList example = theme.get()->example().split(',');
    if (!example.isEmpty()) {
        bool isdark = (gtkTheme == "deepin-dark") ;
        QString path = isdark ? example.last() : example.first();
        QFileInfo file(path);
        if (file.isRelative()) {
            file.setFile(theme->getPath(), path);
            path = file.absoluteFilePath();
        }
        return path;
    }
    return QString();
}

QString getIcon(QString id, QString descFile)
{
    if (!checkScaleFactor()) {
        qInfo() << "scaleFactor <= 0";
        return "";
    }

    QString out = prepareOutputPath("icon", id, iconVersion);
    if (!shouldGenerateNew(descFile, out)) {
        return out;
    }

    double scaleFactor = getScaleFactor();

    if(!genIcon(id, width, height, scaleFactor, out)) {
        return "";
    }

    return out;
}

bool shouldGenerateNew(QString descFile, QString out)
{
    QFileInfo outFileInfo(out);
    if (!outFileInfo.exists()) {
        return true;
    }
    QFileInfo descFileInfo(descFile);
    if (descFileInfo.isFile()) {
        descFileInfo = QFileInfo(descFileInfo.dir().absolutePath());
    }

    return descFileInfo.lastModified() > outFileInfo.lastModified();
}

bool shouldGenerateNewCursor(QString descFile, QString out)
{
    QString dir = DFile::dir(descFile);
    descFile = dir;

    return shouldGenerateNew(descFile,out);
}

QString prepareOutputPath(QString type0, QString id, int version)
{
    QString scaleDir = getScaleDir();
    QString typeDir = getTypeDir(type0,version);

    QString cacheDir = utils::GetUserCacheDir();
    cacheDir+="/deepin/dde-api/theme_thumb";
    QString dirPath = cacheDir +"/"+scaleDir+"/"+typeDir;

    QDir dir;
    if(!dir.mkpath(dirPath))
    {
        return "";
    }

    return dirPath+"/"+id+".png";
}

void UpdateScaleFactor(double scaleFactor)
{
    g_ScaleFactor = scaleFactor;
}