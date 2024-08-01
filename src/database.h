#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qtmetamacros.h>

#include <QList>
#include <QObject>
#include <QPair>
#include <QtSql/QSqlDatabase>

#include "video.h"

namespace yd_gui {

class Database : public QObject {
    Q_OBJECT

   public:
    ~Database() override;

    static Database& get();

    static Database get_temp(const QString& connection_name);

    bool valid() const;

    QList<QPair<QPair<qint64, qint64>, VideoInfo>> fetch_first_chunk();

    QList<QPair<QPair<qint64, qint64>, VideoInfo>> fetch_chunk(
        qint64 last_id, qint64 last_created_at);

   signals:
    void validChanged(bool valid);

    void errorPushed(QString error);

    // Pushes the data needed to construct a ManagedVideo:
    // id, created_at, info
    void videoPushed(QPair<QPair<qint64, qint64>, VideoInfo> video);

   public slots:
    void setValid(bool valid);

    void addVideo(const VideoInfo& info);

    void removeVideo(qint64 id);

    void removeAllVideos();

   private:
    static constexpr auto kDatabaseFileName = "history.db";

    explicit Database(const QString& file_name = kDatabaseFileName,
                      QString connection_name = kDatabaseFileName,
                      QObject* parent = nullptr);

    void log_error(QString message);

    static bool create_database(const QString& file_name,
                                const QString& connection_name);

    static bool create_tables(QSqlDatabase& db);

    static bool create_videos_table(QSqlDatabase& db);

    static bool create_formats_table(QSqlDatabase& db);

    static QSqlQuery create_select_first_chunk_videos(QSqlDatabase& db, qint64 chunk_size);

    static QSqlQuery create_select_chunk_videos(QSqlDatabase& db,
                                                qint64 last_id,
                                                qint64 last_created_at,
                                                qint64 chunk_size);

    static QSqlQuery create_select_formats(QSqlDatabase& db, qint64 videos_id);

    static QList<QPair<QPair<qint64, qint64>, VideoInfo>> extract_videos(
        QSqlQuery videos_query, QSqlDatabase& db, Database& this_db);

    static QList<VideoFormat> extract_formats(QSqlQuery formats_query,
                                              Database& db);

    static QSqlQuery create_insert_video(QSqlDatabase& db,
                                         const VideoInfo& info,
                                         qint64 created_at);

    static QSqlQuery create_insert_format(QSqlDatabase& db,
                                          const VideoFormat& format,
                                          qint64 videos_id);

    bool valid_;
    const QString connection_name_;
};

}  // namespace yd_gui
