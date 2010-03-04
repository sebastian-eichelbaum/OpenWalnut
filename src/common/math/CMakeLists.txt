ADD_SUBDIRECTORY( fiberSimilarity )

FILE( GLOB MATH_SRC "*.cpp" "*.h" )
FILE( GLOB FIBERSIMILARITY_SRC "fiberSimilarity/*.cpp" "fiberSimilarity/*.h" )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${MATH_SRC}"
                               "common"
                               "WMath.cpp" # this declares only a namespace
                             )
ENDIF( OW_COMPILE_TESTS )
