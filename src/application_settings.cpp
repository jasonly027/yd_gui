#include "application_settings.h"

#include <qcoreapplication.h>
#include <qdir.h>
#include <qsettings.h>
#include <qstandardpaths.h>
#include <qvariant.h>

namespace yd_gui {
class ApplicationSettings& ApplicationSettings::get() {
    static ApplicationSettings settings;
    return settings;
}

static QString default_download_dir() {
    QString dir =
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

    return QDir(dir).exists() ? dir : QCoreApplication::applicationDirPath();
}

QString ApplicationSettings::downloadDir() {
    if (contains("downloadDir")) {
        QString dir = value("downloadDir").toString();

        if (!QDir(dir).exists()) {
            emit errorPushed(
                "[Downloader] Download folder doesn't exist. Defaulting to "
                "either the user's download folder or application's folder");
            return default_download_dir();
        }

        return dir;
    }

    return default_download_dir();
}

void ApplicationSettings::setDownloadDir(const QString& dir) {
    const QString current_dir =
        value("downloadDir", default_download_dir()).toString();

    if (dir == current_dir) return;

    setValue("downloadDir", dir);
}

static QString default_theme() { return "light"; }

void ApplicationSettings::setTheme(const QString& theme) {
    const QString current_theme = value("theme", default_theme()).toString();

    if (theme == current_theme) return;

    setValue("theme", theme);
    emit themeChanged();
}

QString ApplicationSettings::theme() const {
    return contains("theme") ? value("theme").toString() : default_theme();
}

ApplicationSettings::ApplicationSettings(QObject* parent) : QSettings(parent) {}

}  // namespace yd_gui
