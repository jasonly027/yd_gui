install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT add_shortcut
)

# Add licenses
set(LICENSE_FILES
    ${PROJECT_SOURCE_DIR}/docs/legal/Qt_License_LGPL3.txt
    ${PROJECT_SOURCE_DIR}/docs/legal/nlohmann_json_MIT.txt
    ${PROJECT_SOURCE_DIR}/docs/legal/Noto_Sans_LICENSE.txt
    ${PROJECT_SOURCE_DIR}/docs/legal/Typicons_LICENSE.md
)
install(FILES ${LICENSE_FILES}
    DESTINATION
    share/licenses
)

install(FILES
    ${PROJECT_SOURCE_DIR}/docs/replacing_qt_lgpl3_modules.txt
    DESTINATION share
)

qt_generate_deploy_qml_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
)
install(SCRIPT ${deploy_script})

# Create installer
set(CPACK_GENERATOR "IFW")

if(NOT DEFINED CPACK_IFW_ROOT)
    message(FATAL_ERROR "CPACK_IFW_ROOT must be set")
endif()

set(CPACK_IFW_PACKAGE_TITLE "${PROJECT_NAME} Installer")
set(CPACK_IFW_PACKAGE_PUBLISHER "jasonly027")
set(CPACK_IFW_PACKAGE_WIZARD_STYLE "Classic")
set(CPACK_IFW_PACKAGE_WIZARD_DEFAULT_WIDTH 600)
set(CPACK_IFW_PACKAGE_WIZARD_DEFAULT_HEIGHT 450)
set(CPACK_IFW_PACKAGE_WIZARD_SHOW_PAGE_LIST ON)
set(CPACK_IFW_PACKAGE_RUN_PROGRAM "@TargetDir@/bin/${PROJECT_NAME}")
set(CPACK_IFW_PACKAGE_MAINTENANCE_TOOL_NAME "Uninstall")
set(CPACK_IFW_PACKAGE_CONTROL_SCRIPT ${PROJECT_SOURCE_DIR}/cmake/controller.qs)

include(CPackIFW)
cpack_add_component(add_shortcut)
cpack_ifw_configure_component(add_shortcut
    VIRTUAL
    FORCED_INSTALLATION
    SCRIPT ${PROJECT_SOURCE_DIR}/cmake/add_shortcut.qs
)

include(CPack)
