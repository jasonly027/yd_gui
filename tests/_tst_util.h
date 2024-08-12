#pragma once

#include <gtest/gtest.h>
#include <qsignalspy.h>
#include <qtypes.h>
#include <qvariant.h>
#include <video.h>

#include <QString>
#include <chrono>
#include <ostream>

#include "gmock/gmock.h"

using  // NOLINT(google-global-names-in-headers)
    testing::PrintToString,
    testing::Pointwise, testing::Eq, testing::WithParamInterface, testing::Test,
    testing::IsSubsetOf, testing::ContainerEq;

void PrintTo(const QString& str, std::ostream* os);

namespace yd_gui {
void PrintTo(DownloadState state, std::ostream* os);
}

template <typename T>
void PrintTo(const QList<T>& list, std::ostream* os) {
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        if (it == list.cbegin())
            *os << "[" << testing::PrintToString(*it);
        else
            *os << ", " << testing::PrintToString(*it);
    }
    *os << "]";
}

namespace tst_util {
std::string test_name();

QString read_file(const QString& path);

void EXPECT_INFOS_EQ_EXCLUDING_FORMATS(const yd_gui::VideoInfo& lhs,
                                       const yd_gui::VideoInfo& rhs);

testing::AssertionResult wait_for_n_signals(
    QSignalSpy& spy, qsizetype n,
    std::chrono::seconds timeout = std::chrono::seconds{10});

template <typename T>
T try_convert(QVariant var) {
    if (!var.canConvert<T>())
        throw std::runtime_error(std::string("Cannot convert to ") +
                                 typeid(T).name());

    return std::move(var).value<T>();
}

MATCHER_P2(IsBetween, min, max,
           (std::string(negation ? "Isn't" : "Is") + " between " +
            PrintToString(min) + " and " + PrintToString(max))) {
    return min <= arg && arg <= max;
}

}  // namespace tst_util
