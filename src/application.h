#pragma once

#include <QtGui/qguiapplication.h>
#include <qqmlapplicationengine.h>

#include <memory>

namespace yd_gui {
class Application {
   public:
    Application(int& argc, char** argv);

    int exec();

   private:
    std::unique_ptr<QGuiApplication> application_;
    std::unique_ptr<QQmlApplicationEngine> engine_;
};
}  // namespace yd_gui
