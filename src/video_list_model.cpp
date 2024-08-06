#include "video_list_model.h"

#include <qabstractitemmodel.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include <QByteArray>
#include <QVariant>
#include <QtAlgorithms>

#include "video.h"

namespace yd_gui {
VideoListModel::VideoListModel(Database& db, QObject* parent)
    : QAbstractListModel(parent), db_(db) {}

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

Q_INVOKABLE void VideoListModel::removeVideo(int row) {
    if (row < 0 || row >= videos_.size()) return;

    beginRemoveRows(QModelIndex(), row, row);
    auto* const video = videos_.takeAt(row);
    endRemoveRows();

    emit video->requestCancelDownload();

    db_.removeVideo(video->id());

    video->deleteLater();
}

Q_INVOKABLE void VideoListModel::removeAllVideos() {
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

void VideoListModel::prependVideos(QList<ManagedVideoParts> parts) {
    while (!parts.empty()) {
        beginInsertRows(QModelIndex(), 0, 0);
        videos_.prepend(new ManagedVideo(parts.takeLast()));
        endInsertRows();
    }
}

void VideoListModel::appendVideos(QList<ManagedVideoParts> parts) {
    QList<ManagedVideo*> videos;
    videos.reserve(parts.size());

    while (!parts.empty()) {
        videos << new ManagedVideo(parts.takeFirst());
    }

    beginInsertRows(QModelIndex(), videos_.size(),
                    videos_.size() + videos.size());
    videos_.append(std::move(videos));
    endInsertRows();
}

}  // namespace yd_gui
