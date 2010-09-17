# Package dependencies:
ADD_SUBDIRECTORY( exceptions )
ADD_SUBDIRECTORY( callbacks )

FILE( GLOB GE_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
FILE( GLOB GE_CALLBACKS_SRC "callbacks/*.cpp" "callbacks/*.h" )
FILE( GLOB GE_ALGORITHMS_SRC "algorithms/*.cpp" "algorithms/*.h" )
FILE( GLOB GE_SRC "*.cpp" "*.h" )

IF ( OPENCL_FOUND )
  ADD_SUBDIRECTORY( OpenCL )
  FILE( GLOB GE_OPENCL_SRC "OpenCL/*.cpp" "OpenCL/*.h" )
ENDIF ( OPENCL_FOUND )

ADD_LIBRARY( OWge SHARED ${GE_SRC} ${GE_EXCEPTIONS_SRC} ${GE_CALLBACKS_SRC} ${GE_ALGORITHMS_SRC} ${GE_OPENCL_SRC} )
TARGET_LINK_LIBRARIES( OWge OWcommon ${OPENGL_gl_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES} ${OPENCL_LIBRARY} )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${GE_SRC}" "OWge;OWkernel"
          "WGESignals.cpp"
          "WGraphicsEngine.cpp"
          "WGEScene.cpp"
          "WGEViewer.cpp"
          "WGEGraphicsWindow.cpp"
          "WGEGroupNode.cpp"
          )
ENDIF( OW_COMPILE_TESTS )
