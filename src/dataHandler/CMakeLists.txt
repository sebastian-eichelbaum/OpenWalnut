# Needed because libeep has includes of form #include <cnt/cnt.h>
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext/libeep )
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext )

ADD_SUBDIRECTORY( io )
ADD_SUBDIRECTORY( datastructures )


FILE( GLOB DATAHANDLER_SRC "*.cpp" "*.h" )
FILE( GLOB DATAHANDLER_DATASTRUCTURES_SRC "datastructures/*.cpp" "datastructures/*.h" )
FILE( GLOB DATAHANDLER_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
FILE( GLOB DATAHANDLER_IO_SRC "io/*.cpp" "io/*.h" ) # former WLoader

#IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
#    FILE( GLOB EXCLUDE_biosig "io/*Biosig.cpp" "io/*Biosig.h" )
#    LIST( REMOVE_ITEM DATAHANDLER_IO_SRC ${EXCLUDE_biosig} )
#    ADD_LIBRARY( dataHandler ${DATAHANDLER_SRC} ${DATAHANDLER_EXCEPTIONS_SRC} ${DATAHANDLER_IO_SRC} ${DATAHANDLER_DATASTRUCTURES_SRC} )
#    TARGET_LINK_LIBRARIES( dataHandler common ${Boost_LIBRARIES} niftiio libeep ${OPENSCENEGRAPH_LIBRARIES})
#ELSE()
    ADD_LIBRARY( OWdataHandler SHARED ${DATAHANDLER_SRC} ${DATAHANDLER_EXCEPTIONS_SRC} ${DATAHANDLER_IO_SRC} ${DATAHANDLER_DATASTRUCTURES_SRC} )
    TARGET_LINK_LIBRARIES( OWdataHandler OWcommon ${Boost_LIBRARIES} OWext_niftiio OWext_biosig OWext_eep ${OPENSCENEGRAPH_LIBRARIES})
#ENDIF()
# Unit tests
IF( OW_COMPILE_TESTS )
    CXXTEST_ADD_TESTS_FROM_LIST( "${DATAHANDLER_SRC}" "OWdataHandler" "WDataTexture3D.cpp" "WDataHandlerEnums.cpp" )

    # Copy fixtures if existend
    IF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures )
        ADD_CUSTOM_TARGET( OWdataHandler_CopyFixtures
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures ${CMAKE_BINARY_DIR}/dataHandler/fixtures/
            COMMENT "Copy fixtures of OWdataHandler"
        )
        ADD_DEPENDENCIES( OWdataHandler OWdataHandler_CopyFixtures )
    ENDIF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures )
    IF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/io/test/fixtures )
        ADD_CUSTOM_TARGET( OWdataHandlerIO_CopyFixtures
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/io/test/fixtures ${CMAKE_BINARY_DIR}/dataHandler/io/fixtures/
            COMMENT "Copy fixtures of OWdataHandler IO"
        )
        ADD_DEPENDENCIES( OWdataHandler OWdataHandlerIO_CopyFixtures )
    ENDIF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/io/test/fixtures )
ENDIF( OW_COMPILE_TESTS )
