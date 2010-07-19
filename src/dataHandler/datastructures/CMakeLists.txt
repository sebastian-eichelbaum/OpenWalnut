FILE( GLOB DATAHANDLER_DATASTRUCTURES_SRC "*.cpp" "*.h" )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${DATAHANDLER_DATASTRUCTURES_SRC}"
                               "OWdataHandler;OWcommon"  # no libs for linking required
                             )
ENDIF( OW_COMPILE_TESTS )
