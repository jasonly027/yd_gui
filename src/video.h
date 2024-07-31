#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include <QList>
#include <QObject>
#include <QString>
#include <cstdint>
#include <ostream>
#include <string>

namespace yd_gui {

using std::string;

class VideoFormat {
    Q_GADGET
    Q_PROPERTY(QString format_id READ format_id CONSTANT)
    Q_PROPERTY(QString container READ container CONSTANT)
    Q_PROPERTY(uint32_t width READ width CONSTANT)
    Q_PROPERTY(uint32_t height READ height CONSTANT)
    Q_PROPERTY(float fps READ fps CONSTANT)

   public:
    explicit VideoFormat(QString format_id, QString container, uint32_t width,
                         uint32_t height, float fps);
    explicit VideoFormat() = default;
    VideoFormat(const VideoFormat& other) = default;
    VideoFormat& operator=(const VideoFormat& other) = default;
    VideoFormat(VideoFormat&& other) = default;
    VideoFormat& operator=(VideoFormat&& other) = default;

    const QString& format_id() const;
    const QString& container() const;
    uint32_t width() const;
    uint32_t height() const;
    float fps() const;

   private:
    QString format_id_;    // format_id
    QString container_;    // file extension
    uint32_t width_ = 0;   // width
    uint32_t height_ = 0;  // height
    float fps_ = 0.0;      // fps
};

bool operator==(const VideoFormat& lhs, const VideoFormat& rhs);

bool operator!=(const VideoFormat& lhs, const VideoFormat& rhs);

std::ostream& operator<<(std::ostream& os, const VideoFormat& format);

class VideoInfo {
    Q_GADGET
    Q_PROPERTY(QString video_id READ video_id CONSTANT)
    Q_PROPERTY(QString author READ author CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(uint32_t seconds READ seconds CONSTANT)
    Q_PROPERTY(QString thumbnail READ thumbnail CONSTANT)
    Q_PROPERTY(QString url READ url CONSTANT)
    Q_PROPERTY(QList<VideoFormat> formats READ formats CONSTANT)
    Q_PROPERTY(bool audio_available READ audio_available CONSTANT)

   public:
    explicit VideoInfo(QString video_id, QString title, QString author,
                       uint32_t seconds, QString thumbnail, QString url,
                       QList<VideoFormat> formats, bool audio_available);
    explicit VideoInfo() = default;
    VideoInfo(const VideoInfo& other) = default;
    VideoInfo& operator=(const VideoInfo& other) = default;
    VideoInfo(VideoInfo&& other) = default;
    VideoInfo& operator=(VideoInfo&& other) = default;

    const QString& video_id() const;
    const QString& title() const;
    const QString& author() const;
    const uint32_t& seconds() const;
    const QString& thumbnail() const;
    const QString& url() const;
    const QList<VideoFormat>& formats() const;
    const bool& audio_available() const;

   private:
    QString video_id_;              // video_id
    QString title_;                 // title of video
    QString author_;                // channel where video is from
    uint32_t seconds_ = 0;          // duration of video in seconds
    QString thumbnail_;             // thumbnail url
    QString url_;                   // url of video
    QList<VideoFormat> formats_;    // list of formats
    bool audio_available_ = false;  // audio available
};

bool operator==(const VideoInfo& lhs, const VideoInfo& rhs);

bool operator!=(const VideoInfo& lhs, const VideoInfo& rhs);

std::ostream& operator<<(std::ostream& os, const VideoInfo& info);

class ManagedVideo : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("");

    Q_PROPERTY(qint64 id READ id CONSTANT)
    Q_PROPERTY(VideoInfo info READ info CONSTANT)
    Q_PROPERTY(qint64 createdAt READ created_at CONSTANT)
    Q_PROPERTY(
        QString progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(QString selectedFormat READ selected_format WRITE
                   setSelectedFormat NOTIFY selectedFormatChanged)

   public:
    explicit ManagedVideo(qint64 id, VideoInfo info, qint64 created_at,
                          QObject* parent = nullptr);
    ~ManagedVideo() override;
    ManagedVideo(const ManagedVideo& other) = delete;
    ManagedVideo& operator=(const ManagedVideo& other) = delete;
    ManagedVideo(ManagedVideo&& other) = delete;
    ManagedVideo& operator=(ManagedVideo&& other) = delete;

   public slots:  // NOLINT(readability-redundant-access-specifiers)
    void setProgress(QString progress);
    void setSelectedFormat(QString selected_format);

   signals:
    void progressChanged();
    void selectedFormatChanged();
    void requestCancelDownload();

   public:
    qint64 id() const;
    const VideoInfo& info() const;
    qint64 created_at() const;
    QString progress() const;
    const QString& selected_format() const;

   private:
    qint64 id_;                // id generated by database
    VideoInfo info_;           // video's info
    qint64 created_at_;        // datetime inserted into database
    QString progress_;         // download progress from 0.0 to 1.0
    QString selected_format_;  // selected format_id for download
};
}  // namespace yd_gui

template <>
struct std::hash<yd_gui::VideoFormat> {
    std::size_t operator()(const yd_gui::VideoFormat& format) const noexcept;
};
