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

# Unlike Utils.cmake, this file contains only build related utilities.

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

    # the test directories should be excluded from normal compilation completely
    FOREACH( file ${H_FILES} )
        STRING( REGEX MATCH "^.*\\/test\\/.*" IsTest "${file}" )
        IF( IsTest )
            LIST( REMOVE_ITEM H_FILES ${file} )
        ENDIF( IsTest )
    ENDFOREACH( file )

    SET( ${_CPPFiles} "${CPP_FILES}" PARENT_SCOPE )
    SET( ${_HFiles} "${H_FILES}" PARENT_SCOPE )
    SET( ${_TestFiles} "${TEST_FILES}" PARENT_SCOPE )
ENDFUNCTION( COLLECT_COMPILE_FILES )

# Recursively searches shader files with extension glsl.
# _DirString where to search
# _GLSLFiles the list of files found
FUNCTION( COLLECT_SHADER_FILES _DirString _GLSLFiles )
    # recursively get all files
    FILE( GLOB_RECURSE GLSL_FILES ${_DirString}/*.glsl )
    SET( ${_GLSLFiles} "${GLSL_FILES}" PARENT_SCOPE )
ENDFUNCTION( COLLECT_SHADER_FILES )

# Add tests and copy their fixtures by specifiying the test headers. This is completely automatic and is done recusrively (fixtures). The test
# filenames need to be absolute. The fixtures are searched in CMAKE_CURRENT_SOURCE_DIR.
# _TEST_FILES the list of test header files.
# _TEST_TARGET for which target are the tests? This is added as link library for each test too.
# Third, unnamed parameter: additional dependencies as list
FUNCTION( SETUP_TESTS _TEST_FILES _TEST_TARGET )
    # Only do something if needed
    IF( OW_COMPILE_TESTS )
        # Abort if no tests are present
        LIST( LENGTH _TEST_FILES TestFileListLength )
        IF( ${TestFileListLength} STREQUAL "0" )
            # MESSAGE( STATUS "No tests for target ${_TEST_TARGET}. You should always consider unit testing!" )
            RETURN()
        ENDIF()

        # the optional parameter is an additional dependencies list
        SET( _DEPENDENCIES ${ARGV2} )

        # target directory for fixtures
        SET( FixtureTargetDirectory "${CMAKE_BINARY_DIR}/core/fixtures/" )

        # -------------------------------------------------------------------------------------------------------------------------------------------
        # Setup CXX test
        # -------------------------------------------------------------------------------------------------------------------------------------------

        # Add each found test and create a target for it
        FOREACH( testfile ${_TEST_FILES} )
            # strip path and extension
            STRING( REGEX REPLACE "^.*/" "" StrippedTestFile "${testfile}" )
            STRING( REGEX REPLACE "\\..*$" "" PlainTestFileName "${StrippedTestFile}" )
            STRING( REGEX REPLACE "_test$" "" TestFileClass "${PlainTestFileName}" )

            # create some name for the test
            SET( UnitTestName "unittest_${TestFileClass}" )

            # create the test-target
            CXXTEST_ADD_TEST( ${UnitTestName} "${UnitTestName}.cc" ${testfile} )
            TARGET_LINK_LIBRARIES( ${UnitTestName} ${_TEST_TARGET} ${_DEPENDENCIES} )

            # unfortunately, the tests search their fixtures relative to their working directory. So we add a preprocessor define containing the
            # path to the fixtures. This is quite ugly but I do not know how to ensure that the working directory of tests can be modified.
            SET_SOURCE_FILES_PROPERTIES( "${UnitTestName}.cc" PROPERTIES COMPILE_DEFINITIONS "W_FIXTURE_PATH=std::string( \"${FixtureTargetDirectory}\" )" )
        ENDFOREACH( testfile )

        # -------------------------------------------------------------------------------------------------------------------------------------------
        # Search fixtures
        # -------------------------------------------------------------------------------------------------------------------------------------------

        # direct globbing of files by only knowing a part of the path is not possible -> so get all files and filter later
        FILE( GLOB_RECURSE everything "${CMAKE_CURRENT_SOURCE_DIR}/*" )
        SET( FixturePaths )
        FOREACH( fixture ${everything} )
            # is this a fixture?
            STRING( REGEX MATCH "test\\/fixtures" IsFixture "${fixture}" )
            IF( IsFixture )             # found a fixture.
                # strip filename from path and keep only path
                STRING( REGEX REPLACE "fixtures/.*$" "fixtures" FixturePath "${fixture}" )
                # add it to a list
                LIST( APPEND FixturePaths ${FixturePath} )
            ENDIF( IsFixture )
        ENDFOREACH( fixture )

        # REMOVE_DUPLICATES throws an error if list is empty. So we check this here
        LIST( LENGTH FixturePaths ListLength )
        IF( NOT ${ListLength} STREQUAL "0" )
            # the list may contain duplicates
            LIST( REMOVE_DUPLICATES FixturePaths )

            # ---------------------------------------------------------------------------------------------------------------------------------------
            # Create copy target for each fixture directory
            # ---------------------------------------------------------------------------------------------------------------------------------------

            # for each fixture, copy to build dir
            FOREACH( FixtureDir ${FixturePaths} )
                # we need a unique name for each fixture dir as target
                STRING( REGEX REPLACE "[^A-Za-z0-9]" "" FixtureDirEscaped "${FixtureDir}" )
                # finally, create the copy target
                ADD_CUSTOM_TARGET( ${_TEST_TARGET}_CopyFixtures_${FixtureDirEscaped}
                    COMMAND ${CMAKE_COMMAND} -E copy_directory "${FixtureDir}" "${FixtureTargetDirectory}"
                    COMMENT "Copy fixtures of ${_TEST_TARGET} from ${FixtureDir}."
                )
                ADD_DEPENDENCIES( ${_TEST_TARGET} ${_TEST_TARGET}_CopyFixtures_${FixtureDirEscaped} )
            ENDFOREACH( FixtureDir )
        ENDIF()
    ENDIF( OW_COMPILE_TESTS )
ENDFUNCTION( SETUP_TESTS )

# This function sets up the build system to ensure that the specified list of shaders is available after build in the target directory. It
# additionally setups the install targets. Since build and install structure are the same, specify only relative targets here which are used for
# both.
# _Shaders list of shaders
# _TargetDir the directory where to put the shaders. Relative to ${PROJECT_BINARY_DIR} and install dir. You should avoid ".." stuff. This can
# break the install targets
FUNCTION( SETUP_SHADERS _Shaders _TargetDir )
    # only if we are allowed to
    IF( OW_HANDLE_SHADERS )
        EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E make_directory ${_TargetDir} )

        # should we copy or link them?
        SET( ShaderOperation "copy_if_different" )
        IF ( OW_LINK_SHADERS ) # link
             SET( ShaderOperation "create_symlink" )
        ENDIF( OW_LINK_SHADERS )

        # now do the operation for each shader into build dir
        FOREACH( fname ${_Shaders} )
            # We need the plain filename (create_symlink needs it)
            STRING( REGEX REPLACE "^.*/" "" StrippedFileName "${fname}" )

            # let cmake do it
            EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E ${ShaderOperation} ${fname} "${PROJECT_BINARY_DIR}/${_TargetDir}/${StrippedFileName}" )
        ENDFOREACH( fname )

        # now add install targets for each shader. All paths are relative to the current source dir.
        FOREACH( fname ${_Shaders} )
            INSTALL( FILES ${fname} DESTINATION ${_TargetDir} )
        ENDFOREACH( fname )
    ENDIF( OW_HANDLE_SHADERS )
