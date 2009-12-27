FILE( GLOB COMMON_SRC "*.cpp" )
FILE( GLOB COMMON_DATASTRUCTURES_SRC "datastructures/*.cpp" )
FILE( GLOB COMMON_EXCEPTIONS_SRC "exceptions/*.cpp" )

ADD_SUBDIRECTORY( datastructures )

IF( CMAKE_HOST_SYSTEM MATCHES Windows )
    ADD_LIBRARY( common ${COMMON_SRC} ${COMMON_DATASTRUCTURES_SRC} ${COMMON_EXCEPTIONS_SRC} )
ELSE()
    ADD_LIBRARY( common SHARED ${COMMON_SRC} ${COMMON_DATASTRUCTURES_SRC} ${COMMON_EXCEPTIONS_SRC} )
ENDIF()

TARGET_LINK_LIBRARIES( common ${Boost_LIBRARIES} )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${COMMON_SRC}"
                               "common;utils"
                               "WSegmentationFault.cpp"
                               "WTransferable.cpp"
                               "WConditionSet.cpp"
                               "WPrototyped.cpp"
                               "WLimits.cpp" # there are only some definitions
                             )
ENDIF( CXXTEST_FOUND )
