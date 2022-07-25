# Find zlib

FIND_PATH(ZLIB_INCLUDE_DIR zlib.h)

SET(ZLIB_NAMES ${ZLIB_NAMES} libz z)
FIND_LIBRARY(ZLIB_LIBRARY NAMES ${ZLIB_NAMES} PATH)

IF (ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
    SET(ZLIB_FOUND TRUE)
ENDIF (ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)

IF (ZLIB_FOUND)
    IF (NOT zlib_FIND_QUIETLY)
        MESSAGE (STATUS "Found zlib: ${ZLIBFILE_LIBRARY}")
    ENDIF (NOT zlib_FIND_QUIETLY)
ELSE (ZLIB_FOUND)
    IF (zlib_FIND_REQUIRED)
        MESSAGE (FATAL_ERROR "Could not find zlib")
    ENDIF (zlib_FIND_REQUIRED)
ENDIF (ZLIB_FOUND)
