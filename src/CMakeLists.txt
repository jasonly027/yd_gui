qt_add_library("${PROJECT_NAME}_lib" STATIC)
qt_add_qml_module("${PROJECT_NAME}_lib"
    URI "YdGui"
    OUTPUT_DIRECTORY "YdGui"
    SOURCES
    video.cpp video.h
    downloader.cpp downloader.h
    video_list_model.cpp video_list_model.h
    database.cpp database.h
)
target_include_directories("${PROJECT_NAME}_lib" PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_link_libraries("${PROJECT_NAME}_lib" PRIVATE nlohmann_json::nlohmann_json Qt6::Sql)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Building in Release mode: Removing Qt Debug comments...")
    target_compile_definitions("${PROJECT_NAME}_lib" PRIVATE
        QT_NO_DEBUG_OUTPUT
        QT_NO_INFO_OUTPUT
        QT_NO_WARNING_OUTPUT
    )
endif()
