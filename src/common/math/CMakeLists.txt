FILE( GLOB MATH_SRC "*.cpp" "*.h" )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${MATH_SRC}"
                               "common"
                             )
ENDIF( OW_COMPILE_TESTS )
