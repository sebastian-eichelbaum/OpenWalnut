FILE( GLOB COMMON_SRC "*.cpp" "*.h" )
FILE( GLOB COMMON_DATASTRUCTURES_SRC "datastructures/*.cpp" "datastructures/*.h" )
FILE( GLOB COMMON_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
FILE( GLOB MATH_SRC "math/*.cpp" "math/*.h" )

ADD_SUBDIRECTORY( datastructures )
ADD_SUBDIRECTORY( constraints )
ADD_SUBDIRECTORY( math )

ADD_LIBRARY( OWcommon SHARED ${COMMON_SRC} ${COMMON_DATASTRUCTURES_SRC} ${COMMON_EXCEPTIONS_SRC} ${MATH_SRC} ${FIBERSIMILARITY_SRC} )
TARGET_LINK_LIBRARIES( OWcommon ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} )

# Set the prefix and suffix of dynamically loadable modules to match
# CMAKEs naming of them. The quotes are necessary so the shell doesn't mess
# with these defines. Unfortunately, the Visual Studio has problems handling
# the quotes on the command line. WSharedLib::getSystemPrefix and 
# getSystemSuffix hard codes the right strings for VC.
# Maybe you have to spend another 500 bucks to have your VC support quotes.
IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
	add_definitions( '-DW_LIB_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"' )
	add_definitions( '-DW_LIB_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"' )
ENDIF()

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${COMMON_SRC}"
                               "OWcommon"
                               "WSegmentationFault.cpp" # can't test seg-faults ;-)
                               "WTransferable.cpp"  # its an empty class
                               "WPropertyBase.cpp" # its an abstract class. Its small functionality is tested in WPropertyVariable_test.h
                               "WLimits.cpp" # there are only some definitions
                             )
ENDIF( OW_COMPILE_TESTS )
