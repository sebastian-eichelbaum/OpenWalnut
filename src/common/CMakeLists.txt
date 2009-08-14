FILE( GLOB COMMON_SRC "*.cpp" "*.hpp" )

ADD_LIBRARY( common SHARED ${COMMON_SRC} )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${COMMON_SRC}"
                               "common"  # no libs for linking required
                               "WSegmentationFault.cpp"
                               "WStringUtils.hpp" # since unit tests are already included via the .hpp file
                             )
ENDIF( CXXTEST_FOUND )
