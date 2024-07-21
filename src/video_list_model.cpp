#include "video_list_model.h"

#include <qalgorithms.h>
#include <qstringview.h>
#include <qvariant.h>

#include "video.h"

namespace yd_gui {
VideoListModel::VideoListModel(QObject* parent) : QAbstractListModel(parent) {
    videos_
        << new ManagedVideo(
               0,
               VideoInfo(
                   "652eccdcf4d64600015fd610", "Sausages and Salad", "", 1438,
                   "https://gvimage.zype.com/5b0820fbdc4390132f0001ca/"
                   "652eccdcf4d64600015fd610/custom_thumbnail/"
                   "1080.jpg?1701815955",
                   "https://www.americastestkitchen.com/cookscountry/episode/"
                   "918-sausages-and-salad",
                   {VideoFormat("hls-360", "mp4", 426, 240, 0),
                    VideoFormat("hls-1126", "mp4", 854, 480, 0),
                    VideoFormat("hls-2928", "mp4", 1280, 720, 0),
                    VideoFormat("hls-4280", "mp4", 1920, 1080, 0)},
                   true),
               0)
        << new ManagedVideo(
               1,
               VideoInfo(
                   "652eccdcf4d64600015fd610", "Sausages and Salad", "", 1438,
                   "https://gvimage.zype.com/5b0820fbdc4390132f0001ca/"
                   "652eccdcf4d64600015fd610/custom_thumbnail/"
                   "1080.jpg?1701815955",
                   "https://www.americastestkitchen.com/cookscountry/episode/"
                   "918-sausages-and-salad",
                   {VideoFormat("hls-360", "mp4", 426, 240, 0),
                    VideoFormat("hls-1126", "mp4", 854, 480, 0),
                    VideoFormat("hls-2928", "mp4", 1280, 720, 0),
                    VideoFormat("hls-4280", "mp4", 1920, 1080, 0)},
                   true),
               0)
        << new ManagedVideo(
               2,
               VideoInfo(
                   "652eccdcf4d64600015fd610", "Sausages and Salad", "", 1438,
                   "https://gvimage.zype.com/5b0820fbdc4390132f0001ca/"
                   "652eccdcf4d64600015fd610/custom_thumbnail/"
                   "1080.jpg?1701815955",
                   "https://www.americastestkitchen.com/cookscountry/episode/"
                   "918-sausages-and-salad",
                   {VideoFormat("hls-360", "mp4", 426, 240, 0),
                    VideoFormat("hls-1126", "mp4", 854, 480, 0),
                    VideoFormat("hls-2928", "mp4", 1280, 720, 0),
                    VideoFormat("hls-4280", "mp4", 1920, 1080, 0)},
                   true),
               0);
}

VideoListModel::~VideoListModel() { qDeleteAll(videos_); }

int VideoListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return videos_.size();
}

QVariant VideoListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    const int row = index.row();
    if (row < 0 || row >= videos_.size()) return QVariant();

    auto role_enum = static_cast<VideoListModelRole>(role);
    if (row < rowCount()) {
        switch (role_enum) {
            case VideoListModelRole::kInfoRole:
                return QVariant::fromValue(videos_.at(row)->info());
            case VideoListModelRole::kProgressRole:
                return videos_.at(row)->progress();
            case VideoListModelRole::kCreatedAtRole:
                return videos_.at(row)->created_at();
            case VideoListModelRole::kSelectedFormatRole:
                return videos_.at(row)->selected_format();
            default:
                return QVariant();
        }
    }

    return QVariant();
}

QHash<int, QByteArray> VideoListModel::roleNames() const {
    static const QHash<int, QByteArray> kRoles{
        {static_cast<int>(VideoListModelRole::kInfoRole), "info"},
        {static_cast<int>(VideoListModelRole::kProgressRole), "progress"},
        {static_cast<int>(VideoListModelRole::kCreatedAtRole), "createdAt"},
        {static_cast<int>(VideoListModelRole::kSelectedFormatRole),
         "selectedFormat"},
    };
    return kRoles;
}
}  // namespace yd_gui
