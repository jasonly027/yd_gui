#include "video_list_model.h"

#include <qabstractitemmodel.h>
#include <qalgorithms.h>
#include <qbytearray.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qtpreprocessorsupport.h>
#include <qvariant.h>

#include "database.h"
#include "video.h"

namespace yd_gui {
VideoListModel::VideoListModel(Database& db, QObject* parent)
    : QAbstractListModel(parent), db_(db) {
    // videos_.append(
    //     {new ManagedVideo(
    //          0, 111,
    //          VideoInfo(
    //              "652eccdcf4d64600015fd610", "Sausages and Salad", "Oscar",
    //              1438,
    //              "https://gvimage.zype.com/5b0820fbdc4390132f0001ca/"
    //              "652eccdcf4d64600015fd610/custom_thumbnail/"
    //              "1080.jpg?1701815955",
    //              "https://www.americastestkitchen.com/cookscountry/episode/"
    //              "918-sausages-and-salad",
    //              {VideoFormat("hls-360", "mp4", 426, 240, 0),
    //               VideoFormat("hls-1126", "mp4", 854, 480, 0),
    //               VideoFormat("hls-2928", "mp4", 1280, 720, 0),
    //               VideoFormat("hls-4280", "mp4", 1920, 1080, 0)},
    //              true),
    //          DownloadState::kAdded),
    //      new ManagedVideo(
    //          1, 222,
    //          VideoInfo(
    //              "652eccdcf4d64600015fd610", "Sausages and Salad 2", "Joseph",
    //              1438,
    //              "https://gvimage.zype.com/5b0820fbdc4390132f0001ca/"
    //              "652eccdcf4d64600015fd610/custom_thumbnail/"
    //              "1080.jpg?1701815955",
    //              "https://www.americastestkitchen.com/cookscountry/episode/"
    //              "918-sausages-and-salad",
    //              {VideoFormat("hls-360", "mp4", 426, 240, 0),
    //               VideoFormat("hls-1126", "mp4", 854, 480, 0),
    //               VideoFormat("hls-2928", "mp4", 1280, 720, 0),
    //               VideoFormat("hls-4280", "mp4", 1920, 1080, 0)},
    //              true),
    //          DownloadState::kAdded)});
}

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
            case VideoListModelRole::kDownloadThumbnail:
                return videos_.at(row)->download_thumbnail();
            case VideoListModelRole::kState:
                return QVariant::fromValue(videos_.at(row)->state());
            default:
                return QVariant();
        }
    }

    return QVariant();
}

bool VideoListModel::setData(const QModelIndex& index, const QVariant& value,
                             int role) {
    if (!index.isValid()) return false;

    const int row = index.row();
    if (row < 0 || row >= videos_.size()) return false;

    auto role_enum = static_cast<VideoListModelRole>(role);
    switch (role_enum) {
        case VideoListModelRole::kInfoRole:
            break;
        case VideoListModelRole::kProgressRole: {
            bool ok = false;
            if (float progress = value.toFloat(&ok);
                ok && progress != videos_[row]->progress()) {
                videos_[row]->setProgress(progress);
                emit dataChanged(index, index, {role});
                return true;
            }
            break;
        }
        case VideoListModelRole::kCreatedAtRole:
            break;
        case VideoListModelRole::kSelectedFormatRole: {
            if (QString selected_format = value.toString();
                selected_format != videos_[row]->selected_format()) {
                videos_[row]->setSelectedFormat(selected_format);
                emit dataChanged(index, index, {role});
                return true;
            }
            break;
        }
        case VideoListModelRole::kDownloadThumbnail: {
            if (bool download_thumbnail = value.toBool();
                download_thumbnail != videos_[row]->download_thumbnail()) {
                videos_[row]->setDownloadThumbnail(download_thumbnail);
                emit dataChanged(index, index, {role});
                return true;
            }
            break;
        }
        case VideoListModelRole::kState: {
            if (!value.canConvert<DownloadState>()) break;
            if (auto state = value.value<DownloadState>();
                state != videos_[row]->state()) {
                videos_[row]->setState(state);
                emit dataChanged(index, index, {role});
                return true;
            }
            break;
        }
    }

    return false;
}

QHash<int, QByteArray> VideoListModel::roleNames() const {
    static const QHash<int, QByteArray> kRoles{
        {static_cast<int>(VideoListModelRole::kInfoRole), "info"},
        {static_cast<int>(VideoListModelRole::kProgressRole), "progress"},
        {static_cast<int>(VideoListModelRole::kCreatedAtRole), "createdAt"},
        {static_cast<int>(VideoListModelRole::kSelectedFormatRole),
         "selectedFormat"},
        {static_cast<int>(VideoListModelRole::kDownloadThumbnail),
         "downloadThumbnail"},
        {static_cast<int>(VideoListModelRole::kState), "state"}};
    return kRoles;
}

void VideoListModel::removeVideo(int row) {
    if (row < 0 || row >= videos_.size()) return;

    beginRemoveRows(QModelIndex(), row, row);
    auto* const video = videos_.takeAt(row);
    endRemoveRows();

    emit video->requestCancelDownload();

    db_.removeVideo(video->id());

    video->deleteLater();
}

void VideoListModel::removeAllVideos() {
    if (videos_.empty()) return;

    beginRemoveRows(QModelIndex(), 0, videos_.size() - 1);
    QList<ManagedVideo*> videos = videos_;
    videos_.clear();
    endRemoveRows();

    db_.removeAllVideos();

    for (auto* const video : videos) {
        emit video->requestCancelDownload();
        video->deleteLater();
    }
}

void VideoListModel::downloadVideo(int row) {
    if (row < 0 || row >= videos_.size()) return;

    emit requestDownloadVideo(videos_[row]);
}

void VideoListModel::downloadAllVideos() {
    for (auto* const video : videos_) {
        emit requestDownloadVideo(video);
    }
}

void VideoListModel::cancelDownload(int row) {
    if (row < 0 || row >= videos_.size()) return;

    emit videos_[row]->requestCancelDownload();
}

void VideoListModel::cancelAllDownloads() {
    for (auto* const video : videos_) {
        emit video->requestCancelDownload();
    }
}

void VideoListModel::prependVideos(QList<ManagedVideoParts> parts) {
    while (!parts.empty()) {
        beginInsertRows(QModelIndex(), 0, 0);
        videos_.prepend(new ManagedVideo(parts.takeLast(), this));
        endInsertRows();
    }
}

void VideoListModel::appendVideos(QList<ManagedVideoParts> parts) {
    QList<ManagedVideo*> videos;
    videos.reserve(parts.size());

    while (!parts.empty()) {
        videos << new ManagedVideo(parts.takeFirst(), this);
    }

    beginInsertRows(QModelIndex(), videos_.size(),
                    videos_.size() + videos.size());
    videos_.append(std::move(videos));
    endInsertRows();
}

}  // namespace yd_gui
