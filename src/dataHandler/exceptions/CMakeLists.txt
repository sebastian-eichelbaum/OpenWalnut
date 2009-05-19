
# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TEST( unittest_WNoSuchDataSetException
                    WNoSuchDataSetException_test.cc
                    ${CMAKE_CURRENT_SOURCE_DIR}/test/WNoSuchDataSetException_test.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/WNoSuchDataSetException.cpp )

ENDIF( CXXTEST_FOUND )
