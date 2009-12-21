ADD_SUBDIRECTORY( fiberSimilarity )

FILE( GLOB MATH_SRC "*.cpp" )
FILE( GLOB FIBERSIMILARITY_SRC "fiberSimilarity/*.cpp" )

IF( CMAKE_HOST_SYSTEM MATCHES Windows )
    ADD_LIBRARY( math ${MATH_SRC} ${FIBERSIMILARITY_SRC} )
ELSE()
    ADD_LIBRARY( math SHARED ${MATH_SRC} ${FIBERSIMILARITY_SRC} )
ENDIF()

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${MATH_SRC}" 
                               "math;common" 
                              )
ENDIF( CXXTEST_FOUND )
