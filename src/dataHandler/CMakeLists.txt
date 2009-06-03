ADD_SUBDIRECTORY( io )
ADD_SUBDIRECTORY( exceptions )

FILE( GLOB DATAHANDLER_SRC "*.cpp" )
FILE( GLOB DATAHANDLER_EXCEPTIONS_SRC "exceptions/*.cpp" )
ADD_LIBRARY( dataHandler SHARED ${DATAHANDLER_SRC} ${DATAHANDLER_EXCEPTIONS_SRC} )

# Unit tests
IF( CXXTEST_FOUND )
    CXXTEST_ADD_TESTS_FROM_LIST( "${DATAHANDLER_SRC}"
                                 dataHandler
                                 "WValueSetBase.cpp" # its abstract
                                 )
ENDIF( CXXTEST_FOUND )
