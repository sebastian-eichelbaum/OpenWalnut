ADD_SUBDIRECTORY( fiberSimilarity )

FILE( GLOB MATH_SRC "*.cpp" )
FILE( GLOB FIBERSIMILARITY_SRC "fiberSimilarity/*.cpp" )

IF( CMAKE_HOST_SYSTEM MATCHES Windows )
    ADD_LIBRARY( math ${MATH_SRC} ${FIBERSIMILARITY_SRC} )
ELSE()
    ADD_LIBRARY( math SHARED ${MATH_SRC} ${FIBERSIMILARITY_SRC} )
ENDIF()

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${MATH_SRC}"
                               "math;common;utils"
                               "WMath.cpp" # this declares only a namespace
                             )
ENDIF( OW_COMPILE_TESTS )
