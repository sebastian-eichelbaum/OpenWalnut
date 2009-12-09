ADD_SUBDIRECTORY( qt4 )

FILE( GLOB GUI_SRC "*.[c,h]pp" )

ADD_LIBRARY( gui SHARED ${GUI_SRC} )
TARGET_LINK_LIBRARIES( gui common )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${GUI_SRC}"
                               "common"
                             )
ENDIF( CXXTEST_FOUND )
