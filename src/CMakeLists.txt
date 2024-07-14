qt_add_library("${PROJECT_NAME}_lib" STATIC)
qt_add_qml_module("${PROJECT_NAME}_lib"
    URI "YdGui"
    OUTPUT_DIRECTORY "YdGui"
    SOURCES
    video.cpp video.h
    downloader.cpp downloader.h
)
target_include_directories("${PROJECT_NAME}_lib" PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_link_libraries("${PROJECT_NAME}_lib" PRIVATE nlohmann_json::nlohmann_json)
