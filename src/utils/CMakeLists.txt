FILE(GLOB UTILS_SRC "*.cpp")


IF( CMAKE_HOST_SYSTEM MATCHES Windows )
	ADD_LIBRARY( utils ${UTILS_SRC} )
ELSE()
	ADD_LIBRARY( utils SHARED ${UTILS_SRC} )
ENDIF()

TARGET_LINK_LIBRARIES( utils ${Boost_PROGRAM_OPTIONS_LIBRARY} common math )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${UTILS_SRC}" "utils;guiqt4;dataHandler;${Boost_LIBRARIES}"
                             )
ENDIF( CXXTEST_FOUND )
