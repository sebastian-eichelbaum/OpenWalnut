#---------------------------------------------------------------------------
#
# Project: OpenWalnut ( http://www.openwalnut.org )
#
# Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
# For more information see http:#www.openwalnut.org/copying
#
# This file is part of OpenWalnut.
#
# OpenWalnut is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# OpenWalnut is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with OpenWalnut. If not, see <http:#www.gnu.org/licenses/>.
#
#---------------------------------------------------------------------------

# Gets the major, minor and patch number from a version string.
# _VersionString the string to split
# _Major the major version number - result
# _Minor the minor version number - result
# _Patch the patch number - result
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

# Ensures a minimal version for a given package name.
# _PackageName The package name. Needed for the message
# _ActualVersion The version that was found
# _MinimumVersion The minimum version
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

# This function converts a given filename to a proper target name. This is very useful if you want to define custom targets for
# files and need a unique name.
# _filename the filename to convert
# _target returns the proper target string
FUNCTION( FILE_TO_TARGETSTRING _filename _target )
    # strip the whole path up to src
    STRING( REGEX REPLACE "^.*/src" "src" fileExcaped "${_filename}" ) 

    # remove all those ugly chars
    STRING( REGEX REPLACE "[^A-Za-z0-9]" "X" fileExcaped "${fileExcaped}" )

    # done. Return value
    SET( ${_target} "${fileExcaped}" PARENT_SCOPE )
ENDFUNCTION( FILE_TO_TARGETSTRING )
