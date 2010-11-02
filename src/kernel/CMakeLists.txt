FILE( GLOB KERNEL_SRC "*.cpp" "*.h" )

FILE( GLOB_RECURSE MODULES_SRC "modules/*.cpp" "modules/*.h" )

FILE( GLOB KERNEL_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
FILE( GLOB KERNEL_COMBINER_SRC "combiner/*.cpp" "combiner/*.h" )

ADD_LIBRARY( OWkernel SHARED ${KERNEL_SRC} ${MODULES_SRC} ${KERNEL_EXCEPTIONS_SRC} ${KERNEL_COMBINER_SRC} )
TARGET_LINK_LIBRARIES( OWkernel OWcommon OWdataHandler OWge ${OPENSCENEGRAPH_LIBRARIES} ${CMAKE_DL_LIBS} )

IF(MSVC_IDE)
    SET_TARGET_PROPERTIES( OWkernel PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)

# Unit tests
IF( OW_COMPILE_TESTS )
    #    SET( testLibs guiqt4 )
  CXXTEST_ADD_TESTS_FROM_LIST( "${KERNEL_SRC}"
                               "OWkernel"
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
