#pragma once

#include <qsettings.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qurl.h>

class QObject;

namespace yd_gui {
class ApplicationSettings : public QSettings {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(QUrl downloadDir READ downloadDir WRITE setDownloadDir NOTIFY
                   downloadDirChanged)
    Q_PROPERTY(
        QString downloadDirStr READ downloadDirStr NOTIFY downloadDirChanged)
    Q_PROPERTY(bool downloadThumbnail READ downloadThumbnail WRITE
                   setDownloadThumbnail NOTIFY downloadThumbnailChanged)
    Q_PROPERTY(QUrl ytdlp READ ytdlp WRITE setYtdlp NOTIFY ytdlpChanged)
    Q_PROPERTY(QString ytdlpStr READ ytdlpStr NOTIFY ytdlpChanged)

   public:
    static ApplicationSettings& get();

    Q_INVOKABLE QUrl downloadDirValidated();

    QUrl downloadDir() const;
    QString downloadDirStr() const;
    QString theme() const;
    bool downloadThumbnail() const;
    QUrl ytdlp() const;
    QString ytdlpStr() const;

   signals:
    void downloadDirChanged();
    void themeChanged();
    void errorPushed(QString error);
    void downloadThumbnailChanged();
    void ytdlpChanged();

   public slots:
    void setDownloadDir(const QUrl& dir);
    void setTheme(const QString& theme);
    void setDownloadThumbnail(bool);
    void setYtdlp(const QUrl& ytdlp);

   private:
    explicit ApplicationSettings(QObject* parent = nullptr);
};

}  // namespace yd_gui
