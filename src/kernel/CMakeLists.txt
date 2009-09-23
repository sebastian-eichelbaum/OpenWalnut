FILE( GLOB KERNEL_SRC "*.cpp" )
FILE( GLOB_RECURSE MODULES_SRC "../modules/*.cpp" )

ADD_LIBRARY( kernel SHARED ${KERNEL_SRC} ${MODULES_SRC} )
TARGET_LINK_LIBRARIES( kernel common ge ${OPENSCENEGRAPH_LIBRARIES} )


# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${KERNEL_SRC}"
                               ""  # no libs for linking required
                               "WKernel.cpp"
                               "WModule.cpp"
                               "../modules/navigationSlices/WNavigationSliceModule.cpp"
                               "../modules/eegTest/WEEGTestModule.cpp"
                             )
ENDIF( CXXTEST_FOUND )
