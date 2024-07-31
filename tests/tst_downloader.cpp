#include <downloader.h>
#include <gtest/gtest.h>
#include <qcoreapplication.h>
#include <qdebug.h>
#include <qdir.h>
#include <qobject.h>
#include <qprocess.h>
#include <qsignalspy.h>
#include <qtimer.h>

#include <QCoreApplication>
#include <QDebug>
#include <QSignalSpy>
#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>

#include "video.h"

using std::optional;

namespace yd_gui {

class VideoFixture : public testing::Test {
   protected:
    static bool infos_equal_no_check_formats(const VideoInfo& lhs,
                                             const VideoInfo& rhs) {
        return lhs.video_id() == rhs.video_id() && lhs.title() == rhs.title() &&
               lhs.author() == rhs.author() && lhs.seconds() == rhs.seconds() &&
               lhs.thumbnail() == rhs.thumbnail() &&
               lhs.audio_available() == rhs.audio_available();
    }

    static void check_is_subset(const QList<VideoFormat>& sublist,
                                const QList<VideoFormat>& superlist) {
        const QSet<VideoFormat> superset(superlist.cbegin(), superlist.cend());

        for (const auto& format : sublist) {
            EXPECT_TRUE(superset.contains(format));
        }
    }

    ManagedVideo cks_video_{
        0,
        VideoInfo(
            "652eccdcf4d64600015fd610", "Sausages and Salad", "", 1438,
            "https://gvimage.zype.com/5b0820fbdc4390132f0001ca/"
            "652eccdcf4d64600015fd610/custom_thumbnail/1080.jpg?1701815955",
            "https://www.americastestkitchen.com/cookscountry/episode/"
            "918-sausages-and-salad",
            {VideoFormat("hls-360", "mp4", 426, 240, 0),
             VideoFormat("hls-1126", "mp4", 854, 480, 0),
             VideoFormat("hls-2928", "mp4", 1280, 720, 0),
             VideoFormat("hls-4280", "mp4", 1920, 1080, 0)},
            true),
        0};
    ManagedVideo jm_video_{
        1,
        VideoInfo("Sv3LXGWKw6Q",
                  "Should this be the future of Angular applications?",
                  "Joshua Morony", 341,
                  "https://i.ytimg.com/vi/Sv3LXGWKw6Q/maxresdefault.jpg",
                  "https://youtu.be/Sv3LXGWKw6Q",
                  // The first three VideoFormats and the last one
                  {VideoFormat("602", "mp4", 256, 144, 15),
                   VideoFormat("394", "mp4", 256, 144, 30),
                   VideoFormat("278", "webm", 256, 144, 30),
                   VideoFormat("625", "mp4", 3840, 2160, 30)},
                  true),
        1};
    ManagedVideo zoo_video_{
        2,
        VideoInfo("jNQXAC9IVRw", "Me at the zoo", "jawed", 19,
                  "https://i.ytimg.com/vi/jNQXAC9IVRw/"
                  "hqdefault.jpg?sqp=-oaymwEmCOADEOgC8quKqQMa8AEB-AG-"
                  "AoAC8AGKAgwIABABGFQgWChlMA8=&rs="
                  "AOn4CLDCa7zhojGoyn64pv8zl0PVdCnpRw",
                  "https://youtu.be/jNQXAC9IVRw",
                  // The first three VideoFormats and the last one
                  {VideoFormat("394", "mp4", 192, 144, 15),
                   VideoFormat("278", "webm", 192, 144, 15),
                   VideoFormat("269", "mp4", 192, 144, 15),
                   VideoFormat("604", "mp4", 320, 240, 15)},
                  true),
        2};

    const VideoInfo& jm_info_ = jm_video_.info();
    const VideoInfo& cks_info_ = cks_video_.info();
    const VideoInfo& zoo_info_ = zoo_video_.info();
};

class ParseRawInfoTest : public VideoFixture {
   protected:
    static QString read_file(const QString& path) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::exception();
        }

        QTextStream in(&file);
        return in.readAll();
    }
};

TEST_F(ParseRawInfoTest, Fmt1) {
    QString raw = read_file(YD_GUI_TEST_DATA_PATH "jm_fmt.json");
    std::optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());
    EXPECT_TRUE(infos_equal_no_check_formats(info.value(), jm_info_));
    EXPECT_EQ(info->formats().at(0), jm_info_.formats().at(0));
    EXPECT_EQ(info->formats().at(1), jm_info_.formats().at(1));
    EXPECT_EQ(info->formats().at(2), jm_info_.formats().at(2));
    EXPECT_EQ(info->formats().last(), jm_info_.formats().last());
}

TEST_F(ParseRawInfoTest, UnFmt) {
    QString raw = read_file(YD_GUI_TEST_DATA_PATH "jm_unfmt.json");
    std::optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());
    EXPECT_TRUE(infos_equal_no_check_formats(info.value(), jm_info_));
    EXPECT_EQ(info->formats().at(0), jm_info_.formats().at(0));
    EXPECT_EQ(info->formats().at(1), jm_info_.formats().at(1));
    EXPECT_EQ(info->formats().at(2), jm_info_.formats().at(2));
    EXPECT_EQ(info->formats().last(), jm_info_.formats().last());
}

TEST_F(ParseRawInfoTest, Fmt2) {
    QString raw = read_file(YD_GUI_TEST_DATA_PATH "cks_fmt.json");
    std::optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());
    EXPECT_EQ(info.value(), cks_info_);
}

