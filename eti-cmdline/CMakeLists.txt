cmake_minimum_required( VERSION 2.8.11 )
######################################################################
#       R E A D T H I S
#####################################################################

# options can be set from cmake command line, e.g. like this:
#   cmake ../eti-cmdline -DAIRSPY=ON -DX64_DEFINED=ON
# alternatives are to use cmake-gui  or  ccmake (for console)

OPTION(AIRSPY      "Input: AirSpy"      OFF)
OPTION(SDRPLAY     "Input: SDRPlay"     OFF)
OPTION(SDRPLAY_V3  "Input: SDRPlay V3"  OFF)
OPTION(RTLSDR      "Input: RTLSDR"      OFF)
OPTION(HACKRF      "Input: HACKRF"      OFF)
OPTION(PLUTO       "Input: PLUTO"       OFF)
OPTION(LIMESDR     "Input: LimeSDR"     OFF)
OPTION(RAWFILES    "Input: RAWFILES"    OFF)
OPTION(WAVFILES    "Input: WAVFILES"    OFF)
OPTION(XMLFILES    "Input: XMLFILES"    OFF)
OPTION(RTL_TCP     "Input: RTL_TCP"     OFF)

OPTION(DUMPING     "Dump to Wave File?"    OFF)
OPTION(X64_DEFINED "optimize for x64/SSE"  OFF)
OPTION(RPI_DEFINED "optimize for ARM/NEON" OFF)

#######################################################################

set (objectName eti_cmdline)
#set (CMAKE_CXX_FLAGS "${CMAKE_XCC_FLAGS} -Wall -std=c++11 -flto -ffast-math")
#set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto -ffast-math")
set (CMAKE_CXX_FLAGS "${CMAKE_XCC_FLAGS} -Wall -std=c++11 -pg -ffast-math")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg -ffast-math")
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

set(INPUTS)

if(AIRSPY)
	set (objectName eti-cmdline-airspy)
	list (APPEND INPUTS "AIRSPY")
endif ()

if(SDRPLAY)
	set (objectName eti-cmdline-sdrplay)
	list (APPEND INPUTS "SDRPLAY")
endif ()

if(SDRPLAY_V3)
	set (objectName eti-cmdline-sdrplay)
	list (APPEND INPUTS "SDRPLAY_V3")
endif ()

if(RTLSDR)
	set (objectName eti-cmdline-rtlsdr)
	list (APPEND INPUTS "RTLSDR")
endif ()

if(HACKRF)
	set (objectName eti-cmdline-hackrf)
	list (APPEND INPUTS "HACKRF")
endif ()

if(PLUTO)
	set (objectName eti-cmdline-pluto)
	list (APPEND INPUTS "PLUTO")
endif ()

if(LIMESDR)
	set (objectName eti-cmdline-limesdr)
	list (APPEND INPUTS "LIMESDR")
endif ()

if(RAWFILES)
	set (objectName eti-cmdline-rawfiles)
	list (APPEND INPUTS "RAWFILES")
endif ()

if(WAVFILES)
	set (objectName eti-cmdline-wavfiles)
	list (APPEND INPUTS "WAVFILES")
endif ()

if(XMLFILES)
	set (objectName eti-cmdline-xml)
	list (APPEND INPUTS "XMLFILES")
endif ()

if(RTL_TCP)
	set (objectName eti-cmdline-rtl_tcp)
	list (APPEND INPUTS "RTL_TCP")
endif ()

list(LENGTH INPUTS NUM_INPUTS)
message(STATUS "selected input devices:		${INPUTS}")
message(STATUS "number of selected inputs:	${NUM_INPUTS}")

IF (NUM_INPUTS EQUAL 0)
	message (FATAL ERROR "please select one input device, e.g. with cmake-gui or ccmake")
endif()

if (NUM_INPUTS GREATER 1)
	message (FATAL ERROR "please select exactly one input device, e.g. with cmake-gui or ccmake")
endif()

if (DUMPING)
	add_definitions (-DHAVE_DUMPING)
endif()

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
	    ./includes/support/fft
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
	     ./includes/eti-handling/semaphore.h
	     ./includes/eti-handling/eti-generator.h
