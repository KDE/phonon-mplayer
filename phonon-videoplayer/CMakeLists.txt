project(videoplayer)

find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDE_DIR})

set(videoplayer_SRCS
	main.cpp
	MainWindow.cpp
)

qt4_add_resources(videoplayer_SRCS
	videoplayer.qrc
)

qt4_wrap_cpp(videoplayer_SRCS
	MainWindow.h
)

qt4_wrap_ui(videoplayer_SRCS
	MainWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(
	-DUNICODE
	-DWIN32
	-DQT_LARGEFILE_SUPPORT
	-DQT_DLL
	#-DQT_NO_DEBUG
	-DQT_PHONON_LIB
	-DQT_GUI_LIB
	-DQT_CORE_LIB
	-DQT_THREAD_SUPPORT
)

add_executable(videoplayer ${videoplayer_SRCS})

target_link_libraries(videoplayer
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_LIBRARY_DIR}/phonond4.lib
)
