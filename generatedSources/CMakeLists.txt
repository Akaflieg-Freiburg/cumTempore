#
# Regenerate the committed icon files from the SVG sources in metadata/.
# Needs rsvg-convert and Python with Pillow. Run with:
#
#   cmake --build --preset linux-debug --target generatedSources
#

find_package(Python3 COMPONENTS Interpreter)

add_custom_target(generatedSources
    COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/generateIcons.py ${PROJECT_SOURCE_DIR} ${APP_ID}
    COMMENT "Generating platform icons"
    VERBATIM
)
