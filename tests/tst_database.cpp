#include <database.h>
#include <downloader.h>
#include <gtest/gtest.h>

#include <QDateTime>
#include <QObject>
#include <QSignalSpy>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QStringBuilder>
#include <QtTypes>
#include <iostream>
#include <limits>
#include <utility>

#include "video.h"

using std::tuple;

namespace yd_gui {

static QString get_test_name() {
    return QString::fromStdString(
        ::testing::UnitTest::GetInstance()->current_test_info()->name());
}

class DatabaseTest : public testing::Test {
   protected:
    DatabaseTest() {
        EXPECT_TRUE(db_.valid());
        QObject::connect(&db_, &Database::errorPushed, [](const QString& data) {
            std::cerr << data.toStdString() << '\n';
        });
    };

    QSqlQuery create_query() {
        return QSqlQuery(QSqlDatabase::database(connection_name_));
    }

    qint64 get_num_rows_in_videos() {
        QSqlQuery query = create_query();
        EXPECT_TRUE(query.exec("SELECT COUNT(*) FROM videos;"))
            << "Query exec failed";
        query.next();

        bool ok = false;
        qint64 rows = query.value(0).toLongLong(&ok);
        EXPECT_TRUE(ok) << "Failed to convert row count to qint64";
        return rows;
    }

    // Convenience variables for when SELECT'ing
    constexpr static auto kVideosColumns{
        " id, created_at, video_id, title, author, seconds, thumbnail, url, "
        "audio_available "};
    constexpr static auto kFormatsColumns{
        " id, format_id, container, width, height, fps, videos_id "};

    // Check ManagedVideo Components are in increasing order
    static void check_managed_video_parts(
        const QList<ManagedVideoParts>& parts) {
        auto [last_id, last_created_at, last_info] = parts.first();

        for (const auto& parts : parts) {
            const auto& [id, created_at, info] = parts;

            // Skip first parts;
            if (id == last_id && created_at == last_created_at &&
                info == last_info) {
                continue;
            }

            EXPECT_GT(id, last_id) << "Parts were not ordered by increasing id";
            EXPECT_GE(created_at, last_created_at)
                << "Parts were not ordered by increasing created_at";

            last_id = id;
            last_created_at = created_at;
        }
    };

    // Expects the query's values to be the same order as kVideosColumns
    // Returns back the query after using it
    static void check_video_query(const QSqlRecord& record,
                                  const VideoInfo& expected_info,
                                  const qint64 expected_id,
                                  const qint64 before_add,
                                  const qint64 after_add) {
        bool ok = false;

        const qint64 id = record.value(0).toLongLong(&ok);
        EXPECT_TRUE(ok) << "Failed to convert value to id";
        const qint64 created_at = record.value(1).toLongLong(&ok);
        EXPECT_TRUE(ok) << "Failed to convert value to created_at ";
        const QString video_id = record.value(2).toString();
        const QString title = record.value(3).toString();
        const QString author = record.value(4).toString();
        const qint64 seconds = record.value(5).toUInt(&ok);
        EXPECT_TRUE(ok) << "Failed to convert value to seconds";
        const QString thumbnail = record.value(6).toString();
        const QString url = record.value(7).toString();
        const bool audio_available = record.value(8).toBool();

        EXPECT_EQ(id, expected_id) << "Unexpected id";
        EXPECT_GE(created_at, before_add)
            << "created_at timestamp was before addVideo initiated";
        EXPECT_LE(created_at, after_add)
            << "created_at timestamp was after addVideo completed";
        EXPECT_EQ(video_id.toStdString(),
                  expected_info.video_id().toStdString())
            << "Unexpected video_id";
        EXPECT_EQ(title.toStdString(), expected_info.title().toStdString())
            << "Unexpected title";
        EXPECT_EQ(author.toStdString(), expected_info.author().toStdString())
            << "Unexpected author";
        EXPECT_EQ(seconds, expected_info.seconds()) << "Unexpected seconds";
        EXPECT_EQ(thumbnail.toStdString(),
                  expected_info.thumbnail().toStdString())
            << "Unexpected thumbnail";
        EXPECT_EQ(url.toStdString(), expected_info.url().toStdString())
            << "Unexpected url";
        EXPECT_EQ(audio_available, expected_info.audio_available())
            << "Unexpected audio_available";
    }

