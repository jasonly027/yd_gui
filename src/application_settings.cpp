#include "application_settings.h"

#include <qcoreapplication.h>
#include <qdir.h>
#include <qsettings.h>
#include <qstandardpaths.h>
#include <qurl.h>
#include <qvariant.h>

class QObject;

namespace yd_gui {
class ApplicationSettings& ApplicationSettings::get() {
    static ApplicationSettings settings;
    return settings;
}

static QUrl default_download_dir() {
    QString dir =
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

    return QDir(dir).exists() ? QUrl::fromLocalFile(dir)
                              : QCoreApplication::applicationDirPath();
}

// CREATE AN Q_PROP FOR THIS BC InputDownloadDir.qml:21 only calls this function
// once
QUrl ApplicationSettings::downloadDirValidated() {
    QUrl dir = downloadDir();

    if (dir.isLocalFile() && QDir(dir.toLocalFile()).exists()) {
        return dir;
    }

    emit errorPushed(
        "[Downloader] Destination folder doesn't exist. Defaulting to either "
        "the user's download folder or the application folder");

    return default_download_dir();
}

QUrl ApplicationSettings::downloadDir() {
    return contains("downloadDir") ? value("downloadDir").toUrl()
                                   : default_download_dir();
}

void ApplicationSettings::setDownloadDir(const QUrl& dir) {
    const QString current_dir =
        value("downloadDir", default_download_dir()).toString();

    if (dir == current_dir) return;

    setValue("downloadDir", dir);
    emit downloadDirChanged();
}

QString ApplicationSettings::downloadDirStr() {
    return downloadDir().toLocalFile();
}

static QString default_theme() { return "darkPurple"; }

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
