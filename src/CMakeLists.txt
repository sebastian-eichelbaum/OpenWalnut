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

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# General CMake Setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT( OpenWalnut )

# guard against in-source builds
IF( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  MESSAGE( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. (you may need to remove CMakeCache.txt" )
ENDIF()

# append search path for FindModules:
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../tools/CMakeModules )
# here, we have certain utilities. For example ASSERT_GE_VERSION which ensures a specific minimum version
INCLUDE( ${PROJECT_SOURCE_DIR}/../tools/CMakeModules/Utils.cmake )

# mimic layout of install dir for build:
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
SET( OW_CONFIG_DIR ${PROJECT_BINARY_DIR}/etc/OpenWalnut )
SET( OW_SHARE_DIR ${PROJECT_BINARY_DIR}/share/OpenWalnut )

# CMAKE automatism to select static vs. shared building:
IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
  OPTION( BUILD_SHARED_LIBS "Build shared libs" ON )
  MARK_AS_ADVANCED( BUILD_SHARED_LIBS )
ENDIF()

# Visual Studio version prior to 2010 do not provide stdint.h and inttypes.h
# TODO(all): WTF!?
IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
  INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/common/platformDependent/visualStudio/" )
ENDIF()

# The build types
IF( NOT CMAKE_BUILD_TYPE )
    SET( CMAKE_BUILD_TYPE Release
         CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo"
         FORCE
       )
ENDIF( NOT CMAKE_BUILD_TYPE )

# guard against typos in build-type strings
STRING( TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_tolower)
IF( NOT cmake_build_type_tolower STREQUAL "debug" AND
    NOT cmake_build_type_tolower STREQUAL "release" AND
    NOT cmake_build_type_tolower STREQUAL "relwithdebinfo" AND
    NOT cmake_build_type_tolower STREQUAL "")
  MESSAGE( SEND_ERROR "Unknown build type \"${CMAKE_BUILD_TYPE}\". Allowed values are Debug, Release, RelWithDebInfo  and \"\" (case-insensitive).")
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# Compilation Options
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Global compiler flags:
#-----------------------
# -Wno-long-long    since on Ubuntu 8.10 it won't compile without it
# -ansi             force ISO-C++98 compliance (not GNU++98)

IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
    FILE( GLOB EXCLUDE_LIBRARIES RELATIVE ${Boost_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS}/libboost* )
    SET( CMAKE_SHARED_LINKER_FLAGS " /STACK:10000000 /machine:I386 /NODEFAULTLIB:${EXCLUDE_LIBRARIES}"  CACHE STRING "" FORCE )
    SET( CMAKE_EXE_LINKER_FLAGS " /STACK:10000000 /machine:I386 /NODEFAULTLIB:${EXCLUDE_LIBRARIES}"  CACHE STRING "" FORCE )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D \"NOMINMAX\"" )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /wd4101" )
    SET( CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows" )
ELSE()
    # Unfortunately libstdc++'s header files don't work with mingw in ansi mode (basically libstdc++'s fault)
    IF( CMAKE_HOST_SYSTEM MATCHES "Windows" )
        SET( CMAKE_CXX_FLAGS "-frtti -pedantic -Wall -Wno-long-long -Wextra " CACHE STRING "" FORCE )
    ELSE()
        SET( CMAKE_CXX_FLAGS "-frtti -pedantic -ansi -Wall -Wno-long-long -Wextra " CACHE STRING "" FORCE )
    ENDIF()

    # Darwin's ld isn't GNU and doesn't like the following
    IF( NOT CMAKE_SYSTEM_NAME MATCHES "Darwin" )
        ## The following allows us to prevent cyclic dependencies even on linux
        SET( CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined -Wl,--allow-shlib-undefined" CACHE STRING "" FORCE )
    ENDIF()
    SET( CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "" FORCE )
    SET( CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -O0" CACHE STRING "" FORCE )
ENDIF()
SET( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -DDEBUG -O2" CACHE STRING "" FORCE )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# Eigen3 specifics

# NOTE: this is included in ext. But we need to set several definitions to make this work on 32 Bit machines due to alignment problems
ADD_DEFINITIONS( -DEIGEN_DONT_VECTORIZE -DEIGEN_DONT_ALIGN -DEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# OpenWalnut specific options
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# other options 
OPTION( OW_HANDLE_SHADERS "This ensures that shaders are available in build directory after build." ON )
OPTION( OW_LINK_SHADERS "If turned on, shaders do not get copied. They get linked. This is a nice option for developers." OFF )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# REQUIRED third party libs
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# -----------------------------------------------------------------------------------------------------------------------------------------------
# Boost, at least 1.39
# See http://www.boost.org/
#
# To see, which boost libs we currently use, you may run the following command # in the src directory on a linux box to make some investigations:
# grep -i include `find . -type f` | grep boost | awk '{print $2}' | sort | uniq

# find the boost packages
FIND_PACKAGE( Boost REQUIRED program_options thread filesystem date_time system signals regex )
# we need at least 1.39
ASSERT_GE_VERSION( "Boost" "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}" 1.39.0 )

# Setup boost options
SET( Boost_USE_MULTITHREAD ON )
# As every time: do something Visual Studio specific: only use static boost libs
IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
  SET( Boost_USE_STATIC_LIBS ON )
ENDIF()

# include the boost headers
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# OpenGL, at least 1.2
# See http://www.opengl.org
#
FIND_PACKAGE( OpenGL REQUIRED )

# include the OpenGL header paths
INCLUDE_DIRECTORIES( ${OPENGL_INCLUDE_DIR} )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# OpenSceneGraph, at least 2.8.0
# See http://www.openscenegraph.org

# find the needed packages
SET( MIN_OSG_VERSION 2.8.0 )
IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
  IF( NOT VERSION LESS 2.6.3 )
    SET ( CMAKE_PREFIX_PATH "$ENV{ProgramFiles}/OpenSceneGraph" )
  ENDIF()
ENDIF()
FIND_PACKAGE( OpenSceneGraph ${MIN_OSG_VERSION} REQUIRED osgDB osgUtil osgGA osgViewer osgSim osgWidget osgText OpenThreads )

# include the header paths
INCLUDE_DIRECTORIES( ${OPENSCENEGRAPH_INCLUDE_DIRS} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# OPTIONAL third party libs
#  - include only libs you need in the OW framework
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# NOTE: please add only libs which are used in core! If you need a third party lib for your module, include it in your module's CMakeLists!

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# Unit Testing
#
#  - We use cxxtest. See http://cxxtest.tigris.org
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Try to find it
FIND_PACKAGE( CxxTest QUIET )
MARK_AS_ADVANCED( CXXTEST_PERL_TESTGEN_EXECUTABLE )
   
# If it is found, and OW_USE_TESTS is on, build the tests and activate CMake's test mechanisms
IF( CXXTEST_FOUND )
  # To enable testing
  OPTION( OW_USE_TESTS "This enables compilation of tests" ON )
  INCLUDE_DIRECTORIES( ${CXXTEST_INCLUDE_DIR} )
  IF( OW_USE_TESTS )
    SET( OW_COMPILE_TESTS ON ) #We need this variable because this is tested more often.
    # Package settings:
    SET( CXXTEST_USE_PYTHON 1 )
    # Activate CTest and "test" target
    ENABLE_TESTING()
  ELSE()
    SET( OW_COMPILE_TESTS OFF )
  ENDIF()
ELSE()
  SET( OW_COMPILE_TESTS OFF )
ENDIF()

# we want a more verbose testing too.
ADD_CUSTOM_TARGET( vtest
                   COMMAND $(MAKE) test ARGS="-V"
                   COMMENT "Runs the test in verboseness to see what actually went wrong"
                 )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# Resource Copy
#  - This simply copies the structure from ../resources to the build directory
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# as all the resources with the correct directory structure reside in ../resources, this target is very easy to handle
SET( ResourcesPath "${PROJECT_SOURCE_DIR}/../resources" )
ADD_CUSTOM_TARGET( ResourceConfiguration
    ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ResourcesPath}" "${PROJECT_BINARY_DIR}/"
    COMMENT "Copying resources to build directory"
)

# this is done once -> create one default walnut.cfg
IF( NOT EXISTS ${OW_CONFIG_DIR}/walnut.cfg )
    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/../resources/etc/OpenWalnut/walnut.cfg.official ${OW_CONFIG_DIR}/walnut.cfg COPYONLY )
    MESSAGE( STATUS "Copied walnut.cfg.official to ${OW_CONFIG_DIR}/walnut.cfg" )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# Num Cores 
#  - Number of paralelle operations on this machine
#  - Needed by several of our following targets
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Determines the number of cores available on this machine
IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
    SET( NUM_CORES_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../tools/numCores )
    SET( NUM_CORES_BINARY_DIR ${PROJECT_BINARY_DIR}/numCores )
    SET( NUM_CORES_BINARY ${NUM_CORES_BINARY_DIR}/numCores )
    ADD_CUSTOM_TARGET( numCores
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${NUM_CORES_BINARY_DIR}
                   COMMAND cd ${NUM_CORES_BINARY_DIR} && ${CMAKE_COMMAND} ${NUM_CORES_SOURCE_DIR} > /dev/null
                   COMMAND $(MAKE) -C ${NUM_CORES_BINARY_DIR} > /dev/null
                   COMMENT "Determines the number of cores available on this machine"
                 )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# Documentation
#  - Call doxygen here
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

#let doxygen do the work
ADD_CUSTOM_TARGET( doc
                   COMMAND doxygen ${PROJECT_SOURCE_DIR}/../doc/developer/doxygenConfig
                   COMMAND chmod -R g+r ${PROJECT_SOURCE_DIR}/../doc/developer/html
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                   COMMENT "Build doxygen documentation"
                   VERBATIM
                 )

# NOTE: this full documentation also includes call/caller graphs, colaboration graphs and nicer inheritance graphs
# but needs "dot" from graphviz to render them.
ADD_CUSTOM_TARGET( fulldoc
                   COMMAND doxygen ${PROJECT_SOURCE_DIR}/../doc/developer/doxygenConfigFull
                   COMMAND chmod -R g+r ${PROJECT_SOURCE_DIR}/../doc/developer/htmlFull
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                   COMMENT "Build verbose doxygen documentation"
                   VERBATIM
                 )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# Style
#  - We use brainlint for this
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# SETUP_STYLECHECKER adds targets for each lib which then add theirself to this target
ADD_CUSTOM_TARGET( stylecheck DEPENDS numCores )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# Convenience targets
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Removes nasty windows line endings
ADD_CUSTOM_TARGET( removewinlineendings
                   COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/../tools/removeWinLineEnding.py ${PROJECT_BINARY_DIR}/brainlintlist
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                   COMMENT "Removes Windows Line endings"
                 )

# Checks if all is well to commit (aka commitCheck, cicheck)
ADD_CUSTOM_TARGET( cicheck
                   COMMAND $(MAKE) stylecheck && $(MAKE) test && $(MAKE) doc
                   COMMENT "Checks if all is well to commit"
                 )

# Makes many special make targets in combination. Default number of cores is 1.
ADD_CUSTOM_TARGET( many
                   COMMAND echo Using \$\$\(${NUM_CORES_BINARY}\) cores.\; $(MAKE) all -j\$\$\(${NUM_CORES_BINARY}\)
                   COMMAND $(MAKE) test
                   COMMAND $(MAKE) stylecheck
                   COMMAND $(MAKE) doc
                   DEPENDS numCores
                   COMMENT "Make many special make targets in combination."
                 )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# Compilation Targets
#  - The GUI + OpenWalnut.cpp ==> walnut binary
#  - Ext ==> libOWext_*
#  - All the others ==> libOWcore
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# This script contains all the needed tools to setup the build
INCLUDE( ${PROJECT_SOURCE_DIR}/../tools/CMakeModules/BuildUtils.cmake )

# libcnt needs absolute inclusion somewhere
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext/libeep )
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext )

# to allow non-core code to access core and ext absolutely
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR} )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# OWext

SET( OWExtNiftiName "OWext_niftiio" )
SET( OWExtEEPName "OWext_eep" )
SET( OWExtBioSigName "OWext_biosig" )

# build external libs in ext/
ADD_SUBDIRECTORY( ext ) # The needed external libraries

# -----------------------------------------------------------------------------------------------------------------------------------------------
# OWcore

SET( OWCoreName "OWcore" )

# build core
ADD_SUBDIRECTORY( core )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# QT4 Walnut binary

SET( OWQt4GuiName "qt4gui" )
SET( OWBinaryName "walnut" )

# build core
ADD_SUBDIRECTORY( qt4gui )

