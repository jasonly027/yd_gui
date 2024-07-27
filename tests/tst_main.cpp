#include <gtest/gtest.h>
#include <qcoreapplication.h>
#include <qtimer.h>

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    QTimer::singleShot(0, [&] {
        testing::InitGoogleTest(&argc, argv);
        auto test_result = RUN_ALL_TESTS();
        QCoreApplication::exit(test_result);
    });
    return QCoreApplication::exec();
}
