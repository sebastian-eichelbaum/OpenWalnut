FILE(GLOB UTILS_SRC "*.cpp")

ADD_LIBRARY( utils SHARED ${UTILS_SRC} )
TARGET_LINK_LIBRARIES( utils ${Boost_PROGRAM_OPTIONS_LIBRARY} guiqt4)

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${UTILS_SRC}"
                               "utils;guiqt4;${Boost_LIBRARIES}"
                             )
ENDIF( CXXTEST_FOUND )
