#
# Linux: desktop entry, AppStream metadata and icon
#

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include(GNUInstallDirs)

    # Date of the latest commit, used as release date in the AppStream file
    find_package(Git QUIET)
    set(GIT_DATE "")
    if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} log -1 --format=%as
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_DATE
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif ()

    configure_file(${APP_ID}.appdata.xml ${APP_ID}.metainfo.xml)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${APP_ID}.desktop
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${APP_ID}.metainfo.xml
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)
    install(FILES ${APP_ID}.svg
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)
endif ()