    void check_formats_query(const QList<VideoFormat>& expected_formats,
                             const qint64 expected_videos_id) {
        QSqlQuery query = create_query();

        query.prepare(QString("SELECT") % kFormatsColumns %
                      "FROM formats "
                      "WHERE videos_id = :videos_id;");
        query.bindValue(":videos_id", expected_videos_id);
        query.setForwardOnly(true);
        EXPECT_TRUE(query.exec()) << "Query failed";

        QList<VideoFormat> formats;
        while (query.next()) {
            bool ok = false;

            // unused
            // const qint64 id = query.value(0).toLongLong(&ok);
            QString format_id = query.value(1).toString();
            QString container = query.value(2).toString();
            const quint32 width = query.value(3).toUInt(&ok);
            EXPECT_TRUE(ok) << "Failed to convert value to width";
            const quint32 height = query.value(4).toUInt(&ok);
            EXPECT_TRUE(ok) << "Failed to convert value to height";
            const float fps = query.value(5).toFloat(&ok);
            EXPECT_TRUE(ok) << "Failed to convert value to fps";

            const qint64 videos_id = query.value(6).toLongLong(&ok);
            EXPECT_TRUE(ok) << "Failed to convert value to videos_id";
            EXPECT_EQ(videos_id, expected_videos_id);

            formats << VideoFormat(std::move(format_id), std::move(container),
                                   width, height, fps);
        }

        EXPECT_EQ(formats, expected_formats) << "Unexpected formats";
    }

    void check_add(const QSqlRecord& record, const VideoInfo& expected_info,
                   const qint64 expected_id, const quint32 before_add,
                   const quint32 after_add) {
        check_video_query(record, expected_info, expected_id, before_add,
                          after_add);
        check_formats_query(expected_info.formats(), expected_id);
    }

    static void check_video_pushed(const ManagedVideoParts& parts,
                                   const VideoInfo& expected_info,
                                   const qint64 expected_id,
                                   const quint32 before_add,
                                   const quint32 after_add) {
        const auto& [id, created_at, info] = parts;

        EXPECT_EQ(id, expected_id) << "Unexpected id";
        EXPECT_GE(created_at, before_add)
            << "created_at timestamp was before addVideo initiated";
        EXPECT_LE(created_at, after_add)
            << "created_at timestamp was after addVideo completed";
        EXPECT_EQ(info, expected_info) << "Unexpected info";
    }

    VideoInfo info1_{"info1",
                     "title1",
                     "author1",
                     1,
                     "thumbnail1",
                     "url1",
                     {VideoFormat{"format1", "mp4", 100, 200, 30}},
                     false};

    VideoInfo info2_{"info2",
                     "title2",
                     "author2",
                     2,
                     "thumbnail2",
                     "url2",
                     {VideoFormat{"format1", "mp4", 100, 200, 30},
                      VideoFormat{"format2", "webm", 300, 400, 60}},
                     true};

    const QString connection_name_{get_test_name()};

    Database db_{Database::get_temp(connection_name_)};

    QSignalSpy video_spy_{&db_, &Database::videosPushed};

    QSignalSpy error_spy_{&db_, &Database::errorPushed};

    QSignalSpy valid_spy_{&db_, &Database::validChanged};

