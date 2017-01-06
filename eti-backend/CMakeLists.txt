cmake_minimum_required( VERSION 2.8.11 )
set (objectName eti-backend)
add_definitions ( -Wall)
#	modify if you want
set (CMAKE_INSTALL_PREFIX ../)

if(MINGW)
    add_definitions ( -municode)
endif()

########################################################################
# select the release build type by default to get optimization flags
########################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

### make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

######################################################################
#       R E A D T H I S
#####################################################################
#	adjust to your likings

#	add_definitions (-DMOT_BASICS__) 	# use at your own risk
#	add_definitions (-DMSC_DATA__)		# use at your own risk
#
#	If you want a tcp stream of the audio rather than
#	audio being sent to the soundcard, uncomment
#	set(TCP_STREAMER true)
########################################################################
	find_package (PkgConfig)

	find_package (Qt5Core REQUIRED)
	find_package (Qt5Widgets REQUIRED)
	find_package (Qt5Declarative REQUIRED)

	include_directories (
	      ${Qt5Network_INCLUDE_DIRS}
	)

        find_package(Portaudio)
        if (NOT PORTAUDIO_FOUND)
            message(FATAL_ERROR "please install portaudio V19")
        endif ()
        list(APPEND extraLibs ${PORTAUDIO_LIBRARIES})

        find_package(Faad)
        if (NOT FAAD_FOUND )
            message(FATAL_ERROR "please install libfaad")
        endif ()

        find_package(zlib)
	if (NOT ZLIB_FOUND)
            message(FATAL_ERROR "please install libz")
        endif ()
	list(APPEND extraLibs ${ZLIB_LIBRARY})

        find_package(LibSndFile)
        if (NOT LIBSNDFILE_FOUND)
            message(FATAL_ERROR "please install libsndfile")
        endif ()
        list(APPEND extraLibs ${LIBSNDFILE_LIBRARY})

	find_library (PTHREADS pthread)
	if (NOT(PTHREADS))
	   message (FATAL_ERROR "please install libpthread")
	else (NOT(PTHREADS))
	   set (extraLibs ${extraLibs} ${PTHREADS})
	endif (NOT(PTHREADS))
