install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Add legal
install(FILES
    ${PROJECT_SOURCE_DIR}/docs/replacing_qt_lgpl3_modules.txt
    DESTINATION share
)
set(LICENSE_FILES
    ${PROJECT_SOURCE_DIR}/docs/legal/Qt_License_LGPL3.txt
    ${PROJECT_SOURCE_DIR}/docs/legal/nlohmann_json_MIT.txt
    ${PROJECT_SOURCE_DIR}/docs/legal/Noto_Sans_OFL.txt
    ${PROJECT_SOURCE_DIR}/docs/legal/Typicons_OFL.md
)
install(FILES ${LICENSE_FILES}
    DESTINATION
    share/licenses
)

# Add Qt dependencies
qt_generate_deploy_qml_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
)
install(SCRIPT ${deploy_script})

# Create installer
if(NOT DEFINED CPACK_IFW_ROOT)
    message(WARNING "CPACK_IFW_ROOT was not set. Skipping creating installer")
    return()
endif()

set(CPACK_GENERATOR "IFW")
include(CPackIFW)

set(CPACK_IFW_PACKAGE_TITLE "${PROJECT_NAME} Installer")
set(CPACK_IFW_PACKAGE_PUBLISHER "jasonly027")
set(CPACK_IFW_PACKAGE_WIZARD_STYLE "Classic")
set(CPACK_IFW_PACKAGE_WIZARD_DEFAULT_WIDTH 600)
set(CPACK_IFW_PACKAGE_WIZARD_DEFAULT_HEIGHT 450)
set(CPACK_IFW_PACKAGE_WIZARD_SHOW_PAGE_LIST ON)
if(APPLE)
    set(ifw_run_program "@TargetDir@/${PROJECT_NAME}.app")
else()
    set(ifw_run_program "@TargetDir@/bin/${PROJECT_NAME}")
endif()
set(CPACK_IFW_PACKAGE_RUN_PROGRAM ${ifw_run_program})
set(CPACK_IFW_PACKAGE_MAINTENANCE_TOOL_NAME "Uninstall")
set(CPACK_IFW_PACKAGE_CONTROL_SCRIPT ${PROJECT_SOURCE_DIR}/cmake/controller.qs)

cpack_add_component(add_shortcut)
cpack_ifw_configure_component(add_shortcut
    VIRTUAL
    FORCED_INSTALLATION
    SCRIPT ${PROJECT_SOURCE_DIR}/cmake/add_shortcut.qs
)

include(CPack)
