#pragma once

#include <qabstractanimation.h>
#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>

#include <QtQmlIntegration>

#include "database.h"
#include "video.h"

namespace yd_gui {

class VideoListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_UNCREATABLE("")

   public:
    enum class VideoListModelRole {
        kIdRole = Qt::UserRole,
        kInfoRole,
        kProgressRole,
        kCreatedAtRole,
        kSelectedFormatRole,
        kDownloadThumbnail,
        kState,
    };

    explicit VideoListModel(Database& db = Database::get(),
                            QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex& index, const QVariant& value,
                 int role = Qt::EditRole) override;

    QHash<int, QByteArray> roleNames() const override;

    QModelIndex find_video(ManagedVideo& video) const;

    void update_video(ManagedVideo& video, const QList<int>& roles);

    Q_INVOKABLE void removeVideo(int row);

    Q_INVOKABLE void removeAllVideos();

    Q_INVOKABLE void downloadVideo(int row);

    Q_INVOKABLE void downloadAllVideos();

    Q_INVOKABLE void cancelDownload(int row);

    Q_INVOKABLE void cancelAllDownloads();

   signals:
    void requestDownloadVideo(ManagedVideo*);

   public slots:
    void prependVideos(QList<ManagedVideoParts>);

    void appendVideos(QList<ManagedVideoParts>);

    void paginate();

   private:
    QList<ManagedVideo*> videos_;
    Database& db_;
};

}  // namespace yd_gui
