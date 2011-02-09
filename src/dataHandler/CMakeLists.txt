# Needed because libeep has includes of form #include <cnt/cnt.h>
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext/libeep )
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext )

ADD_SUBDIRECTORY( io )
ADD_SUBDIRECTORY( datastructures )


FILE( GLOB DATAHANDLER_SRC "*.cpp" "*.h" )
FILE( GLOB DATAHANDLER_DATASTRUCTURES_SRC "datastructures/*.cpp" "datastructures/*.h" )
FILE( GLOB DATAHANDLER_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
FILE( GLOB DATAHANDLER_IO_SRC "io/*.cpp" "io/*.h" ) # former WLoader

SET( LIB_NAME "${OWDatahandlerName}" )

SET( ${LIB_NAME}_COMBINER_SRC ${DATAHANDLER_SRC} ${DATAHANDLER_DATASTRUCTURES_SRC} ${DATAHANDLER_EXCEPTIONS_SRC} ${DATAHANDLER_IO_SRC} )
SET( ${LIB_NAME}_ALL_CPP ${DATAHANDLER_SRC} ${DATAHANDLER_DATASTRUCTURES_SRC} ${DATAHANDLER_EXCEPTIONS_SRC} ${DATAHANDLER_IO_SRC}  )
FILE( GLOB_RECURSE ${LIB_NAME}_REMOVE_H "*.h"  )

LIST( REMOVE_ITEM ${LIB_NAME}_ALL_CPP ${${LIB_NAME}_REMOVE_H} )

IF( OW_BUILD_AS_ONE_BIG_FILE )
	SET( UNITY_BUILD_FILE_NAME "${LIB_NAME}UnityBuildFile.cpp" )
	GENERATE_UNITY_BUILD_FILE( ${LIB_NAME}_ALL_CPP )
	ADD_LIBRARY( ${LIB_NAME} SHARED ${UNITY_BUILD_FILE_NAME} ${${LIB_NAME}_COMBINER_SRC} )
ENDIF( OW_BUILD_AS_ONE_BIG_FILE )

IF( NOT OW_BUILD_AS_ONE_BIG_FILE )
    ADD_LIBRARY( ${LIB_NAME} SHARED ${${LIB_NAME}_COMBINER_SRC} )
ENDIF( NOT OW_BUILD_AS_ONE_BIG_FILE )

IF( ITK_FOUND AND OW_USE_ITK )
    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${OWCommonName} ${Boost_LIBRARIES} OWext_niftiio OWext_biosig OWext_eep ${OPENSCENEGRAPH_LIBRARIES} ${ITK_LIBRARIES} )
ELSE()
    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${OWCommonName} ${Boost_LIBRARIES} OWext_niftiio OWext_biosig OWext_eep ${OPENSCENEGRAPH_LIBRARIES} )
ENDIF()

IF(MSVC_IDE)
    SET_TARGET_PROPERTIES( ${LIB_NAME} PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)

# Unit tests
IF( OW_COMPILE_TESTS )
    # in case of not installed ITK the unit test won't compile, so we remove it
    IF( NOT ITK_FOUND OR NOT OW_USE_ITK )
        LIST( REMOVE_ITEM DATAHANDLER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/WITKImageConversion.cpp" )
    ENDIF()

    CXXTEST_ADD_TESTS_FROM_LIST( "${DATAHANDLER_SRC}" "${LIB_NAME}"
        "WDataTexture3D.cpp"
        "WDataHandlerEnums.cpp" )

    # Copy fixtures if existend
    IF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures )
        ADD_CUSTOM_TARGET( ${LIB_NAME}_CopyFixtures
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures ${CMAKE_BINARY_DIR}/dataHandler/fixtures/
            COMMENT "Copy fixtures of ${LIB_NAME}"
        )
        ADD_DEPENDENCIES( ${LIB_NAME} ${LIB_NAME}_CopyFixtures )
    ENDIF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures )
    IF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/io/test/fixtures )
        ADD_CUSTOM_TARGET( ${LIB_NAME}IO_CopyFixtures
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/io/test/fixtures ${CMAKE_BINARY_DIR}/dataHandler/io/fixtures/
            COMMENT "Copy fixtures of ${LIB_NAME} IO"
        )
        ADD_DEPENDENCIES( ${LIB_NAME} ${LIB_NAME}IO_CopyFixtures )
    ENDIF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/io/test/fixtures )
ENDIF( OW_COMPILE_TESTS )
