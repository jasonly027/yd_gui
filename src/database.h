#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include <QList>
#include <QObject>
#include <QtSql/QSqlDatabase>
#include <tuple>

#include "video.h"

namespace yd_gui {

class Database : public QObject {
    Q_OBJECT

   public:
    ~Database() override;

    static Database& get();

    static Database get_temp(const QString& connection_name);

    bool valid() const;

    QList<std::tuple<qint64, qint64, VideoInfo>> fetch_first_chunk();

    QList<std::tuple<qint64, qint64, VideoInfo>> fetch_chunk(
        qint64 last_id, qint64 last_created_at);

   signals:
    void validChanged(bool valid);

    void errorPushed(QString error);

    // Pushes the data needed to construct a ManagedVideo:
    // id, created_at, info
    void videosPushed(QList<std::tuple<qint64, qint64, VideoInfo>> videos);

   public slots:
    void setValid(bool valid);

    void addVideo(const VideoInfo& info);

    void removeVideo(qint64 id);

    void removeAllVideos();

   private:
    void log_error(QString message);

    friend QList<std::tuple<qint64, qint64, VideoInfo>> extract_videos(
        QSqlQuery videos_query, QSqlDatabase& db, Database& this_db);

    friend QList<VideoFormat> extract_formats(QSqlQuery formats_query,
                                              Database& db);

    static constexpr auto kDatabaseFileName = "history.db";

    explicit Database(const QString& file_name = kDatabaseFileName,
                      QString connection_name = kDatabaseFileName,
                      QObject* parent = nullptr);

    bool valid_;
    const QString connection_name_;
};

}  // namespace yd_gui
