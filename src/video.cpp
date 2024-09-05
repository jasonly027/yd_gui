#include "video.h"

#include <QtCore/qsharedpointer.h>
#include <qabstractitemmodel.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qtmetamacros.h>
#include <qtpreprocessorsupport.h>
#include <qvariant.h>

#include <QtLogging>
#include <iostream>
#include <optional>
#include <utility>

#include "application_settings.h"
#include "video_list_model.h"

using std::optional, std::nullopt;

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
    os << "VideoFormat{" << " format_id: " << format.format_id().toStdString()
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

    os << "VideoInfo{" << " video_id: " << info.video_id().toStdString()
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
                           DownloadState state, QObject* parent)
    : QObject(parent),
      id_(id),
      created_at_(created_at),
      info_(std::move(info)),
      progress_(0),
      selected_format_(
          !info_.formats().empty() ? info_.formats().last().format_id() : ""),
      download_thumbnail_(ApplicationSettings::get().downloadThumbnail()),
      state_(state) {
    if (!info_.formats().empty()) {
        selected_format_ = info_.formats().last().format_id();
    }
}

ManagedVideo::ManagedVideo(ManagedVideoParts parts, QObject* parent)
    : ManagedVideo(parts.id, parts.created_at, std::move(parts.info),
                   parts.state, parent) {}

void ManagedVideo::setCachedIndex(optional<int> cached_index) {
    cached_index_ = cached_index;
}

void ManagedVideo::setProgress(float progress, const bool update_model_parent) {
    if (progress == progress_) return;

    progress_ = progress;
    emit progressChanged();

    if (const optional<VideoListModel*> model = model_parent();
        update_model_parent && model.has_value()) {
        (*model)->update_video(
            *this, {static_cast<int>(
                       VideoListModel::VideoListModelRole::kProgressRole)});
    }
}

void ManagedVideo::setSelectedFormat(QString selected_format,
                                     const bool update_model_parent) {
    if (selected_format == selected_format_) return;
    selected_format_ = std::move(selected_format);
    emit selectedFormatChanged();

    if (const optional<VideoListModel*> model = model_parent();
        update_model_parent && model.has_value()) {
        (*model)->update_video(
            *this,
            {static_cast<int>(
                VideoListModel::VideoListModelRole::kSelectedFormatRole)});
    }
}

void ManagedVideo::setDownloadThumbnail(bool value,
                                        const bool update_model_parent) {
    if (value == download_thumbnail_) return;
    download_thumbnail_ = value;
    emit downloadThumbnailChanged();

    if (const optional<VideoListModel*> model = model_parent();
        update_model_parent && model.has_value()) {
        (*model)->update_video(
            *this,
            {static_cast<int>(
                VideoListModel::VideoListModelRole::kDownloadThumbnail)});
    }
}

void ManagedVideo::setState(DownloadState state,
                            const bool update_model_parent) {
    /* States can only advance to the next sequential state
       However, they can always return to the added state (because of
       cancellable downloads)

       The only reason these state changes are enforced is because
       it cannot be discerned if a QProcess ended naturally or was
       killed. Both events returns a NormalExit and exitCode of 0
       through QProcess::finished.

       A cancelled download should return the video to the added state
       but that cannot be differentiated from a completed download,
       which should set the video to the completed state.
    */
    switch (state_) {
        case DownloadState::kAdded:
            switch (state) {
                case DownloadState::kAdded:
                    return;
                case DownloadState::kQueued:
                    state_ = state;
                    break;
                case DownloadState::kDownloading:
                case DownloadState::kComplete:
                    return;
            }
            break;
        case DownloadState::kQueued:
            switch (state) {
                case DownloadState::kAdded:
                    state_ = state;
                    break;
                case DownloadState::kQueued:
                    return;
                case DownloadState::kDownloading:
                    state_ = state;
                    break;
                case DownloadState::kComplete:
                    return;
            }
            break;
        case DownloadState::kDownloading:
            switch (state) {
                case DownloadState::kAdded:
                    state_ = state;
                    break;
                case DownloadState::kQueued:
                case DownloadState::kDownloading:
                    return;
                case DownloadState::kComplete:
                    state_ = state;
                    break;
            }
            break;
        case DownloadState::kComplete:
            switch (state) {
                case DownloadState::kAdded:
                case DownloadState::kQueued:
                    state_ = state;
                    break;
                case DownloadState::kDownloading:
                case DownloadState::kComplete:
                    return;
            }
            break;
    }

    emit stateChanged(state_);

    if (const optional<VideoListModel*> model = model_parent();
        update_model_parent && model.has_value()) {
        (*model)->update_video(
            *this,
            {static_cast<int>(VideoListModel::VideoListModelRole::kState)});
    }
}

qint64 ManagedVideo::id() const { return id_; }

qint64 ManagedVideo::created_at() const { return created_at_; }

const optional<int>& ManagedVideo::cached_index() const {
    return cached_index_;
}

const VideoInfo& ManagedVideo::info() const { return info_; }

float ManagedVideo::progress() const { return progress_; }

const QString& ManagedVideo::selected_format() const {
    return selected_format_;
}

bool ManagedVideo::download_thumbnail() const { return download_thumbnail_; }

ManagedVideo::DownloadState ManagedVideo::state() const { return state_; }

bool operator==(const ManagedVideoParts& lhs, const ManagedVideoParts& rhs) {
    return lhs.id == rhs.id && lhs.created_at == rhs.created_at &&
           lhs.info == rhs.info && lhs.state == rhs.state;
}

std::optional<VideoListModel*> ManagedVideo::model_parent() {
    auto* const model = qobject_cast<VideoListModel*>(this->parent());

    return model != nullptr ? optional(model) : nullopt;
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
