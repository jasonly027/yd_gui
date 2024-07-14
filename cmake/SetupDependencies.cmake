include(FetchContent)
include(CPM)

CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    VERSION 1.14.0
    OPTIONS
    "INSTALL_GTEST OFF"
    "gtest_force_shared_crt ON CACHE BOOL \"\" FORCE"
)
CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.11.3
    OPTIONS
    "JSON_BuildTests OFF"
    "JSON_ImplicitConversions OFF"
)
find_package(SQLite3 REQUIRED)
CPMAddPackage(
    NAME sqlite_orm
    GITHUB_REPOSITORY fnc12/sqlite_orm
    VERSION 1.8.2
)
find_package(Qt6 6.7 REQUIRED COMPONENTS Quick Test)
qt_standard_project_setup(REQUIRES 6.5)
