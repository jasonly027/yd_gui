#pragma once

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include <QList>
#include <QObject>
#include <QSqlQuery>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtTypes>

#include "video.h"

namespace yd_gui {

class Database : public QObject {
    Q_OBJECT

   public:
    ~Database() override;

    static Database& get();

    static Database get_temp(const QString& connection_name);

    static constexpr qint64 kChunkSize = 25;

    bool valid() const;

    QList<ManagedVideoParts> fetch_first_chunk();

    QList<ManagedVideoParts> fetch_chunk(qint64 last_id,
                                         qint64 last_created_at);

   signals:
    void validChanged(bool valid);

    void errorPushed(QString error);

    void videosPushed(QList<ManagedVideoParts> videos);

   public slots:
    void setValid(bool valid);

    void addVideo(VideoInfo info);

    void removeVideo(qint64 id);

    void removeAllVideos();

   private:
    bool create_tables();

    QList<ManagedVideoParts> extract_videos(QSqlQuery videos_query);

    QList<VideoFormat> extract_formats(QSqlQuery formats_query);

    QSqlQuery create_select_first_chunk_videos(qint64 chunk_size);

    QSqlQuery create_select_chunk_videos(qint64 last_id, qint64 last_created_at,
                                         qint64 chunk_size);

    QSqlQuery create_select_formats(qint64 videos_id);

    std::optional<qint64> fetch_last_insert_id();

    bool insert_video(const VideoInfo& info, qint64 created_at);

    bool insert_format(const VideoFormat& format, qint64 videos_id);

    QSqlQuery make_query();

    QSqlDatabase make_connection();

    void log_error(QString message);

    static constexpr auto kDatabaseFileName = "history.db";

    explicit Database(const QString& file_name = kDatabaseFileName,
                      QString connection_name = kDatabaseFileName,
                      QObject* parent = nullptr);

    QList<ManagedVideoParts> fetch_chunk_impl(QSqlQuery query);

    bool valid_;
    const QString connection_name_;
};

}  // namespace yd_gui
