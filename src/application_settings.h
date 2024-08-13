#pragma once

#include <qobject.h>
#include <qsettings.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace yd_gui {
class ApplicationSettings : public QSettings {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)

   public:
    static ApplicationSettings& get();

    Q_INVOKABLE QString downloadDir();

    Q_INVOKABLE void setDownloadDir(const QString& dir);

    QString theme() const;

   signals:
    void themeChanged();
    void errorPushed(QString error);

   public slots:
    void setTheme(const QString& theme);

   private:
    explicit ApplicationSettings(QObject* parent = nullptr);
};

}  // namespace yd_gui
