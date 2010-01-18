CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( OpenWalnut )


# append search path for FindModules:
#------------------------------------
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../tools/CMakeModules )
# include custom CMake commands like assert
INCLUDE( ${PROJECT_SOURCE_DIR}/../tools/CMakeModules/Utils.cmake )


# platform dependend stuff:
#--------------------------
IF( CMAKE_HOST_SYSTEM MATCHES Linux )
  SET( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib CACHE PATH "The libraries will be put into this directory." )
ELSEIF( CMAKE_HOST_SYSTEM MATCHES Windows )
  SET( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "The libraries will be put into this directory." )
  INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/common/platformDependent/visualStudio/" )
ELSEIF( CMAKE_HOST_SYSTEM MATCHES Darwin )
  SET( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib CACHE PATH "The libraries will be put into this directory." )
ELSE()
  MESSAGE( FATAL_ERROR "Could not determine platform type! (expected Linux or Windows)!" )
ENDIF()


# Package dependencies:
#----------------------
### BOOST ###
SET( Boost_USE_MULTITHREAD ON )

# To see which boost libs we currently use, you may run the following command
# in the trunk/src directory on a linux box to make some investigations:
# grep -i include `find . -type f` | grep boost | awk '{print $2}' | sort | uniq
FIND_PACKAGE( Boost REQUIRED program_options thread filesystem date_time system signals )
ASSERT_GE_VERSION( "Boost" "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}" 1.39.0 )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

### OpenGL ###
FIND_PACKAGE( OpenGL REQUIRED )

### CxxTest ###
FIND_PACKAGE( CxxTest QUIET )
MARK_AS_ADVANCED( CXXTEST_PERL_TESTGEN_EXECUTABLE )
OPTION( OW_USE_TESTS "This enables compilation of tests" ON )
IF( CXXTEST_FOUND AND OW_USE_TESTS ) #We need this variable because this is tested more often.
  SET( OW_COMPILE_TESTS ON )
ELSE()
  SET( OW_COMPILE_TESTS OFF )
ENDIF()
IF( OW_COMPILE_TESTS )
  INCLUDE_DIRECTORIES( ${CXXTEST_INCLUDE_DIR} )
  # Package settings:
  SET( CXXTEST_USE_PYTHON 1 )
  # Activate CTest and "test" target
  ENABLE_TESTING()
ENDIF( OW_COMPILE_TESTS )

### OpenSceneGraph ###
SET( MIN_OSG_VERSION 2.8.0 )
FIND_PACKAGE( osg ${MIN_OSG_VERSION} REQUIRED )
FIND_PACKAGE( osgUtil ${MIN_OSG_VERSION} REQUIRED )
FIND_PACKAGE( osgDB ${MIN_OSG_VERSION} REQUIRED )
FIND_PACKAGE( osgViewer ${MIN_OSG_VERSION} REQUIRED )
FIND_PACKAGE( osgText ${MIN_OSG_VERSION} REQUIRED )
FIND_PACKAGE( osgGA ${MIN_OSG_VERSION} REQUIRED )
FIND_PACKAGE( OpenThreads ${MIN_OSG_VERSION} REQUIRED )
SET( OPENSCENEGRAPH_LIBRARIES ${OSGDB_LIBRARY} ${OSGTEXT_LIBRARY} ${OSGGA_LIBRARY} ${OSGUTIL_LIBRARY} ${OSGVIEWER_LIBRARY} ${OSG_LIBRARY} ${OPENTHREADS_LIBRARY} )
SET( OPENSCENEGRAPH_INCLUDE_DIRS ${OSG_INCLUDE_DIR} ${OSGDB_INCLUDE_DIR} ${OSGTEXT_INCLUDE_DIR} ${OSGGA_INCLUDE_DIR} ${OSGVIEWER_INCLUDE_DIR} ${OSGUTIL_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} )
# When new cmake version available >=2.6.4 we may use this the line below instead the stuff above
# FIND_PACKAGE( OpenSceneGraph 2.8.0 REQUIRED osgDB osgUtil osgGA osgViewer OpenThreads )
INCLUDE_DIRECTORIES( ${OPENSCENEGRAPH_INCLUDE_DIRS} )

# other options:
#---------------
# Set default build type
IF( NOT CMAKE_BUILD_TYPE )
    SET( CMAKE_BUILD_TYPE Release
         CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo"
         FORCE
       )
ENDIF( NOT CMAKE_BUILD_TYPE )
SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "The executables will be put into this directory." )
MARK_AS_ADVANCED( LIBRARY_OUTPUT_PATH )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH )

