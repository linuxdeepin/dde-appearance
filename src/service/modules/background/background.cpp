#include "background.h"
#include "../api/utils.h"

#include <QDBusInterface>
#include <pwd.h>

Background::Background()
{

}

Background::~Background(){

}

void Background::setId(QString id){
    this->id=id;
}

QString Background::getId()
{
    return id;
}

bool Background::getDeleteable()
{
    return deletable;
}
void Background::setDeletable(bool deletable){
    this->deletable=deletable;
}

void Background::Delete()
{
    if(!deletable){
        return;
    }

    struct passwd* user = getpwuid(getuid());
    if(user == nullptr)
    {
        return;
    }

    QDBusInterface daemon("com.deepin.daemon.Daemon","/com/deepin/daemon/Daemon","com.deepin.daemon.Daemon",QDBusConnection::systemBus());
    if(!daemon.isValid())
    {
        return;
    }

    QString uri = utils::deCodeURI(id);

    QDBusMessage reply = daemon.call("DeleteCustomWallPaper",user->pw_name,uri);
    if(!reply.errorMessage().isEmpty())
    {
        return;
    }

    QDBusInterface imageBlur("com.deepin.daemon.Accounts",
                             "/com/deepin/daemon/ImageBlur",
                             "com.deepin.daemon.ImageBlur",
                             QDBusConnection::systemBus());
    if(!imageBlur.isValid())
    {
        return;
    }
    reply = imageBlur.call("Delete",uri);
    if(!reply.errorMessage().isEmpty())
    {
        return;
    }

    QDBusInterface imageEffect("com.deepin.daemon.ImageEffect",
                               "/com/deepin/daemon/ImageEffect",
                               "com.deepin.daemon.ImageEffect",
                               QDBusConnection::systemBus());
    if(!imageBlur.isValid())
    {
        return;
    }
    imageBlur.call("Delete", "all", uri);
}

QString Background::Thumbnail()
{
    return "";
}
