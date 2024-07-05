qt_add_library(TestModule STATIC)
qt_add_qml_module(TestModule
    URI "TestModule"
    QML_FILES
    TestModule/Test.qml
)
target_link_libraries(yd_gui PRIVATE TestModuleplugin)
