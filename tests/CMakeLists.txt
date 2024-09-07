include(GoogleTest)

# Using GTest
add_executable("${PROJECT_NAME}_tests"
    tst_main.cpp
    _tst_util.cpp
    tst_downloader.cpp
    tst_database.cpp
    tst_video_list_model.cpp
)
target_link_libraries("${PROJECT_NAME}_tests"
    PRIVATE
    GTest::gtest
    GTest::gmock
    Qt6::Quick
    Qt6::Sql
    Qt6::Test
    nlohmann_json::nlohmann_json
    "${PROJECT_NAME}_lib"
)

gtest_discover_tests("${PROJECT_NAME}_tests")

target_compile_definitions("${PROJECT_NAME}_tests"
    PRIVATE
    YD_GUI_TEST_DATA_PATH="${PROJECT_SOURCE_DIR}/tests/data/"
)

if(MSVC)
    target_compile_options("${PROJECT_NAME}_tests" PRIVATE /W4)
else()
    target_compile_options("${PROJECT_NAME}_tests" PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
