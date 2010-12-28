FILE( GLOB KERNEL_SRC "*.cpp" "*.h" )

FILE( GLOB_RECURSE MODULES_SRC "modules/*.cpp" "modules/*.h" )

FILE( GLOB KERNEL_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
FILE( GLOB KERNEL_COMBINER_SRC "combiner/*.cpp" "combiner/*.h" )

SET( LIB_NAME "${OWKernelName}" )

SET( ${LIB_NAME}_COMBINER_SRC ${KERNEL_SRC} ${MODULES_SRC} ${KERNEL_EXCEPTIONS_SRC} ${KERNEL_COMBINER_SRC} )
SET( ${LIB_NAME}_ALL_CPP ${KERNEL_SRC} ${MODULES_SRC} ${KERNEL_EXCEPTIONS_SRC} ${KERNEL_COMBINER_SRC} )
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

TARGET_LINK_LIBRARIES( ${LIB_NAME} ${OWCommonName} ${OWDatahandlerName} ${OWgeName} ${OPENSCENEGRAPH_LIBRARIES} ${CMAKE_DL_LIBS} )

IF(MSVC_IDE)
    SET_TARGET_PROPERTIES( ${LIB_NAME} PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)

# Unit tests
IF( OW_COMPILE_TESTS )
    #    SET( testLibs guiqt4 )
  CXXTEST_ADD_TESTS_FROM_LIST( "${KERNEL_SRC}"
                               "${LIB_NAME}"
                               "WKernel.cpp"
                               "WBatchLoader.cpp"
                               "WModule.cpp"
                               "WModuleInputConnector.cpp"
                               "WModuleOutputConnector.cpp"
                               "WModuleSignals.cpp"
                               "WModuleConnectorSignals.cpp"
                               "WModuleInputData.h"
                               "WModuleOutputData.h"
                               "WModuleInputData.cpp"
                               "WModuleOutputData.cpp"
                               "WModuleContainer.cpp"
                               "WModuleFactory.cpp"
                               "../modules/navSlices/WMNavSlices.cpp"
                             )
ENDIF( OW_COMPILE_TESTS )
