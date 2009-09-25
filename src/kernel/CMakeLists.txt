FILE( GLOB KERNEL_SRC "*.cpp" )
FILE( GLOB MODULES_SRC "../modules/navigationSlices/*.cpp" )

ADD_SUBDIRECTORY( exceptions )
FILE( GLOB KERNEL_EXCEPTIONS_SRC "exceptions/*.cpp" )

ADD_LIBRARY( kernel SHARED ${KERNEL_SRC} ${MODULES_SRC} ${KERNEL_EXCEPTIONS_SRC} )
TARGET_LINK_LIBRARIES( kernel common ge ${OPENSCENEGRAPH_LIBRARIES} )


# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${KERNEL_SRC}"
                               ""  # no libs for linking required
                               "WKernel.cpp"
                               "WModule.cpp"
                               "../modules/navigationSlices/WNavigationSliceModule.cpp"
                             )
ENDIF( CXXTEST_FOUND )
