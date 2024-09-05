#include "application.h"

#include <QtGui/qguiapplication.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfontdatabase.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qqmlapplicationengine.h>
#include <qqmlcontext.h>
#include <qstringliteral.h>

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

static void add_fonts() {
    const QList<QString> fonts = {
        QStringLiteral(":/fonts/NotoSans-Italic-VariableFont.ttf"),
        QStringLiteral(":/fonts/NotoSans-VariableFont.ttf"),
        QStringLiteral(":/fonts/Typicons.ttf")};

    for (const auto& font : fonts) {
        if (QFontDatabase::addApplicationFont(font) == -1) {
            qWarning() << "Failed to add font: " << font;
        }
    }
}

static QString get_qt_license() {
    QFile file(":/legal/Qt_License_LGPL3.txt");
    if (!file.open(QIODevice::ReadOnly))
        qWarning() << "Failed to load Qt License";

    QTextStream in(&file);

    return in.readAll();
}

Application::Application(int& argc, char** argv)
    : application_(create_application(argc, argv)),
      engine_(new QQmlApplicationEngine) {
    add_fonts();

    application_->setFont(QStringLiteral("Noto Sans"));

    engine_->rootContext()->setContextProperty("_settings",
                                               &ApplicationSettings::get());
    engine_->rootContext()->setContextProperty("_database", &Database::get());

    engine_->rootContext()->setContextProperty("_qt_legal", get_qt_license());

    QObject::connect(
        engine_.get(), &QQmlApplicationEngine::objectCreationFailed,
        application_.get(), [] { QGuiApplication::exit(-1); },
        Qt::QueuedConnection);

    engine_->loadFromModule("yd_gui", "Main");
}

int Application::exec() { return application_->exec(); }

}  // namespace yd_gui
