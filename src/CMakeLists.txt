qt_add_library(${PROJECT_NAME}_lib STATIC)

qt_add_qml_module(${PROJECT_NAME}_lib
    URI "YdGui"

    OUTPUT_DIRECTORY "YdGui"

    SOURCES
    video.cpp video.h
    downloader.cpp downloader.h
    video_list_model.cpp video_list_model.h
    database.cpp database.h
    application.cpp application.h
    application_settings.cpp application_settings.h
)

target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}_lib PRIVATE nlohmann_json::nlohmann_json Qt6::Sql)

target_compile_definitions(${PROJECT_NAME}_lib
    PRIVATE
    APP_VERSION="${PROJECT_VERSION}"
)

if(MSVC)
    target_compile_options(${PROJECT_NAME}_lib PRIVATE /W4 /EHsc /permissive- /analyze)
else()
    target_compile_options(${PROJECT_NAME}_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Building in Release mode: Removing Qt Debug comments...")
    target_compile_definitions(${PROJECT_NAME}_lib PRIVATE
        QT_NO_DEBUG_OUTPUT
        QT_NO_INFO_OUTPUT
        QT_NO_WARNING_OUTPUT
    )
endif()
