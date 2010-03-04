FILE( GLOB FIBERSIMILARITY_SRC "*.cpp" "*.h" )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${FIBERSIMILARITY_SRC}"
                               "common"
                               "WFiberSimilarity.cpp" # This is just an interface
                              )
ENDIF( OW_COMPILE_TESTS )
