FILE( GLOB COMMON_SRC "*.[c,h]pp" )
FILE( GLOB COMMON_DATASTRUCTURES_SRC "datastructures/*.[c,h]pp" )
FILE( GLOB COMMON_EXCEPTIONS_SRC "exceptions/*.[c,h]pp" )

ADD_SUBDIRECTORY( datastructures )

ADD_LIBRARY( common SHARED ${COMMON_SRC} ${COMMON_DATASTRUCTURES_SRC} ${COMMON_EXCEPTIONS_SRC} )
TARGET_LINK_LIBRARIES( common ${Boost_LIBRARIES} )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${COMMON_SRC}"
                               "common"  # no libs for linking required
                               "WSegmentationFault.cpp"
                               "WStringUtils.h" # since unit tests are already included via the .h file
                             )
ENDIF( CXXTEST_FOUND )
