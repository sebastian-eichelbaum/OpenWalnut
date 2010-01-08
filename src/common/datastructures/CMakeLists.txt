FILE( GLOB COMMON_DATASTRUCTURES_SRC "*.cpp" )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${COMMON_DATASTRUCTURES_SRC}"
                               "common;utils"  # no libs for linking required
                             )
ENDIF( OW_COMPILE_TESTS )
