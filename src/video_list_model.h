#pragma once

#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QObject>
#include <QVariant>
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
        kInfoRole = Qt::UserRole,
        kProgressRole,
        kCreatedAtRole,
        kSelectedFormatRole,
    };

    explicit VideoListModel(Database& db = Database::get(),
                            QObject* parent = nullptr);

    ~VideoListModel() override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void removeVideo(int row);

    Q_INVOKABLE void removeAllVideos();

    Q_INVOKABLE void downloadVideo(int row);

    Q_INVOKABLE void downloadAllVideos();

    Q_INVOKABLE void cancelAllDownloads();

   signals:
    void requestDownloadVideo(ManagedVideo*);

   public slots:
    void prependVideos(QList<ManagedVideoParts>);

    void appendVideos(QList<ManagedVideoParts>);

   private:
    QList<ManagedVideo*> videos_;
    Database& db_;
};

}  // namespace yd_gui
