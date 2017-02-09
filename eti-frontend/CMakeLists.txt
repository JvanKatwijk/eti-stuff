cmake_minimum_required( VERSION 2.8.11 )
set (objectName eti-frontend)
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
#	if you do not want to use the SSE support on your machine, uncomment
#	but recall that MINGW does not support it (yet)
#	set(NO_SSE_SUPPORT true)


#	if you want support for any of these devices, uncomment the line
	set(SDRPLAY true)
	set(AIRSPY true)
	set(DABSTICK true)
#	set(RTLTCP true)

########################################################################
	find_package (PkgConfig)

	find_package (Qt5Core REQUIRED)
	find_package (Qt5Widgets REQUIRED)
	find_package (Qt5Declarative REQUIRED)

	include_directories (
	      ${Qt5Network_INCLUDE_DIRS}
	)

        find_package(FFTW3f)
        if (NOT FFTW3F_FOUND)
            message(FATAL_ERROR "please install FFTW3")
        endif ()

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

	if (NO_SSE_SUPPORT) 
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./src/backend/spiral-code/spiral-no-sse.c )
	else ()
	   add_definitions (-DSSE_AVAILABLE)
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./src/backend/spiral-code/spiral-sse.c )
	endif()

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
	           ./includes/various
	           ./includes/output
	           ./src/input
	           ./src/input/rawfiles
	           ./src/input/wavfiles
	           /usr/include/
	)

	set (${objectName}_HDRS
	     ${${objectName}_HDRS}
	     ./gui.h
	     ./includes/dab-constants.h
	     ./includes/ofdm/ofdm-processor.h
	     ./includes/ofdm/phasereference.h
	     ./includes/ofdm/phasetable.h
	     ./includes/ofdm/freq-interleaver.h
	     ./includes/backend/viterbi.h
	     ./includes/backend/protection.h
	     ./includes/backend/uep-protection.h
	     ./includes/backend/eep-protection.h
	     ./includes/backend/charsets.h
	     ./includes/backend/eti-generator.h
	     ./src/input/virtual-input.h
	     ./src/input/rawfiles/rawfiles.h
	     ./src/input/wavfiles/wavfiles.h
	     ./includes/output/fir-filters.h
	     ./includes/various/fft.h
	     ./includes/various/ringbuffer.h
	     ./includes/various/Xtan2.h
	)

	set (${objectName}_SRCS
	     ${${objectName}_SRCS}
	     ./main.cpp
	     ./gui.cpp
	     ./src/ofdm/ofdm-processor.cpp
	     ./src/ofdm/phasereference.cpp
	     ./src/ofdm/phasetable.cpp
	     ./src/ofdm/freq-interleaver.cpp
	     ./src/backend/viterbi.cpp
	     ./src/backend/fic-handler.cpp
	     ./src/backend/protection.cpp
	     ./src/backend/uep-protection.cpp
	     ./src/backend/eep-protection.cpp
	     ./src/backend/fib-processor.cpp
	     ./src/backend/protTables.cpp
	     ./src/backend/charsets.cpp
	     ./src/backend/eti-generator.cpp
	     ./src/input/virtual-input.cpp
	     ./src/input/rawfiles/rawfiles.cpp
	     ./src/input/wavfiles/wavfiles.cpp
	     ./src/various/fft.cpp
	     ./src/various/Xtan2.cpp
	)

	set (${objectName}_MOCS
	     ./includes/ofdm/ofdm-processor.h
	     ./includes/backend/fic-handler.h
	     ./includes/backend/fib-processor.h
	     ./src/input/rawfiles/rawfiles.h
	     ./src/input/wavfiles/wavfiles.h
	     ./gui.h
	)

	set (${objectName}_UIS
	        ${${objectName}_UIS} ./gui.ui)

	
	if (SDRPLAY)
	   find_path (SDRPLAYLIB_INCLUDE_DIR
                      NAMES mirsdrapi-rsp.h
                      PATHS
                      /usr/local/include/
	   )
           include_directories (${SDRPLAYLIB_INCLUDE_DIR})
	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ./src/input/sdrplay-exp/sdrplay-widget.ui 
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./src/input/sdrplay-exp/sdrplay.h
	   )

	   include_directories (
	     ./src/input/sdrplay-exp
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./src/input/sdrplay-exp/sdrplay.h
           )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./src/input/sdrplay-exp/sdrplay.cpp
	   )

	 add_definitions (-DHAVE_SDRPLAY)
	endif (SDRPLAY)
 
	if (AIRSPY)
           find_package(LibAIRSPY)
           if (NOT LIBAIRSPY_FOUND)
               message(FATAL_ERROR "please install airspy library")
           endif ()
	   ### include_directories (${AIRSPYLIB_INCLUDE_DIR})

	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ./src/input/airspy/airspy-widget.ui 
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./src/input/airspy/airspy-handler.h
	   )

	   include_directories (
	     ./src/input/airspy
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./src/input/airspy/airspy-handler.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./src/input/airspy/airspy-handler.cpp
	   )

	   add_definitions (-DHAVE_AIRSPY)
	endif (AIRSPY)

#
	if (DABSTICK)
           find_package(LibRTLSDR)
           if (NOT LIBRTLSDR_FOUND)
               message(FATAL_ERROR "please install librtlsdr")
           endif ()
	   ###include_directories (${RTLSDR_INCLUDE_DIR})

	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ./src/input/dabstick/dabstick-widget.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./src/input/dabstick/dongleselect.h
	        ./src/input/dabstick/dabstick.h
	   )

	   include_directories (
	        ./src/input/dabstick/
	   )

	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ./src/input/dabstick/dabstick.h 
	        ./src/input/dabstick/dongleselect.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./src/input/dabstick/dabstick.cpp
	        ./src/input/dabstick/dongleselect.cpp
	   )

	   add_definitions (-DHAVE_DABSTICK)
	endif()
#

	if (RTLTCP)
	   find_package (Qt5Network REQUIRED)
	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ./src/input/rtl_tcp/rtl_tcp-widget.ui 
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./src/input/rtl_tcp/rtl_tcp_client.h
	   )

	   include_directories (
	      ./src/input/rtl_tcp
	      ${Qt5Network_INCLUDE_DIRS}
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./src/input/rtl_tcp/rtl_tcp_client.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./src/input/rtl_tcp/rtl_tcp_client.cpp
	   )

	   set (RTLTCP_lib Qt5::Network)
	   add_definitions (-DHAVE_RTL_TCP)
	endif (RTLTCP)

	QT5_WRAP_UI (UIS ${${objectName}_UIS}
	             ./src/input/filereader-widget.ui) 

	include_directories (
	          ${SDRPLAY_INCLUDES}
	          ${QT5Widgets_INCLUDES}
	          ${QT_QTCORE_INCLUDE_DIR}
	          ${QT_QTGUI_INCLUDE_DIR}
	          ${FFTW_INCLUDE_DIRS}
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
	                       ${RTLTCP_lib}
	                       ${FFTW3F_LIBRARIES}
	                       ${extraLibs}
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

