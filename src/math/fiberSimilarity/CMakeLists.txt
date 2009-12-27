FILE( GLOB FIBERSIMILARITY_SRC "*.cpp" )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${FIBERSIMILARITY_SRC}"
                               "math;utils"
                               "WFiberSimilarity.cpp" # This is just an interface
                              )
ENDIF( CXXTEST_FOUND )