TEST_F(ParseRawInfoTest, Fmt3) {
    QString raw = read_file(YD_GUI_TEST_DATA_PATH "zoo_fmt.json");
    std::optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());
    EXPECT_TRUE(infos_equal_no_check_formats(info.value(), zoo_info_));
    EXPECT_EQ(info->formats().at(0), zoo_info_.formats().at(0));
    EXPECT_EQ(info->formats().at(1), zoo_info_.formats().at(1));
    EXPECT_EQ(info->formats().at(2), zoo_info_.formats().at(2));
    EXPECT_EQ(info->formats().last(), zoo_info_.formats().last());
}

TEST_F(ParseRawInfoTest, Malformed) {
    QString raw = read_file(YD_GUI_TEST_DATA_PATH "jm_unfmt_malformed.json");
    std::optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    EXPECT_FALSE(info.has_value());
}

TEST_F(ParseRawInfoTest, MissingId) {
    QString raw = read_file(YD_GUI_TEST_DATA_PATH "cks_fmt_missing_id.json");
    std::optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());
    EXPECT_NE(info.value(), cks_info_);
    EXPECT_EQ(info->video_id(), "");
    EXPECT_EQ(info->title(), cks_info_.title());
}

TEST_F(ParseRawInfoTest, MissingIdAndTitle) {
    QString raw =
        read_file(YD_GUI_TEST_DATA_PATH "cks_fmt_missing_id,title.json");
    std::optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());
    EXPECT_NE(info.value(), cks_info_);
    EXPECT_EQ(info->video_id(), "");
    EXPECT_EQ(info->title(), "");
    EXPECT_EQ(info->formats(), cks_info_.formats());
}

TEST_F(ParseRawInfoTest, JustFormats) {
    QString raw = read_file(YD_GUI_TEST_DATA_PATH "cks_just_formats.json");
    std::optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());
    EXPECT_EQ(info->video_id(), "");
    EXPECT_EQ(info->title(), "");
    EXPECT_EQ(info->author(), "");
    EXPECT_EQ(info->seconds(), 0);
    EXPECT_EQ(info->thumbnail(), "");
    EXPECT_EQ(info->audio_available(), true);
    EXPECT_EQ(info->formats(), cks_info_.formats());
}

TEST_F(ParseRawInfoTest, MissingFormats) {
    QString raw = read_file(YD_GUI_TEST_DATA_PATH "cks_missing_formats.json");
    std::optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_FALSE(info.has_value());
}

TEST_F(ParseRawInfoTest, JustACodecs) {
    QString raw = read_file(YD_GUI_TEST_DATA_PATH "cks_just_acodecs.json");
    std::optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_TRUE(info.has_value());
    EXPECT_TRUE(info->audio_available());
    EXPECT_TRUE(info->formats().empty());
}

TEST_F(ParseRawInfoTest, JustFormatsEmpty) {
    QString raw =
        read_file(YD_GUI_TEST_DATA_PATH "cks_just_formats_empty.json");
    std::optional<VideoInfo> info = Downloader::parseRawInfo(raw);

    ASSERT_FALSE(info.has_value());
}

class DownloaderTest : public VideoFixture {
   protected:
    DownloaderTest() {
        QObject::connect(
            &dl_, &Downloader::standardErrorPushed,
            [](const QString& data) { std::cerr << data.toStdString(); });
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
    dl_.fetch_info(cks_info_.url());

    info_pushed_spy_.wait(10000);

    EXPECT_EQ(standard_err_spy_.count(), 0);
    EXPECT_EQ(fetching_spy_.count(), 2);
    EXPECT_EQ(bad_parse_spy_.count(), 0);
    EXPECT_EQ(program_exists_spy_.count(), 0);
    ASSERT_EQ(info_pushed_spy_.count(), 1);

    auto info = info_pushed_spy_.takeFirst().takeFirst().value<VideoInfo>();

    EXPECT_EQ(info, cks_info_);
}

TEST_F(DownloaderTest, FetchInfoJm) {
    dl_.fetch_info(jm_info_.url());

    info_pushed_spy_.wait(10000);

    EXPECT_EQ(fetching_spy_.count(), 2);
    EXPECT_EQ(bad_parse_spy_.count(), 0);
    EXPECT_EQ(program_exists_spy_.count(), 0);
    ASSERT_EQ(info_pushed_spy_.count(), 1);

    auto info = info_pushed_spy_.takeFirst().takeFirst().value<VideoInfo>();

    EXPECT_TRUE(infos_equal_no_check_formats(info, jm_info_));
    check_is_subset(jm_info_.formats(), info.formats());
}

TEST_F(DownloaderTest, EnqueueOneVideo) {
    dl_.enqueue_video(&zoo_video_);

    while (downloading_spy_.wait(10000) && downloading_spy_.count() < 2);

    EXPECT_EQ(program_exists_spy_.count(), 0);
    EXPECT_EQ(downloading_spy_.count(), 2);
    EXPECT_EQ(zoo_video_.progress().toStdString(), "100%");
}

TEST_F(DownloaderTest, EnqueueTwoVideos) {
    dl_.enqueue_video(&zoo_video_);
    dl_.enqueue_video(&zoo_video_);

    while (downloading_spy_.wait(10000) && downloading_spy_.count() < 4);

    EXPECT_EQ(program_exists_spy_.count(), 0);
    EXPECT_EQ(downloading_spy_.count(), 4);
}

}  // namespace yd_gui
