FILE( GLOB_RECURSE MODULES_SRC "*.cpp" "*.h" )
FILE( GLOB_RECURSE MODULE_SHADERS "shaders/*.glsl" )

# Grab module name and setup target directories
GET_FILENAME_COMPONENT( MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )
SET( MODULE_TARGET_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/modules/${MODULE_NAME} )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MODULE_TARGET_DIR} )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MODULE_TARGET_DIR} )
SET( MODULE_NAME "OWmodule_${MODULE_NAME}" )      # prefix all module names with "OWmodule_" to separate them from other libs

# Copy local resources to module target directory
EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E make_directory ${MODULE_TARGET_DIR}/shaders/ ) # create shader dir
#FOREACH( fname ${MODULE_SHADERS} )
#    CONFIGURE_FILE( ${fname} ${MODULE_TARGET_DIR}/shaders/ COPYONLY )
#ENDFOREACH( fname )


add_custom_command( OUTPUT 
                    DEPENDS ${MODULE_SHADERS}
                    COMMAND echo "commqnd"
                    COMMENT "commaasd"
                 )

ADD_CUSTOM_TARGET( ${MODULE_NAME}_CopyShaders
        COMMAND ${CMAKE_COMMAND} -E copy ${MODULE_SHADERS} ${MODULE_TARGET_DIR}/shaders/ 
        COMMENT "Copy shaders of ${MODULE_NAME}"
        DEPENDS ${MODULE_SHADERS}
)

# Build module lib
ADD_LIBRARY( ${MODULE_NAME} SHARED ${MODULES_SRC} )
TARGET_LINK_LIBRARIES( ${MODULE_NAME} OWkernel )
ADD_DEPENDENCIES( ${MODULE_NAME} ${MODULE_NAME}_CopyShaders )

# Build unit tests
IF( OW_COMPILE_TESTS )
    # This ensures that the test is copied to the module directory
    SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MODULE_TARGET_DIR} )
    CXXTEST_ADD_TESTS_FROM_LIST( "${MODULES_SRC}"
                                 "OWkernel;${MODULE_NAME}"
                               )
ENDIF( OW_COMPILE_TESTS )
