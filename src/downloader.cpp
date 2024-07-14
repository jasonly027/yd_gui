#include "downloader.h"

#include <qcontainerfwd.h>
#include <qobject.h>
#include <qprocess.h>
#include <qstandardpaths.h>

#include <QDebug>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>

#include "video.h"

namespace yd_gui {
using json = nlohmann::json;
using std::nullopt;

static constexpr auto kProgram = "yt-dlp";

// Create new yt-dlp process. The QProcess will be deleted when its
// finish signal is emit
QProcess* Downloader::createProgram() {
    auto* yt_dlp = new QProcess();
    yt_dlp->setProgram(kProgram);
    yt_dlp->setWorkingDirectory(
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    QObject::connect(yt_dlp, &QProcess::finished, yt_dlp,
                     &QObject::deleteLater);

    return yt_dlp;
}

optional<VideoInfo> Downloader::parseRawInfo(const QString& raw_info) {
    const json info = json::parse(raw_info.toStdString(), nullptr, false);
    // Check if invalid JSON
    if (info.is_discarded()) return nullopt;

    QString video_id = "";
    if (auto it = info.find("id"); it != info.end() && it->is_string()) {
        video_id = QString::fromStdString(it->get<string>());
    }

    QString title = "";
    if (auto it = info.find("title"); it != info.end() && it->is_string()) {
        title = QString::fromStdString(it->get<string>());
    }

    QString author = "";
    if (auto it = info.find("channel"); it != info.end() && it->is_string()) {
        author = QString::fromStdString(it->get<string>());
    }

    uint32_t seconds = 0;
    if (auto it = info.find("duration");
        it != info.end() && it->is_number_unsigned()) {
        seconds = it->get<uint32_t>();
    }

    QString thumbnail = "";
    if (auto it = info.find("thumbnail"); it != info.end() && it->is_string()) {
        thumbnail = QString::fromStdString(it->get<string>());
    }

    bool audio_available = false;

    QList<VideoFormat> formats;
    if (auto it = info.find("formats"); it != info.end() && it->is_array()) {
        for (const auto& format_it : *it) {
            QString format_id = "";
            if (auto it = format_it.find("format_id");
                it != format_it.end() && it->is_string()) {
                format_id = QString::fromStdString(it->get<string>());
            }
            // If this somehow doesn't have an id, there's no point in using it,
            // because we can't target it for download.
            if (format_id == "") continue;

            // Check if this video as a whole (i.e., not necessarily this
            // specific format) has audio available
            if (auto it = format_it.find("acodec");
                it != format_it.end() && it->is_string()) {
                string acodec = it->get<string>();
                if (acodec != "none") audio_available = true;
            }

            // We are only storing formats that have video.
            // If the vcodec field exists, skip this format if it's none
            // If the vcodec field doesn't exist, skip this format.
            if (auto it = format_it.find("vcodec");
                it != format_it.end() && it->is_string()) {
                string vcodec = it->get<string>();
                if (vcodec == "none") continue;
            } else {
                continue;
            }

            QString container = "";
            if (auto it = format_it.find("ext");
                it != format_it.end() && it->is_string()) {
                container = QString::fromStdString(it->get<string>());
            }

            uint32_t width = 0;
            if (auto it = format_it.find("width");
                it != format_it.end() && it->is_number_unsigned()) {
                width = it->get<uint32_t>();
            }

            uint32_t height = 0;
            if (auto it = format_it.find("height");
                it != format_it.end() && it->is_number_unsigned()) {
                height = it->get<uint32_t>();
            }

            float fps = 0.0;
            if (auto it = format_it.find("fps");
                it != format_it.end() && it->is_number()) {
                fps = it->get<float>();
            }

            formats << VideoFormat(format_id, container, width, height, fps);
        }
    }

    // There's nothing we can download
    if (formats.empty() && !audio_available) return nullopt;

    return VideoInfo(video_id, title, author, seconds, thumbnail, formats,
                     audio_available);
}

void Downloader::fetch_info(const QString& url) {
    if (is_fetching_) return;
    if (bool found = check_program_exists(); !found) return;

    QProcess* yt_dlp = createProgram();
    yt_dlp->setArguments({url, "--dump-json"});

    QObject::connect(yt_dlp, &QProcess::readyReadStandardError, this,
                     [=, this] {
                         const QString data = yt_dlp->readAllStandardError();
                         emit this->standardErrorPushed(data);
                     });
    QObject::connect(yt_dlp, &QProcess::finished, this, [=, this] {
        const QString raw_info = yt_dlp->readAllStandardOutput();
        const optional<VideoInfo> info = parseRawInfo(raw_info);

        if (!info.has_value()) {
            emit this->fetchInfoBadParse();
            return;
        }
        emit this->infoPushed(info.value());
    });

    yt_dlp->start();
}

bool Downloader::is_fetching() const { return is_fetching_; }

bool Downloader::program_exists() const { return program_exists_; }

void Downloader::set_is_fetching(bool is_fetching) {
    if (is_fetching == is_fetching_) return;
    is_fetching_ = is_fetching;
    emit isFetchingChanged();
}

void Downloader::set_program_exists(bool program_exists) {
    if (program_exists == program_exists_) return;
    program_exists_ = program_exists;
    emit programExistsChanged();
}

bool Downloader::check_program_exists() {
    const bool found = QStandardPaths::findExecutable(kProgram) != "";
    set_program_exists(found);
    return found;
}

}  // namespace yd_gui
