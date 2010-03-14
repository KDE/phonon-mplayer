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


add_subdirectory(mplayer)
