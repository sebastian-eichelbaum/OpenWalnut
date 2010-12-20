FILE( GLOB_RECURSE MODULES_SRC "*.cpp" "*.h" )

# Grab module name and setup target directories
GET_FILENAME_COMPONENT( MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )
SET( MODULE_TARGET_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/modules/${MODULE_NAME} )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MODULE_TARGET_DIR} )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MODULE_TARGET_DIR} )
SET( MODULE_DIRNAME ${MODULE_NAME} )
SET( MODULE_NAME    "${OWmodulesPrefix}_${MODULE_NAME}" )      # prefix all module names with "OWmodule_" to separate them from other libs

SET( ${MODULE_NAME}_COMBINER_SRC ${MODULES_SRC} )
SET( ${MODULE_NAME}_ALL_CPP ${MODULES_SRC} )
FILE( GLOB_RECURSE ${MODULE_NAME}_REMOVE_H "*.h"  )

LIST( REMOVE_ITEM ${MODULE_NAME}_ALL_CPP ${${MODULE_NAME}_REMOVE_H} )

IF( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/" )
    FILE( GLOB_RECURSE ${MODULE_NAME}_REMOVE_TEST "test/*.cpp" "test/*.h" )
    LIST( REMOVE_ITEM ${MODULE_NAME}_ALL_CPP ${${MODULE_NAME}_REMOVE_TEST} )
    LIST( REMOVE_ITEM ${MODULE_NAME}_COMBINER_SRC ${${MODULE_NAME}_REMOVE_TEST} )
ENDIF( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/" )

LIST( LENGTH ${MODULE_NAME}_ALL_CPP cpp_count )

IF( ${cpp_count} GREATER 3 )
    IF( OW_BUILD_AS_ONE_BIG_FILE )
        SET( UNITY_BUILD_FILE_NAME "${MODULE_NAME}UnityBuildFile.cpp" )
        GENERATE_UNITY_BUILD_FILE( ${MODULE_NAME}_ALL_CPP )
        ADD_LIBRARY( ${MODULE_NAME} SHARED ${UNITY_BUILD_FILE_NAME} ${${MODULE_NAME}_COMBINER_SRC} )
    ENDIF( OW_BUILD_AS_ONE_BIG_FILE )
ENDIF( ${cpp_count} GREATER 3 )

IF( ( NOT OW_BUILD_AS_ONE_BIG_FILE ) OR NOT ( ${cpp_count} GREATER 3 ) )
    ADD_LIBRARY( ${MODULE_NAME} SHARED ${${MODULE_NAME}_COMBINER_SRC} )
ENDIF( ( NOT OW_BUILD_AS_ONE_BIG_FILE ) OR NOT ( ${cpp_count} GREATER 3 ) )

# Build module lib
#ADD_LIBRARY( ${MODULE_NAME} SHARED ${MODULES_SRC} )

TARGET_LINK_LIBRARIES( ${MODULE_NAME} ${OWKernelName} )

IF(MSVC_IDE)
    SET_TARGET_PROPERTIES( ${MODULE_NAME} PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)

# Copy local shaders to module target directory
IF( OW_COPY_SHADERS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/shaders )    # copy shaders only if the user wants it
    ADD_CUSTOM_TARGET( ${MODULE_NAME}_CopyShaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${MODULE_TARGET_DIR}/shaders/
        COMMENT "Copy shaders of ${MODULE_NAME}"
    )
    ADD_DEPENDENCIES( ${MODULE_NAME} ${MODULE_NAME}_CopyShaders )
ENDIF()

# Build unit tests
IF( OW_COMPILE_TESTS )
    # This ensures that the test is copied to the module directory
    SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MODULE_TARGET_DIR} )
    CXXTEST_ADD_TESTS_FROM_LIST( "${MODULES_SRC}"
                                 "${OWKernelName};${MODULE_NAME}"
                               )

    # Copy fixtures if they exist
    IF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures )
        ADD_CUSTOM_TARGET( ${MODULE_NAME}_CopyFixtures
            # as the "test" target runs in CMakes temporary build dir, the fixtures need to be placed there too.
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures ${CMAKE_BINARY_DIR}/modules/${MODULE_DIRNAME}/fixtures/
            COMMENT "Copy fixtures of ${MODULE_NAME}"
        )
        ADD_DEPENDENCIES( ${MODULE_NAME} ${MODULE_NAME}_CopyFixtures )
    ENDIF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures )

ENDIF( OW_COMPILE_TESTS )
