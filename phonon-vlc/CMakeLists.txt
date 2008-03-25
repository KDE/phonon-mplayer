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

target_link_libraries(phonon_vlc
	${QT_QTCORE_LIBRARY}
	${QT_LIBRARY_DIR}/phonond4.lib
	${QT_QTGUI_LIBRARY}
)

install(TARGETS phonon_vlc
	RUNTIME DESTINATION "C:/Qt/4.4.0/plugins/phonon_backend"
	LIBRARY DESTINATION "C:/Qt/4.4.0/plugins/phonon_backend"
	ARCHIVE DESTINATION "C:/Qt/4.4.0/plugins/phonon_backend"
)
