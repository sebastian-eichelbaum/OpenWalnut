# This script searches niftilib. See  http://niftilib.sourceforge.net
#
# The following variables will be filled:
#   * NIFTILIB_FOUND - if nifti1.h and lib was found
#   * NIFTILIB_INCLUDE_DIR - the path of nifti1.h if found
#   * NIFTILIB_LIBRARY - the path to the library
#

FIND_PATH( NIFTILIB_INCLUDE_DIR nifti1.h /usr/include/nifti /usr/local/include/nifti )
FIND_LIBRARY( NIFTILIB_LIBRARY NAMES niftiio PATH /usr/lib /usr/local/lib )

SET( NIFTILIB_FOUND FALSE )
IF( NIFTILIB_INCLUDE_DIR AND NIFTILIB_LIBRARY )
    SET( NIFTILIB_FOUND TRUE )
ENDIF()

IF( NIFTILIB_FOUND )
   IF( NOT NIFTILIB_FIND_QUIETLY )
       MESSAGE( STATUS "Found niftilib: ${NIFTILIB_LIBRARY} and include in ${NIFTILIB_INCLUDE_DIR}" )
   ENDIF()
ELSE()
   IF( NIFTILIB_FIND_REQUIRED )
      MESSAGE( FATAL_ERROR "Could not find niftilib." )
   ENDIF()
ENDIF()

