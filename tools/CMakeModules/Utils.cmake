FUNCTION( SPLIT_VERSION_STRING _VersionString _Major _Minor _Patch )
  STRING( STRIP _VersionString ${_VersionString} )
  STRING( REGEX MATCH "^[0-9]+" _MajorProxy "${_VersionString}" )
  STRING( REGEX MATCH "[^0-9][0-9]+[^0-9]" _MinorProxy "${_VersionString}" )
  STRING( REGEX MATCH "[0-9]+" _MinorProxy "${_MinorProxy}" )
  STRING( REGEX MATCH "[0-9]+$" _PatchProxy "${_VersionString}" )
  SET( ${_Major} "${_MajorProxy}" PARENT_SCOPE )
  SET( ${_Minor} "${_MinorProxy}" PARENT_SCOPE )
  SET( ${_Patch} "${_PatchProxy}" PARENT_SCOPE )
ENDFUNCTION( SPLIT_VERSION_STRING )


FUNCTION( ASSERT_GE_VERSION _PackageName _ActualVersion _MinimumVersion )
  SPLIT_VERSION_STRING( ${_ActualVersion} _ActualMajor _ActualMinor _ActualPatch )
  SPLIT_VERSION_STRING( ${_MinimumVersion} _MinimumMajor _MinimumMinor _MinimumPatch )
  SET( _FAIL_MSG "Version conflict for package ${_PackageName}: needed at least ${_MinimumVersion} but got: ${_ActualVersion}" )

  # CMake 2.6 issue:
  # The reason for writing here stupid code and repeating my self is that cmake
  # is not able to evaluate AND expressions before OR expressions nor it does 
  # not support operator precedence. Just look into the book of crap:
  # "Mastering CMake" page 38 for the most moronic way of operator precedence!
  IF( _ActualMajor LESS _MinimumMajor )
      MESSAGE( FATAL_ERROR "Assertion failed: ${_FAIL_MSG}" )
  ELSEIF( _ActualMajor EQUAL _MinimumMajor AND _ActualMinor LESS _MinimumMinor )
      MESSAGE( FATAL_ERROR "Assertion failed: ${_FAIL_MSG}" )
  ELSEIF( _ActualMajor EQUAL _MinimumMajor AND _ActualMinor EQUAL _MinimumMinor AND  _ActualPatch LESS _MinimumPatch )
      MESSAGE( FATAL_ERROR "Assertion failed: ${_FAIL_MSG}" )
  ENDIF()
ENDFUNCTION( ASSERT_GE_VERSION )

# Recursively searches compile files (headers, sources). 
# _DirString: where to search
# _CPPFiles contains the cpp files afterwards
# _HFiles contains the h files afterwards, without tests
# _TestFiles contains only the test headers
FUNCTION( COLLECT_COMPILE_FILES _DirString _CPPFiles _HFiles _TestFiles )
    # recursively get all files
    FILE( GLOB_RECURSE CPP_FILES ${_DirString}/*.cpp )
    FILE( GLOB_RECURSE H_FILES   ${_DirString}/*.h )
    FILE( GLOB_RECURSE TEST_FILES   ${_DirString}/*_test.h )

    # Unfortunately, this will contain the tests -> remove them explicitly
    FOREACH( file ${TEST_FILES} )
        LIST( REMOVE_ITEM H_FILES ${file} )
    ENDFOREACH( file )

    SET( ${_CPPFiles} "${CPP_FILES}" PARENT_SCOPE )
    SET( ${_HFiles} "${H_FILES}" PARENT_SCOPE )
    SET( ${_TestFiles} "${TEST_FILES}" PARENT_SCOPE )
ENDFUNCTION( COLLECT_COMPILE_FILES )

