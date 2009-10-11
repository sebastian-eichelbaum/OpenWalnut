FILE( GLOB COMMON_SRC "*.[c,h]pp" )

ADD_LIBRARY( common SHARED ${COMMON_SRC} )
TARGET_LINK_LIBRARIES( common ${Boost_LIBRARIES} )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${COMMON_SRC}"
                               "common"  # no libs for linking required
                                ${Boost_LIBRARIES}
                               "WSegmentationFault.cpp"
                               "WStringUtils.hpp" # since unit tests are already included via the .hpp file
                             )
ENDIF( CXXTEST_FOUND )
