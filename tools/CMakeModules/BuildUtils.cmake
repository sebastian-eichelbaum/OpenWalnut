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

    # Unfortunately, this will contain the tests -> remove them explicitly
    FOREACH( file ${TEST_FILES} )
        LIST( REMOVE_ITEM H_FILES ${file} )
    ENDFOREACH( file )

    SET( ${_CPPFiles} "${CPP_FILES}" PARENT_SCOPE )
    SET( ${_HFiles} "${H_FILES}" PARENT_SCOPE )
    SET( ${_TestFiles} "${TEST_FILES}" PARENT_SCOPE )
ENDFUNCTION( COLLECT_COMPILE_FILES )

# Add tests and copy their fixtures by specifiying the test headers. This is completely automatic and is done recusrively (fixtures). The test
# filenames need to be absolute. The fixtures are searched in CMAKE_CURRENT_SOURCE_DIR.
# _TEST_FILES the list of test header files.
# _TEST_TARGET for which target are the tests? This is added as link library for each test too.
# Third, unnamed parameter: additional dependencies as list
FUNCTION( SETUP_TESTS _TEST_FILES _TEST_TARGET )
    # Only do something if needed
    IF( OW_COMPILE_TESTS )
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

        # the list may contain duplicates
        LIST( REMOVE_DUPLICATES FixturePaths )
       
        # -------------------------------------------------------------------------------------------------------------------------------------------
        # Create copy target for each fixture directory
        # -------------------------------------------------------------------------------------------------------------------------------------------

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
    ENDIF( OW_COMPILE_TESTS )
ENDFUNCTION( SETUP_TESTS )

