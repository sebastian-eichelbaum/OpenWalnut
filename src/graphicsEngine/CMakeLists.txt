# Package dependencies:
ADD_SUBDIRECTORY( exceptions )
ADD_SUBDIRECTORY( callbacks )
ADD_SUBDIRECTORY( platformDependent )
ADD_SUBDIRECTORY( widgets )
ADD_SUBDIRECTORY( offscreen )
ADD_SUBDIRECTORY( shaders )
ADD_SUBDIRECTORY( geodes )
ADD_SUBDIRECTORY( postprocessing )
ADD_SUBDIRECTORY( animation )
ADD_SUBDIRECTORY( algorithms )

FILE( GLOB GE_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
FILE( GLOB GE_CALLBACKS_SRC "callbacks/*.cpp" "callbacks/*.h" )
FILE( GLOB GE_ALGORITHMS_SRC "algorithms/*.cpp" "algorithms/*.h" )
FILE( GLOB GE_SHADERS_SRC "shaders/*.cpp" "shaders/*.h" )
FILE( GLOB GE_OFFSCREEN_SRC "offscreen/*.cpp" "offscreen/*.h" )
FILE( GLOB GE_GEODES_SRC "geodes/*.cpp" "geodes/*.h" )
FILE( GLOB_RECURSE GE_WIDGETS_SRC "widgets/*.cpp" "widgets/*.h" )
FILE( GLOB GE_POSTPROCESSING_SRC "postprocessing/*.cpp" "postprocessing/*.h" )
FILE( GLOB GE_ANIMATION_SRC "animation/*.cpp" "animation/*.h" )

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

IF ( OPENCL_FOUND )
  ADD_SUBDIRECTORY( OpenCL )
  FILE( GLOB GE_OPENCL_SRC "OpenCL/*.cpp" "OpenCL/*.h" )
ENDIF ( OPENCL_FOUND )

SET( LIB_NAME "${OWgeName}" )

SET( ${LIB_NAME}_COMBINER_SRC ${GE_EXCEPTIONS_SRC} ${GE_CALLBACKS_SRC} ${GE_ALGORITHMS_SRC} ${GE_OFFSCREEN_SRC} ${GE_SHADERS_SRC}
        ${GE_PLATFORMDEPENDENT_SRC} ${GE_SRC} ${GE_GEODES_SRC} ${GE_WIDGETS_SRC} ${GE_POSTPROCESSING_SRC} ${GE_OPENCL_SRC} ${GE_ANIMATION_SRC} )
SET( ${LIB_NAME}_ALL_CPP ${GE_EXCEPTIONS_SRC} ${GE_CALLBACKS_SRC} ${GE_ALGORITHMS_SRC} ${GE_OFFSCREEN_SRC} ${GE_SHADERS_SRC}
        ${GE_PLATFORMDEPENDENT_SRC} ${GE_SRC} ${GE_GEODES_SRC} ${GE_WIDGETS_SRC} ${GE_POSTPROCESSING_SRC} ${OPENCL_LIBRARY} ${GE_ANIMATION_SRC} )
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

TARGET_LINK_LIBRARIES( ${LIB_NAME} ${OWCommonName} ${OWDatahandlerName} ${OPENGL_gl_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES} )

IF(MSVC_IDE)
    SET_TARGET_PROPERTIES( ${LIB_NAME} PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${${LIB_NAME}_COMBINER_SRC}" "${LIB_NAME};${OWKernelName}"
          "WGESignals.cpp"
          "WGraphicsEngine.cpp"
          "WGEScene.cpp"
          "WGEViewer.cpp"
          "WGEGraphicsWindow.cpp"
          "WGEGroupNode.cpp"
          )

    # Copy fixtures if existend
    IF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures )
        ADD_CUSTOM_TARGET( ${LIB_NAME}_CopyFixtures
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures ${CMAKE_BINARY_DIR}/graphicsEngine/fixtures/
            COMMENT "Copy fixtures of ${LIB_NAME}"
        )
        ADD_DEPENDENCIES( ${LIB_NAME} ${LIB_NAME}_CopyFixtures )
    ENDIF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures )
    IF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/test/fixtures )
        ADD_CUSTOM_TARGET( ${LIB_NAME}_CopyFixtures
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/test/fixtures ${CMAKE_BINARY_DIR}/graphicsEngine/algorithms/fixtures/
            COMMENT "Copy fixtures of ${LIB_NAME}"
        )
        ADD_DEPENDENCIES( ${LIB_NAME} ${LIB_NAME}_CopyFixtures )
    ENDIF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/test/fixtures )
ENDIF( OW_COMPILE_TESTS )
