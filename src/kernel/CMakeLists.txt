FILE( GLOB KERNEL_SRC "*.[c,h]pp" )

FILE( GLOB_RECURSE MODULES_SRC "../modules/*.[c,h]pp" )

ADD_SUBDIRECTORY( exceptions )
FILE( GLOB KERNEL_EXCEPTIONS_SRC "exceptions/*.[c,h]pp" )

ADD_LIBRARY( kernel SHARED ${KERNEL_SRC} ${MODULES_SRC} ${KERNEL_EXCEPTIONS_SRC} )
TARGET_LINK_LIBRARIES( kernel common dataHandler ge math ${OPENSCENEGRAPH_LIBRARIES} )


# Unit tests
IF( CXXTEST_FOUND )
    SET( testLibs guiqt4 )
  CXXTEST_ADD_TESTS_FROM_LIST( "${KERNEL_SRC}"
                               ${testLibs}
                               "WKernel.cpp"
                               "WModule.cpp"
                               "WModuleInputConnector.cpp"
                               "WModuleOutputConnector.cpp"
                               "WModuleInputData.hpp"
                               "WModuleOutputData.hpp"
                               "WModuleInputData.cpp"
                               "WModuleOutputData.cpp"
                               "../modules/navigationSlices/WNavigationSliceModule.cpp"
                               "../modules/eegTest/WEEGTestModule.cpp"
                             )
ENDIF( CXXTEST_FOUND )
