project(phonon)

cmake_minimum_required(VERSION 2.8)

set(QT_MIN_VERSION 4.6.0)
find_package(Qt4 REQUIRED)



set(PHONON_LIBS
	${QT_PHONON_LIBRARY}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Includes all wanted definitions for Qt
include(${QT_USE_FILE})

# Remove all Qt modules from include path since there are not needed
include(cmake/ReduceQtIncludeDirs.cmake)
reduce_qt_include_dirs()

set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )

_set_fancy(EXEC_INSTALL_PREFIX         "${CMAKE_INSTALL_PREFIX}"                   "Base directory for executables and libraries")
_set_fancy(SHARE_INSTALL_PREFIX        "${CMAKE_INSTALL_PREFIX}/share"             "Base directory for files which go to share/")
_set_fancy(BIN_INSTALL_DIR             "${EXEC_INSTALL_PREFIX}/bin"                "The install dir for executables (default ${EXEC_INSTALL_PREFIX}/bin)")
_set_fancy(LIB_INSTALL_DIR             "${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX}"   "The subdirectory relative to the install prefix where libraries will be installed (default is ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX})")
_set_fancy(INCLUDE_INSTALL_DIR         "${CMAKE_INSTALL_PREFIX}/include"           "The subdirectory to the header prefix")
_set_fancy(PLUGIN_INSTALL_DIR          "${LIB_INSTALL_DIR}/kde4"                   "The subdirectory relative to the install prefix where plugins will be installed (default is ${LIB_INSTALL_DIR}/kde4)")
_set_fancy(ICON_INSTALL_DIR            "${SHARE_INSTALL_PREFIX}/icons"             "The icon install dir (default ${SHARE_INSTALL_PREFIX}/share/icons/)")
_set_fancy(SERVICES_INSTALL_DIR        "${SHARE_INSTALL_PREFIX}/kde4/services"     "The install dir for service (desktop, protocol, ...) files")
_set_fancy(DBUS_INTERFACES_INSTALL_DIR "${SHARE_INSTALL_PREFIX}/dbus-1/interfaces" "The dbus interfaces install dir (default: ${SHARE_INSTALL_PREFIX}/dbus-1/interfaces)")
_set_fancy(DBUS_SERVICES_INSTALL_DIR   "${SHARE_INSTALL_PREFIX}/dbus-1/services"   "The dbus services install dir (default: ${SHARE_INSTALL_PREFIX}/dbus-1/services)")


add_subdirectory(mplayer)
