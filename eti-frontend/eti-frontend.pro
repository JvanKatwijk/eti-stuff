######################################################################
# Automatically generated by qmake (2.01a) Tue Oct 6 19:48:14 2009
# but modified by me to accomodate for the includes for qwt, hamlib and
# portaudio
######################################################################

TEMPLATE	= app
TARGET		= eti-frontend
QT		+= widgets 
CONFIG		+= console
#QMAKE_CFLAGS	+=  -flto -ffast-math 
#QMAKE_CXXFLAGS	+=  -flto -ffast-math 
#QMAKE_LFLAGS	+=  -flto 
QMAKE_CFLAGS	+=  -g
QMAKE_CXXFLAGS	+=  -g
QMAKE_LFLAGS	+=  -g
DEPENDPATH += . \
	      ./src \
	      ./includes \
	      ./src/ofdm \
	      ./src/backend \
	      ./src/various \
	      ./src/input \
	      ./src/input/rawfiles \
	      ./src/input/wavfiles \
	      ./includes/ofdm \
	      ./includes/backend \
	      ./includes/output \
	      ./includes/various 

INCLUDEPATH += . \
	      ./ \
	      ./src \
	      ./includes \
	      ./includes/ofdm \
	      ./includes/backend \
	      ./includes/output \
	      ./includes/various \
	      ./src/input \
	      ./src/input/rawfiles \
	      ./src/input/wavfiles 

# Input
HEADERS		+= ./gui.h \
	   ./includes/dab-constants.h \
	   ./includes/ofdm/ofdm-processor.h \
	   ./includes/ofdm/phasereference.h \
	   ./includes/ofdm/phasetable.h \
	   ./includes/ofdm/freq-interleaver.h \
	   ./includes/backend/charsets.h \
	   ./includes/backend/viterbi.h \
	   ./includes/backend/fic-handler.h \
	   ./includes/backend/fib-processor.h  \
#	   ./includes/backend/dab_tables.h \
	   ./includes/backend/eti-generator.h \
	   ./includes/backend/protection.h \
	   ./includes/backend/eep-protection.h \
	   ./includes/backend/uep-protection.h \
           ./includes/various/fft.h \
	   ./includes/various/ringbuffer.h \
	   ./includes/various/Xtan2.h \
	   ./src/input/virtual-input.h \
	   ./src/input/rawfiles/rawfiles.h \
	   ./src/input/wavfiles/wavfiles.h 

FORMS +=	./src/input/filereader-widget.ui 

SOURCES += ./main.cpp \
	   ./gui.cpp \
	   ./src/ofdm/ofdm-processor.cpp \
	   ./src/ofdm/phasereference.cpp \
	   ./src/ofdm/phasetable.cpp \
	   ./src/ofdm/freq-interleaver.cpp \
	   ./src/backend/charsets.cpp \
	   ./src/backend/viterbi.cpp \
	   ./src/backend/fic-handler.cpp \
	   ./src/backend/eti-generator.cpp \
	   ./src/backend/protection.cpp \
	   ./src/backend/eep-protection.cpp \
	   ./src/backend/uep-protection.cpp \
	   ./src/backend/fib-processor.cpp  \
	   ./src/backend/protTables.cpp \
           ./src/various/fft.cpp \
	   ./src/various/Xtan2.cpp \
	   ./src/input/virtual-input.cpp \
	   ./src/input/rawfiles/rawfiles.cpp \
	   ./src/input/wavfiles/wavfiles.cpp 
#
FORMS 		+= ./gui.ui 
#	for unix systems this is about it. Adapt when needed for naming
#	and locating libraries. If you do not need a device as
#	listed, just comment the line out.
#
#	Note
unix {
CONFIG		+= dabstick
CONFIG		+= sdrplay-exp
CONFIG		+= rtl_tcp
CONFIG		+= airspy
DESTDIR		= ./linux-bin
INCLUDEPATH	+= /usr/local/include
LIBS		+= -lfftw3f  -lusb-1.0 -ldl -lsndfile  #
LIBS		+= -lz
}
#
# an attempt to have it run under W32
win32 {
DESTDIR	= ../../windows-bin
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
LIBS		+= -L/usr/i686-w64-mingw32/sys-root/mingw/lib
LIBS		+= -lfftw3f
LIBS		+= -lole32
LIBS		+= -lwinpthread
LIBS		+= -lwinmm
LIBS 		+= -lstdc++
LIBS		+= -lws2_32
LIBS		+= -lsndfile
LIBS		+= -lfaad
LIBS		+= -lusb-1.0
LIBS		+= -lz
DEFINES		+= MSC_DATA__		# use at your own risk
CONFIG		+= extio
CONFIG		+= airspy
CONFIG		+= rtl_tcp
CONFIG		+= dabstick
CONFIG		+= sdrplay-exp
#CONFIG		+= tcp-streamer
FORMS 		+= ./gui.ui 
}


