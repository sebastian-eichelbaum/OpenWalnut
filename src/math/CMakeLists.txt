FILE( GLOB MATH_SRC "*.cpp" "*.hpp" )

ADD_LIBRARY( math SHARED ${MATH_SRC} )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${MATH_SRC}" 
                               "" #no modules to link against yet 
                              )
ENDIF( CXXTEST_FOUND )
