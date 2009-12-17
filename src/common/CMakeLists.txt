FILE( GLOB COMMON_SRC "*.cpp" )
FILE( GLOB COMMON_DATASTRUCTURES_SRC "datastructures/*.cpp" )
FILE( GLOB COMMON_EXCEPTIONS_SRC "exceptions/*.cpp" )

ADD_SUBDIRECTORY( datastructures )

ADD_LIBRARY( common SHARED ${COMMON_SRC} ${COMMON_DATASTRUCTURES_SRC} ${COMMON_EXCEPTIONS_SRC} )
TARGET_LINK_LIBRARIES( common ${Boost_LIBRARIES} )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${COMMON_SRC}"
                               "common"  # no libs for linking required
                               "WSegmentationFault.cpp"
                               "WTransferable.cpp"
                               "WConditionSet.cpp"
                               "WPrototyped.cpp"
                               "WStringUtils.h" # since unit tests are already included via the .h file
                             )
ENDIF( CXXTEST_FOUND )
