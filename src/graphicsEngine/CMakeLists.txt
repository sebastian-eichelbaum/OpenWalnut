# Package dependencies:
ADD_SUBDIRECTORY( exceptions )
ADD_SUBDIRECTORY( callbacks )
ADD_SUBDIRECTORY( platformDependent )

FILE( GLOB GE_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
FILE( GLOB GE_CALLBACKS_SRC "callbacks/*.cpp" "callbacks/*.h" )
FILE( GLOB GE_ALGORITHMS_SRC "algorithms/*.cpp" "algorithms/*.h" )
IF( CMAKE_HOST_SYSTEM MATCHES Darwin )
  FILE( GLOB GE_PLATFORMDEPENDENT_SRC 
    "platformDependent/WGraphicsEngineMac.cpp" 
    "platformDependent/WGraphicsEngineMac.h"
    "platformDependent/WGEViewerMac.cpp" 
    "platformDependent/WGEViewerMac.h"
    "platformDependent/WGEGraphicsWindowMac.cpp" 
    "platformDependent/WGEGraphicsWindowMac.h" )
ELSE()
  FILE( GLOB GE_PLATFORMDEPENDENT_SRC 
    "platformDependent/WGraphicsEngineAll.cpp" 
  "platformDependent/WGraphicsEngineAll.h"
  "platformDependent/WGEViewerAll.cpp" 
  "platformDependent/WGEViewerAll.h"
  "platformDependent/WGEGraphicsWindowAll.cpp" 
  "platformDependent/WGEGraphicsWindowAll.h" )
ENDIF()
FILE( GLOB GE_SRC "*.cpp" "*.h" )

ADD_LIBRARY( OWge SHARED ${GE_SRC} ${GE_EXCEPTIONS_SRC} ${GE_CALLBACKS_SRC} ${GE_ALGORITHMS_SRC} ${GE_PLATFORMDEPENDENT_SRC} )
TARGET_LINK_LIBRARIES( OWge OWcommon ${OPENGL_gl_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES} )

IF(MSVC_IDE)
    SET_TARGET_PROPERTIES( OWge PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)

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
