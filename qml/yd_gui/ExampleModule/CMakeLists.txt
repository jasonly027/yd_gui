qt_add_library(ExampleModule STATIC)
qt_add_qml_module(ExampleModule
    URI "${PROJECT_NAME}.ExampleModule"
    QML_FILES
    Example.qml
)
target_link_libraries(yd_gui PRIVATE ExampleModuleplugin)