#	     ./includes/eti-handling/viterbi-handler.h
	     ./includes/eti-handling/viterbi-spiral/viterbi-spiral.h
	     ./includes/eti-handling/protection.h
	     ./includes/eti-handling/uep-protection.h
	     ./includes/eti-handling/eep-protection.h
	     ./includes/eti-handling/protTables.h
	     ./includes/eti-handling/charsets.h
	     ./includes/support/ringbuffer.h
	     ./includes/support/band-handler.h
	     ./includes/support/dab-params.h
	     ./includes/support/fft/fft.h
	     ./includes/support/fft/kiss_fft.h 
	     ./includes/support/fft/_kiss_fft_guts.h 
	     ./includes/support/fft/_kiss_fftr.h 
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
	     ./src/eti-handling/semaphore.cpp
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
	     ./src/support/band-handler.cpp
	     ./src/support/dab-params.cpp
	     ./src/support/fft/fft.cpp
	     ./src/support/fft/kiss_fft.c
	     ./src/support/fft/kiss_fftr.c
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
	   # compiler options moved below add_executable .. and target specific
	   #set (CMAKE_CXX_FLAGS "${CMAKE_XCC_FLAGS} -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4")
	   #set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4")
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

	if (SDRPLAY_V3)
	   find_path (SDRPLAYLIB_INCLUDE_DIR
                      NAMES sdrplay_api.h
                      PATHS
                      /usr/local/include/
	   )
           include_directories (${SDRPLAYLIB_INCLUDE_DIR})

           find_library (SDRPLAYLIB sdrplay_api)
           if(NOT(SDRPLAYLIB))
              message(FATAL_ERROR "please install -lsdrplay_api")
           else(NOT(SDRPLAYLIB))
              list (APPEND extraLibs ${SDRPLAYLIB})
           endif(NOT(SDRPLAYLIB))

	   include_directories (
	     ./devices/sdrplay-handler-v3
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/sdrplay-handler-v3/sdrplay-handler-v3.h
           )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/sdrplay-handler-v3/sdrplay-handler-v3.cpp
	   )

	   add_definitions (-DHAVE_SDRPLAY_V3)
	endif (SDRPLAY_V3)

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

	if (PLUTO)
           find_library (PLUTOLIB iio)
           if(NOT(PLUTOLIB))
              message(FATAL_ERROR "please install libiio")
           else(NOT(PLUTOLIB))
              list (APPEND extraLibs ${PLUTOLIB})
           endif(NOT(PLUTOLIB))

	   include_directories (
	     ./devices/pluto
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/pluto/dabFilter.h
	        ./devices/pluto/pluto-handler.h
           )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/pluto/pluto-handler.cpp
	   )

	 add_definitions (-DHAVE_PLUTO)
	endif (PLUTO)

	if (LIMESDR)
           find_library (LIMELIB LimeSuite)
           if(NOT(LIMELIB))
              message(FATAL_ERROR "please install LimeSuite")
           else(NOT(LIMELIB))
              list (APPEND extraLibs ${LIMELIB})
           endif(NOT(LIMELIB))

	   include_directories (
	     ./devices/lime-handler
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/lime-handler/LMS7002M_parameters.h
	        ./devices/lime-handler/LimeSuite.h
	        ./devices/lime-handler/lime-handler.h
           )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/lime-handler/lime-handler.cpp
	   )

	 add_definitions (-DHAVE_LIME)
	endif (LIMESDR)

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

	if (XMLFILES)
	   include_directories (
	     ./devices/xml-filereader
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/xml-filereader/rapidxml.hpp
	        ./devices/xml-filereader/xml-filereader.h
	        ./devices/xml-filereader/xmlreader.h
	        ./devices/xml-filereader/element-reader.h
	        ./devices/xml-filereader/xml-descriptor.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/xml-filereader/xml-filereader.cpp
	        ./devices/xml-filereader/xml-reader.cpp
	        ./devices/xml-filereader/xml-descriptor.cpp
	   )

	   add_definitions (-DHAVE_XML_FILES)
	endif (XMLFILES)

	include_directories (
	          ${SDRPLAY_INCLUDES}
#	          ${FFTW_INCLUDE_DIRS}
	          ${PORTAUDIO_INCLUDE_DIRS}
	          ${FAAD_INCLUDE_DIRS}
	          ${SNDFILES_INCLUDE_DIRS}
	)

#####################################################################

	add_executable (${objectName} 
	                ${${objectName}_SRCS}
	)

	if (RPI_DEFINED)
	   # compiler options seem changed with gcc versions. gcc 8.3.0 on Raspbian didn't accept -mcpu ..
	   # test options with "make VERBOSE=1"
	   target_compile_options(${objectName}  PRIVATE -march=armv7-a -mfloat-abi=hard -mfpu=neon-vfpv4 )
	endif()
	target_link_libraries (${objectName}
#	                       ${FFTW3F_LIBRARIES}
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
