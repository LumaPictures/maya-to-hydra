set(SRC
    defaultLightDelegate.cpp
    plugin.cpp
    pluginDebugCodes.cpp
    renderGlobals.cpp
    renderOverride.cpp
    shadingModeExporter.cpp
    tokens.cpp
    usdPreviewSurface.cpp
    utils.cpp
    viewCommand.cpp)

add_library(mtoh SHARED ${SRC})
target_link_libraries(mtoh PRIVATE hdmaya usdShade)
target_include_directories(mtoh PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
set_target_properties(mtoh PROPERTIES PREFIX "")
set_target_properties(mtoh PROPERTIES INSTALL_RPATH_USE_LINK_PATH ON)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(mtoh PROPERTIES INSTALL_RPATH "@loader_path/../lib")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set_target_properties(mtoh PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
endif ()
maya_set_plugin_properties(mtoh)

# M3dView needs this (technically, MNativeWindowHdl.h)
# private atm because M3dView is only used in .cpp right now
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # gcc will NOT set this automatically
    target_compile_definitions(mtoh PRIVATE "LINUX")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Not sure if msvcc sets this automatically, but won't hurt to redefine
    target_compile_definitions(mtoh PRIVATE "_WIN32")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # Not sure if osx sets this automatically, but won't hurt to redefine
    target_compile_definitions(mtoh PRIVATE "OSMac_")
endif (CMAKE_SYSTEM_NAME STREQUAL "Linux")

install(
    TARGETS mtoh
    DESTINATION plug-ins)

#install(
#    DIRECTORY scripts
#    DESTINATION .
#)

if (INSTALL_MAYA_MOD)
    install(CODE
    "file(WRITE \"${CMAKE_INSTALL_PREFIX}/mtoh.mod\"
\"+ mtoh any ${CMAKE_INSTALL_PREFIX}
MAYA_SCRIPT_PATH +:= scripts
PYTHONPATH +:= scripts
\")")
endif ()