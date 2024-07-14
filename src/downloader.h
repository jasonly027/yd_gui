#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include <QList>
#include <QObject>
#include <QProcess>
#include <QSharedPointer>
#include <QtQml>
#include <optional>

#include "video.h"

namespace yd_gui {
using std::optional;

class Downloader : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool isFetching READ is_fetching NOTIFY isFetchingChanged);
    Q_PROPERTY(
        bool programExists READ program_exists NOTIFY programExistsChanged);

   public:

    explicit Downloader() = default;

    static QProcess* createProgram();

    static optional<VideoInfo> parseRawInfo(const QString& raw_info);

    Q_INVOKABLE void fetch_info(const QString& url);

    // Q_INVOKABLE void enqueue_video(const video::ManagedVideo& video);

    bool is_fetching() const;

    bool program_exists() const;

   private:
    void set_is_fetching(bool is_fetching);

    void set_program_exists(bool program_exists);

    bool check_program_exists();

   signals:
    void isFetchingChanged();
    void programExistsChanged();
    void standardErrorPushed(QString data);
    void infoPushed(VideoInfo info);
    void fetchInfoBadParse();

   private:
    bool is_fetching_ = false;
    bool program_exists_ = true;
    QList<QSharedPointer<ManagedVideo>> queue_;
};
}  // namespace yd_gui
