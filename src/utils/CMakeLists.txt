FILE(GLOB UTILS_SRC "*.cpp")

ADD_LIBRARY( utils SHARED ${UTILS_SRC} )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TEST( unittest_WOptionHandler WOptionHandler_test.cc ${CMAKE_CURRENT_SOURCE_DIR}/test/WOptionHandler_test.h )
  TARGET_LINK_LIBRARIES( unittest_WOptionHandler utils guiqt4 ${Boost_LIBRARIES} )
ENDIF( CXXTEST_FOUND )
