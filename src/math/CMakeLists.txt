ADD_SUBDIRECTORY( fiberSimilarity )

FILE( GLOB MATH_SRC "*.[c,h]pp" )
FILE( GLOB FIBERSIMILARITY_SRC "fiberSimilarity/*.[c,h]pp" )

ADD_LIBRARY( math SHARED ${MATH_SRC} ${FIBERSIMILARITY_SRC} )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${MATH_SRC}" 
                               "math;common" 
                              )
ENDIF( CXXTEST_FOUND )
