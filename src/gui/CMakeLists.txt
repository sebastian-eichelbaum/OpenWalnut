ADD_SUBDIRECTORY( qt4 )

FILE( GLOB GUI_SRC "*.cpp" )

IF( CMAKE_HOST_SYSTEM MATCHES Windows )
    ADD_LIBRARY( gui ${GUI_SRC} )
ELSE()
    ADD_LIBRARY( gui SHARED ${GUI_SRC} )
ENDIF()

TARGET_LINK_LIBRARIES( gui common )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${GUI_SRC}"
                               "common;utils"
                               "WGUI.cpp"
                             )
ENDIF( OW_COMPILE_TESTS )
