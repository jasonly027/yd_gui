#include <database.h>
#include <downloader.h>
#include <gtest/gtest.h>
#include <qcontainerfwd.h>
#include <qlist.h>
#include <qsignalspy.h>
#include <qtypes.h>

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

#include "_tst_util.h"  // IWYU pragma: keep
#include "gmock/gmock.h"
#include "video.h"

using namespace tst_util;  // NOLINT(google-build-using-namespace)

namespace yd_gui {

class DatabaseTest : public testing::Test {
   protected:
    DatabaseTest() {
        EXPECT_TRUE(db_.valid());
        QObject::connect(&db_, &Database::errorPushed,
                         [this](const QString& data) {
                             std::cerr << data.toStdString() << '\n';
                             EXPECT_EQ(error_spy_.count(), 0)
                                 << "errorPushed signal was emit";
                         });
    };

    QSqlQuery make_query() {
        return QSqlQuery(QSqlDatabase::database(connection_name_));
    }

    qint64 rows_in_videos() {
        QSqlQuery query = make_query();

        EXPECT_TRUE(query.exec("SELECT COUNT(*) FROM videos;") &&
                    query.next() && query.value(0).canConvert<qint64>())
            << "Failed to fetch videos row count";

        return query.value(0).value<qint64>();
    }

    // Convenience variables for when SELECT'ing
    constexpr static auto kVideosColumns{
        " id, created_at, video_id, title, author, seconds, thumbnail, url, "
        "audio_available "};
    constexpr static auto kFormatsColumns{
        " id, format_id, container, width, height, fps, videos_id "};

    // Check ManagedVideo Components are in increasing order
    static void EXPECT_PARTS_ASC(const QList<ManagedVideoParts>& chunk) {
        const ManagedVideoParts& first_parts = chunk.first();

        auto [last_id, last_created_at, last_info, last_state] = first_parts;
        for (const auto& parts : chunk) {
            if (parts == first_parts) continue;  // Skip first parts;

            const auto& [id, created_at, info, state] = parts;

            EXPECT_GT(id, last_id) << "Parts were not ordered by increasing id";
            EXPECT_GE(created_at, last_created_at)
                << "Parts were not ordered by increasing created_at";

            last_id = id;
            last_created_at = created_at;
        }
    };

    // Expects the query's values to be the same order as kVideosColumns
    // Returns back the query after using it
    static void EXPECT_QUERY_EQ_INFO(const QSqlRecord& record,
                                     const VideoInfo& expected_info,
                                     const qint64 expected_id,
                                     const qint64 before_add,
                                     const qint64 after_add) {
        const auto id = try_convert<qint64>(record.value(0));
        const auto created_at = try_convert<qint64>(record.value(1));
        const auto video_id = try_convert<QString>(record.value(2));
        const auto title = try_convert<QString>(record.value(3));
        const auto author = try_convert<QString>(record.value(4));
        const auto seconds = try_convert<qint64>(record.value(5));
        const auto thumbnail = try_convert<QString>(record.value(6));
        const auto url = try_convert<QString>(record.value(7));
        const auto audio_available = try_convert<bool>(record.value(8));

        EXPECT_EQ(id, expected_id);

        EXPECT_THAT(created_at, IsBetween(before_add, after_add));

        EXPECT_EQ(video_id, expected_info.video_id());

        EXPECT_EQ(title, expected_info.title());

        EXPECT_EQ(author, expected_info.author());

        EXPECT_EQ(seconds, expected_info.seconds());

        EXPECT_EQ(thumbnail, expected_info.thumbnail());

        EXPECT_EQ(url, expected_info.url());

        EXPECT_EQ(audio_available, expected_info.audio_available());
    }

