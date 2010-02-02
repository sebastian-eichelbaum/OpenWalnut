# Package dependencies:
ADD_SUBDIRECTORY( exceptions )

FILE( GLOB GE_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
FILE( GLOB GE_SRC "*.cpp" "*.h" )

IF( CMAKE_HOST_SYSTEM MATCHES Windows )
    ADD_LIBRARY( ge ${GE_SRC} ${GE_EXCEPTIONS_SRC} )
ELSE()
    ADD_LIBRARY( ge SHARED ${GE_SRC} ${GE_EXCEPTIONS_SRC} )
ENDIF()

TARGET_LINK_LIBRARIES( ge common ${OPENGL_gl_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES} )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${GE_SRC}" "ge;math"
          "WGESignals.cpp"
          "WGraphicsEngine.cpp"
          "WGEScene.cpp"
          "WGEResourceManager.cpp"
          "WGEViewer.cpp"
          "WGEGraphicsWindow.cpp"
          "WGEGroupNode.cpp"
          )
ENDIF( OW_COMPILE_TESTS )
