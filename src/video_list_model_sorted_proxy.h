#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qsortfilterproxymodel.h>
#include <qtmetamacros.h>

#include "video_list_model.h"

namespace yd_gui {
class VideoListModelSortedProxy : public QSortFilterProxyModel {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
   public:
    explicit VideoListModelSortedProxy(QObject* parent = nullptr);

    Q_INVOKABLE void setModel(VideoListModel*);

    Q_INVOKABLE void removeVideo(int row);

    Q_INVOKABLE void downloadVideo(int row);

    Q_INVOKABLE void cancelDownload(int row);

   protected:
    bool lessThan(const QModelIndex& source_left,
                  const QModelIndex& source_right) const override;

   private:
    VideoListModel* source_model_;
};
}  // namespace yd_gui
