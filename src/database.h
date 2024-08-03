#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include <QList>
#include <QObject>
#include <QtSql/QSqlDatabase>

#include "video.h"

namespace yd_gui {

class Database : public QObject {
    Q_OBJECT

   public:
    ~Database() override;

    static Database& get();

    static Database get_temp(const QString& connection_name);

    static constexpr qint64 kChunkSize = 25;

    bool valid() const;

    QList<ManagedVideoParts> fetch_first_chunk();

    QList<ManagedVideoParts> fetch_chunk(qint64 last_id,
                                         qint64 last_created_at);

   signals:
    void validChanged(bool valid);

    void errorPushed(QString error);

    void videosPushed(QList<ManagedVideoParts> videos);

   public slots:
    void setValid(bool valid);

    void addVideo(const VideoInfo& info);

    void removeVideo(qint64 id);

    void removeAllVideos();

   private:
    friend QList<ManagedVideoParts> extract_videos(QSqlQuery videos_query,
                                                   QSqlDatabase& db,
                                                   Database& this_db);

    friend QList<VideoFormat> extract_formats(QSqlQuery formats_query,
                                              Database& db);

    void log_error(QString message);

    static constexpr auto kDatabaseFileName = "history.db";

    explicit Database(const QString& file_name = kDatabaseFileName,
                      QString connection_name = kDatabaseFileName,
                      QObject* parent = nullptr);

    QList<ManagedVideoParts> fetch_chunk_impl(QSqlDatabase db, QSqlQuery query);

    bool valid_;
    const QString connection_name_;
};

}  // namespace yd_gui
