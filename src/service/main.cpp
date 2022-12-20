#include <QGuiApplication>
#include "appearance1adaptor.h"
#include "impl/appearance1.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("deepin");
    app.setApplicationName("org.deepin.dde.appearance");
    QTranslator translator;
    translator.load("/usr/share/dde-appearance/translations/dde-appearance_" + QLocale::system().name());
    app.installTranslator(&translator);
    Appearance1 *appearance = new Appearance1();
    new Appearance1Adaptor(appearance);

    QDBusConnection::sessionBus().registerService("org.deepin.dde.Appearance1");
    QDBusConnection::sessionBus().registerObject("/org/deepin/dde/Appearance1", "org.deepin.dde.Appearance1", appearance);

    return app.exec();
}
