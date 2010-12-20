# Package dependencies:
ADD_SUBDIRECTORY( exceptions )
ADD_SUBDIRECTORY( callbacks )
ADD_SUBDIRECTORY( platformDependent )

FILE( GLOB GE_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
FILE( GLOB GE_CALLBACKS_SRC "callbacks/*.cpp" "callbacks/*.h" )
FILE( GLOB GE_ALGORITHMS_SRC "algorithms/*.cpp" "algorithms/*.h" )
FILE( GLOB GE_GEODES_SRC "geodes/*.cpp" "geodes/*.h" )

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

SET( LIB_NAME "${OWgeName}" )

SET( ${LIB_NAME}_COMBINER_SRC ${GE_EXCEPTIONS_SRC} ${GE_CALLBACKS_SRC} ${GE_ALGORITHMS_SRC} ${GE_PLATFORMDEPENDENT_SRC} ${GE_SRC} )
SET( ${LIB_NAME}_ALL_CPP ${GE_EXCEPTIONS_SRC} ${GE_CALLBACKS_SRC} ${GE_ALGORITHMS_SRC} ${GE_PLATFORMDEPENDENT_SRC} ${GE_SRC} )
FILE( GLOB_RECURSE ${LIB_NAME}_REMOVE_H "*.h"  )

LIST( REMOVE_ITEM ${LIB_NAME}_ALL_CPP ${${LIB_NAME}_REMOVE_H} )

IF( OW_BUILD_AS_ONE_BIG_FILE )
	SET( UNITY_BUILD_FILE_NAME "${LIB_NAME}UnityBuildFile.cpp" )
	GENERATE_UNITY_BUILD_FILE( ${LIB_NAME}_ALL_CPP )
	
	ADD_LIBRARY( ${LIB_NAME} SHARED ${UNITY_BUILD_FILE_NAME} ${${LIB_NAME}_COMBINER_SRC} )
ENDIF( OW_BUILD_AS_ONE_BIG_FILE )

IF( NOT OW_BUILD_AS_ONE_BIG_FILE )
    ADD_LIBRARY( ${LIB_NAME} SHARED ${${LIB_NAME}_COMBINER_SRC} )
ENDIF( NOT OW_BUILD_AS_ONE_BIG_FILE )

TARGET_LINK_LIBRARIES( ${LIB_NAME} ${OWCommonName} ${OPENGL_gl_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES} )

IF(MSVC_IDE)
    SET_TARGET_PROPERTIES( ${LIB_NAME} PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${GE_SRC}" "${LIB_NAME};${OWKernelName}"
          "WGESignals.cpp"
          "WGraphicsEngine.cpp"
          "WGEScene.cpp"
          "WGEViewer.cpp"
          "WGEGraphicsWindow.cpp"
          "WGEGroupNode.cpp"
          )
ENDIF( OW_COMPILE_TESTS )
