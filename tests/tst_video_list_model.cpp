#include <gtest/gtest.h>
#include <qabstractitemmodel.h>
#include <qlist.h>

#include "video.h"
#include "video_list_model.h"

namespace yd_gui {

using VideoListModelRole = VideoListModel::VideoListModelRole;

static QString get_test_name() {
    return QString::fromStdString(
        ::testing::UnitTest::GetInstance()->current_test_info()->name());
}

class VideoListModelTest : public testing::Test {
   protected:
    explicit VideoListModelTest() { EXPECT_TRUE(db_.valid()); }

    QList<ManagedVideoParts> parts_same_{{.id = 4,
                                          .created_at = 4,
                                          .info = VideoInfo(),
                                          .state = DownloadState::kAdded},
                                         {.id = 4,
                                          .created_at = 4,
                                          .info = VideoInfo(),
                                          .state = DownloadState::kAdded},
                                         {.id = 4,
                                          .created_at = 4,
                                          .info = VideoInfo(),
                                          .state = DownloadState::kAdded}};
    QList<ManagedVideoParts> parts_ascending_{{.id = 1,
                                               .created_at = 1,
                                               .info = VideoInfo(),
                                               .state = DownloadState::kAdded},
                                              {.id = 2,
                                               .created_at = 2,
                                               .info = VideoInfo(),
                                               .state = DownloadState::kAdded},
                                              {.id = 3,
                                               .created_at = 3,
                                               .info = VideoInfo(),
                                               .state = DownloadState::kAdded}};
    QList<ManagedVideoParts> parts_descending_{
        {.id = 3,
         .created_at = 3,
         .info = VideoInfo(),
         .state = DownloadState::kAdded},
        {.id = 2,
         .created_at = 2,
         .info = VideoInfo(),
         .state = DownloadState::kAdded},
        {.id = 1,
         .created_at = 1,
         .info = VideoInfo(),
         .state = DownloadState::kAdded}};

    static constexpr int kCreatedAtRole =
        static_cast<int>(VideoListModelRole::kCreatedAtRole);

    Database db_{Database::get_temp(get_test_name())};

    VideoListModel model_{db_};
};

TEST_F(VideoListModelTest, AppendVideos) {
    EXPECT_EQ(model_.rowCount(), 0);

    model_.appendVideos(parts_ascending_);

    EXPECT_EQ(model_.rowCount(), parts_ascending_.size());

    for (qsizetype i = 0; i < model_.rowCount(); ++i) {
        const auto index = model_.index(i);

        bool ok = false;
        const qint64 created_at =
            model_.data(index, kCreatedAtRole).toLongLong(&ok);
        EXPECT_TRUE(ok);

        const qint64 expected_created_at = parts_ascending_[i].created_at;

        EXPECT_EQ(created_at, expected_created_at)
            << "Videos are in an unexpected order";
    }
}

TEST_F(VideoListModelTest, PrependVideos) {
    EXPECT_EQ(model_.rowCount(), 0);

    model_.prependVideos(parts_ascending_);

    EXPECT_EQ(model_.rowCount(), parts_ascending_.size());

    for (qsizetype i = 0; i < model_.rowCount(); ++i) {
        const auto index = model_.index(i);

        bool ok = false;
        const qint64 created_at =
            model_.data(index, kCreatedAtRole).toLongLong(&ok);
        EXPECT_TRUE(ok);

        const qint64 expected_created_at = parts_ascending_[i].created_at;

        EXPECT_EQ(created_at, expected_created_at)
            << "Videos are in an unexpected order";
    }
}

TEST_F(VideoListModelTest, AppendThenPrependVideos) {
    EXPECT_EQ(model_.rowCount(), 0);

    model_.appendVideos(parts_ascending_);
    model_.prependVideos(parts_descending_);

    const auto expected_parts = parts_descending_ + parts_ascending_;

    EXPECT_EQ(model_.rowCount(), expected_parts.size());

    for (qsizetype i = 0; i < model_.rowCount(); ++i) {
        const auto index = model_.index(i);

        bool ok = false;
        const qint64 created_at =
            model_.data(index, kCreatedAtRole).toLongLong(&ok);
        EXPECT_TRUE(ok);

        const qint64 expected_created_at = expected_parts[i].created_at;

        EXPECT_EQ(created_at, expected_created_at)
            << "Videos are in an unexpected order";
    }
}

TEST_F(VideoListModelTest, PrependThenAppendVideos) {
    EXPECT_EQ(model_.rowCount(), 0);

    model_.prependVideos(parts_ascending_);
    model_.appendVideos(parts_descending_);

    const auto expected_parts = parts_ascending_ + parts_descending_;

    EXPECT_EQ(model_.rowCount(), expected_parts.size());

    for (qsizetype i = 0; i < model_.rowCount(); ++i) {
        const auto index = model_.index(i);

        bool ok = false;
        const qint64 created_at =
            model_.data(index, kCreatedAtRole).toLongLong(&ok);
        EXPECT_TRUE(ok);

        const qint64 expected_created_at = expected_parts[i].created_at;

        EXPECT_EQ(created_at, expected_created_at)
            << "Videos are in an unexpected order";
    }
}

TEST_F(VideoListModelTest, RemoveAllVideosOnEmptyModel) {
    EXPECT_EQ(model_.rowCount(), 0);

    model_.removeAllVideos();

    EXPECT_EQ(model_.rowCount(), 0);
}

TEST_F(VideoListModelTest, RemoveAllVideos) {
    EXPECT_EQ(model_.rowCount(), 0);

    model_.appendVideos(parts_same_);

    EXPECT_EQ(model_.rowCount(), parts_same_.size());

    model_.removeAllVideos();

    EXPECT_EQ(model_.rowCount(), 0);
}

}  // namespace yd_gui
