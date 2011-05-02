FILE( GLOB LINALG_SRC "*.cpp" "*.h" )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${LINALG_SRC}"
                               "OWcommon"
                               "WLinearAlgebra" # only header
                               "WPosition" # is a WMatrixFixed. Gets tested there
                               "WVectorFixed" # is a WMatrixFixed. Gets tested there
                             )
ENDIF( OW_COMPILE_TESTS )

