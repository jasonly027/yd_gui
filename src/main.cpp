#include "application.h"

int main(int argc, char* argv[]) {
    yd_gui::Application app(argc, argv);

    return app.exec();
}
