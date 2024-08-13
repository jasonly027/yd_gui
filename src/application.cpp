#include "application.h"

#include <QtGui/qguiapplication.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qqmlapplicationengine.h>
#include <qqmlcontext.h>

#include "application_settings.h"
#include "database.h"

namespace yd_gui {
static QGuiApplication* create_application(int& argc, char** argv) {
    QGuiApplication::setApplicationName("yd_gui");
    QGuiApplication::setApplicationDisplayName("yd_gui - Video Downloader");
    QGuiApplication::setOrganizationName("jasonly027");
    QGuiApplication::setApplicationVersion(APP_VERSION);

    return new QGuiApplication(argc, argv);
}

Application::Application(int& argc, char** argv)
    : application_(create_application(argc, argv)),
      engine_(new QQmlApplicationEngine) {
    engine_->rootContext()->setContextProperty("_settings",
                                               &ApplicationSettings::get());
    engine_->rootContext()->setContextProperty("_database", &Database::get());

    QObject::connect(
        engine_.get(), &QQmlApplicationEngine::objectCreationFailed,
        application_.get(), [] { QGuiApplication::exit(-1); },
        Qt::QueuedConnection);

    engine_->loadFromModule("yd_gui", "Main");
}

int Application::exec() { return application_->exec(); }

}  // namespace yd_gui