#######################################################################
#
#	Here we really start


	include_directories (
	           ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	           ${QT5Widgets_INCLUDES}
	           ${QT_QTCORE_INCLUDE_DIR}
	           ${QT_QTGUI_INCLUDE_DIR}
	           .
	           ./
	           ./includes
	           ./includes/ofdm
	           ./includes/backend
	           ./includes/backend/audio
	           ./includes/backend/data
	           ./includes/backend/data/journaline
	           ./includes/various
	           ./includes/output
	           /usr/include/
	)

	set (${objectName}_HDRS
	     ./gui.h
	     ./includes/dab-constants.h
	     ./includes/backend/eti-controller.h
	     ./includes/backend/firecode-checker.h
	     ./includes/backend/dab-processor.h
	     ./includes/backend/fib-processor.h
	     ./includes/backend/dab-virtual.h
	     ./includes/backend/charsets.h
	     ./includes/backend/galois.h
	     ./incluces/backend/reed-solomon.h
	     ./includes/backend/audio/faad-decoder.h
	     ./includes/backend/audio/mp4processor.h 
	     ./includes/backend/audio/mp2processor.h 
	     ./includes/backend/data/ip-datahandler.h 
	     ./includes/backend/data/journaline-datahandler.h 
	     ./includes/backend/data/journaline/dabdatagroupdecoder.h
	     ./includes/backend/data/journaline/crc_8_16.h 
	     ./includes/backend/data/journaline/log.h 
	     ./includes/backend/data/journaline/newssvcdec_impl.h 
	     ./includes/backend/data/journaline/Splitter.h 
	     ./includes/backend/data/journaline/dabdgdec_impl.h 
	     ./includes/backend/data/journaline/newsobject.h 
	     ./includes/backend/data/journaline/NML.h 
	     ./includes/backend/data/mot-databuilder.h 
	     ./includes/backend/data/virtual-datahandler.h 
	     ./includes/backend/data/data-processor.h 
	     ./includes/backend/data/pad-handler.h 
	     ./includes/backend/data/mot-data.h 
	     ./includes/output/fir-filters.h
	     ./includes/output/audio-base.h
	     ./includes/output/audiosink.h
	     ./includes/various/ringbuffer.h
	     ./includes/various/Xtan2.h
	)

	set (${objectName}_SRCS
	     ${${objectName}_SRCS}
	     ./main.cpp
	     ./gui.cpp
	     ./src/backend/eti-controller.cpp
	     ./src/backend/fib-processor.cpp
	     ./src/backend/firecode-checker.cpp
	     ./src/backend/dab-virtual.cpp
	     ./src/backend/dab-processor.cpp
	     ./src/backend/charsets.cpp
	     ./src/backend/dab-virtual.cpp 
	     ./src/backend/galois.cpp
	     ./src/backend/reed-solomon.cpp
	     ./src/backend/audio/mp4processor.cpp 
	     ./src/backend/audio/mp2processor.cpp 
	     ./src/backend/data/ip-datahandler.cpp 
	     ./src/backend/data/journaline-datahandler.cpp 
	     ./src/backend/data/journaline/crc_8_16.c 
	     ./src/backend/data/journaline/log.c 
	     ./src/backend/data/journaline/newssvcdec_impl.cpp 
	     ./src/backend/data/journaline/Splitter.cpp 
	     ./src/backend/data/journaline/dabdgdec_impl.c 
	     ./src/backend/data/journaline/newsobject.cpp 
	     ./src/backend/data/journaline/NML.cpp 
	     ./src/backend/data/mot-databuilder.cpp 
	     ./src/backend/data/virtual-datahandler.cpp 
	     ./src/backend/data/pad-handler.cpp 
             ./src/backend/data/data-processor.cpp 
	     ./src/backend/data/mot-data.cpp 
	     ./src/output/audio-base.cpp
	     ./src/output/audiosink.cpp
	     ./src/output/fir-filters.cpp
	     ./src/various/Xtan2.cpp
	)

	set (${objectName}_MOCS
	     ./gui.h
	     ./includes/output/audio-base.h
	     ./includes/output/audiosink.h
	     ./includes/backend/fib-processor.h
	     ./includes/backend/eti-controller.h
	     ./includes/backend/audio/faad-decoder.h
	     ./includes/backend/audio/mp2processor.h
	     ./includes/backend/audio/mp4processor.h
	     ./includes/backend/data/virtual-datahandler.h
	     ./includes/backend/data/pad-handler.h
	     ./includes/backend/data/mot-data.h
	     ./includes/backend/data/ip-datahandler.h
	     ./includes/backend/data/journaline-datahandler.h
	     ./includes/backend/data/mot-databuilder.h
             ./includes/backend/data/data-processor.h
	)

#
	if (TCP_STREAMER)
	   add_definitions (-DTCP_STREAMER)
	   find_package (Qt5Network REQUIRED)
	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./includes/output/tcp-streamer.h
	   )
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ./includes/output/tcp-streamer.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./src/output/tcp-streamer.cpp
	   )
	endif (TCP_STREAMER)

	QT5_WRAP_UI (UIS ${${objectName}_UIS}
	             ./gui.ui) 

	include_directories (
	          ${SDRPLAY_INCLUDES}
	          ${QT5Widgets_INCLUDES}
	          ${QT_QTCORE_INCLUDE_DIR}
	          ${QT_QTGUI_INCLUDE_DIR}
	          ${QWT_INCLUDE_DIRS}
	          ${FFTW_INCLUDE_DIRS}
	          ${PORTAUDIO_INCLUDE_DIRS}
	          ${FAAD_INCLUDE_DIRS}
	          ${SNDFILES_INCLUDE_DIRS}
	)

	QT5_WRAP_CPP (MOCS ${${objectName}_MOCS})

	add_executable (${objectName}
	                ${${objectName}_SRCS}
	                ${UIS}
	                ${RSCS}
	                ${TRS}
	                ${MOCS}
	)

	target_link_libraries (${objectName}
	                       Qt5::Widgets
	                       Qt5::Network
	                       ${extraLibs}
	                       ${FAAD_LIBRARIES}
	                       ${CMAKE_DL_LIBS}
	)

	INSTALL (TARGETS ${objectName} DESTINATION ./linux-bin)

########################################################################
# Create uninstall target
########################################################################

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

