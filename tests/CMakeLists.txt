include(GoogleTest)

add_executable("${PROJECT_NAME}_tests"
    downloader.test.cpp
)
target_link_libraries("${PROJECT_NAME}_tests"
    PRIVATE
    GTest::gtest_main
    Qt6::Quick
    Qt6::Test
    nlohmann_json::nlohmann_json
    "${PROJECT_NAME}_lib"
)

gtest_discover_tests("${PROJECT_NAME}_tests")

target_compile_definitions("${PROJECT_NAME}_tests"
    PRIVATE
    YD_GUI_TEST_DATA_PATH="${PROJECT_SOURCE_DIR}/tests/data/"
)