ENDFUNCTION( SETUP_SHADERS )

# Sets up the stylecheck mechanism. Use this to add your codes to the stylecheck mechanism.
# _TargetName the name of the target which gets added here
# _CheckFiles the list of files to check
# _Excludes a list of exclusion rules. These are regular expressions.
FUNCTION( SETUP_STYLECHECKER _TargetName _CheckFiles _Excludes )

    # to exlude some files, check each file against each exlusion rule
    FOREACH( filename ${_CheckFiles} )
        FOREACH( excludeRule ${_Excludes} )
            STRING( REGEX MATCH "${excludeRule}" IsExcluded "${filename}" )
            IF( IsExcluded )
                LIST( REMOVE_ITEM _CheckFiles ${filename} )
            ENDIF( IsExcluded )
        ENDFOREACH( excludeRule )
    ENDFOREACH( filename )

    # the stylechecker allows coloring the output. Enable if color make is active
    IF( CMAKE_COLOR_MAKEFILE )
        SET( STYLECHECK_OPTIONS "--color" )
    ELSE()
        SET( STYLECHECK_OPTIONS "" )
    ENDIF()

    # Further system specific options
    IF( CMAKE_HOST_WIN32 )
        SET( XARGS_OPTIONS "-n 128" )
    ELSEIF( CMAKE_HOST_UNIX )
        SET( XARGS_OPTIONS -P \$\$\(${NUM_CORES_BINARY}\) -n 64 )
    ELSE()
        SET( XARGS_OPTIONS "" )
    ENDIF()

    # Export our filtered file list to a file in build dir
    SET( BrainLinterListFile "${PROJECT_BINARY_DIR}/brainlint/brainlintlist_${_TargetName}" )
    FILE( WRITE ${BrainLinterListFile} "" )
    FOREACH( filename ${_CheckFiles} )
        FILE( APPEND ${BrainLinterListFile} "${filename}\n" )
    ENDFOREACH( filename )

    # add a new target for this lib
    ADD_CUSTOM_TARGET( stylecheck_${_TargetName}
                       COMMAND  cat ${BrainLinterListFile} | xargs ${XARGS_OPTIONS} ${PROJECT_SOURCE_DIR}/../tools/brainlint.py ${STYLECHECK_OPTIONS} 2>&1 | grep -iv 'Total errors found: 0$$' | cat
                       DEPENDS numCores
                       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                       COMMENT "Check if ${_TargetName} complies to CodingStandard"
    )

    # make the stylecheck taret depend upon this
   ADD_DEPENDENCIES( stylecheck "stylecheck_${_TargetName}" )
