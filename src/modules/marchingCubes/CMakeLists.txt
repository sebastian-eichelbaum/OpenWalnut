FILE( GLOB_RECURSE MODULES_SRC "*.[c,h]pp" )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${MODULES_SRC}"
                               "kernel"
                             )
ENDIF( CXXTEST_FOUND )