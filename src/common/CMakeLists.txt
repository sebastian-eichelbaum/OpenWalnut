FILE( GLOB COMMON_SRC "*.cpp" )

ADD_LIBRARY( common SHARED ${COMMON_SRC} )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${COMMON_SRC}"
                               "common"  # no libs for linking required
                             )
ENDIF( CXXTEST_FOUND )
