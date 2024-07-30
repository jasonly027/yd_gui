#include "database.h"

#include <QtSql/qsqldatabase.h>
#include <qdatetime.h>
#include <qobject.h>
#include <qpair.h>
#include <qsqlquery.h>
#include <qtypes.h>

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringBuilder>
#include <algorithm>
#include <cstdint>

#include "video.h"

namespace yd_gui {

Database::~Database() { QSqlDatabase::removeDatabase(connection_name_); }

Database& Database::get() {
    static Database db;
    return db;
}

Database Database::get_temp(const QString& connection_name) {
    return Database(":memory:", connection_name);
}

bool Database::valid() const { return valid_; }

static constexpr qint64 kChunkSize = 25;

QList<QPair<QPair<qint64, qint64>, VideoInfo>> Database::fetch_first_chunk() {
    QSqlDatabase db = QSqlDatabase::database(connection_name_);
    QSqlQuery videos_query = create_select_first_chunk_videos(db, kChunkSize);
    videos_query.setForwardOnly(true);

    if (!videos_query.exec()) {
        log_error("Failed to fetch chunk of history");
        return {};
    }

    // videos is currently in the order of newest to oldest
    QList<QPair<QPair<qint64, qint64>, VideoInfo>> videos =
        extract_videos(std::move(videos_query), db, *this);

    // Must be reversed because it will be PREpended to a model
    // that has videos from oldest to newest.
    // i.e., after reversal, videos should now be from oldest to newest.
    std::reverse(videos.begin(), videos.end());

    return videos;
}

QList<QPair<QPair<qint64, qint64>, VideoInfo>> Database::fetch_chunk(
    const qint64 last_id, const qint64 last_created_at) {
    QSqlDatabase db = QSqlDatabase::database(connection_name_);

    QSqlQuery videos_query =
        create_select_chunk_videos(db, last_id, last_created_at, kChunkSize);
    videos_query.setForwardOnly(true);

    if (!videos_query.exec()) {
        log_error("Failed to fetch chunk of history");
        return {};
    }

    // videos is currently in the order of newest to oldest
    QList<QPair<QPair<qint64, qint64>, VideoInfo>> videos =
        extract_videos(std::move(videos_query), db, *this);

    // Must be reversed because it will be PREpended to a model
    // that has videos from oldest to newest.
    // i.e., after reversal, videos should now be from oldest to newest.
    std::reverse(videos.begin(), videos.end());

    return videos;
}

void Database::setValid(const bool valid) {
    if (valid == valid_) return;
    valid_ = valid;
    emit validChanged(valid_);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Database::addVideo(const VideoInfo& info) {
    QSqlDatabase db = QSqlDatabase::database(connection_name_);
    db.transaction();

    const qint64 created_at = QDateTime::currentSecsSinceEpoch();

    QSqlQuery insert_video = create_insert_video(db, info, created_at);
    if (!insert_video.exec()) {
        log_error("Failed to add video (initial insert)");
        db.rollback();
        return;
    }

    QSqlQuery last_id_query(db);
    qint64 videos_id{};
    bool ok = false;
    if (last_id_query.exec("SELECT last_insert_rowid();") &&
        last_id_query.next()) {
        videos_id = last_id_query.value(0).toLongLong(&ok);
    }
    if (!ok) {
        log_error("Failed to add video (id retrieval)");
        db.rollback();
        return;
    }

    for (const auto& format : info.formats()) {
        QSqlQuery insert_format = create_insert_format(db, format, videos_id);
        if (!insert_format.exec()) {
            log_error("Failed to add video (format insert)");
            db.rollback();
            return;
        }
    }

    if (!db.commit()) {
        log_error("Failed to add video (commit)");
        db.rollback();
        return;
    }

    emit videoPushed(qMakePair(qMakePair(videos_id, created_at), info));
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Database::removeVideo(const qint64 id) {
    const QSqlDatabase db = QSqlDatabase::database(connection_name_);
    QSqlQuery remove_video_query(db);
    remove_video_query.prepare(
        "DELETE FROM videos "
        "WHERE id = :id;");
    remove_video_query.bindValue(":id", id);

    if (!remove_video_query.exec()) log_error("Failed to remove video");
}

void Database::removeAllVideos() {
    const QSqlDatabase db = QSqlDatabase::database(connection_name_);
    QSqlQuery remove_all_query(db);
    if (!remove_all_query.exec("DELETE FROM videos"))
        log_error("Failed to clear");
}

Database::Database(const QString& file_name, QString connection_name,
                   QObject* parent)
    : QObject(parent),
      valid_(false),
      connection_name_(std::move(connection_name)) {
    valid_ = create_database(file_name, connection_name_);
    if (!valid_) return;

    QSqlDatabase db = QSqlDatabase::database(connection_name_);
    valid_ = create_tables(db);
}

void Database::log_error(QString message) {
    emit errorPushed("[History] " % std::move(message));
}

bool Database::create_database(const QString& file_name,
                               const QString& connection_name) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connection_name);
    db.setDatabaseName(file_name);

    bool ok = db.open();
    if (!ok) qDebug() << "[History] Failed to open history";

    return ok;
}

bool Database::create_tables(QSqlDatabase& db) {
    bool ok = false;
    db.transaction();

    ok = create_videos_table(db);
    if (!ok) {
        qDebug() << "[History] Failed to create videos table";
        db.rollback();
        return ok;
    }

    ok = create_formats_table(db);
    if (!ok) {
        qDebug() << "[History] Failed to create formats table";
        db.rollback();
        return ok;
    }

    ok = db.commit();
    if (!ok) {
        qDebug() << "[History] Failed to create tables";
        return ok;
    }

    qInfo() << "[History] Successfully setup history";
    return ok;
}

bool Database::create_videos_table(QSqlDatabase& db) {
    QSqlQuery create_videos(db);
    return create_videos.exec(
        "CREATE TABLE IF NOT EXISTS videos ("
        "    id                 INTEGER     PRIMARY KEY AUTOINCREMENT,"
        "    created_at         INTEGER     NOT NULL,"
        "    video_id           TEXT        NOT NULL,"
        "    title              TEXT        NOT NULL,"
        "    author             TEXT        NOT NULL,"
        "    seconds            INTEGER     NOT NULL,"
        "    thumbnail          TEXT        NOT NULL,"
        "    url                TEXT        NOT NULL,"
        "    audio_available    BOOLEAN     NOT NULL"
        ");");
}

bool Database::create_formats_table(QSqlDatabase& db) {
    QSqlQuery create_formats(db);
    return create_formats.exec(
        "CREATE TABLE IF NOT EXISTS formats ("
        "    id             INTEGER     PRIMARY KEY AUTOINCREMENT,"
        "    format_id      TEXT        NOT NULL,"
        "    container      TEXT        NOT NULL,"
        "    width          INTEGER     NOT NULL,"
        "    height         INTEGER     NOT NULL,"
        "    fps            REAL        NOT NULL,"
        "    videos_id      INTEGER     NOT NULL,"
        "    FOREIGN KEY (videos_id) REFERENCES videos (id) ON DELETE CASCADE"
        ");");
}

// Selected from newest to oldest
QSqlQuery Database::create_select_first_chunk_videos(QSqlDatabase& db,
                                                     qint64 chunk_size) {
    QSqlQuery videos_query(db);
    videos_query.prepare(
        "SELECT id, created_at, video_id, title, author,"
        "    seconds, thumbnail, url, audio_available "
        "FROM videos "

        "ORDER BY created_at DESC, id DESC "

        "LIMIT :limit;");
    videos_query.bindValue(":limit", chunk_size);

    return videos_query;
}

// Selected from newest to oldest
QSqlQuery Database::create_select_chunk_videos(QSqlDatabase& db,
                                               const qint64 last_id,
                                               const qint64 last_created_at,
                                               const qint64 chunk_size) {
    QSqlQuery videos_query(db);
    videos_query.prepare(
        "SELECT id, created_at, video_id, title, author,"
        "    seconds, thumbnail, url, audio_available "
        "FROM videos "

        "WHERE (created_at < :last_created_at) "
        "OR (created_at = :last_created_at AND id < :last_id) "

        "ORDER BY created_at DESC, id DESC "

        "LIMIT :limit;");
    videos_query.bindValue(":id", last_id);
    videos_query.bindValue(":last_created_at", last_created_at);
    videos_query.bindValue(":limit", chunk_size);

    return videos_query;
}

// Selected by oldest to newest
QSqlQuery Database::create_select_formats(QSqlDatabase& db,
                                          const qint64 videos_id) {
    QSqlQuery formats_query(db);
    formats_query.prepare(
        "SELECT format_id, container, width, height, fps "
        "FROM formats "

        "WHERE videos_id = :id "

        "ORDER BY id ASC;");
    formats_query.bindValue(":videos_id", videos_id);

    return formats_query;
}

QList<QPair<QPair<qint64, qint64>, VideoInfo>> Database::extract_videos(
    QSqlQuery videos_query, QSqlDatabase& db, Database& this_db) {
    QList<QPair<QPair<qint64, qint64>, VideoInfo>> videos;
    while (videos_query.next()) {
        bool ok = false;

        const qint64 id = videos_query.value(0).toLongLong(&ok);
        if (!ok) {
            this_db.log_error("id parse failed");
            continue;
        }

        const qint64 created_at = videos_query.value(1).toLongLong(&ok);
        if (!ok) {
            this_db.log_error("created_at parse failed");
            continue;
        }

        QString video_id = videos_query.value(2).toString();

        QString title = videos_query.value(3).toString();

        QString author = videos_query.value(4).toString();

        const uint32_t seconds = videos_query.value(5).toUInt(&ok);
        if (!ok) {
            this_db.log_error("seconds parse failed");
            continue;
        }

        QString thumbnail = videos_query.value(6).toString();

        QString url = videos_query.value(7).toString();
        if (url.isEmpty()) {
            this_db.log_error("url parsed was empty");
            continue;
        }

        const bool audio_available = videos_query.value(8).toBool();

        QSqlQuery formats_query = create_select_formats(db, id);
        formats_query.setForwardOnly(true);

        ok = formats_query.exec();
        if (!ok) {
            this_db.log_error("formats fetch failed");
            continue;
        }

        QList<VideoFormat> formats =
            extract_formats(std::move(formats_query), this_db);

        videos << qMakePair(
            qMakePair(id, created_at),
            VideoInfo(std::move(video_id), std::move(title), std::move(author),
                      seconds, std::move(thumbnail), std::move(url),
                      std::move(formats), audio_available));
    }

    return videos;
}

QList<VideoFormat> Database::extract_formats(QSqlQuery formats_query,
                                             Database& db) {
    QList<VideoFormat> formats;
    while (formats_query.next()) {
        QString format_id = formats_query.value(0).toString();
        if (format_id.isEmpty()) {
            db.log_error("format_id parsed was empty");
            continue;
        }

        QString container = formats_query.value(1).toString();

        bool ok = false;
        const uint32_t width = formats_query.value(2).toUInt(&ok);
        if (!ok) {
            db.log_error("width parse failed");
            continue;
        }

        const uint32_t height = formats_query.value(3).toUInt(&ok);
        if (!ok) {
            db.log_error("height parse failed");
            continue;
        }

        const float fps = formats_query.value(4).toFloat(&ok);
        if (!ok) {
            db.log_error("fps parse failed");
            continue;
        }

        formats << VideoFormat(std::move(format_id), std::move(container),
                               width, height, fps);
    }

    return formats;
}

QSqlQuery Database::create_insert_video(QSqlDatabase& db, const VideoInfo& info,
                                        const qint64 created_at) {
    QSqlQuery insert_video(db);
    insert_video.prepare(
        "INSERT INTO videos"
        "("
        "    created_at, video_id, title, author, seconds,"
        "    thumbnail, url, audio_available"
        ")"

        "VALUES"
        "("
        "    :created_at, :video_id, :title, :author, :seconds,"
        "    :thumbnail, :url, :audio_available"
        ");");

    insert_video.bindValue(":created_at", created_at);
    insert_video.bindValue(":video_id", info.video_id());
    insert_video.bindValue(":title", info.title());
    insert_video.bindValue(":author", info.author());
    insert_video.bindValue(":seconds", info.seconds());
    insert_video.bindValue(":thumbnail", info.thumbnail());
    insert_video.bindValue(":url", info.url());
    insert_video.bindValue(":audio_available", info.audio_available());

    return insert_video;
}

QSqlQuery Database::create_insert_format(QSqlDatabase& db,
                                         const VideoFormat& format,
                                         const qint64 videos_id) {
    QSqlQuery insert_format(db);
    insert_format.prepare(
        "INSERT INTO formats"
        "("
        "    format_id, container, width, height, fps, videos_id"
        ")"

        "VALUES"
        "("
        "    :format_id, :container, :width, :height, :fps, :videos_id"
        ");");
    insert_format.bindValue(":format_id", format.format_id());
    insert_format.bindValue(":container", format.container());
    insert_format.bindValue(":width", format.width());
    insert_format.bindValue(":height", format.height());
    insert_format.bindValue(":fps", format.fps());
    insert_format.bindValue(":videos_id", videos_id);

    return insert_format;
}

}  // namespace yd_gui
