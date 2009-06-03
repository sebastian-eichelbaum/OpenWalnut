FILE(GLOB UTILS_SRC "*.cpp")

ADD_LIBRARY( utils SHARED ${UTILS_SRC} )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${UTILS_SRC}"
                               "utils;guiqt4;${Boost_LIBRARIES}"
                             )
ENDIF( CXXTEST_FOUND )
