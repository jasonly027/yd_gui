#include "database.h"

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qtypes.h>

#include <QDateTime>
#include <QObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringBuilder>
#include <QtTypes>
#include <algorithm>
#include <optional>
#include <tuple>

#include "video.h"

namespace yd_gui {

using std::make_tuple, std::tuple, std::nullopt, std::optional;

Database::~Database() { QSqlDatabase::removeDatabase(connection_name_); }

Database& Database::get() {
    static Database db;
    return db;
}

// For testing purposes
Database Database::get_temp(const QString& connection_name) {
    return Database(":memory:", connection_name);
}

bool Database::valid() const { return valid_; }

QList<ManagedVideoParts> Database::fetch_first_chunk() {
    return fetch_chunk_impl(create_select_first_chunk_videos(kChunkSize));
}

QList<ManagedVideoParts> Database::fetch_chunk(const qint64 last_id,
                                               const qint64 last_created_at) {
    return fetch_chunk_impl(
        create_select_chunk_videos(last_id, last_created_at, kChunkSize));
}

// Query should be passed from create_select_first_chunk_videos() or
// create_select_chunk_videos()
QList<ManagedVideoParts> Database::fetch_chunk_impl(QSqlQuery query) {
    if (!query.exec()) {
        log_error("Failed to fetch chunk of history");
        return {};
    }

    // videos is currently in the order of newest to oldest
    auto videos = extract_videos(std::move(query));

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
    if (!db.transaction()) {
        log_error("Failed to add video (start add)");
        return;
    }

    const qint64 created_at = QDateTime::currentSecsSinceEpoch();

    if (!insert_video(info, created_at)) {
        db.rollback();
        return;
    }

    optional<qint64> opt_videos_id = fetch_last_insert_id();
    if (!opt_videos_id.has_value()) {
        db.rollback();
        return;
    }
    qint64 videos_id = opt_videos_id.value();

    for (const auto& format : info.formats()) {
        if (!insert_format(format, videos_id)) {
            db.rollback();
            return;
        }
    }

    if (!db.commit()) {
        log_error("Failed to commit video");
        db.rollback();
        return;
    }

    emit videosPushed({make_tuple(videos_id, created_at, info)});
}

void Database::removeVideo(const qint64 id) {
    QSqlQuery query = make_query();
    if (!query.prepare("DELETE FROM videos "
                       "WHERE id = :id;")) {
        log_error("Failed to prepare query for video removal");
    }
    query.bindValue(":id", id);

    if (!query.exec()) log_error("Failed to remove video");
}

void Database::removeAllVideos() {
    if (!make_query().exec("DELETE FROM videos")) log_error("Failed to clear");
}

static bool create_videos_table(const QSqlDatabase& db) {
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

static bool create_formats_table(const QSqlDatabase& db) {
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

bool Database::create_tables() {
    QSqlDatabase db = make_connection();

    if (!db.transaction()) {
        log_error("Failed to start creating tables");
        return false;
    }

    if (!create_videos_table(db)) {
        log_error("Failed to create videos table");
        db.rollback();
        return false;
    }

    if (!create_formats_table(db)) {
        log_error("Failed to create formats table");
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        log_error("Failed to create tables");
        return false;
    }

    qInfo() << "[History] Successfully setup history";
    return true;
}

QList<ManagedVideoParts> Database::extract_videos(QSqlQuery videos_query) {
    QList<ManagedVideoParts> videos;
    while (videos_query.next()) {
        bool ok = false;

        const qint64 id = videos_query.value(0).toLongLong(&ok);
        if (!ok) {
            log_error("id parse failed");
            continue;
        }

        const qint64 created_at = videos_query.value(1).toLongLong(&ok);
        if (!ok) {
            log_error("created_at parse failed");
            continue;
        }

        QString video_id = videos_query.value(2).toString();

        QString title = videos_query.value(3).toString();

        QString author = videos_query.value(4).toString();

        const quint32 seconds = videos_query.value(5).toUInt(&ok);
        if (!ok) {
            log_error("seconds parse failed");
            continue;
        }

        QString thumbnail = videos_query.value(6).toString();

        QString url = videos_query.value(7).toString();
        if (url.isEmpty()) {
            log_error("url parsed was empty");
            continue;
        }

        const bool audio_available = videos_query.value(8).toBool();

        QSqlQuery formats_query = create_select_formats(id);

        ok = formats_query.exec();
        if (!ok) {
            log_error("formats fetch failed");
            continue;
        }

        auto formats = extract_formats(std::move(formats_query));

        videos << make_tuple(
            id, created_at,
            VideoInfo(std::move(video_id), std::move(title), std::move(author),
                      seconds, std::move(thumbnail), std::move(url),
                      std::move(formats), audio_available));
    }

    return videos;
}

QList<VideoFormat> Database::extract_formats(QSqlQuery formats_query) {
    QList<VideoFormat> formats;
    while (formats_query.next()) {
        QString format_id = formats_query.value(0).toString();
        if (format_id.isEmpty()) {
            log_error("format_id parsed was empty");
            continue;
        }

        QString container = formats_query.value(1).toString();

        bool ok = false;
        const quint32 width = formats_query.value(2).toUInt(&ok);
        if (!ok) {
            log_error("width parse failed");
            continue;
        }

        const quint32 height = formats_query.value(3).toUInt(&ok);
        if (!ok) {
            log_error("height parse failed");
            continue;
        }

        const float fps = formats_query.value(4).toFloat(&ok);
        if (!ok) {
            log_error("fps parse failed");
            continue;
        }

        formats << VideoFormat(std::move(format_id), std::move(container),
                               width, height, fps);
    }

    return formats;
}

// Selected from newest to oldest
QSqlQuery Database::create_select_first_chunk_videos(qint64 chunk_size) {
    QSqlQuery query = make_query();
    if (!query.prepare("SELECT id, created_at, video_id, title, author,"
                       "    seconds, thumbnail, url, audio_available "
                       "FROM videos "

                       "ORDER BY created_at DESC, id DESC "

                       "LIMIT :limit;")) {
        log_error("Failed to prepare query for select first chunk");
    }
    query.bindValue(":limit", chunk_size);
    query.setForwardOnly(true);

    return query;
}

// Selected from newest to oldest
QSqlQuery Database::create_select_chunk_videos(const qint64 last_id,
                                               const qint64 last_created_at,
                                               const qint64 chunk_size) {
    QSqlQuery query = make_query();
    if (!query.prepare("SELECT id, created_at, video_id, title, author,"
                       "    seconds, thumbnail, url, audio_available "
                       "FROM videos "

                       "WHERE (created_at < :last_created_at) "
                       "OR (created_at = :last_created_at AND id < :last_id) "

                       "ORDER BY created_at DESC, id DESC "

                       "LIMIT :limit;")) {
        log_error("Failed to prepare query for select chunk");
    }
    query.bindValue(":last_id", last_id);
    query.bindValue(":last_created_at", last_created_at);
    query.bindValue(":limit", chunk_size);
    query.setForwardOnly(true);

    return query;
}

// Selected by oldest to newest
QSqlQuery Database::create_select_formats(const qint64 videos_id) {
    QSqlQuery query = make_query();
    if (!query.prepare("SELECT format_id, container, width, height, fps "
                       "FROM formats "

                       "WHERE videos_id = :videos_id "

                       "ORDER BY id ASC;")) {
        log_error("Failed to prepare query for selecting formats");
    }
    query.bindValue(":videos_id", videos_id);
    query.setForwardOnly(true);

    return query;
}

optional<qint64> Database::fetch_last_insert_id() {
    QSqlQuery query = make_query();

    if (query.exec("SELECT last_insert_rowid();") && query.next() &&
        query.value(0).canConvert<qint64>()) {
        return query.value(0).toLongLong();
    }
    log_error("Failed to fetch last insert id");

    return nullopt;
}

bool Database::insert_video(const VideoInfo& info, const qint64 created_at) {
    QSqlQuery query = make_query();
    if (!query.prepare("INSERT INTO videos"
                       "("
                       "    created_at, video_id, title, author, seconds,"
                       "    thumbnail, url, audio_available"
                       ")"

                       "VALUES"
                       "("
                       "    :created_at, :video_id, :title, :author, :seconds,"
                       "    :thumbnail, :url, :audio_available"
                       ");")) {
        log_error("Failed to prepare query for inserting video");
    }
    query.bindValue(":created_at", created_at);
    query.bindValue(":video_id", info.video_id());
    query.bindValue(":title", info.title());
    query.bindValue(":author", info.author());
    query.bindValue(":seconds", info.seconds());
    query.bindValue(":thumbnail", info.thumbnail());
    query.bindValue(":url", info.url());
    query.bindValue(":audio_available", info.audio_available());

    bool ok = query.exec();
    if (!ok) log_error("Failed to add video");

    return ok;
}

bool Database::insert_format(const VideoFormat& format,
                             const qint64 videos_id) {
    QSqlQuery query = make_query();
    if (!query.prepare(
            "INSERT INTO formats"
            "("
            "    format_id, container, width, height, fps, videos_id"
            ")"

            "VALUES"
            "("
            "    :format_id, :container, :width, :height, :fps, :videos_id"
            ");")) {
        log_error("Failed to prepare query for inserting format");
    }
    query.bindValue(":format_id", format.format_id());
    query.bindValue(":container", format.container());
    query.bindValue(":width", format.width());
    query.bindValue(":height", format.height());
    query.bindValue(":fps", format.fps());
    query.bindValue(":videos_id", videos_id);

    bool ok = query.exec();
    if (!ok) log_error("Failed to add format");

    return ok;
}

QSqlQuery Database::make_query() { return QSqlQuery(make_connection()); }

QSqlDatabase Database::make_connection() {
    return QSqlDatabase::database(connection_name_);
}

void Database::log_error(QString message) {
    emit errorPushed("[History] " % std::move(message));
}

static bool create_database(const QString& file_name, const QString& connection_name) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connection_name);
    db.setDatabaseName(file_name);

    if (!db.open()) {
        qDebug() << "[History] Failed to open history";
        return false;
    }

    return true;
}

Database::Database(const QString& file_name, QString connection_name,
                   QObject* parent)
    : QObject(parent),
      valid_(false),
      connection_name_(std::move(connection_name)) {
    valid_ = create_database(file_name, connection_name_);
    if (!valid_) return;

    QSqlDatabase db = QSqlDatabase::database(connection_name_);
    valid_ = create_tables();
}

}  // namespace yd_gui
