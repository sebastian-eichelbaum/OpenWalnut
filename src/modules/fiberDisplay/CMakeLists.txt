FILE( GLOB_RECURSE MODULES_SRC "*.cpp" )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${MODULES_SRC}"
                               "kernel"
                             )
ENDIF( CXXTEST_FOUND )
