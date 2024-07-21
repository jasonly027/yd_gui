#pragma once

#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qvariant.h>

#include <QAbstractListModel>

#include "video.h"

namespace yd_gui {

class VideoListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_UNCREATABLE("")

   public:
    enum class VideoListModelRole {
        kInfoRole = Qt::UserRole,
        kProgressRole,
        kCreatedAtRole,
        kSelectedFormatRole,
    };

    explicit VideoListModel(QObject* parent = nullptr);

    ~VideoListModel() override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

   private:
    QList<ManagedVideo*> videos_;
};

}  // namespace yd_gui
