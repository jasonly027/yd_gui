#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qprocess.h>
#include <qstringview.h>
#include <qtmetamacros.h>

#include <QSharedPointer>
#include <QtQml>
#include <QtQmlIntegration>
#include <optional>

#include "video.h"

namespace yd_gui {

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

    static std::optional<VideoInfo> parseRawInfo(const QString& raw_info);

    Q_INVOKABLE void fetchInfo(const QString& url);

   signals:
    void isFetchingChanged();

    void isDownloadingChanged();

    void programExistsChanged();

    void standardErrorPushed(QString data);

    void standardOutputPushed(QString data);

    void infoPushed(VideoInfo info);

    void fetchInfoBadParse();

   public slots:
    void enqueue_video(ManagedVideo* video);

   public:  // NOLINT(readability-redundant-access-specifiers)
    bool is_fetching() const;

    bool is_downloading() const;

    bool program_exists() const;

   private:
    void async_parse_raw_info(const QByteArray& data);

    QProcess* create_fetch_process(const QString& url);

    QProcess* create_download_process(ManagedVideo& video);

    QProcess* create_generic_process();

    void start_download();

    void set_is_fetching(bool is_fetching);

    void set_is_downloading(bool is_downloading);

    void set_program_exists(bool program_exists);

    bool check_program_exists();

    bool is_fetching_;
    bool is_downloading_;
    bool program_exists_;
    QList<ManagedVideo*> queue_;
};
}  // namespace yd_gui
