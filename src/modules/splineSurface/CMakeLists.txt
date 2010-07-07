FILE( GLOB_RECURSE MODULES_SRC "*.cpp" "*.h" )

SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/modules )
GET_FILENAME_COMPONENT( modname ${CMAKE_CURRENT_SOURCE_DIR} NAME )
ADD_LIBRARY( ${modname} SHARED ${MODULES_SRC} )
TARGET_LINK_LIBRARIES( ${modname} kernel )
INSTALL( TARGETS ${modname} LIBRARY DESTINATION bin/modules ARCHIVE DESTINATION bin/modules )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${MODULES_SRC}"
                               "kernel;${modname}"
                             )
ENDIF( OW_COMPILE_TESTS )
