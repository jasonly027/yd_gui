#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include <QList>
#include <QObject>
#include <QString>
#include <cstdint>
#include <string>

namespace yd_gui {

using std::string;

class VideoFormat {
    Q_GADGET
    Q_PROPERTY(QString format_id READ format_id CONSTANT);
    Q_PROPERTY(QString container READ container CONSTANT);
    Q_PROPERTY(uint32_t width READ width CONSTANT);
    Q_PROPERTY(uint32_t height READ height CONSTANT);
    Q_PROPERTY(float fps READ fps CONSTANT);

   public:
    explicit VideoFormat(QString format_id, QString container, uint32_t width,
                         uint32_t height, float fps);
    explicit VideoFormat() = default;
    VideoFormat(const VideoFormat& other) = default;
    VideoFormat& operator=(const VideoFormat& other) = default;
    VideoFormat(VideoFormat&& other) = default;
    VideoFormat& operator=(VideoFormat&& other) = default;

    friend bool operator==(const VideoFormat&, const VideoFormat&) = default;
    // friend bool operator==(const VideoFormat& lhs, const VideoFormat& rhs) {
    //     return lhs.format_id_ == rhs.format_id_ &&
    //            lhs.container_ == rhs.container_ && lhs.width_ == rhs.width_ &&
    //            lhs.height_ == rhs.height_ && lhs.fps_ == rhs.fps_;
    // }

    const QString& format_id() const;
    const QString& container() const;
    uint32_t width() const;
    uint32_t height() const;
    float fps() const;

   private:
    QString format_id_;
    QString container_;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    float fps_ = 0.0;
};

class VideoInfo {
    Q_GADGET
    Q_PROPERTY(QString video_id READ video_id CONSTANT);
    Q_PROPERTY(QString author READ author CONSTANT);
    Q_PROPERTY(QString title READ title CONSTANT);
    Q_PROPERTY(uint32_t seconds READ seconds CONSTANT);
    Q_PROPERTY(QString thumbnail READ thumbnail CONSTANT);
    Q_PROPERTY(QList<VideoFormat> formats READ formats CONSTANT);
    Q_PROPERTY(bool audio_available READ audio_available CONSTANT);

   public:
    explicit VideoInfo(QString video_id, QString title, QString author,
                       uint32_t seconds, QString thumbnail,
                       QList<VideoFormat> formats, bool audio_available);
    explicit VideoInfo() = default;
    VideoInfo(const VideoInfo& other) = default;
    VideoInfo& operator=(const VideoInfo& other) = default;
    VideoInfo(VideoInfo&& other) = default;
    VideoInfo& operator=(VideoInfo&& other) = default;

    friend bool operator==(const VideoInfo&, const VideoInfo&) = default;

    const QString& video_id() const;
    const QString& title() const;
    const QString& author() const;
    const uint32_t& seconds() const;
    const QString& thumbnail() const;
    const QList<VideoFormat>& formats() const;
    const bool& audio_available() const;

   private:
    QString video_id_;
    QString title_;
    QString author_;
    uint32_t seconds_ = 0;
    QString thumbnail_;
    QList<VideoFormat> formats_;
    bool audio_available_ = false;
};

class ManagedVideo : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("");

    Q_PROPERTY(uint64_t id READ id CONSTANT);
    Q_PROPERTY(VideoInfo info READ info CONSTANT);
    Q_PROPERTY(uint64_t created_at READ created_at CONSTANT);
    Q_PROPERTY(
        float progress READ progress WRITE setProgress NOTIFY progressChanged);

   public:
    explicit ManagedVideo(uint64_t id, VideoInfo info, uint64_t created_at,
                          float progress = 0.0, QObject* parent = nullptr);
    ManagedVideo(const ManagedVideo& other) = delete;
    ManagedVideo& operator=(const ManagedVideo& other) = delete;
    ManagedVideo(ManagedVideo&& other) = delete;
    ManagedVideo& operator=(ManagedVideo&& other) = delete;

    uint64_t id() const;
    const VideoInfo& info() const;
    uint64_t created_at() const;
    float progress() const;

   public slots:  // NOLINT(readability-redundant-access-specifiers)
    void setProgress(float progress);

   signals:
    void progressChanged();

   private:
    uint64_t id_;
    VideoInfo info_;
    uint64_t created_at_;
    float progress_;
};
}  // namespace yd_gui