    void EXPECT_QUERY_EQ_FORMATS(const QList<VideoFormat>& expected_formats,
                                 const qint64 expected_videos_id) {
        QSqlQuery query = make_query();

        EXPECT_TRUE(query.prepare(QString("SELECT") % kFormatsColumns %
                                  "FROM formats "
                                  "WHERE videos_id = :videos_id;"));
        query.bindValue(":videos_id", expected_videos_id);
        query.setForwardOnly(true);
        EXPECT_TRUE(query.exec()) << "Query failed";

        QList<VideoFormat> formats;
        while (query.next()) {
            auto format_id = try_convert<QString>(query.value(1));
            auto container = try_convert<QString>(query.value(2));
            const auto width = try_convert<quint32>(query.value(3));
            const auto height = try_convert<quint32>(query.value(4));
            const auto fps = try_convert<float>(query.value(5));
            const auto videos_id = try_convert<qint64>(query.value(6));

            EXPECT_EQ(videos_id, expected_videos_id);

            formats << VideoFormat(std::move(format_id), std::move(container),
                                   width, height, fps);
        }

        EXPECT_THAT(formats, ContainerEq(expected_formats));
    }

    void EXPECT_QUERY_EQ_INFO_FORMATS(const QSqlRecord& record,
                                      const VideoInfo& expected_info,
                                      const qint64 expected_id,
                                      const quint32 before_add,
                                      const quint32 after_add) {
        {
            SCOPED_TRACE("");
            EXPECT_QUERY_EQ_INFO(record, expected_info, expected_id, before_add,
                                 after_add);
        }
        {
            SCOPED_TRACE("");
            EXPECT_QUERY_EQ_FORMATS(expected_info.formats(), expected_id);
        }
    }

