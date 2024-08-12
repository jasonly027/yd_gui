#include <gtest/gtest.h>
#include <qabstractitemmodel.h>
#include <qlist.h>
#include <qtypes.h>

#include "_tst_util.h"
#include "video.h"
#include "video_list_model.h"

using namespace tst_util;  // NOLINT(google-build-using-namespace)

namespace yd_gui {

using VideoListModelRole = VideoListModel::VideoListModelRole;

class VideoListModelTest : public testing::Test {
   protected:
    explicit VideoListModelTest() { EXPECT_TRUE(db_.valid()); }

    static constexpr int kCreatedAtRole =
        static_cast<int>(VideoListModelRole::kCreatedAtRole);

    static void EXPECT_EQ_CREATED_AT(
        const VideoListModel& model,
        const QList<ManagedVideoParts>& expected_parts) {
        for (qsizetype i = 0; i < model.rowCount(); ++i) {
            const auto created_at =
                try_convert<qint64>(model.data(model.index(i), kCreatedAtRole));

            EXPECT_EQ(created_at, expected_parts[i].created_at) << "At index " << i;
        }
    }

    QList<ManagedVideoParts> parts_{{.id = 4,
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
    QList<ManagedVideoParts> parts_asc_{{.id = 1,
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
    QList<ManagedVideoParts> parts_desc_{{.id = 3,
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

    Database db_{Database::get_temp(QString::fromStdString(test_name()))};

    VideoListModel model_{db_};
};

TEST_F(VideoListModelTest, AppendVideos) {
    EXPECT_EQ(model_.rowCount(), 0);

    model_.appendVideos(parts_asc_);

    EXPECT_EQ(model_.rowCount(), parts_asc_.size());

    {
        SCOPED_TRACE("");
        EXPECT_EQ_CREATED_AT(model_, parts_asc_);
    }
}

TEST_F(VideoListModelTest, PrependVideos) {
    EXPECT_EQ(model_.rowCount(), 0);

    model_.prependVideos(parts_asc_);

    EXPECT_EQ(model_.rowCount(), parts_asc_.size());

    {
        SCOPED_TRACE("");
        EXPECT_EQ_CREATED_AT(model_, parts_asc_);
    }
}

TEST_F(VideoListModelTest, AppendThenPrependVideos) {
    EXPECT_EQ(model_.rowCount(), 0);

    model_.appendVideos(parts_asc_);
    model_.prependVideos(parts_desc_);

    const auto expected_parts = parts_desc_ + parts_asc_;

    EXPECT_EQ(model_.rowCount(), expected_parts.size());

    {
        SCOPED_TRACE("");
        EXPECT_EQ_CREATED_AT(model_, expected_parts);
    }
}

TEST_F(VideoListModelTest, PrependThenAppendVideos) {
    EXPECT_EQ(model_.rowCount(), 0);

    model_.prependVideos(parts_asc_);
    model_.appendVideos(parts_desc_);

    const auto expected_parts = parts_asc_ + parts_desc_;

    EXPECT_EQ(model_.rowCount(), expected_parts.size());

    {
        SCOPED_TRACE("");
        EXPECT_EQ_CREATED_AT(model_, expected_parts);
    }
}

TEST_F(VideoListModelTest, RemoveAllVideosOnEmptyModel) {
    EXPECT_EQ(model_.rowCount(), 0);

    model_.removeAllVideos();

    EXPECT_EQ(model_.rowCount(), 0);
}

TEST_F(VideoListModelTest, RemoveAllVideos) {
    EXPECT_EQ(model_.rowCount(), 0);

    model_.appendVideos(parts_);

    EXPECT_EQ(model_.rowCount(), parts_.size());

    model_.removeAllVideos();

    EXPECT_EQ(model_.rowCount(), 0);
}

}  // namespace yd_gui
