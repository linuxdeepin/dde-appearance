#include <QCoreApplication>
#include "appearance1adaptor.h"
#include "impl/appearance1.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Appearance1 * appearance = new Appearance1();
    new Appearance1Adaptor(appearance);

    QDBusConnection::sessionBus().registerService("org.deepin.daemon.Appearance1");
    QDBusConnection::sessionBus().registerObject("/org/deepin/daemon/Appearance1", "org.deepin.daemon.Appearance1", appearance);

    return app.exec();
}
