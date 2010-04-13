FILE( GLOB_RECURSE MODULES_SRC "*.cpp" "*.h" )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${MODULES_SRC}"
                               "kernel"
                             )
ENDIF( OW_COMPILE_TESTS )
