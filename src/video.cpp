#include "video.h"

#include <qtmetamacros.h>

#include <QDebug>
#include <QtLogging>
#include <utility>

namespace yd_gui {
VideoFormat::VideoFormat(QString format_id, QString container, quint32 width,
                         quint32 height, float fps)
    : format_id_(std::move(format_id)),
      container_(std::move(container)),
      width_(width),
      height_(height),
      fps_(fps) {}

// Getters
const QString& VideoFormat::format_id() const { return format_id_; }
const QString& VideoFormat::container() const { return container_; }
quint32 VideoFormat::width() const { return width_; }
quint32 VideoFormat::height() const { return height_; }
float VideoFormat::fps() const { return fps_; }

bool operator==(const VideoFormat& lhs, const VideoFormat& rhs) {
    return lhs.format_id() == rhs.format_id() &&
           lhs.container() == rhs.container() && lhs.width() == rhs.width() &&
           lhs.height() == rhs.height() && lhs.fps() == rhs.fps();
}

bool operator!=(const VideoFormat& lhs, const VideoFormat& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const VideoFormat& format) {
    os << "VideoFormat{"
       << " format_id: " << format.format_id().toStdString()
       << ", container: " << format.container().toStdString()
       << ", width: " << format.width() << ", height: " << format.height()
       << ", fps: " << format.fps() << " }";
    return os;
}

VideoInfo::VideoInfo(QString video_id, QString title, QString author,
                     quint32 seconds, QString thumbnail, QString url,
                     QList<VideoFormat> formats, bool audio_available)
    : video_id_(std::move(video_id)),
      title_(std::move(title)),
      author_(std::move(author)),
      seconds_(seconds),
      thumbnail_(std::move(thumbnail)),
      url_(std::move(url)),
      formats_(std::move(formats)),
      audio_available_(audio_available) {}

// Getters
const QString& VideoInfo::video_id() const { return video_id_; }
const QString& VideoInfo::title() const { return title_; }
const QString& VideoInfo::author() const { return author_; }
const quint32& VideoInfo::seconds() const { return seconds_; }
const QString& VideoInfo::thumbnail() const { return thumbnail_; }
const QString& VideoInfo::url() const { return url_; }
const QList<VideoFormat>& VideoInfo::formats() const { return formats_; }
const bool& VideoInfo::audio_available() const { return audio_available_; }

bool operator==(const VideoInfo& lhs, const VideoInfo& rhs) {
    return lhs.video_id() == rhs.video_id() && lhs.title() == rhs.title() &&
           lhs.author() == rhs.author() && lhs.seconds() == rhs.seconds() &&
           lhs.thumbnail() == rhs.thumbnail() && lhs.url() == rhs.url() &&
           lhs.formats() == rhs.formats() &&
           lhs.audio_available() == rhs.audio_available();
}

bool operator!=(const VideoInfo& lhs, const VideoInfo& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const VideoInfo& info) {
    const auto& formats = info.formats();

    os << "VideoInfo{"
       << " video_id: " << info.video_id().toStdString()
       << ", title: " << info.title().toStdString()
       << ", author: " << info.author().toStdString()
       << ", seconds: " << info.seconds()
       << ", thumbnail: " << info.thumbnail().toStdString()
       << ", url: " << info.url().toStdString() << ", formats: [ ";

    for (const auto& format : formats) {
        os << format << ' ';
    }

    os << "], audio_available: " << info.audio_available() << " }";
    return os;
}

ManagedVideo::ManagedVideo(qint64 id, qint64 created_at, VideoInfo info,
                           QObject* parent)
    : QObject(parent),
      id_(id),
      created_at_(created_at),
      info_(std::move(info)),
      selected_format_(
          !info_.formats().empty() ? info_.formats().last().format_id() : "") {
    if (!info_.formats().empty()) {
        selected_format_ = info_.formats().last().format_id();
    }
}

ManagedVideo::ManagedVideo(ManagedVideoParts parts, QObject* parent)
    : ManagedVideo(get<0>(parts), get<1>(parts), get<2>(std::move(parts)),
                   parent) {}

void ManagedVideo::setProgress(QString progress) {
    if (progress == progress_) return;
    progress_ = std::move(progress);
    emit progressChanged();
}

void ManagedVideo::setSelectedFormat(QString selected_format) {
    if (selected_format == selected_format_) return;
    selected_format_ = std::move(selected_format);
    emit selectedFormatChanged();
}

qint64 ManagedVideo::id() const { return id_; }

qint64 ManagedVideo::created_at() const { return created_at_; }

const VideoInfo& ManagedVideo::info() const { return info_; }

QString ManagedVideo::progress() const { return progress_; }

const QString& ManagedVideo::selected_format() const {
    return selected_format_;
}

}  // namespace yd_gui

std::size_t std::hash<yd_gui::VideoFormat>::operator()(
    const yd_gui::VideoFormat& format) const noexcept {
    std::size_t h1 = std::hash<QString>{}(format.format_id());
    std::size_t h2 = std::hash<QString>{}(format.container());
    std::size_t h3 = std::hash<quint32>{}(format.width());
    std::size_t h4 = std::hash<quint32>{}(format.height());
    std::size_t h5 = std::hash<float>{}(format.fps());

    std::size_t seed = 0;
    seed ^= h1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= h4 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= h5 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}
