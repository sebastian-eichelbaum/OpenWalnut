FILE( GLOB MATH_SRC "*.cpp" "*.h" )

ADD_SUBDIRECTORY( linearAlgebra )

#remove OSSIM Helper files from list, if it isn't found
IF ( NOT OSSIM_FOUND )
    LIST( REMOVE_ITEM MATH_SRC ${CMAKE_CURRENT_SOURCE_DIR}/WOSSIMHelper.h )
    LIST( REMOVE_ITEM MATH_SRC ${CMAKE_CURRENT_SOURCE_DIR}/WOSSIMHelper.cpp )
ENDIF( NOT OSSIM_FOUND )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${MATH_SRC}"
                               "OWcommon"
                             )
ENDIF( OW_COMPILE_TESTS )
