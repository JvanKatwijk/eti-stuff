cmake_minimum_required( VERSION 2.8.11 )
set (objectName eti_cmdline)
set (CMAKE_CXX_FLAGS "${CMAKE_XCC_FLAGS} -Wall -std=c++11 -flto -ffast-math")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto -ffast-math")
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/usr/local/bin" CACHE PATH "default install path" FORCE )
endif()
#set (CMAKE_INSTALL_PREFIX /usr/local/bin)
#	modify if you want

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
   set (objectName eti-cmdline-airspy)
endif ()

if(DEFINED SDRPLAY)
   set(SDRPLAY true)
   set (objectName eti-cmdline-sdrplay)
endif ()

if(DEFINED RTLSDR)
   set(RTLSDR true)
   set (objectName eti-cmdline-rtlsdr)
endif ()

if(DEFINED HACKRF)
   set(HACKRF true)
   set (objectName eti-cmdline-hackrf)
endif ()

if(DEFINED RAWFILES)
   set(RAWFILES true)
   set (objectName eti-cmdline-rawfiles)
endif ()

if(DEFINED WAVFILES)
   set(WAVFILES true)
   set (objectName eti-cmdline-wavfiles)
endif ()

if(DEFINED RTL_TCP)
   set(RTL_TCP true)
   set (objectName eti-cmdline-rtl_tcp)
endif ()

if(DEFINED DUMPING)
   set(DUMPING true)
endif ()

if(DEFINED X64)
   set(X64_DEFINED true)
endif ()

if(DEFINED RPI_DEFINED)
   set(RPI_DEFINED true)
endif ()
########################################################################
	find_package (PkgConfig)

        find_package(FFTW3f)
        if (NOT FFTW3F_FOUND)
            message(FATAL_ERROR "please install FFTW3")
        endif ()

	find_package(LibSndFile)
	if (NOT LIBSNDFILE_FOUND)
	    message(FATAL_ERROR "please install libsndfile")
	endif ()
	list(APPEND extraLibs ${LIBSNDFILE_LIBRARY})

	find_package(LibSampleRate)
	if (NOT LIBSAMPLERATE_FOUND)
	   message(FATAL_ERROR "please install libsamplerate")
	endif ()
	list(APPEND extraLibs ${LIBSAMPLERATE_LIBRARY})
	add_definitions (-DHAVE_DUMPING)

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
	           ./includes/eti-handling/viterbi-spiral
	           ./includes/support
	           ./devices
	           /usr/include/
	)

	set (${objectName}_HDRS
	     ./includes/dab-constants.h
	     ./callback-types.h
	     ./eti-class.h
	     ./includes/ofdm/ofdm-processor.h
	     ./includes/ofdm/phasereference.h
	     ./includes/ofdm/phasetable.h
	     ./includes/ofdm/freq-interleaver.h
	     ./includes/eti-handling/eti-generator.h
#	     ./includes/eti-handling/viterbi-handler.h
	     ./includes/eti-handling/viterbi-spiral/viterbi-spiral.h
	     ./includes/eti-handling/protection.h
	     ./includes/eti-handling/uep-protection.h
	     ./includes/eti-handling/eep-protection.h
	     ./includes/eti-handling/protTables.h
	     ./includes/eti-handling/charsets.h
	     ./includes/support/ringbuffer.h
	     ./includes/support/fft.h
	     ./includes/support/band-handler.h
	     ./includes/support/dab-params.h
	     ./devices/device-handler.h
	)

	set (${objectName}_SRCS
	     ${${objectName}_SRCS}
	     ./main.cpp
	     ./eti-class.cpp
	     ./src/ofdm/ofdm-processor.cpp
	     ./src/ofdm/phasereference.cpp
	     ./src/ofdm/phasetable.cpp
	     ./src/ofdm/freq-interleaver.cpp
	     ./src/eti-handling/eti-generator.cpp
#	     ./src/eti-handling/viterbi-handler.cpp
	     ./src/eti-handling/viterbi-spiral/viterbi-spiral.cpp
	     ./src/eti-handling/fic-handler.cpp
	     ./src/eti-handling/fib-processor.cpp
	     ./src/eti-handling/protection.cpp
	     ./src/eti-handling/eep-protection.cpp
	     ./src/eti-handling/uep-protection.cpp
	     ./src/eti-handling/protTables.cpp
	     ./src/eti-handling/charsets.cpp
	     ./src/support/fft.cpp
	     ./src/support/band-handler.cpp
	     ./src/support/dab-params.cpp
	     ./devices/device-handler.cpp
	)

	if (X64_DEFINED) 
	   set (${objectName}_SRCS
                ${${objectName}_SRCS}
              ./src/eti-handling/viterbi-spiral/spiral-sse.c
	   )
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
             ./src/eti-handling/viterbi-spiral/spiral-sse.h
	   )
           add_definitions (-DSSE_AVAILABLE)
	elseif (RPI_DEFINED)
	   set (${objectName}_SRCS
                ${${objectName}_SRCS}
              ./src/eti-handling/viterbi-spiral/spiral-neon.c
	   )
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
             ./src/eti-handling/viterbi-spiral/spiral-neon.h
	   )
   	   add_definitions (-DNEON_AVAILABLE)
	   set (CMAKE_CXX_FLAGS "${CMAKE_XCC_FLAGS} -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4")
	   set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4")
	else (X64_DEFINED)
	   set (${objectName}_SRCS
                ${${objectName}_SRCS}
              ./src/eti-handling/viterbi-spiral/spiral-no-sse.c
	   )
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
             ./src/eti-handling/viterbi-spiral/spiral-no-sse.h
	   )
	endif (X64_DEFINED)

