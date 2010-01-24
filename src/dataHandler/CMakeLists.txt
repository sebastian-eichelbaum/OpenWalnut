ADD_SUBDIRECTORY( io )

IF( CMAKE_HOST_SYSTEM MATCHES Windows )
    FILE( GLOB DATAHANDLER_SRC "*.cpp" "*.h" )
    FILE( GLOB DATAHANDLER_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
    FILE( GLOB DATAHANDLER_IO_SRC "io/*.cpp" "io/*.h" ) # former WLoader
    FILE( GLOB EXCLUDE_biosig "io/*Biosig.cpp" "io/*Biosig.h" )
    LIST( REMOVE_ITEM DATAHANDLER_IO_SRC ${EXCLUDE_biosig} )

    ADD_LIBRARY( dataHandler ${DATAHANDLER_SRC} ${DATAHANDLER_EXCEPTIONS_SRC} ${DATAHANDLER_IO_SRC} )
    TARGET_LINK_LIBRARIES( dataHandler common math ${Boost_LIBRARIES} niftiio ${OPENSCENEGRAPH_LIBRARIES})
ELSE()
    FILE( GLOB DATAHANDLER_SRC "*.cpp" "*.h" )
    FILE( GLOB DATAHANDLER_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
    FILE( GLOB DATAHANDLER_IO_SRC "io/*.cpp" "io/*.h" ) # former WLoader

    ADD_LIBRARY( dataHandler SHARED ${DATAHANDLER_SRC} ${DATAHANDLER_EXCEPTIONS_SRC} ${DATAHANDLER_IO_SRC} )
    TARGET_LINK_LIBRARIES( dataHandler common math ${Boost_LIBRARIES} niftiio biosig ${OPENSCENEGRAPH_LIBRARIES})
ENDIF()
# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${DATAHANDLER_SRC}" "dataHandler" "WDataTexture3D.cpp" )
ENDIF( OW_COMPILE_TESTS )
