project(phonon-vlc)

find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(phonon_vlc_SRCS
	backend.cpp
	mediaobject.cpp
	videowidget.cpp
	audiooutput.cpp
	vlcloader.cpp
	vlcmediaobject.cpp
	vlc_symbols.cpp
	libloader.cpp
)

qt4_wrap_cpp(phonon_vlc_SRCS
	backend.h
	mediaobject.h
	videowidget.h
	audiooutput.h
	vlcloader.h
	vlcmediaobject.h
	vlc_symbols.h
	libloader.h
)

add_library(phonon_vlc MODULE ${phonon_vlc_SRCS})

if (UNIX)
	set(QT_PHONON_LIBRARY_DEBUG
		${QT_LIBRARY_DIR}/libphonon.so.4.1.0.debug
	)
endif (UNIX)

if (WIN32)
	set(QT_PHONON_LIBRARY_DEBUG
		${QT_LIBRARY_DIR}/phonond4.lib
	)
endif (WIN32)

target_link_libraries(phonon_vlc
	${QT_QTCORE_LIBRARY_DEBUG}
	${QT_PHONON_LIBRARY_DEBUG}
	${QT_QTGUI_LIBRARY_DEBUG}
)

install(TARGETS phonon_vlc
	RUNTIME DESTINATION ${QT_PLUGINS_DIR}/phonon_backend
	LIBRARY DESTINATION ${QT_PLUGINS_DIR}/phonon_backend
	ARCHIVE DESTINATION ${QT_PLUGINS_DIR}/phonon_backend
)