#	if (SDRPLAY)
#	   find_path (SDRPLAYLIB_INCLUDE_DIR
#                      NAMES mirsdrapi-rsp.h
#                      PATHS
#                      /usr/local/include/
#	   )
#           include_directories (${SDRPLAYLIB_INCLUDE_DIR})
#
#	   include_directories (
#	     ./devices/sdrplay-handler
#	   )
#
#	   set ($(objectName)_HDRS
#	        ${${objectName}_HDRS}
#	        ./devices/sdrplay-handler/sdrplay-handler.h
#           )
#
#	   set (${objectName}_SRCS
#	        ${${objectName}_SRCS}
#	        ./devices/sdrplay-handler/sdrplay-handler.cpp
#	   )
#
#	 add_definitions (-DHAVE_SDRPLAY)
#	endif (SDRPLAY)

	if (SDRPLAY)
	   find_path (SDRPLAYLIB_INCLUDE_DIR
                      NAMES mirsdrapi-rsp.h
                      PATHS
                      /usr/local/include/
	   )
           include_directories (${SDRPLAYLIB_INCLUDE_DIR})

           find_library (SDRPLAYLIB mirsdrapi-rsp)
           if(NOT(SDRPLAYLIB))
              message(FATAL_ERROR "please install -lmirsdrapi-rsp")
           else(NOT(SDRPLAYLIB))
              list (APPEND extraLibs ${SDRPLAYLIB})
           endif(NOT(SDRPLAYLIB))

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

	if (HACKRF)
           find_path (HACKLIB_INCLUDE_DIR
                      NAMES hackrf.h
                      PATHS
                      ./devices/hackrf-handler/libhackrf/
                       /usr/local/include/
           )
           include_directories (${HACKLIB_INCLUDE_DIR})

           find_library (HACKLIB hackrf)
           if(NOT(HACKLIB))
              message (FATAL_ERROR "please install -lhackrf")
           else(NOT(HACKLIB))
              list (APPEND extraLibs ${HACKLIB})
           endif(NOT(HACKLIB))

           include_directories (
             ./devices/hackrf-handler
           )
	   
	   set ($(objectName)_HDRS
                ${${objectName}_HDRS}
                ./devices/hackrf-handler/hackrf-handler.h
           )

           set (${objectName}_SRCS
                ${${objectName}_SRCS}
                ./devices/hackrf-handler/hackrf-handler.cpp
           )

           add_definitions (-DHAVE_HACKRF)
        endif (HACKRF)

	if (AIRSPY)
           find_package(LibAIRSPY)
           if (NOT LIBAIRSPY_FOUND)
               message(FATAL_ERROR "please install airspy library")
           endif ()
	   ### include_directories (${AIRSPYLIB_INCLUDE_DIR})

	   include_directories (
	     ./devices/airspy-handler
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/input/airspy-handler/airspy-handler.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/airspy-handler/airspy-handler.cpp
	   )

	   add_definitions (-DHAVE_AIRSPY)
	endif (AIRSPY)
#
	if (RTLSDR)
           find_package(LibRTLSDR)
           if (NOT LIBRTLSDR_FOUND)
               message(FATAL_ERROR "please install librtlsdr")
           endif ()
	   include_directories (${LIBRTLSDR_INCLUDE_DIR})
	   list(APPEND extraLibs ${LIBRTLSDR_LIBRARIES})

	   include_directories (
	      ./devices/rtlsdr-handler/
	   )

	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ./devices/rtlsdr-handler/rtlsdr-handler.h 
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/rtlsdr-handler/rtlsdr-handler.cpp
	   )

	   add_definitions (-DHAVE_RTLSDR)
	endif()
#
	if (RAWFILES)
	   include_directories (
	     ./devices/rawfile-handler
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/rawfile-handler/rawfile-handler.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/rawfile-handler/rawfile-handler.cpp
	   )

	   add_definitions (-DHAVE_RAWFILES)
	endif (RAWFILES)

	if (RTL_TCP)
	   include_directories (
	      ./devices/rtl_tcp
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/rtl_tcp/rtl_tcp-client.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/rtl_tcp/rtl_tcp-client.cpp
	   )

	   add_definitions (-DHAVE_RTL_TCP)
	endif (RTL_TCP)

	if (WAVFILES)
	   include_directories (
	     ./devices/wavfile-handler
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/wavfile-handler/wavfile-handler.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/wavfile-handler/wavfile-handler.cpp
	   )

	   add_definitions (-DHAVE_WAVFILES)
	endif (WAVFILES)

	include_directories (
	          ${SDRPLAY_INCLUDES}
	          ${FFTW_INCLUDE_DIRS}
	          ${PORTAUDIO_INCLUDE_DIRS}
	          ${FAAD_INCLUDE_DIRS}
	          ${SNDFILES_INCLUDE_DIRS}
	)

#####################################################################

	add_executable (${objectName} 
	                ${${objectName}_SRCS}
	)

	target_link_libraries (${objectName}
	                       ${FFTW3F_LIBRARIES}
	                       ${extraLibs}
	                       ${CMAKE_DL_LIBS}
	)

	INSTALL (TARGETS ${objectName} DESTINATION  ${CMAKE_INSTALL_PREFIX})
#	INSTALL (TARGETS ${objectName} DESTINATION ./linux-bin)


########################################################################
# Create uninstall target
########################################################################

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
