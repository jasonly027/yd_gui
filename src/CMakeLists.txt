qt_add_library(${PROJECT_NAME}_lib STATIC)

set(qml_singletons
    qml/Theme.qml
    qml/Constants.qml
)

set_source_files_properties(${qml_singletons}
    PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(${PROJECT_NAME}_lib
    URI "YdGui"

    OUTPUT_DIRECTORY "YdGui"

    SOURCES
    video.cpp video.h
    downloader.cpp downloader.h
    video_list_model.cpp video_list_model.h
    video_list_model_sorted_proxy.cpp video_list_model_sorted_proxy.h
    database.cpp database.h
    application.cpp application.h
    application_settings.cpp application_settings.h

    QML_FILES
    qml/InputUrl.qml
    qml/Theme.qml
    qml/RaisedButton.qml
    qml/SettingsDrawer.qml
    qml/SettingsContent.qml
    qml/Constants.qml
    qml/InputDownloadDir.qml
    qml/DropShadow.qml
    qml/Header.qml
    qml/ThemePicker.qml
    qml/ClearHistoryDialog.qml
    qml/VideosListContent.qml
    qml/VideoDelegate.qml
    qml/ThemeDelegate.qml
    qml/TitleAudioText.qml
    qml/TextTrailingIcon.qml
    qml/LinkText.qml
    qml/FormatComboBox.qml
    qml/FormatDelegate.qml
    qml/ProgressBar.qml
    qml/Console.qml
    qml/RemoveButton.qml
    qml/DownloadButton.qml
    qml/LicenseDialog.qml
    qml/InputYtdlp.qml
    qml/InputFfmpeg.qml
)

qt_add_resources(${PROJECT_NAME}_lib "fonts"
    PREFIX "/"
    FILES
    fonts/NotoSans-VariableFont.ttf
    fonts/NotoSans-Italic-VariableFont.ttf
    fonts/Typicons.ttf
)

qt_add_resources(${PROJECT_NAME}_lib "images"
    PREFIX "/"
    FILES
    images/darkPurple.svg
    images/lightPurple.svg
    images/garden.svg
    images/sea.svg
)

qt_add_resources(${PROJECT_NAME}_lib "legal"
    PREFIX "/"
    FILES
    legal/Qt_License_LGPL3.txt
)

target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}_lib PRIVATE nlohmann_json::nlohmann_json Qt6::Gui Qt6::Sql)

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
