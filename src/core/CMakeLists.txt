# Set the prefix and suffix of dynamically loadable modules to match
# CMAKEs naming of them. The quotes are necessary so the shell doesn't mess
# with these defines. Unfortunately, the Visual Studio has problems handling
# the quotes on the command line. WSharedLib::getSystemPrefix and
# getSystemSuffix hard codes the right strings for VC.
# Maybe you have to spend another 500 bucks to have your VC support quotes.
IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
    ADD_DEFINITIONS( '-DW_LIB_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"' )
    ADD_DEFINITIONS( '-DW_LIB_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"' )
ENDIF()

# Collect the compile-files for OWCore
COLLECT_COMPILE_FILES( "${CMAKE_CURRENT_SOURCE_DIR}" CORE_CPP_FILES CORE_H_FILES CORE_TEST_FILES )

# Setup the OWCore target
ADD_LIBRARY( ${OWCoreName} SHARED ${CORE_CPP_FILES} ${CORE_H_FILES} )
TARGET_LINK_LIBRARIES( ${OWCoreName} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} ${OPENGL_gl_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES} ${OWExtBioSigName} ${OWExtEEPName} ${OWExtNiftiName} )

# Setup tests of this target
# Unit tests
IF( OW_COMPILE_TESTS )

    # Add each found test and create a target for it
    FOREACH( testfile ${CORE_TEST_FILES} )
        # strip path and extension
        STRING( REGEX REPLACE "^.*/" "" StrippedTestFile "${testfile}" )
        STRING( REGEX REPLACE "\\..*$" "" PlainTestFileName "${StrippedTestFile}" )
        STRING( REGEX REPLACE "_test$" "" TestFileClass "${PlainTestFileName}" )

        # create some name for the test
        SET( UnitTestName "unittest_${TestFileClass}" )

        # create the test-target
        CXXTEST_ADD_TEST( ${UnitTestName} "${UnitTestName}.cc" ${testfile} )
        TARGET_LINK_LIBRARIES( ${UnitTestName} ${OWCoreName} )
    ENDFOREACH( testfile )

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
    
    # for each fixture, copy to build dir
    FOREACH( FixtureDir ${FixturePaths} )
        # we need a unique name for each fixture dir as target
        STRING( REGEX REPLACE "[^A-Za-z0-9]" "" FixtureDirEscaped "${FixtureDir}" )

        # finally, create the copy target
        ADD_CUSTOM_TARGET( ${OWCoreName}_CopyFixtures_${FixtureDirEscaped}
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${FixtureDir}" ${CMAKE_BINARY_DIR}/core/fixtures/
            COMMENT "Copy fixtures of ${OWCoreName} from ${FixtureDir}."
        )
        ADD_DEPENDENCIES( ${OWCoreName} ${OWCoreName}_CopyFixtures_${FixtureDirEscaped} )
    ENDFOREACH( FixtureDir )

ENDIF( OW_COMPILE_TESTS )
