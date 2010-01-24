FILE( GLOB KERNEL_SRC "*.cpp" "*.h" )

FILE( GLOB_RECURSE MODULES_SRC "../modules/*.cpp" "../modules/*.h" )

ADD_SUBDIRECTORY( exceptions )
FILE( GLOB KERNEL_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )

IF( CMAKE_HOST_SYSTEM MATCHES Windows )
    ADD_LIBRARY( kernel ${KERNEL_SRC} ${MODULES_SRC} ${KERNEL_EXCEPTIONS_SRC} )
ELSE()
    ADD_LIBRARY( kernel SHARED ${KERNEL_SRC} ${MODULES_SRC} ${KERNEL_EXCEPTIONS_SRC} )
ENDIF()

TARGET_LINK_LIBRARIES( kernel common dataHandler ge math ${OPENSCENEGRAPH_LIBRARIES} )


# Unit tests
IF( OW_COMPILE_TESTS )
    #    SET( testLibs guiqt4 )
  CXXTEST_ADD_TESTS_FROM_LIST( "${KERNEL_SRC}"
                               "kernel"
                               "WKernel.cpp"
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
