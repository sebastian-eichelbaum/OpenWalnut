# This script searches eigen3. See http://eigen.tuxfamily.org
#
# The following variables will be filled:
#   * EIGEN3_FOUND - if Eigen (header) was found
#   * EIGEN3_INCLUDE_DIR - the path of Eigne header if found
#

FIND_PATH( EIGEN3_INCLUDE_DIR Eigen/Core /usr/include/eigen3 /usr/local/include/eigen3 )

SET( EIGEN3_FOUND FALSE )
IF( EIGEN3_INCLUDE_DIR )
    SET( EIGEN3_FOUND TRUE )
ENDIF()

IF( EIGEN3_FOUND )
   IF( NOT EIGEN3_FIND_QUIETLY )
       MESSAGE( STATUS "Found eigen3: ${EIGEN3_INCLUDE_DIR}" )
   ENDIF()
ELSE()
   IF( EIGEN3_FIND_REQUIRED )
      MESSAGE( FATAL_ERROR "Could not find eigen3." )
   ENDIF()
ENDIF()

