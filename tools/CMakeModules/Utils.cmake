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

