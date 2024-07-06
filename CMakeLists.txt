cmake_minimum_required(VERSION 3.19)

project(yd_gui
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS OFF)

# Setup QT

find_package(Qt6 6.7 REQUIRED COMPONENTS Quick)
qt_standard_project_setup(REQUIRES 6.5)

# Create executable

qt_add_executable(yd_gui src/main.cpp)

qt_add_qml_module(yd_gui
    URI yd_gui
    QML_FILES
    qml/Main.qml
)

target_link_libraries(yd_gui PRIVATE Qt6::Quick)

add_subdirectory(qml/yd_gui)

if(APPLE)
    set_target_properties(yd_gui PROPERTIES MACOSX_BUNDLE TRUE)
endif()

# Testing

enable_testing()
add_subdirectory(tests)