    static void EXPECT_PUSHED_EQ_INFO(const ManagedVideoParts& parts,
                                      const VideoInfo& expected_info,
                                      const qint64 expected_id,
                                      const quint32 before_add,
                                      const quint32 after_add) {
        const auto& [id, created_at, info, state] = parts;

        EXPECT_EQ(id, expected_id);
        EXPECT_THAT(created_at, IsBetween(before_add, after_add));
        EXPECT_EQ(info, expected_info);
        EXPECT_EQ(state, DownloadState::kAdded);
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

    const QString connection_name_{QString::fromStdString(test_name())};

    Database db_{Database::get_temp(connection_name_)};

    QSignalSpy video_spy_{&db_, &Database::videosPushed};

    QSignalSpy error_spy_{&db_, &Database::errorPushed};

    QSignalSpy valid_spy_{&db_, &Database::validChanged};

    QSqlQuery query_{make_query()};
};

// Fetch tests assume add function works as intended

TEST_F(DatabaseTest, FetchFirstChunkNoVideosInDb) {
    const auto chunk = db_.fetch_first_chunk();

    EXPECT_TRUE(chunk.empty()) << "Fetch shouldn't have returned any parts";
}

TEST_F(DatabaseTest, FetchFirstChunkTwoVideosInDb) {
    db_.addVideo(info1_);
    db_.addVideo(info2_);

    const auto chunk = db_.fetch_first_chunk();
    EXPECT_EQ(chunk.size(), 2);

    EXPECT_EQ(chunk[0].info, info1_);
    EXPECT_EQ(chunk[1].info, info2_);

    {
        SCOPED_TRACE("");
        EXPECT_PARTS_ASC(chunk);
    }
}

TEST_F(DatabaseTest, FetchFirstChunkMoreVideosinDbThanChunkSize) {
    for (std::remove_const<decltype(Database::kChunkSize)>::type i = 0;
         i < Database::kChunkSize; ++i) {
        db_.addVideo(info1_);
    }
    // Add one more over the chunk size
    db_.addVideo(info2_);

    const auto chunk = db_.fetch_first_chunk();

    EXPECT_EQ(ManagedVideo(chunk.last()).info(), info2_)
        << "Last video should be the most recently added";

    EXPECT_EQ(chunk.size(), Database::kChunkSize);

    {
        SCOPED_TRACE("");
        EXPECT_PARTS_ASC(chunk);
    }
}

TEST_F(DatabaseTest, FetchChunkNoVideosInDb) {
    const auto chunk = db_.fetch_chunk(0, 0);

    EXPECT_TRUE(chunk.empty()) << "Fetch shouldn't have returned any parts";
}

TEST_F(DatabaseTest, FetchChunkTwoVideosInDbGetBoth) {
    db_.addVideo(info1_);
    db_.addVideo(info2_);

    constexpr auto kMax = std::numeric_limits<qint64>::max();
    const auto chunk = db_.fetch_chunk(kMax, kMax);
    EXPECT_EQ(chunk.size(), 2);

    EXPECT_EQ(chunk[0].info, info1_);
    EXPECT_EQ(chunk[1].info, info2_);

    {
        SCOPED_TRACE("");
        EXPECT_PARTS_ASC(chunk);
    }
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

    const auto first_chunk = db_.fetch_first_chunk();
    EXPECT_EQ(first_chunk.size(), Database::kChunkSize);
    // First chunk should be the most recent added videos
    for (qsizetype i = 0; i < first_chunk.size(); ++i) {
        const auto& parts = first_chunk[i];
        EXPECT_EQ(parts.id, i + Database::kChunkSize + 1);
        EXPECT_EQ(parts.info, info2_);
    }

    // Destructure oldest part from first chunk
    const auto& [last_id, last_created_at, last_info, last_state] =
        first_chunk.first();

    const auto second_chunk = db_.fetch_chunk(last_id, last_created_at);
    EXPECT_EQ(second_chunk.size(), Database::kChunkSize);
    // Second chunk should be the chronologically first added videos
    for (qsizetype i = 0; i < second_chunk.size(); ++i) {
        const auto& part = second_chunk[i];
        EXPECT_EQ(part.id, i + 1);
        EXPECT_EQ(part.info, info1_);
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
    EXPECT_FALSE(try_convert<bool>(valid_spy_.takeFirst().takeFirst()))
        << "Valid's new value should be false";
}

TEST_F(DatabaseTest, AddOneVideo) {
    const auto before_add = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info1_);
    const auto after_add = QDateTime::currentSecsSinceEpoch();

    EXPECT_EQ(rows_in_videos(), 1);

    // Check info in db
    EXPECT_TRUE(query_.exec(QString("SELECT") % kVideosColumns %
                            "FROM videos "
                            "LIMIT 1;"));
    EXPECT_TRUE(query_.next());

    {
        SCOPED_TRACE("");
        EXPECT_QUERY_EQ_INFO_FORMATS(query_.record(), info1_, 1, before_add,
                                     after_add);
    }

    // Check pushed video
    EXPECT_EQ(video_spy_.count(), 1);

    const auto parts_list = try_convert<QList<ManagedVideoParts>>(
        video_spy_.takeFirst().takeFirst());

    EXPECT_EQ(parts_list.size(), 1);
    {
        SCOPED_TRACE("");
        EXPECT_PUSHED_EQ_INFO(parts_list.first(), info1_, 1, before_add,
                              after_add);
    }
}

TEST_F(DatabaseTest, AddTwoVideos) {
    const auto before_add1 = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info1_);
    const auto after_add1 = QDateTime::currentSecsSinceEpoch();

    EXPECT_EQ(rows_in_videos(), 1);

    const auto before_add2 = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info2_);
    const auto after_add2 = QDateTime::currentSecsSinceEpoch();

    EXPECT_EQ(rows_in_videos(), 2);

    // Check info in db
    ASSERT_TRUE(query_.exec(QString("SELECT") % kVideosColumns %
                            "FROM videos "
                            "LIMIT 2;"));
    EXPECT_TRUE(query_.next());
    {
        SCOPED_TRACE("");
        EXPECT_QUERY_EQ_INFO_FORMATS(query_.record(), info1_, 1, before_add1,
                                     after_add1);
    }
    EXPECT_TRUE(query_.next());
    {
        SCOPED_TRACE("");
        EXPECT_QUERY_EQ_INFO_FORMATS(query_.record(), info2_, 2, before_add2,
                                     after_add2);
    }

