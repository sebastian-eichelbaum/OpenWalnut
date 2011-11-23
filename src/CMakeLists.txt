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

# where to find our utils and cmake modules?
SET( OW_TOOLS_DIR ${PROJECT_SOURCE_DIR}/../tools )

# append search path for FindModules:
LIST( APPEND CMAKE_MODULE_PATH ${OW_TOOLS_DIR}/cmake )
# here, we have certain utilities. For example ASSERT_GE_VERSION which ensures a specific minimum version
INCLUDE( Utils )

# These scripts contains all the needed tools to setup the build
INCLUDE( BuildUtils )
INCLUDE( BuildModuleUtils )

# CMAKE automatism to select static vs. shared building:
IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
  OPTION( BUILD_SHARED_LIBS "Build shared libs" ON )
  MARK_AS_ADVANCED( BUILD_SHARED_LIBS )
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
# Install/Target-dir Options
#  - NOTE: in our setup, build dir structure and install structure are the same
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# mimic layout of install dir for build:

# these dirs are the same for all parts of OW
SET( OW_RUNTIME_DIR_RELATIVE "bin" )
SET( OW_RUNTIME_DIR ${PROJECT_BINARY_DIR}/${OW_RUNTIME_DIR_RELATIVE} )
SET( OW_LIBRARY_DIR_RELATIVE "lib" )
SET( OW_LIBRARY_DIR ${PROJECT_BINARY_DIR}/${OW_LIBRARY_DIR_RELATIVE} )
SET( OW_ARCHIVE_DIR_RELATIVE "lib" )
SET( OW_ARCHIVE_DIR ${PROJECT_BINARY_DIR}/${OW_ARCHIVE_DIR_RELATIVE} )
SET( OW_MODULE_DIR_RELATIVE "lib/openwalnut" )
SET( OW_MODULE_DIR ${PROJECT_BINARY_DIR}/${OW_MODULE_DIR_RELATIVE} )
SET( OW_MAN_DIR_RELATIVE "share/man" )
SET( OW_MAN_DIR "${PROJECT_BINARY_DIR}/share/man" )

# One could also define this for every part of OW, but it does not make sense. It is "share" because it is shared among the different parts
SET( OW_SHARE_DIR_RELATIVE "share/openwalnut" )
SET( OW_SHARE_DIR ${PROJECT_BINARY_DIR}/${OW_SHARE_DIR_RELATIVE} )

