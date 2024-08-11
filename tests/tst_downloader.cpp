#include <downloader.h>
#include <gtest/gtest.h>
#include <qcontainerfwd.h>
#include <qcoreapplication.h>
#include <qdebug.h>
#include <qdir.h>
#include <qlist.h>
#include <qobject.h>
#include <qprocess.h>
#include <qsignalspy.h>
#include <qtimer.h>
#include <qtypes.h>
#include <qvariant.h>

#include <QCoreApplication>
#include <QDebug>
#include <QSignalSpy>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <utility>

#include "_tst_util.h"  // IWYU pragma: keep
#include "gmock/gmock.h"
#include "video.h"

using namespace tst_util;  // NOLINT(google-build-using-namespace)

using std::optional, std::make_pair;

namespace yd_gui {

class VideoFixture : public Test {
   public:
    static const VideoInfo kJmInfo;

    static const VideoInfo kCksInfo;

    static const VideoInfo kZooInfo;

   protected:
    ManagedVideo cks_video_{0, 0, kCksInfo, DownloadState::kAdded};
    ManagedVideo jm_video_{1, 1, kJmInfo, DownloadState::kAdded};
    ManagedVideo zoo_video_{2, 2, kZooInfo, DownloadState::kAdded};

    // Should be a playlist with two videos:
    // 1) Joshua Morony's Angular video
    // 2) jawed's zoo video
    const QString playlist_{
        "https://www.youtube.com/"
        "playlist?list=PLoBsP3alZEYf8GPkgs8IHDzTxPJtqvPc5"};
};

const VideoInfo VideoFixture::kCksInfo{
    "652eccdcf4d64600015fd610",
    "Sausages and Salad",
    "",
    1438,
    "https://gvimage.zype.com/5b0820fbdc4390132f0001ca/"
    "652eccdcf4d64600015fd610/custom_thumbnail/1080.jpg?1701815955",
    "https://www.americastestkitchen.com/cookscountry/episode/"
    "918-sausages-and-salad",
    {VideoFormat("hls-360", "mp4", 426, 240, 0),
     VideoFormat("hls-1126", "mp4", 854, 480, 0),
     VideoFormat("hls-2928", "mp4", 1280, 720, 0),
     VideoFormat("hls-4280", "mp4", 1920, 1080, 0)},
    true};

const VideoInfo VideoFixture::kJmInfo{
    "Sv3LXGWKw6Q",
    "Should this be the future of Angular applications?",
    "Joshua Morony",
    341,
    "https://i.ytimg.com/vi/Sv3LXGWKw6Q/maxresdefault.jpg",
    "https://youtu.be/Sv3LXGWKw6Q",
    // Only a sample of all the formats
    {VideoFormat("602", "mp4", 256, 144, 15),
     VideoFormat("394", "mp4", 256, 144, 30),
     VideoFormat("278", "webm", 256, 144, 30),
     VideoFormat("625", "mp4", 3840, 2160, 30)},
    true};

const VideoInfo VideoFixture::kZooInfo{
    "jNQXAC9IVRw",
    "Me at the zoo",
    "jawed",
    19,
    "https://i.ytimg.com/vi/jNQXAC9IVRw/"
    "hqdefault.jpg?sqp=-oaymwEmCOADEOgC8quKqQMa8AEB-AG-"
    "AoAC8AGKAgwIABABGFQgWChlMA8=&rs="
    "AOn4CLDCa7zhojGoyn64pv8zl0PVdCnpRw",
    "https://youtu.be/jNQXAC9IVRw",
    // Only a sample of all the formats
    {VideoFormat("394", "mp4", 192, 144, 15),
     VideoFormat("278", "webm", 192, 144, 15),
     VideoFormat("269", "mp4", 192, 144, 15),
     VideoFormat("604", "mp4", 320, 240, 15)},
    true};

class ParseRawInfoTest : public VideoFixture {};

class ParseRawInfoWithParamsTest
    : public VideoFixture,
      public WithParamInterface<std::pair<QString, VideoInfo>> {};

INSTANTIATE_TEST_SUITE_P(
    GoodJSONs, ParseRawInfoWithParamsTest,
    // JSON files and their expected parsed VideoInfo, respectively
    testing::Values(make_pair("jm_fmt.json", VideoFixture::kJmInfo),
                    make_pair("jm_unfmt.json", VideoFixture::kJmInfo),
                    make_pair("zoo_fmt.json", VideoFixture::kZooInfo)));

TEST_P(ParseRawInfoWithParamsTest, ParseJSONPartialCheck) {
    const QString raw = read_file(YD_GUI_TEST_DATA_PATH + GetParam().first);
    const optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());

    {
        SCOPED_TRACE("");
        EXPECT_INFOS_EQ_EXCLUDING_FORMATS(*info, GetParam().second);
    }

    EXPECT_THAT(GetParam().second.formats(), IsSubsetOf(info->formats()));
}

