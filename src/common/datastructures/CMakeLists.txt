FILE( GLOB COMMON_DATASTRUCTURES_SRC "*.cpp" )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${COMMON_DATASTRUCTURES_SRC}"
                               "common"  # no libs for linking required
                             )
ENDIF( CXXTEST_FOUND )