# set our paths for install targets
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OW_RUNTIME_DIR} )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OW_LIBRARY_DIR} )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OW_ARCHIVE_DIR} )

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
        SET( CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined -Wl,--allow-shlib-undefined,--as-needed" CACHE STRING "" FORCE )
        SET( CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed" CACHE STRING "" FORCE )
    ENDIF()
    SET( CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "" FORCE )
    SET( CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -O0" CACHE STRING "" FORCE )
ENDIF()
SET( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -DDEBUG -O2" CACHE STRING "" FORCE )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# Eigen3 specifics

# NOTE: this is included in ext. But we need to set several definitions to make this work on 32 Bit machines due to alignment problems
SET( EIGEN3_DEFINES -DEIGEN_DONT_VECTORIZE -DEIGEN_DONT_ALIGN -DEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT )
ADD_DEFINITIONS( ${EIGEN3_DEFINES} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# OpenWalnut specific options
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# other options
OPTION( OW_HANDLE_SHADERS "This ensures that shaders are available in build directory after build." ON )

# sorry, linking not available properly on windows, Cygwin supports this but we do not want special rules for thousands of environments.
# ==> keep it clean
IF( NOT CMAKE_HOST_SYSTEM MATCHES "Windows" )
    OPTION( OW_LINK_SHADERS "If turned on, shaders do not get copied. They get linked. This is a nice option for developers." OFF )
ENDIF()

# Provide several options to control some aspects of resource copy.
OPTION( OW_PACKAGE_BUILD "Enable this to get fine-grained control over several resources and files getting installed. This is very handy for package building." OFF )
IF( OW_PACKAGE_BUILD )
    SET( OW_PACKAGE_PACKAGER "cpack" CACHE STRING "Package builder. Set this to enable packager-specific options during install." )

    OPTION( OW_PACKAGE_NOCOPY_LICENSE "Disable to copy our licensing information. Enabling this can be useful for package maintainer since several packaging systems have their own licence mechanism (i.e. Debian)." OFF )
    OPTION( OW_PACKAGE_NOCOPY_COREFONTS "Enable this if you have liberation fonts installed on your system. They will be linked. If disabled, our fonts are copied." OFF )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# The openwalnut executable should print the revision/tag
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Generate needed headers
# NOTE: add a line ADD_DEPENDENCIES( XYZ OW_generate_version_header ) to your target XYZ if you need the header!
SET( OW_VERSION_HEADER_DIRECTORY ${PROJECT_BINARY_DIR}/versionHeader )
SET( OW_VERSION_HEADER ${OW_VERSION_HEADER_DIRECTORY}/WVersion.h )
# to allow all those targets to find the header:
INCLUDE_DIRECTORIES( ${OW_VERSION_HEADER_DIRECTORY} )
# Setup the target
SETUP_VERSION_HEADER( ${OW_VERSION_HEADER} )

# Set the OW version string. This can be used by others for setting target versions during compilation.
GET_VERSION_STRING( OW_VERSION OW_LIB_VERSION )
MESSAGE( STATUS "OW Version: \"${OW_VERSION}\"; OW Lib Version: \"${OW_LIB_VERSION}\"." )

# We need a SOVERSION too. This somehow describes the API compatibility. We use the major number here.
SPLIT_VERSION_STRING( ${OW_LIB_VERSION} OW_VERSION_MAJOR OW_VERSION_MINOR OW_VERSION_PATCH )
SET( OW_SOVERSION ${OW_VERSION_MAJOR} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# REQUIRED third party libs
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# NOTE: please also have a look in src/ext/CMakeLists.txt. Other third-party libs might be searched there.

# -----------------------------------------------------------------------------------------------------------------------------------------------
# Boost, at least 1.39
# See http://www.boost.org/
#
# To see, which boost libs we currently use, you may run the following command # in the src directory on a linux box to make some investigations:
# grep -i include `find . -type f` | grep boost | awk '{print $2}' | sort | uniq


# Setup boost options
SET( Boost_USE_MULTITHREAD ON )
# As every time: do something Visual Studio specific: only use static boost libs
IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
  SET( Boost_USE_STATIC_LIBS ON )
ENDIF()

# find the boost packages
FIND_PACKAGE( Boost 1.39.0 REQUIRED program_options thread filesystem date_time system signals regex )

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
FIND_PACKAGE( OpenSceneGraph ${MIN_OSG_VERSION} REQUIRED osgDB osgUtil osgGA osgViewer osgSim osgWidget osgText )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# Eigen3, at least 3.0.0
# See http://eigen.tuxfamily.org

FIND_PACKAGE( eigen3 REQUIRED )
IF( EIGEN3_FOUND )
    INCLUDE_DIRECTORIES( ${EIGEN3_INCLUDE_DIR} )
ENDIF()

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

# If it is found, and OW_USE_TESTS is on, build the tests and activate CMake's test mechanisms
IF( CXXTEST_FOUND )
  # To enable testing
  OPTION( OW_USE_TESTS "This enables compilation of tests" ON )
  INCLUDE_DIRECTORIES( ${CXXTEST_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES( ${CXXTEST_INCLUDE_DIR} ) # NOTE: old FindCXXTest versions used this name
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
# Resource/Doc Copy
#  - Resource copy is done by the OW parts. This just copies additional helpers
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# copy our debug utilities. This is only useful if compiled debug or RelWithDebInfo
IF( NOT cmake_build_type_tolower STREQUAL "release" )
    # NOTE: DO NOT add and install target here. Debugging tools are not useful and wished in an OpenWalnut installation.
    ADD_CUSTOM_TARGET( CopyDebugUtilities
        ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../tools/debugging" "${PROJECT_BINARY_DIR}/"
        COMMENT "Copying debug utilities"
    )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Num Cores
#  - Number of paralelle operations on this machine
#  - Needed by several of our following targets
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# TODO(ebaum): use try_run for this
# Determines the number of cores available on this machine
IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
    SET( NUM_CORES_SOURCE_DIR ${OW_TOOLS_DIR}/cmake/numCores )
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

# some convenience install targets:
ADD_CUSTOM_TARGET( list_install_tarets
                   COMMAND echo The following components can be installed:
                   COMMAND echo " * install - install everything."
                   COMMAND echo " * install_runtime - install only openwalnut-qt4."
                   COMMAND echo " * install_lib - install only libopenwalnut."
                   COMMAND echo " * install_modules - install only the compiled modules."
                   COMMAND echo " * install_dev - install the development headers for libopenwalnut."
                   COMMAND echo " * install_devdoc - install the development documentation for libopenwalnut."
                   COMMENT "List installation targets."
                 )

# install targets for different parts
ADD_CUSTOM_TARGET( install_runtime
                    ${CMAKE_COMMAND}
                        -DCOMPONENT=QT4GUI
                        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
                 )
ADD_CUSTOM_TARGET( install_lib
                    ${CMAKE_COMMAND}
                        -DCOMPONENT=CORE
                        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
                    ${CMAKE_COMMAND}
                        -DCOMPONENT=EXT
                        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
                 )
ADD_CUSTOM_TARGET( install_modules
                    ${CMAKE_COMMAND}
                        -DCOMPONENT=MODULES
                        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
                 )
ADD_CUSTOM_TARGET( install_dev
                    ${CMAKE_COMMAND}
                        -DCOMPONENT=CORE_DEV
                        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
                 )
ADD_CUSTOM_TARGET( install_devdoc
                    ${CMAKE_COMMAND}
                        -DCOMPONENT=CORE_DOC
                        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
                    DEPENDS core_devdoc
                 )
ADD_DEPENDENCIES( install_devdoc core_devdoc )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Compilation Targets
#  - The GUI + OpenWalnut.cpp ==> openwalnut binary
#  - Ext ==> libOWext_*
#  - All the others ==> libOWcore
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# to allow non-core code to access core and ext absolutely
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR} )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# openwalnut core library

SET( OWCoreName "openwalnut" )

# build core
ADD_SUBDIRECTORY( core )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# QT4 OpenWalnut binary

SET( OWQt4GuiName "qt4gui" )
SET( OWBinaryName "openwalnut-qt4" )

# build core
ADD_SUBDIRECTORY( qt4gui )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# Modules

# build modules
ADD_SUBDIRECTORY( modules )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Advanced variables
#  - Here, we mark all variables advanced which are from mandatory dependencies.
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# NOTE: sorry but this is needed since those vars spam the ccmake. The user should find the important options fast!

# Remove all of them
MARK_AS_ADVANCED( FORCE QT_QMAKE_EXECUTABLE )
   
MARK_AS_ADVANCED( FORCE OSG_INCLUDE_DIR )
MARK_AS_ADVANCED( FORCE OSG_LIBRARY )
MARK_AS_ADVANCED( FORCE OSG_LIBRARY_DEBUG )
MARK_AS_ADVANCED( FORCE OSGDB_INCLUDE_DIR )
MARK_AS_ADVANCED( FORCE OSGDB_LIBRARY )
MARK_AS_ADVANCED( FORCE OSGDB_LIBRARY_DEBUG )
MARK_AS_ADVANCED( FORCE OSGGA_INCLUDE_DIR )
MARK_AS_ADVANCED( FORCE OSGGA_LIBRARY )
MARK_AS_ADVANCED( FORCE OSGGA_LIBRARY_DEBUG )
MARK_AS_ADVANCED( FORCE OSGSIM_INCLUDE_DIR )
MARK_AS_ADVANCED( FORCE OSGSIM_LIBRARY )
MARK_AS_ADVANCED( FORCE OSGSIM_LIBRARY_DEBUG )
MARK_AS_ADVANCED( FORCE OSGUTIL_INCLUDE_DIR )
MARK_AS_ADVANCED( FORCE OSGUTIL_LIBRARY )
MARK_AS_ADVANCED( FORCE OSGUTIL_LIBRARY_DEBUG )
MARK_AS_ADVANCED( FORCE OSGTEXT_INCLUDE_DIR )
MARK_AS_ADVANCED( FORCE OSGTEXT_LIBRARY )
MARK_AS_ADVANCED( FORCE OSGTEXT_LIBRARY_DEBUG )
MARK_AS_ADVANCED( FORCE OSGWIDGET_INCLUDE_DIR )
MARK_AS_ADVANCED( FORCE OSGWIDGET_LIBRARY )
MARK_AS_ADVANCED( FORCE OSGWIDGET_LIBRARY_DEBUG )
MARK_AS_ADVANCED( FORCE OSGVIEWER_INCLUDE_DIR )
MARK_AS_ADVANCED( FORCE OSGVIEWER_LIBRARY )
MARK_AS_ADVANCED( FORCE OSGVIEWER_LIBRARY_DEBUG )

MARK_AS_ADVANCED( FORCE OPENTHREADS_INCLUDE_DIR )
MARK_AS_ADVANCED( FORCE OPENTHREADS_LIBRARY )
MARK_AS_ADVANCED( FORCE OPENTHREADS_LIBRARY_DEBUG )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# CPack - Packaging
#  - TGZ, Deb, RPM
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# for package setup:
INCLUDE( Packaging )