    QSqlQuery query_{create_query()};
};

// Fetch tests assume add function works as intended

TEST_F(DatabaseTest, FetchFirstChunkNoVideosInDb) {
    const auto parts = db_.fetch_first_chunk();

    EXPECT_TRUE(parts.empty()) << "Fetch shouldn't have returned any parts";
}

TEST_F(DatabaseTest, FetchFirstChunkTwoVideosInDb) {
    db_.addVideo(info1_);
    db_.addVideo(info2_);

    const auto parts = db_.fetch_first_chunk();
    EXPECT_EQ(parts.size(), 2) << "Didn't fetch expected number of parts";

    auto [id1, created_at1, info1] = parts[0];
    auto [id2, created_at2, info2] = parts[1];

    EXPECT_EQ(info1, info1_) << "Unexpected info";
    EXPECT_EQ(info2, info2_) << "Unexpected info";

    check_managed_video_parts(parts);
}

TEST_F(DatabaseTest, FetchFirstChunkMoreVideosinDbThanChunkSize) {
    for (std::remove_const<decltype(Database::kChunkSize)>::type i = 0;
         i < Database::kChunkSize; ++i) {
        db_.addVideo(info1_);
    }
    // Add one more over the chunk size
    db_.addVideo(info2_);

    const auto parts = db_.fetch_first_chunk();

    EXPECT_EQ(parts.size(), Database::kChunkSize)
        << "Fetch should have returned parts equal to the max chunk size";

    check_managed_video_parts(parts);
}

TEST_F(DatabaseTest, FetchChunkNoVideosInDb) {
    auto parts = db_.fetch_chunk(0, 0);

    EXPECT_TRUE(parts.empty()) << "Fetch shouldn't have returned any parts";
}

TEST_F(DatabaseTest, FetchChunkTwoVideosInDbGetBoth) {
    db_.addVideo(info1_);
    db_.addVideo(info2_);

    constexpr auto kMax = std::numeric_limits<qint64>::max();
    const auto parts = db_.fetch_chunk(kMax, kMax);
    EXPECT_EQ(parts.size(), 2) << "Did not fetch expected number of videos";

    auto [id1, created_at1, info1] = parts[0];
    auto [id2, created_at2, info2] = parts[1];

    EXPECT_EQ(info1, info1_) << "Unexpected info";
    EXPECT_EQ(info2, info2_) << "Unexpected info";

    check_managed_video_parts(parts);
}

TEST_F(DatabaseTest, FetchSecondChunk) {
    for (std::remove_const<decltype(Database::kChunkSize)>::type i = 0;
         i < Database::kChunkSize; ++i) {
        db_.addVideo(info1_);
    }
    for (std::remove_const<decltype(Database::kChunkSize)>::type i = 0;
         i < Database::kChunkSize; ++i) {
        db_.addVideo(info2_);
    }

    const auto first_parts = db_.fetch_first_chunk();
    EXPECT_EQ(first_parts.size(), Database::kChunkSize);
    // First chunk should be the most recent added videos
    for (qsizetype i = 0; i < first_parts.size(); ++i) {
        const auto& part = first_parts[i];
        EXPECT_EQ(get<0>(part), i + Database::kChunkSize + 1);
        EXPECT_EQ(get<2>(part), info2_);
    }

    // Get oldest parts from first chunk
    const auto& [last_id, last_created_at, last_info] = first_parts.first();

    const auto second_parts = db_.fetch_chunk(last_id, last_created_at);
    EXPECT_EQ(second_parts.size(), Database::kChunkSize);
    // Second chunk should be the chronologically first added chunk of videos
    for (qsizetype i = 0; i < second_parts.size(); ++i) {
        const auto& part = second_parts[i];
        EXPECT_EQ(get<0>(part), i + 1);
        EXPECT_EQ(get<2>(part), info1_);
    }
}

TEST_F(DatabaseTest, SetValidToTrue) {
    db_.setValid(true);

    EXPECT_EQ(valid_spy_.count(), 0)
        << "Should be true initially, resulting in no signal";
}

TEST_F(DatabaseTest, SetValidToFalse) {
    db_.setValid(false);

    EXPECT_EQ(valid_spy_.count(), 1)
        << "Should be true initially, resulting in a signal when set to false";
    EXPECT_FALSE(valid_spy_.takeFirst().takeFirst().toBool())
        << "Valid's new value should be false";
}

TEST_F(DatabaseTest, AddOneVideo) {
    auto before_add = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info1_);
    auto after_add = QDateTime::currentSecsSinceEpoch();

    EXPECT_EQ(error_spy_.count(), 0);

    EXPECT_EQ(get_num_rows_in_videos(), 1);

    // Check info in db
    EXPECT_TRUE(query_.exec(QString("SELECT") % kVideosColumns %
                            "FROM videos "
                            "LIMIT 1;"));
    EXPECT_TRUE(query_.next());
    check_add(query_.record(), info1_, 1, before_add, after_add);

    // Check pushed video
    EXPECT_EQ(video_spy_.count(), 1);

    auto components = video_spy_.takeFirst()
                          .takeFirst()
                          .value<QList<ManagedVideoParts>>()
                          .takeFirst();
    check_video_pushed(components, info1_, 1, before_add, after_add);
}