TEST_F(ParseRawInfoTest, ParseJSONFullCheck) {
    const QString raw = read_file(YD_GUI_TEST_DATA_PATH "cks_fmt.json");
    const optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());

    EXPECT_EQ(*info, kCksInfo);
}

TEST_F(ParseRawInfoTest, Malformed) {
    const QString raw =
        read_file(YD_GUI_TEST_DATA_PATH "jm_unfmt_malformed.json");
    const optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    EXPECT_FALSE(info.has_value());
}

TEST_F(ParseRawInfoTest, MissingId) {
    const QString raw =
        read_file(YD_GUI_TEST_DATA_PATH "cks_fmt_missing_id.json");
    const optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());

    EXPECT_NE(*info, kCksInfo)
        << "Shouldn't be equal because video_id is missing";

    EXPECT_EQ(info->video_id(), "") << "Should be the default value";

    EXPECT_EQ(info->title(), kCksInfo.title())
        << "Fields other than video_id should still be ok";
}

TEST_F(ParseRawInfoTest, MissingIdAndTitle) {
    const QString raw =
        read_file(YD_GUI_TEST_DATA_PATH "cks_fmt_missing_id,title.json");
    const optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());

    EXPECT_NE(*info, kCksInfo)
        << "Shouldn't be equal because video_id and title are missing";

    EXPECT_EQ(info->video_id(), "") << "Should be the default value";
    EXPECT_EQ(info->title(), "") << "Should be the default value";

    EXPECT_THAT(info->formats(), ContainerEq(kCksInfo.formats()))
        << "Fields other than video_id and title should still be ok";
}

TEST_F(ParseRawInfoTest, JustFormats) {
    const QString raw =
        read_file(YD_GUI_TEST_DATA_PATH "cks_just_formats.json");
    const optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());

    // Should be default values
    EXPECT_EQ(info->video_id(), "");
    EXPECT_EQ(info->title(), "");
    EXPECT_EQ(info->author(), "");
    EXPECT_EQ(info->seconds(), 0);
    EXPECT_EQ(info->thumbnail(), "");
    EXPECT_EQ(info->audio_available(), true);

    EXPECT_THAT(info->formats(), ContainerEq(kCksInfo.formats()))
        << "Formats should still be ok";
}

TEST_F(ParseRawInfoTest, MissingFormats) {
    const QString raw =
        read_file(YD_GUI_TEST_DATA_PATH "cks_missing_formats.json");
    const optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_FALSE(info.has_value()) << "VideoInfo shouldn't have been created "
                                      "if there's nothing to download from it";
}

TEST_F(ParseRawInfoTest, JustACodecs) {
    const QString raw =
        read_file(YD_GUI_TEST_DATA_PATH "cks_just_acodecs.json");
    const optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());

    EXPECT_TRUE(info->audio_available());

    EXPECT_TRUE(info->formats().empty());
}

TEST_F(ParseRawInfoTest, JustFormatsEmpty) {
    const QString raw =
        read_file(YD_GUI_TEST_DATA_PATH "cks_just_formats_empty.json");
    const optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_FALSE(info.has_value());
}

class DownloaderTest : public VideoFixture {
   protected:
    DownloaderTest() {
        QObject::connect(
            &dl_, &Downloader::standardErrorPushed,
            [](const QString& data) { std::cerr << data.toStdString(); });

        QObject::connect(&dl_, &Downloader::programExistsChanged, [this] {
            EXPECT_EQ(program_exists_spy_.count(), 0)
                << "programExistsChanged signal was emit";
        });

        QObject::connect(&dl_, &Downloader::fetchInfoBadParse, [this] {
            EXPECT_EQ(bad_parse_spy_.count(), 0)
                << "fetchInfoBadParse signal was emit";
        });
    }

    static QList<DownloadState> extract_states(
        const QList<QList<QVariant>>& list) {
        QList<DownloadState> states;

        for (const auto& var_list : list) {
            EXPECT_EQ(var_list.size(), 1)
                << "Signal emission output container size isn't 1";
            if (var_list.size() != 1) throw;

            states << try_convert<DownloadState>(var_list.first());
        }

        return states;
    }

    Downloader dl_;

    QSignalSpy fetching_spy_{&dl_, &Downloader::isFetchingChanged};

    QSignalSpy downloading_spy_{&dl_, &Downloader::isDownloadingChanged};

    QSignalSpy info_pushed_spy_{&dl_, &Downloader::infoPushed};

    QSignalSpy bad_parse_spy_{&dl_, &Downloader::fetchInfoBadParse};

    QSignalSpy program_exists_spy_{&dl_, &Downloader::programExistsChanged};

    QSignalSpy standard_err_spy_{&dl_, &Downloader::standardErrorPushed};
};

TEST_F(DownloaderTest, FetchInfoCks) {
    dl_.fetch_info(kCksInfo.url());

    EXPECT_TRUE(wait_for_n_signals(fetching_spy_, 2));

    ASSERT_EQ(info_pushed_spy_.count(), 1);

    const auto should_be_cks_info =
        try_convert<VideoInfo>(info_pushed_spy_.takeFirst().takeFirst());

    EXPECT_EQ(should_be_cks_info, kCksInfo);
}

