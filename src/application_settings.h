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

   public:
    static ApplicationSettings& get();

    Q_INVOKABLE QUrl downloadDirValidated();

    QUrl downloadDir() const;
    QString downloadDirStr() const;
    QString theme() const;
    bool downloadThumbnail() const;

   signals:
    void downloadDirChanged();
    void themeChanged();
    void errorPushed(QString error);
    void downloadThumbnailChanged();

   public slots:
    void setDownloadDir(const QUrl& dir);
    void setTheme(const QString& theme);
    void setDownloadThumbnail(bool);

   private:
    explicit ApplicationSettings(QObject* parent = nullptr);
};

}  // namespace yd_gui
