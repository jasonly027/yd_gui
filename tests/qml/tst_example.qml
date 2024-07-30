import QtQuick
import QtTest
import YdGui

TestCase {
    function test_math() {
        compare(2 + 2, 4, "2 + 2 = 4");
    }

    name: "MathTests"
}