TEST_F(DownloaderTest, FetchInfoJm) {
    dl_.fetch_info(kJmInfo.url());

    EXPECT_TRUE(wait_for_n_signals(fetching_spy_, 2));

    ASSERT_EQ(info_pushed_spy_.count(), 1);

    const auto should_be_jm_info =
        try_convert<VideoInfo>(info_pushed_spy_.takeFirst().takeFirst());

    {
        SCOPED_TRACE("");
        EXPECT_INFOS_EQ_EXCLUDING_FORMATS(should_be_jm_info, kJmInfo);
    }

    EXPECT_THAT(kJmInfo.formats(), IsSubsetOf(should_be_jm_info.formats()));
}

TEST_F(DownloaderTest, FetchInfoPlaylist) {
    dl_.fetch_info(playlist_);

    EXPECT_TRUE(wait_for_n_signals(fetching_spy_, 2));

    ASSERT_EQ(info_pushed_spy_.count(), 2);

    // Infos are expected to come in in the reverse order of the playlist

    const auto should_be_zoo_info =
        try_convert<VideoInfo>(info_pushed_spy_.takeFirst().takeFirst());

    {
        SCOPED_TRACE("");
        EXPECT_INFOS_EQ_EXCLUDING_FORMATS(should_be_zoo_info, kZooInfo);
    }

    EXPECT_THAT(kZooInfo.formats(), IsSubsetOf(should_be_zoo_info.formats()));

    const auto should_be_jm_info =
        try_convert<VideoInfo>(info_pushed_spy_.takeFirst().takeFirst());

    {
        SCOPED_TRACE("");
        EXPECT_INFOS_EQ_EXCLUDING_FORMATS(should_be_jm_info, kJmInfo);
    }

    EXPECT_THAT(kJmInfo.formats(), IsSubsetOf(should_be_jm_info.formats()));
}

TEST_F(DownloaderTest, EnqueueOneVideo) {
    QSignalSpy state_spy(&zoo_video_, &ManagedVideo::stateChanged);
    dl_.enqueue_video(&zoo_video_);

    EXPECT_TRUE(wait_for_n_signals(downloading_spy_, 2));

    EXPECT_EQ(state_spy.count(), 3) << PrintToString(extract_states(state_spy));

    EXPECT_EQ(zoo_video_.progress(), "100%");

    EXPECT_THAT(
        extract_states(state_spy),
        Pointwise(Eq(), {DownloadState::kQueued, DownloadState::kDownloading,
                         DownloadState::kComplete}));
}

TEST_F(DownloaderTest, EnqueueTwoVideos) {
    ManagedVideo zoo_video_copy(zoo_video_.id() + 1,
                                zoo_video_.created_at() + 1, kZooInfo,
                                zoo_video_.state());

    QSignalSpy zoo_state_spy(&zoo_video_, &ManagedVideo::stateChanged);
    QSignalSpy zoo_copy_state_spy(&zoo_video_copy, &ManagedVideo::stateChanged);

    dl_.enqueue_video(&zoo_video_);
    dl_.enqueue_video(&zoo_video_copy);

    EXPECT_TRUE(wait_for_n_signals(downloading_spy_, 4));

    EXPECT_EQ(zoo_state_spy.count(), 3)
        << PrintToString(extract_states(zoo_state_spy));
    EXPECT_EQ(zoo_copy_state_spy.count(), 3)
        << PrintToString(extract_states(zoo_copy_state_spy));

    EXPECT_THAT(
        extract_states(zoo_state_spy),
        Pointwise(Eq(), {DownloadState::kQueued, DownloadState::kDownloading,
                         DownloadState::kComplete}));

    EXPECT_THAT(
        extract_states(zoo_copy_state_spy),
        Pointwise(Eq(), {DownloadState::kQueued, DownloadState::kDownloading,
                         DownloadState::kComplete}));
}

TEST_F(DownloaderTest, CancelDownloadingVideo) {
    QSignalSpy state_spy(&zoo_video_, &ManagedVideo::stateChanged);

    // Cancel download when the download initiates
    QObject::connect(&zoo_video_, &ManagedVideo::stateChanged, &dl_, [this] {
        if (zoo_video_.state() == DownloadState::kDownloading) {
            emit zoo_video_.requestCancelDownload();
        }
    });

    dl_.enqueue_video(&zoo_video_);

    EXPECT_TRUE(wait_for_n_signals(state_spy, 3))
        << PrintToString(extract_states(state_spy));

    EXPECT_EQ(downloading_spy_.count(), 2);

    EXPECT_THAT(
        extract_states(state_spy),
        Pointwise(Eq(), {DownloadState::kQueued, DownloadState::kDownloading,
                         DownloadState::kAdded}));
}

}  // namespace yd_gui
