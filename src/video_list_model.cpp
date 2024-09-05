#include "video_list_model.h"

#include <qabstractitemmodel.h>
#include <qalgorithms.h>
#include <qbytearray.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qtpreprocessorsupport.h>
#include <qtypes.h>
#include <qvariant.h>

#include <optional>

#include "database.h"
#include "video.h"

namespace yd_gui {
VideoListModel::VideoListModel(Database& db, QObject* parent)
    : QAbstractListModel(parent), db_(db) {
    paginate();
}

int VideoListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return videos_.size();
}

QVariant VideoListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || !hasIndex(index.row(), index.column()))
        return QVariant();

    const int row = index.row();

    auto role_enum = static_cast<VideoListModelRole>(role);
    if (row < rowCount()) {
        switch (role_enum) {
            case VideoListModelRole::kIdRole:
                return videos_.at(row)->id();
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
    if (!index.isValid() || !hasIndex(index.row(), index.column()))
        return false;

    const int row = index.row();

    auto role_enum = static_cast<VideoListModelRole>(role);
    switch (role_enum) {
        case VideoListModelRole::kIdRole:
        case VideoListModelRole::kInfoRole:
            break;
        case VideoListModelRole::kProgressRole: {
            bool ok = false;
            float progress = value.toFloat(&ok);
            if (!ok) break;

            if (progress == videos_[row]->progress()) return true;

            videos_[row]->setProgress(progress, false);
            emit dataChanged(index, index, {role});
            return true;
        }
        case VideoListModelRole::kCreatedAtRole:
            break;
        case VideoListModelRole::kSelectedFormatRole: {
            QString selected_format = value.toString();

            if (selected_format == videos_[row]->selected_format()) return true;

            videos_[row]->setSelectedFormat(selected_format, false);
            emit dataChanged(index, index, {role});
            return true;
        }
        case VideoListModelRole::kDownloadThumbnail: {
            bool download_thumbnail = value.toBool();

            if (download_thumbnail == videos_[row]->download_thumbnail())
                return true;

            videos_[row]->setDownloadThumbnail(download_thumbnail, false);
            emit dataChanged(index, index, {role});
            return true;
        }
        case VideoListModelRole::kState: {
            if (!value.canConvert<DownloadState>()) break;

            auto state = value.value<DownloadState>();

            if (state == videos_[row]->state()) return true;

            videos_[row]->setState(state, false);
            emit dataChanged(index, index, {role});
            return true;
        }
    }

    return false;
}

QHash<int, QByteArray> VideoListModel::roleNames() const {
    static const QHash<int, QByteArray> kRoles{
        {static_cast<int>(VideoListModelRole::kIdRole), "dbId"},
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

QModelIndex VideoListModel::find_video(ManagedVideo& video) const {
    if (video.cached_index().has_value() &&
        hasIndex(*video.cached_index(), 0) &&
        &video == videos_[*video.cached_index()]) {
        return this->index(*video.cached_index());
    }

    const qsizetype index = videos_.indexOf(&video);

    if (index == static_cast<qsizetype>(-1)) return QModelIndex();

    video.setCachedIndex(index);

    return this->index(index);
}

void VideoListModel::update_video(ManagedVideo& video,
                                  const QList<int>& roles) {
    QModelIndex index = find_video(video);
    if (!index.isValid()) return;

    emit dataChanged(index, index, roles);
}

void VideoListModel::removeVideo(int row) {
    if (!hasIndex(row, 0)) return;

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
    if (!hasIndex(row, 0)) return;

    if (videos_[row]->state() == DownloadState::kAdded ||
        videos_[row]->state() == DownloadState::kComplete)
        emit requestDownloadVideo(videos_[row]);
}

void VideoListModel::downloadAllVideos() {
    for (auto* const video : videos_) {
        if (video->state() == DownloadState::kAdded)
            emit requestDownloadVideo(video);
    }
}

void VideoListModel::cancelDownload(int row) {
    if (!hasIndex(row, 0)) return;

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
    if (parts.empty()) return;

    QList<ManagedVideo*> videos;
    videos.reserve(parts.size());

    while (!parts.empty()) {
        videos << new ManagedVideo(parts.takeFirst(), this);
    }

    beginInsertRows(QModelIndex(), videos_.size(),
                    videos_.size() + videos.size() - 1);
    videos_.append(std::move(videos));
    endInsertRows();
}

void VideoListModel::paginate() {
    prependVideos(videos_.empty()
                      ? db_.fetch_first_chunk()
                      : db_.fetch_chunk(videos_.first()->id(),
                                        videos_.first()->created_at()));
}

}  // namespace yd_gui
