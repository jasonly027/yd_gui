cmake_minimum_required(VERSION 3.19)

project(yd_gui
    VERSION 1.0.0
    LANGUAGES CXX
)

option(ENABLE_TESTING "" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

# Setup Dependencies
include(SetupDependencies)

# Create executable
qt_add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE src/main.cpp)
qt_add_qml_module(${PROJECT_NAME}
    URI ${PROJECT_NAME}
    QML_FILES
    src/Main.qml
)

# Add sources
add_subdirectory(src)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Quick
    ${PROJECT_NAME}_libplugin
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Testing
if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install
include(SetupInstall)
