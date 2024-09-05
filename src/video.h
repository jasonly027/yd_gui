#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include <QtQmlIntegration>
#include <cstddef>
#include <ostream>

namespace yd_gui {

class VideoFormat {
    Q_GADGET
    QML_VALUE_TYPE(videoFormat)
    Q_PROPERTY(QString formatId READ format_id CONSTANT)
    Q_PROPERTY(QString container READ container CONSTANT)
    Q_PROPERTY(quint32 width READ width CONSTANT)
    Q_PROPERTY(quint32 height READ height CONSTANT)
    Q_PROPERTY(float fps READ fps CONSTANT)

   public:
    explicit VideoFormat(QString format_id, QString container, quint32 width,
                         quint32 height, float fps);

    explicit VideoFormat() = default;

    VideoFormat(const VideoFormat& other) = default;

    VideoFormat& operator=(const VideoFormat& other) = default;

    VideoFormat(VideoFormat&& other) = default;

    VideoFormat& operator=(VideoFormat&& other) = default;

    const QString& format_id() const;
    const QString& container() const;
    quint32 width() const;
    quint32 height() const;
    float fps() const;

   private:
    QString format_id_;   // format_id
    QString container_;   // file extension
    quint32 width_ = 0;   // width
    quint32 height_ = 0;  // height
    float fps_ = 0.0;     // fps
};

bool operator==(const VideoFormat& lhs, const VideoFormat& rhs);

bool operator!=(const VideoFormat& lhs, const VideoFormat& rhs);

std::ostream& operator<<(std::ostream& os, const VideoFormat& format);

class VideoInfo {
    Q_GADGET
    QML_VALUE_TYPE(videoInfo)
    Q_PROPERTY(QString videoId READ video_id CONSTANT)
    Q_PROPERTY(QString author READ author CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(quint32 seconds READ seconds CONSTANT)
    Q_PROPERTY(QString thumbnail READ thumbnail CONSTANT)
    Q_PROPERTY(QString url READ url CONSTANT)
    Q_PROPERTY(QList<VideoFormat> formats READ formats CONSTANT)
    Q_PROPERTY(bool audioAvailable READ audio_available CONSTANT)

   public:
    explicit VideoInfo(QString video_id, QString title, QString author,
                       quint32 seconds, QString thumbnail, QString url,
                       QList<VideoFormat> formats, bool audio_available);

    explicit VideoInfo() = default;

    VideoInfo(const VideoInfo& other) = default;

    VideoInfo& operator=(const VideoInfo& other) = default;

    VideoInfo(VideoInfo&& other) = default;

    VideoInfo& operator=(VideoInfo&& other) = default;

    const QString& video_id() const;
    const QString& title() const;
    const QString& author() const;
    const quint32& seconds() const;
    const QString& thumbnail() const;
    const QString& url() const;
    const QList<VideoFormat>& formats() const;
    const bool& audio_available() const;

   private:
    QString video_id_;              // video_id
    QString title_;                 // title of video
    QString author_;                // channel where video is from
    quint32 seconds_ = 0;           // duration of video in seconds
    QString thumbnail_;             // thumbnail url
    QString url_;                   // url of video
    QList<VideoFormat> formats_;    // list of formats
    bool audio_available_ = false;  // audio available
};

bool operator==(const VideoInfo& lhs, const VideoInfo& rhs);

bool operator!=(const VideoInfo& lhs, const VideoInfo& rhs);

std::ostream& operator<<(std::ostream& os, const VideoInfo& info);

struct ManagedVideoParts;

class VideoListModel;

class ManagedVideo : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("");

    Q_PROPERTY(qint64 id READ id CONSTANT)
    Q_PROPERTY(qint64 createdAt READ created_at CONSTANT)
    Q_PROPERTY(VideoInfo info READ info CONSTANT)
    Q_PROPERTY(
        float progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(QString selectedFormat READ selected_format WRITE
                   setSelectedFormat NOTIFY selectedFormatChanged)
    Q_PROPERTY(bool downloadThumbnail READ download_thumbnail WRITE
                   setDownloadThumbnail NOTIFY downloadThumbnailChanged)
    Q_PROPERTY(
        DownloadState state READ state WRITE setState NOTIFY stateChanged)

   public:
    enum class DownloadState { kAdded, kQueued, kDownloading, kComplete };

    Q_ENUM(DownloadState)

    explicit ManagedVideo(qint64 id, qint64 created_at, VideoInfo info,
                          DownloadState state = DownloadState::kAdded,
                          QObject* parent = nullptr);

    explicit ManagedVideo(ManagedVideoParts parts, QObject* parent = nullptr);

    ManagedVideo(const ManagedVideo& other) = delete;

    ManagedVideo& operator=(const ManagedVideo& other) = delete;

    ManagedVideo(ManagedVideo&& other) = delete;

    ManagedVideo& operator=(ManagedVideo&& other) = delete;

    std::optional<VideoListModel*> model_parent();

   signals:
    void progressChanged();
    void selectedFormatChanged();
    void downloadThumbnailChanged();
    void stateChanged(DownloadState state);
    void requestCancelDownload();
    void downloadFinished();

   public slots:
    void setCachedIndex(std::optional<int> cached_index);
    void setProgress(float progress, bool update_model_parent = true);
    void setSelectedFormat(QString selected_format, bool update_model_parent = true);
    void setDownloadThumbnail(bool, bool update_model_parent = true);
    void setState(DownloadState state, bool update_model_parent = true);

   public:  // NOLINT(readability-redundant-access-specifiers)
    qint64 id() const;
    qint64 created_at() const;
    const std::optional<int>& cached_index() const;
    const VideoInfo& info() const;
    float progress() const;
    const QString& selected_format() const;
    bool download_thumbnail() const;
    DownloadState state() const;

   private:
    qint64 id_;          // id generated by database
    qint64 created_at_;  // datetime inserted into database
    std::optional<int>
        cached_index_;  // if this' parent is a VideoListModel, this is its last
                        // known index in the model
    VideoInfo info_;    // video's info
    float progress_;    // download progress from 0.0 to 1.0
    QString selected_format_;  // selected format_id for download
    bool download_thumbnail_;  // whether the thumbnail should be downloaded
    DownloadState state_;      // state of the video
};

using DownloadState = ManagedVideo::DownloadState;

struct ManagedVideoParts {
    qint64 id;
    qint64 created_at;
    VideoInfo info;
    DownloadState state;
};

bool operator==(const ManagedVideoParts& lhs, const ManagedVideoParts& rhs);

}  // namespace yd_gui

template <>
struct std::hash<yd_gui::VideoFormat> {
    std::size_t operator()(const yd_gui::VideoFormat& format) const noexcept;
};
