#include "_tst_util.h"

#include <qvariant.h>

#include <QFile>
#include <QIODevice>
#include <QString>
#include <QTextStream>
#include <ostream>
#include <stdexcept>

#include "gtest/gtest.h"

using std::string, testing::AssertionResult, testing::AssertionSuccess,
    testing::AssertionFailure, std::chrono::seconds;

void PrintTo(const QString& str, std::ostream* os) { *os << str.toStdString(); }

namespace yd_gui {
void PrintTo(DownloadState state, std::ostream* os) {
    *os << QVariant::fromValue(state).toString().toStdString();
}

}  // namespace yd_gui

// void PrintTo(const QList<yd_gui::DownloadState>& states, std::ostream* os) {
//     for (const auto state : states) {
//         *os << QVariant::fromValue(state).toString().toStdString() << " -> ";
//     }
// }

namespace tst_util {

std::string test_name() {
    return ::testing::UnitTest::GetInstance()->current_test_info()->name();
}

QString read_file(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Failed to open file");
    }

    QTextStream in(&file);
    return in.readAll();
}

void EXPECT_INFOS_EQ_EXCLUDING_FORMATS(const yd_gui::VideoInfo& lhs,
                                       const yd_gui::VideoInfo& rhs) {
    EXPECT_EQ(lhs.video_id().toStdString(), rhs.video_id().toStdString());
    EXPECT_EQ(lhs.title().toStdString(), rhs.title().toStdString());
    EXPECT_EQ(lhs.author().toStdString(), rhs.author().toStdString());
    EXPECT_EQ(lhs.seconds(), rhs.seconds());
    EXPECT_EQ(lhs.thumbnail().toStdString(), rhs.thumbnail().toStdString());
    EXPECT_EQ(lhs.audio_available(), rhs.audio_available());
}

AssertionResult wait_for_n_signals(QSignalSpy& spy, qsizetype n,
                                   seconds timeout) {
    while (spy.wait(timeout) && spy.count() < n);

    if (spy.count() != n) {
        return AssertionFailure()
               << "Expected " << n
               << " signals but timed out and received only " << spy.count();
    }

    return AssertionSuccess();
}

}  // namespace tst_util
