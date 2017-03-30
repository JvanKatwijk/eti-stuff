cmake_minimum_required( VERSION 2.8.11 )
set (objectName eti_lib)
add_definitions ( -Wall -g -std=c++11)
#	modify if you want
set (CMAKE_INSTALL_PREFIX /usr/local/)

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

if(DEFINED AIRSPY)
   set(AIRSPY true)
endif ()

if(DEFINED SDRPLAY)
   set(SDRPLAY true)
endif ()

if(DEFINED DABSTICK)
   set(DABSTICK true)
endif ()

if(DEFINED RTLTCP)
   set(RTLTCP true)
endif ()
########################################################################
# if you are compiling the wrapper for Python in, uncomment
#	set(PYTHON true)
########################################################################
	find_package (PkgConfig)

        find_package(FFTW3f)
        if (NOT FFTW3F_FOUND)
            message(FATAL_ERROR "please install FFTW3")
        endif ()


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
	           .
	           ./
	           ./includes
	           ./includes/ofdm
	           ./includes/eti-handling
	           ./includes/various
	           ./devices
	           /usr/include/
#	           /usr/include/python3.5m
	)

	if (PYTHON)
	   include_directories (/usr/include/python3.5m)
	endif (PYTHON)

	set (${objectName}_HDRS
	     ./eti-api.h
	     ./includes/eti-class.h
	     ./includes/dab-constants.h
	     ./includes/ofdm/ofdm-processor.h
	     ./includes/ofdm/phasereference.h
	     ./includes/ofdm/phasetable.h
	     ./includes/ofdm/freq-interleaver.h
	     ./includes/eti-handling/eti-generator.h
	     ./includes/eti-handling/viterbi.h
	     ./includes/eti-handling/protection.h
	     ./includes/eti-handling/uep-protection.h
	     ./includes/eti-handling/eep-protection.h
	     ./includes/eti-handling/protTables.h
	     ./includes/eti-handling/charsets.h
	     ./includes/various/ringbuffer.h
	     ./includes/various/fft.h
	     ./includes/various/ensemble-handler.h
	     ./includes/various/dab-params.h
	     ./devices/device-handler.h
	)

	if (PYTHON)
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./dab-python.cpp
           )
	endif (PYTHON)

	set (${objectName}_SRCS
	     ${${objectName}_SRCS}
	     ./src/eti-api.cpp
	     ./src/eti-class.cpp
	     ./src/ofdm/ofdm-processor.cpp
	     ./src/ofdm/phasereference.cpp
	     ./src/ofdm/phasetable.cpp
	     ./src/ofdm/freq-interleaver.cpp
	     ./src/eti-handling/eti-generator.cpp
	     ./src/eti-handling/viterbi.cpp
	     ./src/eti-handling/fic-handler.cpp
	     ./src/eti-handling/fib-processor.cpp
	     ./src/eti-handling/protection.cpp
	     ./src/eti-handling/eep-protection.cpp
	     ./src/eti-handling/uep-protection.cpp
	     ./src/eti-handling/protTables.cpp
	     ./src/eti-handling/charsets.cpp
	     ./src/various/fft.cpp
	     ./src/various/ensemble-handler.cpp
	     ./src/various/dab-params.cpp
	     ./devices/device-handler.cpp
	)

	if (SDRPLAY)
	   find_path (SDRPLAYLIB_INCLUDE_DIR
                      NAMES mirsdrapi-rsp.h
                      PATHS
                      /usr/local/include/
	   )
           include_directories (${SDRPLAYLIB_INCLUDE_DIR})

	   include_directories (
	     ./devices/sdrplay-handler
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/sdrplay-handler/sdrplay-handler.h
           )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/sdrplay-handler/sdrplay-handler.cpp
	   )

	 add_definitions (-DHAVE_SDRPLAY)
	endif (SDRPLAY)
 
	if (AIRSPY)
           find_package(LibAIRSPY)
           if (NOT LIBAIRSPY_FOUND)
               message(FATAL_ERROR "please install airspy library")
           endif ()
	   ### include_directories (${AIRSPYLIB_INCLUDE_DIR})

	   include_directories (
	     ./devices/airspy
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/input/airspy/airspy-handler.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/airspy/airspy-handler.cpp
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

	   include_directories (
	      ./devices/rtlsdr/
	   )

	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ./devices/rtlsdr/rtlsdr.h 
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/rtlsdr/rtlsdr.cpp
	   )

	   add_definitions (-DHAVE_DABSTICK)
	endif()
#
	include_directories (
	          ${SDRPLAY_INCLUDES}
	          ${FFTW_INCLUDE_DIRS}
	          ${PORTAUDIO_INCLUDE_DIRS}
	          ${FAAD_INCLUDE_DIRS}
	          ${SNDFILES_INCLUDE_DIRS}
	)

	if (PYTHON)
	   list(APPEND extraLibs -lpython3.5m)
	endif (PYTHON)
#####################################################################

	add_library (${objectName} SHARED
	                ${${objectName}_SRCS}
	)

	target_link_libraries (${objectName}
	                       ${FFTW3F_LIBRARIES}
	                       ${extraLibs}
	                       ${FAAD_LIBRARIES}
	                       ${CMAKE_DL_LIBS}
	)

	INSTALL (TARGETS ${objectName} DESTINATION ./lib)

########################################################################
# Create uninstall target
########################################################################

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