#######################################

#	devices
#
#	dabstick
dabstick_osmo {
	DEFINES		+= HAVE_DABSTICK
	INCLUDEPATH	+= /home/jan/rtl-sdr/include
	INCLUDEPATH	+= ./src/input/dabstick-osmo
	HEADERS		+= ./src/input/dabstick-osmo/dabstick.h \
	                   ./src/input/dabstick-osmo/dongleselect.h
	SOURCES		+= ./src/input/dabstick-osmo/dabstick.cpp \
	                   ./src/input/dabstick-osmo/dongleselect.cpp
	FORMS		+= ./src/input/dabstick-osmo/dabstick-widget-osmo.ui
}
#
#	the SDRplay
#
sdrplay {
	DEFINES		+= HAVE_SDRPLAY
	INCLUDEPATH	+= ./src/input/sdrplay
	HEADERS		+= ./src/input/sdrplay/sdrplay.h \
	                   ./src/input/sdrplay/sdrplay-loader.h \
	                   ./src/input/sdrplay/sdrplay-worker.h 
	SOURCES		+= ./src/input/sdrplay/sdrplay.cpp \
	                   ./src/input/sdrplay/sdrplay-loader.cpp \
	                   ./src/input/sdrplay/sdrplay-worker.cpp 
	FORMS		+= ./src/input/sdrplay/sdrplay-widget.ui
}
#
#
sdrplay-exp {
	DEFINES		+= HAVE_SDRPLAY
	INCLUDEPATH	+= ./src/input/sdrplay-exp
	HEADERS		+= ./src/input/sdrplay-exp/sdrplay.h 
	SOURCES		+= ./src/input/sdrplay-exp/sdrplay.cpp 
	FORMS		+= ./src/input/sdrplay-exp/sdrplay-widget.ui
}
#
#
# airspy support
#
airspy {
	DEFINES		+= HAVE_AIRSPY
	INCLUDEPATH	+= ./src/input/airspy \
	                    /usr/local/include/libairspy
	HEADERS		+= ./src/input/airspy/airspy-handler.h \
	                    /usr/local/include/libairspy/airspy.h
	SOURCES		+= ./src/input/airspy/airspy-handler.cpp 
	FORMS		+= ./src/input/airspy/airspy-widget.ui
}

# airspy support (still struggling with the sliders)
#
airspy-exp {
	DEFINES		+= HAVE_AIRSPY
	INCLUDEPATH	+= ./src/input/airspy-exp \
	                   /usr/local/include/libairspy
	HEADERS		+= ./src/input/airspy-exp/airspy-handler.h \
	                   /usr/local/include/libairspy/airspy.h
	SOURCES		+= ./src/input/airspy-exp/airspy-handler.cpp 
	FORMS		+= ./src/input/airspy-exp/airspy-widget.ui
}

tcp-streamer	{
	DEFINES		+= TCP_STREAMER
	QT		+= network
	HEADERS		+= ./includes/output/tcp-streamer.h
	SOURCES		+= ./src/output/tcp-streamer.cpp
}

#
rtl_tcp {
	DEFINES		+= HAVE_RTL_TCP
	QT		+= network
	INCLUDEPATH	+= ./src/input/rtl_tcp
	HEADERS		+= ./src/input/rtl_tcp/rtl_tcp_client.h
	SOURCES		+= ./src/input/rtl_tcp/rtl_tcp_client.cpp
	FORMS		+= ./src/input/rtl_tcp/rtl_tcp-widget.ui
}


##for FreeBSD use the third set (Thanks to Juergen Lock)
#unix {
#DESTDIR	= ../../linux-bin
#	QMAKE_CXXFLAGS += -D__FREEBSD__
#	INCLUDEPATH += ${LOCALBASE}/include/qwt
#	INCLUDEPATH += ${LOCALBASE}/include/portaudio2
#	INCLUDEPATH += ${LOCALBASE}/include/ffmpeg1
#	INCLUDEPATH += ${LOCALBASE}/include
#	INCLUDEPATH += /usr/include/
#	QMAKE_LIBDIR = ${LOCALBASE}/lib/portaudio2
#	QMAKE_LIBDIR += ${LOCALBASE}/lib/ffmpeg${FFMPEG_SUFFIX}
#	QMAKE_LIBDIR += ${LOCALBASE}/lib
#	LIBS+=  -lqwt -lusb -lrt -lportaudio  -lsndfile -lfftw3  -lrtlsdr -lz
#	LIBS += -lfaad
#}