    // Check pushed videos
    EXPECT_EQ(video_spy_.count(), 2);

    const auto parts1 = try_convert<QList<ManagedVideoParts>>(
        video_spy_.takeFirst().takeFirst());
    EXPECT_EQ(parts1.size(), 1);

    {
        SCOPED_TRACE("");
        EXPECT_PUSHED_EQ_INFO(parts1.first(), info1_, 1, before_add1,
                              after_add1);
    }

    const auto parts2 = try_convert<QList<ManagedVideoParts>>(
        video_spy_.takeFirst().takeFirst());
    EXPECT_EQ(parts2.size(), 1);

    {
        SCOPED_TRACE("");
        EXPECT_PUSHED_EQ_INFO(parts2.first(), info2_, 2, before_add1,
                              after_add1);
    }
}

TEST_F(DatabaseTest, RemoveOnEmptyDb) {
    db_.removeVideo(1);  // Checked by error signal
}

TEST_F(DatabaseTest, RemoveOnNonExistingId) {
    db_.addVideo(info1_);
    db_.addVideo(info2_);
    EXPECT_EQ(rows_in_videos(), 2);

    db_.removeVideo(3);
    EXPECT_EQ(rows_in_videos(), 2);
}

TEST_F(DatabaseTest, RemoveAllOnEmptyDb) {
    db_.removeAllVideos();  // Checked by error signal
}

TEST_F(DatabaseTest, RemoveOneVideo) {
    db_.addVideo(info1_);
    EXPECT_EQ(rows_in_videos(), 1);

    db_.removeVideo(1);
    EXPECT_EQ(rows_in_videos(), 0);
}

TEST_F(DatabaseTest, RemoveFirstOfTwoVideos) {
    db_.addVideo(info1_);

    const auto before_add = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info2_);
    const auto after_add = QDateTime::currentSecsSinceEpoch();

    EXPECT_EQ(rows_in_videos(), 2);

    db_.removeVideo(1);

    EXPECT_EQ(rows_in_videos(), 1);

    EXPECT_TRUE(query_.exec(QString("SELECT") % kVideosColumns %
                            "FROM videos "
                            "LIMIT 1;"));
    EXPECT_TRUE(query_.next());

    // Check remaining info, info2_, is still there
    {
        SCOPED_TRACE("");
        EXPECT_QUERY_EQ_INFO_FORMATS(query_.record(), info2_, 2, before_add,
                                     after_add);
    }
}

TEST_F(DatabaseTest, RemoveSecondOfTwoVideos) {
    const auto before_add = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info1_);
    const auto after_add = QDateTime::currentSecsSinceEpoch();

    db_.addVideo(info2_);

    EXPECT_EQ(rows_in_videos(), 2);

    db_.removeVideo(2);

    EXPECT_EQ(rows_in_videos(), 1);

    EXPECT_TRUE(query_.exec(QString("SELECT") % kVideosColumns %
                            "FROM videos "
                            "LIMIT 1;"));
    EXPECT_TRUE(query_.next());

    // Check remaining info, info1_, is still there
    {
        SCOPED_TRACE("");
        EXPECT_QUERY_EQ_INFO_FORMATS(query_.record(), info1_, 1, before_add,
                                     after_add);
    }
}

TEST_F(DatabaseTest, RemoveAllOnTwoVideos) {
    db_.addVideo(info1_);
    db_.addVideo(info2_);

    EXPECT_EQ(rows_in_videos(), 2);

    db_.removeAllVideos();

    EXPECT_EQ(rows_in_videos(), 0);
}

}  // namespace yd_gui