TEST_F(DatabaseTest, AddTwoVideos) {
    auto before_add1 = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info1_);
    auto after_add1 = QDateTime::currentSecsSinceEpoch();

    EXPECT_EQ(get_num_rows_in_videos(), 1);

    auto before_add2 = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info2_);
    auto after_add2 = QDateTime::currentSecsSinceEpoch();

    EXPECT_EQ(get_num_rows_in_videos(), 2);

    EXPECT_EQ(error_spy_.count(), 0);

    // Check info in db
    ASSERT_TRUE(query_.exec(QString("SELECT") % kVideosColumns %
                            "FROM videos "
                            "LIMIT 2;"));
    EXPECT_TRUE(query_.next());
    check_add(query_.record(), info1_, 1, before_add1, after_add1);
    EXPECT_TRUE(query_.next());
    check_add(query_.record(), info2_, 2, before_add2, after_add2);

    // Check pushed videos
    EXPECT_EQ(video_spy_.count(), 2);

    auto components1 = video_spy_.takeFirst()
                           .takeFirst()
                           .value<QList<ManagedVideoParts>>()
                           .takeFirst();
    check_video_pushed(components1, info1_, 1, before_add1, after_add1);

    auto components2 = video_spy_.takeFirst()
                           .takeFirst()
                           .value<QList<ManagedVideoParts>>()
                           .takeFirst();
    check_video_pushed(components2, info2_, 2, before_add1, after_add1);
}

TEST_F(DatabaseTest, RemoveOnEmptyDb) {
    db_.removeVideo(1);

    EXPECT_EQ(error_spy_.count(), 0);
}

TEST_F(DatabaseTest, RemoveOnNonExistingId) {
    db_.addVideo(info1_);
    db_.addVideo(info2_);
    EXPECT_EQ(get_num_rows_in_videos(), 2);

    db_.removeVideo(3);
    EXPECT_EQ(get_num_rows_in_videos(), 2);

    EXPECT_EQ(error_spy_.count(), 0);
}

TEST_F(DatabaseTest, RemoveAllOnEmptyDb) {
    db_.removeAllVideos();

    EXPECT_EQ(error_spy_.count(), 0);
}

TEST_F(DatabaseTest, RemoveOneVideo) {
    db_.addVideo(info1_);
    EXPECT_EQ(get_num_rows_in_videos(), 1);

    db_.removeVideo(1);
    EXPECT_EQ(get_num_rows_in_videos(), 0);

    EXPECT_EQ(error_spy_.count(), 0);
}

TEST_F(DatabaseTest, RemoveFirstOfTwoVideos) {
    db_.addVideo(info1_);
    auto before_add = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info2_);
    auto after_add = QDateTime::currentSecsSinceEpoch();
    EXPECT_EQ(get_num_rows_in_videos(), 2);

    db_.removeVideo(1);

    EXPECT_EQ(get_num_rows_in_videos(), 1);

    EXPECT_TRUE(query_.exec(QString("SELECT") % kVideosColumns %
                            "FROM videos "
                            "LIMIT 1;"));
    EXPECT_TRUE(query_.next());

    check_add(query_.record(), info2_, 2, before_add, after_add);

    EXPECT_EQ(error_spy_.count(), 0);
}

TEST_F(DatabaseTest, RemoveSecondOfTwoVideos) {
    auto before_add = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info1_);
    auto after_add = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info2_);
    EXPECT_EQ(get_num_rows_in_videos(), 2);

    db_.removeVideo(2);

    EXPECT_EQ(get_num_rows_in_videos(), 1);

    EXPECT_TRUE(query_.exec(QString("SELECT") % kVideosColumns %
                            "FROM videos "
                            "LIMIT 1;"));
    EXPECT_TRUE(query_.next());

    check_add(query_.record(), info1_, 1, before_add, after_add);

    EXPECT_EQ(error_spy_.count(), 0);
}

TEST_F(DatabaseTest, RemoveAllOnTwoVideos) {
    db_.addVideo(info1_);
    db_.addVideo(info2_);
    EXPECT_EQ(get_num_rows_in_videos(), 2);

    db_.removeAllVideos();

    EXPECT_EQ(get_num_rows_in_videos(), 0);

    EXPECT_EQ(error_spy_.count(), 0);
}

}  // namespace yd_gui
