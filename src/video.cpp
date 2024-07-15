#include "video.h"

#include <qlogging.h>
#include <qobject.h>

#include <QDebug>
#include <utility>

namespace yd_gui {
VideoFormat::VideoFormat(QString format_id, QString container, uint32_t width,
                         uint32_t height, float fps)
    : format_id_(std::move(format_id)),
      container_(std::move(container)),
      width_(width),
      height_(height),
      fps_(fps) {}

// Getters
const QString& VideoFormat::format_id() const { return format_id_; }
const QString& VideoFormat::container() const { return container_; }
uint32_t VideoFormat::width() const { return width_; }
uint32_t VideoFormat::height() const { return height_; }
float VideoFormat::fps() const { return fps_; }

VideoInfo::VideoInfo(QString video_id, QString title, QString author,
                     uint32_t seconds, QString thumbnail, QString url,
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
const uint32_t& VideoInfo::seconds() const { return seconds_; }
const QString& VideoInfo::thumbnail() const { return thumbnail_; }
const QString& VideoInfo::url() const { return url_; }
const QList<VideoFormat>& VideoInfo::formats() const { return formats_; }
const bool& VideoInfo::audio_available() const { return audio_available_; }

ManagedVideo::ManagedVideo(uint64_t id, VideoInfo info, uint64_t created_at,
                           QObject* parent)
    : id_(id),
      info_(std::move(info)),
      created_at_(created_at),
      progress_(0.0),
      selected_audio_(false),
      QObject(parent) {
    if (!info_.formats().empty()) {
        selected_format_ = info_.formats().last().format_id();
    }
    selected_audio_ = info_.audio_available();
}

void ManagedVideo::setProgress(float progress) {
    if (progress == progress_) return;
    progress_ = progress;
    emit progressChanged();
}

void ManagedVideo::setSelectedFormat(QString selected_format) {
    if (selected_format == selected_format_) return;
    selected_format_ = std::move(selected_format);
    emit selectedFormatChanged();
}

void ManagedVideo::setSelectedAudio(bool selected_audio) {
    if (selected_audio == selected_audio_) return;
    selected_audio_ = selected_audio;
    emit selectedAudioChanged();
}

uint64_t ManagedVideo::id() const { return id_; }

const VideoInfo& ManagedVideo::info() const { return info_; }

uint64_t ManagedVideo::created_at() const { return created_at_; }

float ManagedVideo::progress() const { return progress_; }

const QString& ManagedVideo::selected_format() const {
    return selected_format_;
}

bool ManagedVideo::selected_audio() const { return selected_audio_; }

}  // namespace yd_gui
