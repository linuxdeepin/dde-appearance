#include "theme.h"

Theme::Theme(QString id, QString filePath, bool deletable)
    :id(id),
    filePath(filePath),
    deletable(deletable)
{

}

Theme::Theme(QString fileName)
{
    QStringList nodes = fileName.split("/");
    if(nodes.size()>=2)
    {
        this->id= nodes[nodes.size()-2];
    }

    this->filePath = fileName.left(fileName.lastIndexOf("/"));

    QString home = getenv("HOME");
    if(fileName.indexOf(home)!=-1){
        this->deletable=true;
    }else {
        this->deletable=false;
    }
}



bool Theme::Delete()
{
    if (!this->deletable) {
        qInfo() << "permission denied";
    }

    QDir dir(filePath.left(filePath.lastIndexOf("/")));
    if (!dir.exists()) {
        qInfo() << "not dir";
        return false;
    }

    return  dir.removeRecursively();
}

Theme::~Theme()
{

}

QString Theme::getId(){
    return id;
}

QString Theme::getPath()
{
    return filePath;
}

bool Theme::getDeleteable()
{
    return deletable;
}
