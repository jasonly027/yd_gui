#include "downloader.h"

#include <qcontainerfwd.h>
#include <qobject.h>
#include <qpointer.h>
#include <qprocess.h>
#include <qqmlintegration.h>
#include <qstandardpaths.h>
#include <qtmetamacros.h>

#include <QDebug>
#include <QStringBuilder>
#include <cassert>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <utility>

#include "video.h"

namespace yd_gui {
using json = nlohmann::json;
using std::nullopt;

static constexpr auto kProgram = "yt-dlp";

// Deserialize from JSON to VideoInfo
optional<VideoInfo> Downloader::parseRawInfo(const QString& raw_info) {
    const json info = json::parse(raw_info.toStdString(), nullptr, false);
    // Check if invalid JSON
    if (info.is_discarded()) return nullopt;

    const auto get_string = [](const json& json,
                               const char field[]) -> QString {
        if (const auto it = json.find(field);
            it != json.cend() && it->is_string())
            return QString::fromStdString(it->get<string>());
        return "";
    };
    const auto get_uin32_t = [](const json& json,
                                const char field[]) -> uint32_t {
        if (const auto it = json.find(field);
            it != json.cend() && it->is_number_unsigned())
            return json[field].get<uint32_t>();
        return 0;
    };
    const auto get_float = [](const json& json, const char field[]) -> float {
        if (const auto it = json.find(field);
            it != json.cend() && it->is_number())
            return json[field].get<float>();
        return 0.0;
    };

    QString video_id = get_string(info, "id");
    QString title = get_string(info, "title");
    QString author = get_string(info, "channel");
    uint32_t seconds = get_uin32_t(info, "duration");
    QString thumbnail = get_string(info, "thumbnail");
    QString url = get_string(info, "original_url");
    bool audio_available = false;
    QList<VideoFormat> formats;

    const auto formats_it = info.find("formats");
    if (formats_it != info.cend() && formats_it->is_array()) {
        for (const auto& format_it : *formats_it) {
            // Check if this video as a whole (i.e., not necessarily this
            // specific format) has audio available
            const auto acodec = format_it.find("acodec");
            if (acodec != format_it.cend() && acodec->is_string() &&
                acodec->get<string>() != "none") {
                audio_available = true;
            }

            QString format_id = get_string(format_it, "format_id");
            auto vcodec = format_it.find("vcodec");
            // We will skip this format on any of these conditions:
            // 1) format_id is missing (No point in using it if we can't target
            // it) 2) vcodec is N/A (We're only storing formats that have video)
            if (format_id == "" || vcodec == format_it.cend() ||
                !vcodec->is_string() || vcodec->get<string>() == "none") {
                continue;
            }

            QString container = get_string(format_it, "ext");
            uint32_t width = get_uin32_t(format_it, "width");
            uint32_t height = get_uin32_t(format_it, "height");
            float fps = get_float(format_it, "fps");

            formats << VideoFormat(std::move(format_id), std::move(container),
                                   width, height, fps);
        }
    }

    // There's nothing we can download
    if (formats.empty() && !audio_available) return nullopt;

    return VideoInfo(std::move(video_id), std::move(title), std::move(author),
                     std::move(seconds), std::move(thumbnail), std::move(url),
                     std::move(formats), audio_available);
}

/* Fetch video metadata of url parameter and emit it through infoPushed.
    No simultaneous fetches. fetchInfoBadParse is emit if the metadata was
   unusable.
 */
void Downloader::fetch_info(const QString& url) {
    if (is_fetching_ || !check_program_exists()) return;
    set_is_fetching(true);

    QProcess* yt_dlp = create_fetch_process(url);

    QObject::connect(yt_dlp, &QProcess::finished, this, [=, this] {
        this->set_is_fetching(false);

        const QString raw_info = yt_dlp->readAllStandardOutput();
        optional<VideoInfo> info = parseRawInfo(raw_info);

        if (!info.has_value()) {
            emit this->fetchInfoBadParse();
            return;
        }

        emit this->infoPushed(std::move(info.value()));
    });

    yt_dlp->start();
}

void Downloader::enqueue_video(ManagedVideo* const video) {
    queue_ << video;
    if (queue_.size() == 1 && !is_downloading_) {
        start_download();
    }
}

QProcess* Downloader::create_fetch_process(const QString& url) {
    QProcess* yt_dlp = create_generic_process();
    yt_dlp->setArguments({"--dump-json", "\"" % url % "\""});
    return yt_dlp;
}

QProcess* Downloader::create_download_process(const QString& format_id,
                                              const QString& url) {
    QProcess* yt_dlp = create_generic_process();
    QString format_arg = "\"ba\"";
    if (format_id != "") {
        format_arg = "\"" % format_id % "+ba\"";
    }
    yt_dlp->setArguments({"-f", std::move(format_arg), "\"" % url % "\""});

    return yt_dlp;
}

// Create new yt-dlp process. The QProcess will be deleted when its
// finish signal is emit. Sets the write location to the download directory
QProcess* Downloader::create_generic_process() {
    auto* yt_dlp = new QProcess();
    yt_dlp->setProgram(kProgram);
    yt_dlp->setWorkingDirectory(
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    QObject::connect(yt_dlp, &QProcess::finished, yt_dlp,
                     &QObject::deleteLater);
    QObject::connect(yt_dlp, &QProcess::readyReadStandardError, this,
                     [yt_dlp, this] {
                         QString data = yt_dlp->readAllStandardError();
                         emit this->standardErrorPushed(std::move(data));
                     });

    return yt_dlp;
}

void Downloader::start_download() {
    if (queue_.empty() || !check_program_exists()) return;
    set_is_downloading(true);

    ManagedVideo* const video = queue_.takeFirst();
    QProcess* yt_dlp =
        create_download_process(video->selected_format(), video->info().url());
    QObject::connect(video, &ManagedVideo::requestCancelDownload, yt_dlp,
                     &QProcess::kill);
    QObject::connect(yt_dlp, &QProcess::finished, this, [this] {
        set_is_downloading(false);
        this->start_download();
    });

    yt_dlp->start();
}

bool Downloader::is_fetching() const { return is_fetching_; }

bool Downloader::is_downloading() const { return is_downloading_; }

bool Downloader::program_exists() const { return program_exists_; }

void Downloader::set_is_fetching(bool is_fetching) {
    if (is_fetching == is_fetching_) return;
    is_fetching_ = is_fetching;
    emit isFetchingChanged();
}

void Downloader::set_is_downloading(bool is_downloading) {
    if (is_downloading == is_downloading_) return;
    is_downloading_ = is_downloading;
    emit isDownloadingChanged();
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

namespace downloader_util {}
}  // namespace yd_gui