# Global compiler flags:
#-----------------------
# -Wno-long-long    since on Ubuntu 8.10 it won't compile without it
# -ansi             force ISO-C++98 compliance (not GNU++98)

IF( CMAKE_HOST_SYSTEM MATCHES Windows )
    FILE( GLOB EXCLUDE_LIBRARIES RELATIVE ${Boost_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS}/libboost* )
    SET( CMAKE_SHARED_LINKER_FLAGS " /STACK:10000000 /machine:I386 /NODEFAULTLIB:${EXCLUDE_LIBRARIES}"  CACHE STRING "" FORCE )
    SET( CMAKE_EXE_LINKER_FLAGS " /STACK:10000000 /machine:I386 /NODEFAULTLIB:${EXCLUDE_LIBRARIES}"  CACHE STRING "" FORCE )
ELSE()
    SET( CMAKE_CXX_FLAGS "-frtti -pedantic -ansi -Wall -Wno-long-long -Wextra" CACHE STRING "" FORCE )
    SET( CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "" FORCE )	
    SET( CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -O0" CACHE STRING "" FORCE )
ENDIF()

SET( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -DDEBUG -O2" CACHE STRING "" FORCE )

ADD_SUBDIRECTORY( math )
ADD_SUBDIRECTORY( dataHandler )
ADD_SUBDIRECTORY( graphicsEngine )
ADD_SUBDIRECTORY( gui )
ADD_SUBDIRECTORY( utils )
ADD_SUBDIRECTORY( kernel )
ADD_SUBDIRECTORY( common )
ADD_SUBDIRECTORY( modules )
ADD_EXECUTABLE( walnut-bin OpenWalnut.cpp )
TARGET_LINK_LIBRARIES( walnut-bin kernel dataHandler gui guiqt4 common ${Boost_LIBRARIES} )

#---------- Shaders -------------
option( OW_COPY_SHADERS "This enables copying shaders with every make call." ON )
if( OW_COPY_SHADERS )
  add_custom_target( shaderDirectoryConfiguration
    ALL
    COMMAND ${CMAKE_COMMAND} -D OW_SOURCE_DIR:STRING=${PROJECT_SOURCE_DIR} -P ${PROJECT_SOURCE_DIR}/../tools/CMakeModules/UpdateShaders.cmake
    COMMENT "Put all shaders to the right place or update them if they are already there."
    )
endif( OW_COPY_SHADERS )

#---------- fonts -------------
add_custom_target( fontsDirectoryConfiguration
  ALL
  COMMAND ${CMAKE_COMMAND} -D OW_SOURCE_DIR:STRING=${PROJECT_SOURCE_DIR} -P ${PROJECT_SOURCE_DIR}/../tools/CMakeModules/CopyFonts.cmake
  COMMENT "Put all fonts to the right place."
  )

#---------- shell scripts -------------
add_custom_target( binDirectoryConfiguration
  ALL
  COMMAND ${CMAKE_COMMAND} -D OW_SOURCE_DIR:STRING=${PROJECT_SOURCE_DIR} -P ${PROJECT_SOURCE_DIR}/../tools/CMakeModules/CopyShellScripts.cmake
  COMMENT "Put all shell scripts to the right place."
  )

# Special targets:
#-------------------------------------------------------------------------------------------------------------
# Creates API via Doxygen
ADD_CUSTOM_TARGET( doc
                   COMMAND doxygen ${PROJECT_SOURCE_DIR}/../doc/developer/doxygenConfig
                   COMMAND chmod -R g+r ${PROJECT_SOURCE_DIR}/../doc/developer/html
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                   COMMENT "Build doxygen documentation"
                   VERBATIM
                 )

# NOTE: this full documentation also includes call/caller graphs, colaboration graphs and nicer inheritance graphs
# but need "dot" from graphviz to render them.
ADD_CUSTOM_TARGET( fulldoc
                   COMMAND doxygen ${PROJECT_SOURCE_DIR}/../doc/developer/doxygenConfigFull
                   COMMAND chmod -R g+r ${PROJECT_SOURCE_DIR}/../doc/developer/html
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                   COMMENT "Build doxygen documentation"
                   VERBATIM
                 )

#-------------------------------------------------------------------------------------------------------------
# Uses ctags to create source tags, usable in many editors like VIM or Emacs
ADD_CUSTOM_COMMAND( OUTPUT ctags
                    COMMAND ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/
                    COMMENT "Build ctags tagfile"
                  )

#-------------------------------------------------------------------------------------------------------------
# Uses cscope to create several source graphs (dependency, call, caller, ...), usable in many editors like VIM or Emacs
ADD_CUSTOM_COMMAND( OUTPUT cscope
                    COMMAND cscope -R -b
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/
                    COMMENT "Build cscope tagfile"
                  )

#-------------------------------------------------------------------------------------------------------------
# The command "make tags" creates ctags and cscope tag files
ADD_CUSTOM_TARGET( tags DEPENDS ctags DEPENDS cscope )

#-------------------------------------------------------------------------------------------------------------
# Checks style guide lines via the BrainLint tool against all source code files
#    The output is colored when variable CMAKE_COLOR_MAKEFILE is set to ON,
#    noncolored if it is set to OFF
FILE( GLOB_RECURSE STYLE_CPP ${PROJECT_SOURCE_DIR}/*.cpp )
FILE( GLOB_RECURSE STYLE_H ${PROJECT_SOURCE_DIR}/*.h )
# Set dirs to exclude from stylecheck
SET( EXCLUDE_DIRS "/dataHandler/io/nifti/;/dataHandler/io/biosig/;/dataHandler/io/biosig/XMLParser/;/common/platformDependent/visualStudio/" )
FOREACH( dirname ${EXCLUDE_DIRS} )
    FILE( GLOB EXCLUDE_H   ${PROJECT_SOURCE_DIR}${dirname}*.h )
    FILE( GLOB EXCLUDE_CPP ${PROJECT_SOURCE_DIR}${dirname}*.cpp )

    FOREACH( fname ${EXCLUDE_H} )
      LIST( REMOVE_ITEM STYLE_H ${fname} )
    ENDFOREACH( fname )

    FOREACH( fname ${EXCLUDE_CPP} )
        LIST( REMOVE_ITEM STYLE_CPP ${fname} )
    ENDFOREACH( fname )
ENDFOREACH( dirname )
IF( CMAKE_COLOR_MAKEFILE )
    ADD_CUSTOM_TARGET( stylecheck
                       COMMAND  ${PROJECT_SOURCE_DIR}/../tools/brainlint.py --color ${STYLE_CPP} ${STYLE_H}
                       WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                       COMMENT "Check if code complies to CodingStandard"
                     )
ELSE()
    ADD_CUSTOM_TARGET( stylecheck
                       COMMAND  ${PROJECT_SOURCE_DIR}/../tools/brainlint.py ${STYLE_CPP} ${STYLE_H}
                       WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                       COMMENT "Check if code complies to CodingStandard and use noncolored output for violation messages"
                     )
ENDIF()

#-------------------------------------------------------------------------------------------------------------
# Prints the verbose output of the test runners, so you may identify the source of error early, if any
ADD_CUSTOM_TARGET( vtest
                   COMMAND make test ARGS="-V"
                   COMMENT "Runs the test in verboseness to see what actually went wrong"
                 )

#-------------------------------------------------------------------------------------------------------------
# Checks if all is well to commit (aka commitCheck, cicheck)
ADD_CUSTOM_TARGET( cicheck
                   COMMAND make stylecheck && make test
                   COMMENT "Checks if all is well to commit"
                 )


#-------------------------------------------------------------------------------------------------------------
# Copies fixtures used for tests to the binary directory (where they are expected).
ADD_CUSTOM_TARGET( fixtures
                   ALL
                   COMMAND cp -rf ${PROJECT_SOURCE_DIR}/dataHandler/test/fixtures ${PROJECT_BINARY_DIR}/dataHandler/
                   COMMAND cp -rf ${PROJECT_SOURCE_DIR}/dataHandler/io/test/fixtures ${PROJECT_BINARY_DIR}/dataHandler/io
                   # TODO(wiebel): we need to find a way to do this automatically
                   COMMAND cp -rf ${PROJECT_SOURCE_DIR}/modules/marchingCubes/test/fixtures ${PROJECT_BINARY_DIR}/modules/marchingCubes
                   COMMENT "Copy fixtures to the right place."
                 )

#-------------------------------------------------------------------------------------------------------------
# Makes many special make targets in comination.
ADD_CUSTOM_TARGET( many
                   cmake . && make all -j4 && make fixtures && make stylecheck && make test && make doc
                   COMMENT "Make many special make targets in combination."
                 )

#-------------------------------------------------------------------------------------------------------------
# Make target for MacOS. The difference to the normal one is that we need to fix some paths in libraries.
IF( CMAKE_HOST_SYSTEM MATCHES Darwin )
ADD_CUSTOM_TARGET( macos
                   make all && ${PROJECT_SOURCE_DIR}/../tools/MacOS/macOS_install_tool
                   COMMENT "Make on MacOS. This incudes fixing paths in libraries."
                 )
ENDIF()
