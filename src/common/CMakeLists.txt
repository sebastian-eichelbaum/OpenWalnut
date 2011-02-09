FILE( GLOB COMMON_SRC "*.cpp" "*.h" )
FILE( GLOB COMMON_DATASTRUCTURES_SRC "datastructures/*.cpp" "datastructures/*.h" )
FILE( GLOB COMMON_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
FILE( GLOB MATH_SRC "math/*.cpp" "math/*.h" )

ADD_SUBDIRECTORY( datastructures )
ADD_SUBDIRECTORY( constraints )
ADD_SUBDIRECTORY( math )

SET( LIB_NAME "${OWCommonName}" )

SET( ${LIB_NAME}_COMBINER_SRC ${COMMON_SRC} ${COMMON_DATASTRUCTURES_SRC} ${COMMON_EXCEPTIONS_SRC} ${MATH_SRC} )
SET( ${LIB_NAME}_ALL_CPP ${COMMON_SRC} ${COMMON_DATASTRUCTURES_SRC} ${COMMON_EXCEPTIONS_SRC} ${MATH_SRC}  )
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

TARGET_LINK_LIBRARIES( ${LIB_NAME} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} ${OPENSCENEGRAPH_LIBRARIES} )

IF(MSVC_IDE)
    SET_TARGET_PROPERTIES( ${LIB_NAME} PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)

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
                               "${LIB_NAME}"
                               "WSegmentationFault.cpp" # can't test seg-faults ;-)
                               "WTransferable.cpp"  # its an empty class
                               "WPropertyBase.cpp" # its an abstract class. Its small functionality is tested in WPropertyVariable_test.h
                               "WLimits.cpp" # there are only some definitions
                             )
ENDIF( OW_COMPILE_TESTS )
