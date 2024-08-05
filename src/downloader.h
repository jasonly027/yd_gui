#pragma once

#include <qtmetamacros.h>

#include <QList>
#include <QObject>
#include <QProcess>
#include <QSharedPointer>
#include <QtQml>
#include <QtQmlIntegration>
#include <optional>

#include "video.h"

namespace yd_gui {
using std::optional;

class Downloader : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool isFetching READ is_fetching NOTIFY isFetchingChanged)
    Q_PROPERTY(
        bool isDownloading READ is_downloading NOTIFY isDownloadingChanged)
    Q_PROPERTY(
        bool programExists READ program_exists NOTIFY programExistsChanged)

   public:
    explicit Downloader(QObject* parent = nullptr);

    static optional<VideoInfo> parseRawInfo(const QString& raw_info);

    Q_INVOKABLE void fetch_info(const QString& url);

    Q_INVOKABLE void enqueue_video(ManagedVideo* video);

   signals:
    void isFetchingChanged();

    void isDownloadingChanged();

    void programExistsChanged();

    void standardErrorPushed(QString data);

    void standardOutputPushed(QString data);

    void infoPushed(VideoInfo info);

    void fetchInfoBadParse();

   public:
    bool is_fetching() const;

    bool is_downloading() const;

    bool program_exists() const;

   private:
    QProcess* create_fetch_process(const QString& url);

    QProcess* create_download_process(const QString& format_id,
                                      const QString& url);

    QProcess* create_generic_process();

    void start_download();

    void set_is_fetching(bool is_fetching);

    void set_is_downloading(bool is_downloading);

    void set_program_exists(bool program_exists);

    bool check_program_exists();

    bool is_fetching_ = false;
    bool is_downloading_ = false;
    bool program_exists_ = true;
    QList<ManagedVideo*> queue_;
};
}  // namespace yd_gui
