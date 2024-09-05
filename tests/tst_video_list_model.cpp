#include <gtest/gtest.h>
#include <qabstractitemmodel.h>
#include <qlist.h>
#include <qsignalspy.h>
#include <qtypes.h>
#include <qvariant.h>

#include <climits>

#include "_tst_util.h"
#include "gmock/gmock.h"
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

            EXPECT_EQ(created_at, expected_parts[i].created_at)
                << "At index " << i;
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

    QSignalSpy data_spy_{&model_, &VideoListModel::dataChanged};

    QSignalSpy request_download_spy_{&model_,
                                     &VideoListModel::requestDownloadVideo};
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

TEST_F(VideoListModelTest, RemoveVideo) {
    model_.appendVideos(parts_);

    EXPECT_EQ(model_.rowCount(), parts_.size());

    model_.removeVideo(0);

    EXPECT_EQ(model_.rowCount(), parts_.size() - 1);
}

TEST_F(VideoListModelTest, RemoveVideoLessThanBounds) {
    model_.appendVideos(parts_);

    EXPECT_EQ(model_.rowCount(), parts_.size());

    model_.removeVideo(-1);

    EXPECT_EQ(model_.rowCount(), parts_.size());
}

TEST_F(VideoListModelTest, RemoveVideoGreaterThanBounds) {
    model_.appendVideos(parts_);

    EXPECT_EQ(model_.rowCount(), parts_.size());

    model_.removeVideo(parts_.size());

    EXPECT_EQ(model_.rowCount(), parts_.size());
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

TEST_F(VideoListModelTest, DownloadVideo) {
    model_.appendVideos(parts_);

    model_.downloadVideo(0);

    EXPECT_EQ(request_download_spy_.count(), 1);
}

TEST_F(VideoListModelTest, DownloadVideoLessThanBounds) {
    model_.appendVideos(parts_);

    model_.downloadVideo(-1);

    EXPECT_EQ(request_download_spy_.count(), 0);
}

TEST_F(VideoListModelTest, DownloadVideoGreaterThanBounds) {
    model_.appendVideos(parts_);

    model_.downloadVideo(parts_.size());

    EXPECT_EQ(request_download_spy_.count(), 0);
}

TEST_F(VideoListModelTest, DownloadAllVideos) {
    model_.appendVideos(parts_);

    model_.downloadAllVideos();

    EXPECT_EQ(request_download_spy_.count(), parts_.size());
}

template <typename T>
static void generic_set_data_test(VideoListModel& model, QSignalSpy& data_spy,
                                  const QList<ManagedVideoParts>& parts,
                                  VideoListModelRole role, T new_value) {
    model.appendVideos(parts);

    const QModelIndex first_idx = model.index(0);
    const int int_role = static_cast<int>(role);

    QVariant current_variant = model.data(first_idx, int_role);

    ASSERT_TRUE(current_variant.canConvert<T>())
        << "Cannot convert to same value as typename T";
    T current_value = current_variant.value<T>();

    EXPECT_TRUE(
        model.setData(first_idx, QVariant::fromValue(current_value), int_role));

    EXPECT_EQ(data_spy.count(), 0)
        << "Should not emit if the new value is the same as the former";

    ASSERT_EQ(current_variant.typeId(), QVariant::fromValue(new_value).typeId())
        << "new_value should be the same type as the current role's data";

    ASSERT_NE(current_value, new_value)
        << "kNewProgress needs to be set to a different value, else following "
           "tests will fail";

    EXPECT_TRUE(
        model.setData(first_idx, QVariant::fromValue(new_value), int_role));

    current_variant = model.data(first_idx, int_role);
    ASSERT_TRUE(current_variant.canConvert<T>())
        << "Cannot convert to same value as typename T";
    current_value = current_variant.value<T>();

    EXPECT_EQ(current_value, new_value)
        << "New value wasn't updated in the model";

    ASSERT_EQ(data_spy.count(), 1);

    QList<QVariant> emission = data_spy.takeFirst();

    ASSERT_EQ(emission.size(), 3)
        << "Expected three values of (start_index, end_index, {role}) from "
           "signal dataChanged";

    auto start_idx = try_convert<QModelIndex>(emission[0]);
    auto end_idx = try_convert<QModelIndex>(emission[1]);
    auto roles = try_convert<QList<int>>(emission[2]);

    EXPECT_EQ(start_idx.row(), 0);
    EXPECT_EQ(end_idx.row(), 0);

    QList<int> expected_roles = {int_role};
    EXPECT_THAT(roles, ContainerEq(expected_roles));
}

TEST_F(VideoListModelTest, SetDataProgress) {
    {
        SCOPED_TRACE("");
        generic_set_data_test(model_, data_spy_, parts_,
                              VideoListModelRole::kProgressRole, 1.0F);
    }
}

TEST_F(VideoListModelTest, SetDataSelectedFormat) {
    {
        SCOPED_TRACE("");
        generic_set_data_test(model_, data_spy_, parts_,
                              VideoListModelRole::kSelectedFormatRole,
                              QString("new_format"));
    }
}

TEST_F(VideoListModelTest, SetDataDownloadThumbnail) {
    {
        SCOPED_TRACE("");
        generic_set_data_test(model_, data_spy_, parts_,
                              VideoListModelRole::kDownloadThumbnail, true);
    }
}

TEST_F(VideoListModelTest, SetDataState) {
    {
        SCOPED_TRACE("");
        generic_set_data_test(model_, data_spy_, parts_,
                              VideoListModelRole::kState,
                              DownloadState::kQueued);
    }
}

TEST_F(VideoListModelTest, SetDataOutOfBoundsIndex) {
    model_.appendVideos(parts_);

    EXPECT_FALSE(
        model_.setData(model_.index(INT_MAX), 0.5,
                       static_cast<int>(VideoListModelRole::kProgressRole)));

    EXPECT_EQ(data_spy_.count(), 0);
}

TEST_F(VideoListModelTest, SetDataEmptyVariant) {
    model_.appendVideos(parts_);

    EXPECT_FALSE(
        model_.setData(model_.index(0), QVariant(),
                       static_cast<int>(VideoListModelRole::kProgressRole)));

    EXPECT_EQ(data_spy_.count(), 0);
}

TEST_F(VideoListModelTest, SetDataInvalidRole) {
    model_.appendVideos(parts_);

    EXPECT_FALSE(model_.setData(model_.index(0), 0.5, INT_MAX));

    EXPECT_EQ(data_spy_.count(), 0);
}

}  // namespace yd_gui
