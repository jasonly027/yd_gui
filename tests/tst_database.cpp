#include <database.h>
#include <downloader.h>
#include <gtest/gtest.h>
#include <qdatetime.h>
#include <qobject.h>
#include <qsignalspy.h>
#include <qsqldatabase.h>
#include <qsqldriver.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>

#include <QSqlDriver>
#include <QSqlRecord>
#include <QString>
#include <QStringBuilder>
#include <cstdint>
#include <iostream>
#include <utility>

#include "video.h"

using std::pair;

namespace yd_gui {

static QString get_test_name() {
    return QString::fromStdString(
        ::testing::UnitTest::GetInstance()->current_test_info()->name());
}

class DatabaseTest : public testing::Test {
   protected:
    DatabaseTest() {
        QObject::connect(&db_, &Database::errorPushed, [](const QString& data) {
            std::cerr << data.toStdString() << '\n';
        });
    };

    QSqlQuery create_query() {
        return QSqlQuery(QSqlDatabase::database(connection_name_));
    }

    int64_t get_num_rows_in_videos() {
        QSqlQuery query = create_query();
        EXPECT_TRUE(query.exec("SELECT COUNT(*) FROM videos;"));
        query.next();

        bool ok = false;
        int64_t rows = query.value(0).toLongLong(&ok);
        EXPECT_TRUE(ok);
        return rows;
    }

    // Convenience variables for when SELECT'ing
    constexpr static auto kVideosColumns{
        " id, created_at, video_id, title, author, seconds, thumbnail, url, "
        "audio_available "};
    constexpr static auto kFormatsColumns{
        " id, format_id, container, width, height, fps, videos_id "};

    // Expects the query's values to be the same order as kVideosColumns
    // Returns back the query after using it
    static void check_video_query(const QSqlRecord& record,
                                  const VideoInfo& expected_info,
                                  const int64_t expected_id,
                                  const uint32_t before_add,
                                  const uint32_t after_add) {
        bool ok = false;

        const int64_t id = record.value(0).toLongLong(&ok);
        EXPECT_TRUE(ok);
        const int64_t created_at = record.value(1).toLongLong(&ok);
        EXPECT_TRUE(ok);
        const QString video_id = record.value(2).toString();
        const QString title = record.value(3).toString();
        const QString author = record.value(4).toString();
        const int64_t seconds = record.value(5).toUInt(&ok);
        EXPECT_TRUE(ok);
        const QString thumbnail = record.value(6).toString();
        const QString url = record.value(7).toString();
        const bool audio_available = record.value(8).toBool();

        EXPECT_EQ(id, expected_id);
        EXPECT_GE(created_at, before_add);
        EXPECT_LE(created_at, after_add);
        EXPECT_EQ(video_id.toStdString(),
                  expected_info.video_id().toStdString());
        EXPECT_EQ(title.toStdString(), expected_info.title().toStdString());
        EXPECT_EQ(author.toStdString(), expected_info.author().toStdString());
        EXPECT_EQ(seconds, expected_info.seconds());
        EXPECT_EQ(thumbnail.toStdString(),
                  expected_info.thumbnail().toStdString());
        EXPECT_EQ(url.toStdString(), expected_info.url().toStdString());
        EXPECT_EQ(audio_available, expected_info.audio_available());
    }

    void check_formats_query(const QList<VideoFormat>& expected_formats,
                             const int64_t expected_videos_id) {
        QSqlQuery query = create_query();

        query.prepare(QString("SELECT") % kFormatsColumns %
                      "FROM formats "
                      "WHERE videos_id = :videos_id;");
        query.bindValue(":videos_id", expected_videos_id);
        query.setForwardOnly(true);
        EXPECT_TRUE(query.exec());

        QList<VideoFormat> formats;
        while (query.next()) {
            bool ok = false;
            // unused
            const int64_t id = query.value(0).toLongLong(&ok);
            EXPECT_TRUE(ok);

            QString format_id = query.value(1).toString();
            QString container = query.value(2).toString();
            const uint32_t width = query.value(3).toUInt(&ok);
            EXPECT_TRUE(ok);
            const uint32_t height = query.value(4).toUInt(&ok);
            EXPECT_TRUE(ok);
            const float fps = query.value(5).toFloat(&ok);
            EXPECT_TRUE(ok);

            const int64_t videos_id = query.value(6).toLongLong(&ok);
            EXPECT_TRUE(ok);
            EXPECT_EQ(videos_id, expected_videos_id);

            formats << VideoFormat(std::move(format_id), std::move(container),
                                   width, height, fps);
        }

        EXPECT_EQ(formats, expected_formats);
    }

    void check_add(const QSqlRecord& record, const VideoInfo& expected_info,
                   const int64_t expected_id, const uint32_t before_add,
                   const uint32_t after_add) {
        check_video_query(record, expected_info, expected_id, before_add,
                          after_add);
        check_formats_query(expected_info.formats(), expected_id);
    }

    static void check_video_pushed(
        const QPair<QPair<int64_t, int64_t>, VideoInfo>& components,
        const VideoInfo& expected_info, const int64_t expected_id,
        const uint32_t before_add, const uint32_t after_add) {
        const auto& id = components.first.first;
        const auto& created_at = components.first.second;
        const auto& info = components.second;

        EXPECT_EQ(id, expected_id);
        EXPECT_GE(created_at, before_add);
        EXPECT_LE(created_at, after_add);
        EXPECT_EQ(info, expected_info);
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

    QSignalSpy video_spy_{&db_, &Database::videoPushed};

    QSignalSpy error_spy_{&db_, &Database::errorPushed};

    QSignalSpy valid_spy_{&db_, &Database::validChanged};

    QSqlQuery query_{create_query()};
};

TEST_F(DatabaseTest, SetValidToTrue) {
    db_.setValid(true);

    EXPECT_EQ(valid_spy_.count(), 0)
        << "Should be true initially, resulting in no signal";
}

TEST_F(DatabaseTest, SetValidToFalse) {
    db_.setValid(false);

    EXPECT_EQ(valid_spy_.count(), 1)
        << "Should be true initially, resulting in a signal when set to false";
    EXPECT_FALSE(valid_spy_.takeFirst().takeFirst().toBool());
}

TEST_F(DatabaseTest, AddOneVideo) {
    uint32_t before_add = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info1_);
    uint32_t after_add = QDateTime::currentSecsSinceEpoch();

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
                          .value<QPair<QPair<int64_t, int64_t>, VideoInfo>>();
    check_video_pushed(components, info1_, 1, before_add, after_add);
}

TEST_F(DatabaseTest, AddTwoVideos) {
    uint32_t before_add1 = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info1_);
    uint32_t after_add1 = QDateTime::currentSecsSinceEpoch();

    EXPECT_EQ(get_num_rows_in_videos(), 1);

    uint32_t before_add2 = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info2_);
    uint32_t after_add2 = QDateTime::currentSecsSinceEpoch();

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
                           .value<QPair<QPair<int64_t, int64_t>, VideoInfo>>();
    check_video_pushed(components1, info1_, 1, before_add1, after_add1);

    auto components2 = video_spy_.takeFirst()
                           .takeFirst()
                           .value<QPair<QPair<int64_t, int64_t>, VideoInfo>>();
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
    uint32_t before_add = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info2_);
    uint32_t after_add = QDateTime::currentSecsSinceEpoch();
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
    uint32_t before_add = QDateTime::currentSecsSinceEpoch();
    db_.addVideo(info1_);
    uint32_t after_add = QDateTime::currentSecsSinceEpoch();
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
