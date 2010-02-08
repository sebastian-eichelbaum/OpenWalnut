FILE( GLOB COMMON_SRC "*.cpp" "*.h" )
FILE( GLOB COMMON_DATASTRUCTURES_SRC "datastructures/*.cpp" "datastructures/*.h" )
FILE( GLOB COMMON_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )

ADD_SUBDIRECTORY( datastructures )
ADD_SUBDIRECTORY( constraints )

IF( CMAKE_HOST_SYSTEM MATCHES Windows )
    ADD_LIBRARY( common ${COMMON_SRC} ${COMMON_DATASTRUCTURES_SRC} ${COMMON_EXCEPTIONS_SRC} )
ELSE()
    ADD_LIBRARY( common SHARED ${COMMON_SRC} ${COMMON_DATASTRUCTURES_SRC} ${COMMON_EXCEPTIONS_SRC} )
ENDIF()

TARGET_LINK_LIBRARIES( common ${Boost_LIBRARIES} )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${COMMON_SRC}"
                               "common"
                               "WSegmentationFault.cpp"
                               "WTransferable.cpp"
                               "WConditionSet.cpp"
                               "WPrototyped.cpp"
                               "WLimits.cpp" # there are only some definitions
                             )
ENDIF( OW_COMPILE_TESTS )
