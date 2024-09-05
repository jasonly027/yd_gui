#include "video_list_model_sorted_proxy.h"

#include <qabstractitemmodel.h>
#include <qabstractproxymodel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qsortfilterproxymodel.h>
#include <qtypes.h>
#include <qvariant.h>

#include <cassert>

#include "video_list_model.h"

namespace yd_gui {
VideoListModelSortedProxy::VideoListModelSortedProxy(QObject* parent)
    : QSortFilterProxyModel(parent) {
    sort(0, Qt::DescendingOrder);
}

void VideoListModelSortedProxy::setModel(VideoListModel* model) {
    source_model_ = model;
    setSourceModel(source_model_);
}

void VideoListModelSortedProxy::removeVideo(int row) {
    if (!hasIndex(row, 0)) return;
    QModelIndex proxy_index = index(row, 0);
    source_model_->removeVideo(mapToSource(proxy_index).row());
}

void VideoListModelSortedProxy::downloadVideo(int row) {
    if (!hasIndex(row, 0)) return;
    QModelIndex proxy_index = index(row, 0);
    source_model_->downloadVideo(mapToSource(proxy_index).row());
}

void VideoListModelSortedProxy::cancelDownload(int row) {
    if (!hasIndex(row, 0)) return;
    QModelIndex proxy_index = index(row, 0);
    source_model_->cancelDownload(mapToSource(proxy_index).row());
}

bool VideoListModelSortedProxy::lessThan(
    const QModelIndex& source_left, const QModelIndex& source_right) const {
    const QVariant left_data = sourceModel()->data(
        source_left,
        static_cast<int>(VideoListModel::VideoListModelRole::kIdRole));
    const QVariant right_data = sourceModel()->data(
        source_right,
        static_cast<int>(VideoListModel::VideoListModelRole::kIdRole));

    assert(left_data.typeId() == QMetaType::LongLong &&
           right_data.typeId() == QMetaType::LongLong);

    return left_data.toLongLong() < right_data.toLongLong();
}
}  // namespace yd_gui
