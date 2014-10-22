# This script searches liblas. See  http://liblas.org
#
# The following variables will be filled:
#   * LIBLAS_FOUND - if liblas.hpp and lib was found
#   * LIBLAS_INCLUDE_DIR - the path of liblas.hpp if found
#   * LIBLAS_LIBRARY - the path to the library
#

FIND_PATH( LIBLAS_INCLUDE_DIR liblas.hpp 
        $ENV{LIBLAS_INCLUDE_DIR}/liblas
        $ENV{LIBLAS_INCLUDE_DIR}
        $ENV{HOME}/.local/include/liblas
        /usr/include/liblas
        /usr/local/include/liblas )

# This hack is inspired by FindBoost.cmake. It ensures that only shared objects are found. Linking a SO with a static lib is not possible 
# in Linux. On other systems, this should be no problem.
SET( _ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
IF( CMAKE_HOST_SYSTEM MATCHES "Linux" )
    SET( CMAKE_FIND_LIBRARY_SUFFIXES .so )
ENDIF()
FIND_LIBRARY( LIBLAS_LIBRARY NAMES las HINTS $ENV{LIBLAS_LIBRARY_DIR} /usr/lib /usr/local/lib )

SET( CMAKE_FIND_LIBRARY_SUFFIXES ${_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES} )

SET( LIBLAS_FOUND FALSE )
IF( LIBLAS_INCLUDE_DIR AND LIBLAS_LIBRARY )
    SET( LIBLAS_FOUND TRUE )
ENDIF()

IF( LIBLAS_FOUND )
   IF( NOT liblas_FIND_QUIETLY )
       MESSAGE( STATUS "Found liblas: ${LIBLAS_LIBRARY} and include in ${LIBLAS_INCLUDE_DIR}" )
   ENDIF()
ELSE()
   IF( liblas_FIND_REQUIRED )
       MESSAGE( FATAL_ERROR "Could not find liblas. You can specify LIBLAS_INCLUDE_DIR and LIBLAS_LIBRARY_DIR environment variables to help OpenWalnut finding it." )
   ENDIF()
ENDIF()

