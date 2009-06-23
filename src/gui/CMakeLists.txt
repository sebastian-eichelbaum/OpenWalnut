ADD_SUBDIRECTORY( qt4 )

FILE( GLOB GUI_SRC "*.cpp" )

ADD_LIBRARY( gui SHARED ${GUI_SRC} )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${GUI_SRC}"
                               ""  # no libs for linking required
                             )
ENDIF( CXXTEST_FOUND )