ENDFUNCTION( SETUP_STYLECHECKER )

# This function handles local resources needed for program execution. Place your resources in "${CMAKE_CURRENT_SOURCE_DIR}/../resources/". They
# get copied to the build directory and a proper install target is provided too
FUNCTION( SETUP_RESOURCES )
    # as all the resources with the correct directory structure reside in ../resources, this target is very easy to handle
    SET( ResourcesPath "${CMAKE_CURRENT_SOURCE_DIR}/../resources/" )
    ADD_CUSTOM_TARGET( ResourceConfiguration
        ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${ResourcesPath}" "${PROJECT_BINARY_DIR}/"
        COMMENT "Copying resources to build directory"
    )

    # Also specify install target
    INSTALL( DIRECTORY ${ResourcesPath}
             DESTINATION "."
             PATTERN "bin/*"            # binaries need to be executable
                 PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                             GROUP_READ GROUP_EXECUTE
                             WORLD_READ WORLD_EXECUTE
             )
ENDFUNCTION( SETUP_RESOURCES )

# This function tries to find a proper version string. It therefore uses the file src/../VERSION and mercurial. If the file exists and is not
# empty, the contents of it get combined with the mercurial results if mercurial is installed. If not, only the file content will be used. If
# both methods fail, a default string is used.
# _version the returned version string
# _default a default string you specify if all version check methods fail
FUNCTION( GET_VERSION_STRING _version _default )
    # Undef the OW_VERSION variable
    UNSET( OW_VERSION_HG )
    UNSET( OW_VERSION_FILE )

    # Grab version file.
    SET( OW_VERSION_FILENAME ${PROJECT_SOURCE_DIR}/../VERSION )
    IF( EXISTS ${OW_VERSION_FILENAME} )
        # Read the version file
        FILE( READ ${OW_VERSION_FILENAME} OW_VERSION_FILE )
        # this wil contain an line-break. Remove it.
        STRING( REGEX REPLACE "\n" "" OW_VERSION_FILE "${OW_VERSION_FILE}" )
        
        # this is ugly because, if the version file is empty, the OW_VERSION_FILE content is "". Unfortunately, this is not UNDEFINED as it would be
        # by SET( VAR "" ) ... so set it manually
        IF( OW_VERSION_FILE STREQUAL "" )
            UNSET( OW_VERSION_FILE )
        ENDIF()
    ENDIF()

    # Use hg to query version information.
    # -> the nice thing is: if hg is not available, no compilation errors anymore
    # NOTE: it is run insde the project source directory
    EXECUTE_PROCESS( COMMAND hg parents --template "{rev}:{node|short} {branches} {tags}" OUTPUT_VARIABLE OW_VERSION_HG RESULT_VARIABLE hgParentsRetVar 
                     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                    )
    IF( NOT ${hgParentsRetVar} STREQUAL 0 )
        UNSET( OW_VERSION_HG )
    ENDIF()

    # Use the version strings and set them as #define
    IF( DEFINED OW_VERSION_HG AND DEFINED OW_VERSION_FILE )
        SET( ${_version} "${OW_VERSION_FILE} - ${OW_VERSION_HG}" PARENT_SCOPE )
    ELSEIF( DEFINED OW_VERSION_FILE )
        SET( ${_version} "${OW_VERSION_FILE}" PARENT_SCOPE )
    ELSEIF( DEFINED OW_VERSION_HG )
        SET( ${_version} "${OW_VERSION_HG}" PARENT_SCOPE )
    ELSE()
        SET( ${_version} ${_default} PARENT_SCOPE )
    ENDIF()
