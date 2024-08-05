#include "downloader.h"

#include <qtmetamacros.h>

#include <QDebug>
#include <QDir>
#include <QList>
#include <QObject>
#include <QOverload>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QtQmlIntegration>
#include <cassert>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <utility>

#include "video.h"

namespace yd_gui {

using nlohmann::basic_json;
using nlohmann::json;
using std::nullopt;
using std::string;

Downloader::Downloader(QObject* parent) : QObject(parent) {}

static constexpr auto kProgram = "yt-dlp";

template <typename T>
static T from_field(const char field[], const json& json,
                    bool (json::basic_json::*condition)() const) {
    const auto it = json.find(field);
    if (it != json.cend() && ((*it).*condition)()) {
        return it->get<T>();
    }
    return T{};
}

// Deserialize from JSON to VideoInfo
optional<VideoInfo> Downloader::parseRawInfo(const QString& raw_info) {
    const json info = json::parse(raw_info.toStdString(), nullptr, false);
    // Check if invalid JSON
    if (info.is_discarded()) return nullopt;

    auto video_id = QString::fromStdString(
        from_field<string>("id", info, &basic_json<>::is_string));

    auto title = QString::fromStdString(
        from_field<string>("title", info, &basic_json<>::is_string));

    auto author = QString::fromStdString(
        from_field<string>("channel", info, &basic_json<>::is_string));

    auto seconds = from_field<quint32>("duration", info,
                                       &basic_json<>::is_number_unsigned);

    auto thumbnail = QString::fromStdString(
        from_field<string>("thumbnail", info, &basic_json<>::is_string));

    auto url = QString::fromStdString(
        from_field<string>("original_url", info, &basic_json<>::is_string));

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

            auto format_id = QString::fromStdString(from_field<string>(
                "format_id", format_it, &basic_json<>::is_string));

            auto vcodec = format_it.find("vcodec");
            // We will skip this format on any of these conditions:
            // 1) format_id is missing (No point in using it if we can't target
            // it) 2) vcodec is N/A (We're only storing formats that have video)
            if (format_id == "" || vcodec == format_it.cend() ||
                !vcodec->is_string() || vcodec->get<string>() == "none") {
                continue;
            }

            auto container = QString::fromStdString(
                from_field<string>("ext", format_it, &basic_json<>::is_string));

            auto width = from_field<quint32>("width", format_it,
                                             &basic_json<>::is_number_unsigned);

            auto height = from_field<quint32>(
                "height", format_it, &basic_json<>::is_number_unsigned);

            auto fps =
                from_field<quint32>("fps", format_it, &basic_json<>::is_number);

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
        const QString raw_info = yt_dlp->readAllStandardOutput();
        optional<VideoInfo> info = parseRawInfo(raw_info);

        if (!info.has_value()) {
            emit this->fetchInfoBadParse();
        } else {
            emit this->infoPushed(std::move(info.value()));
        }

        this->set_is_fetching(false);
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
    yt_dlp->setArguments({"--dump-json", url});
    return yt_dlp;
}

QProcess* Downloader::create_download_process(const QString& format_id,
                                              const QString& url) {
    QProcess* yt_dlp = create_generic_process();
    QString format_arg = "ba";
    if (format_id != "") {
        format_arg = format_id % "+" % format_arg;
    }
    yt_dlp->setArguments({"--quiet", "--progress", "--progress-template",
                          "%(progress._percent_str)s", "--newline", "-f",
                          std::move(format_arg), url});

    return yt_dlp;
}

// Create new yt-dlp process. The QProcess will be deleted when its
// finish signal is emit. Sets the write location to the download directory.
// Standard error is forwarded to standardErrorPushed signal.
QProcess* Downloader::create_generic_process() {
    auto* yt_dlp = new QProcess();

    yt_dlp->setProgram(kProgram);

    if (QString dir =
            QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        QDir(dir).exists()) {
        yt_dlp->setWorkingDirectory(dir);
    }

    QObject::connect(yt_dlp, &QProcess::finished, yt_dlp,
                     &QObject::deleteLater);

    QObject::connect(yt_dlp, &QProcess::readyReadStandardError, this,
                     [yt_dlp, this] {
                         QString data = yt_dlp->readAllStandardError();
                         emit this->standardErrorPushed(std::move(data));
                     });

    QObject::connect(
        yt_dlp, &QProcess::errorOccurred, this,
        [this](QProcess::ProcessError err) {
            std::cerr << "Process error occured, Code: " << err << '\n';
            emit this->standardErrorPushed("Process error occured");
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

    QObject::connect(
        yt_dlp, &QProcess::readyReadStandardOutput, [yt_dlp, video] {
            const QString data = yt_dlp->readAllStandardOutput();

            const QRegularExpression re(
                R"#(^( (?<percent>100(\.0{1,2})?|[1-9]?\d(\.\d{1,2})?%)\n)+$)#");
            const auto match = re.match(data);

            if (match.hasMatch()) {
                video->setProgress(match.captured("percent"));
            }
        });

    QObject::connect(
        yt_dlp, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, [video, this](int exit_code, QProcess::ExitStatus exit_status) {
            if (exit_status == QProcess::ExitStatus::NormalExit &&
                exit_code == 0) {
                video->setProgress("100%");
            }
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
