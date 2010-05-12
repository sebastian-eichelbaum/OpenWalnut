FILE( GLOB COMMON_SRC "*.cpp" "*.h" )
FILE( GLOB COMMON_DATASTRUCTURES_SRC "datastructures/*.cpp" "datastructures/*.h" )
FILE( GLOB COMMON_EXCEPTIONS_SRC "exceptions/*.cpp" "exceptions/*.h" )
FILE( GLOB MATH_SRC "math/*.cpp" "math/*.h" )

ADD_SUBDIRECTORY( datastructures )
ADD_SUBDIRECTORY( constraints )
ADD_SUBDIRECTORY( math )

ADD_LIBRARY( common ${COMMON_SRC} ${COMMON_DATASTRUCTURES_SRC} ${COMMON_EXCEPTIONS_SRC} ${MATH_SRC} ${FIBERSIMILARITY_SRC} )
TARGET_LINK_LIBRARIES( common ${Boost_LIBRARIES} )


# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${COMMON_SRC}"
                               "common"
                               "WSegmentationFault.cpp" # can't test seg-faults ;-)
                               "WTransferable.cpp"  # its an empty class
                               "WPropertyBase.cpp" # its an abstract class. Its small functionality is tested in WPropertyVariable_test.h
                               "WLimits.cpp" # there are only some definitions
                             )
ENDIF( OW_COMPILE_TESTS )
