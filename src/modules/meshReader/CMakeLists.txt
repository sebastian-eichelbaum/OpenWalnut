FILE( GLOB_RECURSE MODULES_SRC "*.cpp" "*.h" )

GET_FILENAME_COMPONENT( MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )
SET( MODULE_NAME "OWmodule_${MODULE_NAME}" )      # prefix all module names with "OWmodule_" to separate them from other libs
ADD_LIBRARY( ${MODULE_NAME} SHARED ${MODULES_SRC} )
TARGET_LINK_LIBRARIES( ${MODULE_NAME} OWkernel )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${MODULES_SRC}"
                               "OWkernel;${MODULE_NAME}"
                             )
ENDIF( OW_COMPILE_TESTS )
