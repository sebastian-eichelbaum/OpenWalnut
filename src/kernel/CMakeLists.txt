FILE( GLOB KERNEL_SRC "*.cpp" )

ADD_LIBRARY( kernel SHARED ${KERNEL_SRC} )
TARGET_LINK_LIBRARIES( kernel common ge )


# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TESTS_FROM_LIST( "${KERNEL_SRC}"
                               ""  # no libs for linking required
                               "WKernel.cpp"
                               "WModule.cpp"
                               "WTestModule.cpp"
                               "WNavigationSliceModule.cpp"
                             )
ENDIF( CXXTEST_FOUND )
