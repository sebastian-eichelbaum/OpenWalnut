ADD_SUBDIRECTORY( qt4 )

FILE( GLOB GUI_SRC "*.cpp" "*.h" )

ADD_LIBRARY( OWgui ${GUI_SRC} )
TARGET_LINK_LIBRARIES( OWgui OWcommon )

IF(MSVC_IDE)
    SET_TARGET_PROPERTIES( OWgui PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${GUI_SRC}"
                               "OWcommon"
                               "WGUI.cpp"
                             )
ENDIF( OW_COMPILE_TESTS )
