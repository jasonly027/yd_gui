#include "application_settings.h"

#include <qcoreapplication.h>
#include <qdir.h>
#include <qsettings.h>
#include <qstandardpaths.h>
#include <qstringliteral.h>
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

QUrl ApplicationSettings::downloadDirValidated() {
    QUrl dir = downloadDir();

    if (dir.isLocalFile() && QDir(dir.toLocalFile()).exists()) {
        return dir;
    }

    emit errorPushed(
        "[Downloader] Destination folder doesn't exist. Defaulting to either "
        "the user's download folder or the application folder\n");

    return default_download_dir();
}

QUrl ApplicationSettings::downloadDir() const {
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

QString ApplicationSettings::downloadDirStr() const {
    return downloadDir().toLocalFile();
}

static QString default_theme() { return "darkPurple"; }

QString ApplicationSettings::theme() const {
    return contains("theme") ? value("theme").toString() : default_theme();
}

void ApplicationSettings::setTheme(const QString& theme) {
    const QString current_theme = value("theme", default_theme()).toString();

    if (theme == current_theme) return;

    setValue("theme", theme);
    emit themeChanged();
}

static bool default_download_thumbnail() { return false; }

bool ApplicationSettings::downloadThumbnail() const {
    return contains("downloadThumbnail") ? value("downloadThumbnail").toBool()
                                         : default_download_thumbnail();
}

void ApplicationSettings::setDownloadThumbnail(bool val) {
    const bool current_download_thumbnail =
        value("downloadThumbnail", default_download_thumbnail()).toBool();

    if (val == current_download_thumbnail) return;

    setValue("downloadThumbnail", val);
    emit downloadThumbnailChanged();
}

static QUrl default_ytdlp() {
    return QUrl::fromLocalFile(QStringLiteral("yt-dlp"));
}

QUrl ApplicationSettings::ytdlp() const {
    return contains("ytdlp") ? value("ytdlp").toUrl() : default_ytdlp();
}

void ApplicationSettings::setYtdlp(const QUrl& ytdlp) {
    const QUrl current_ytdlp = value("ytdlp", default_ytdlp()).toUrl();

    if (ytdlp == current_ytdlp) return;

    setValue("ytdlp", ytdlp);
    emit ytdlpChanged();
}

QString ApplicationSettings::ytdlpStr() const { return ytdlp().toLocalFile(); }

static QUrl default_ffmpeg_dir() {
    return QUrl::fromLocalFile(QStringLiteral(""));
}

QUrl ApplicationSettings::ffmpegDir() const {
    return contains("ffmpegDir") ? value("ffmpegDir").toUrl()
                                 : default_ffmpeg_dir();
}

void ApplicationSettings::setFfmpegDir(const QUrl& ffmpegDir) {
    const QUrl current_ffmpeg_dir =
        value("ffmpegDir", default_ffmpeg_dir()).toUrl();

    if (ffmpegDir == current_ffmpeg_dir) return;

    setValue("ffmpegDir", ffmpegDir);
    emit ffmpegDirChanged();
}

QString ApplicationSettings::ffmpegDirStr() const {
    return ffmpegDir().toLocalFile();
}

ApplicationSettings::ApplicationSettings(QObject* parent) : QSettings(parent) {}

}  // namespace yd_gui