ENDFUNCTION( GET_VERSION_STRING )

# This functions adds a custom target for generating the specified version header. This is very useful if you want to include build-time version
# information in your code. If your executable or lib is created in the same CMakeLists as this function is called, just add the
# _OW_VERSION_Header filename you specified here to your compile files. If you call this and add target XYZ in a subdirectory, you NEED to add
# the lines: 
#  ADD_DEPENDENCIES( XYZ OW_generate_version_header )
#  SET_SOURCE_FILES_PROPERTIES( ${OW_VERSION_HEADER} PROPERTIES GENERATED ON )
# This is needed since CMake can only use the ADD_CUSTOM_COMMAND output as dependency if the custom command was called inside the SAME directory
# as the target is. If not, an additional target needs to be defined and CMake needs information about generated files.
#
# _OW_VERSION_HEADER the filename where to store the header. Should be absolute.
FUNCTION( SETUP_VERSION_HEADER _OW_VERSION_HEADER )

    # The file WVersion.* needs the version definition.
    ADD_CUSTOM_COMMAND( OUTPUT ${_OW_VERSION_HEADER}
                        DEPENDS ${PROJECT_SOURCE_DIR}/../VERSION ${PROJECT_SOURCE_DIR}/../.hg/dirstate
                        COMMAND ${CMAKE_COMMAND} -D PROJECT_SOURCE_DIR:STRING=${PROJECT_SOURCE_DIR} -D HEADER_FILENAME:STRING=${_OW_VERSION_HEADER} -P BuildVersionHeader.cmake
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../tools/cmake/
                        COMMENT "Creating Version Header ${_OW_VERSION_HEADER}."
    )

    # this is needed if the file is also a dependency of an target XYZ in a CMakeLists file in some subdirectory. Then, you should add 
    # ADD_DEPENDENCIES( XYZ OW_generate_version_header )
    ADD_CUSTOM_TARGET( OW_generate_version_header DEPENDS ${_OW_VERSION_HEADER} )
ENDFUNCTION( SETUP_VERSION_HEADER )

# Comvenience function which does what SETUP_VERSION_HEADER requests from you if you need the version header in some CMakeLists different from
# the one which called SETUP_VERSION_HEADER. It uses ${OW_VERSION_HEADER} automatically. So it needs to be set.
#
# _target: the target name which needs the header
FUNCTION( SETUP_USE_VERSION_HEADER _target )
    SET_SOURCE_FILES_PROPERTIES( ${OW_VERSION_HEADER} PROPERTIES GENERATED ON )
    ADD_DEPENDENCIES( ${_target} OW_generate_version_header )
ENDFUNCTION( SETUP_USE_VERSION_HEADER )
