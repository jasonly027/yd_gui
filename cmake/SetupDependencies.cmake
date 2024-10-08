include(FetchContent)
include(CPM)

if(ENABLE_TESTING)
    CPMAddPackage(
        NAME googletest
        GITHUB_REPOSITORY google/googletest
        VERSION 1.14.0
        OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON CACHE BOOL \"\" FORCE"
    )
endif()

CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.11.3
    OPTIONS
    "JSON_BuildTests OFF"
    "JSON_ImplicitConversions OFF"
)
find_package(Qt6 6.6...6.7.2 REQUIRED COMPONENTS Quick Gui Sql Test QuickTest Svg)
add_compile_definitions(QUICK_TEST_SOURCE_DIR="${PROJECT_SOURCE_DIR}/tests/qml")
qt_standard_project_setup(REQUIRES 6.5)
