import QtQuick
import QtTest
import YdGui

TestCase {
    function test_fail() {
        compare(2 + 2, 5, "2 + 2 = 5");
    }
    function test_math() {
        compare(2 + 2, 4, "2 + 2 = 4");
    }

    name: "MathTests"
}
