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
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../tools/cmake )
# here, we have certain utilities. For example ASSERT_GE_VERSION which ensures a specific minimum version
INCLUDE( ${CMAKE_MODULE_PATH}/Utils.cmake )

# These scripts contains all the needed tools to setup the build
INCLUDE( ${CMAKE_MODULE_PATH}/BuildUtils.cmake )
INCLUDE( ${CMAKE_MODULE_PATH}/BuildModuleUtils.cmake )

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
# Install/Target-dir Options
#  - NOTE: in our setup, build dir structure and install structure are the same
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# mimic layout of install dir for build:
SET( OW_CONFIG_DIR_RELATIVE "share/openwalnut" )
SET( OW_CONFIG_DIR ${PROJECT_BINARY_DIR}/${OW_CONFIG_DIR_RELATIVE} )
SET( OW_SHARE_DIR_RELATIVE "share/openwalnut" )
SET( OW_SHARE_DIR ${PROJECT_BINARY_DIR}/${OW_SHARE_DIR_RELATIVE} )
SET( OW_MAN_DIR_RELATIVE "share/man" )
SET( OW_MAN_DIR "${PROJECT_BINARY_DIR}/share/man" )
SET( OW_DOC_DIR_RELATIVE "share/doc/openwalnut" )
SET( OW_DOC_DIR ${PROJECT_BINARY_DIR}/${OW_DOC_DIR_RELATIVE} )
SET( OW_RUNTIME_DIR_RELATIVE "bin" )
SET( OW_RUNTIME_DIR ${PROJECT_BINARY_DIR}/${OW_RUNTIME_DIR_RELATIVE} )
SET( OW_LIBRARY_DIR_RELATIVE "lib" )
SET( OW_LIBRARY_DIR ${PROJECT_BINARY_DIR}/${OW_LIBRARY_DIR_RELATIVE} )
SET( OW_ARCHIVE_DIR_RELATIVE "lib" )
SET( OW_ARCHIVE_DIR ${PROJECT_BINARY_DIR}/${OW_ARCHIVE_DIR_RELATIVE} )
SET( OW_MODULE_DIR_RELATIVE "lib/openwalnut" )
SET( OW_MODULE_DIR ${PROJECT_BINARY_DIR}/${OW_MODULE_DIR_RELATIVE} )

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

# sorry, linking not available properly on windows, Cygwin supports this but we do not want special rules for thousands of environments.
# ==> keep it clean
IF( NOT CMAKE_HOST_SYSTEM MATCHES "Windows" )
    OPTION( OW_LINK_SHADERS "If turned on, shaders do not get copied. They get linked. This is a nice option for developers." OFF )
ENDIF()

# Provide several options to control some aspects of resource copy.
OPTION( OW_PACKAGE_BUILD "Enable this to get fine-grained control over several resources and files getting installed. This is very handy for package building." OFF )
IF( OW_PACKAGE_BUILD )
    OPTION( OW_PACKAGE_NOCOPY_LICENSE "Disable to copy our licensing information. Enabling this can be useful for package maintainer since several packaging systems have their own licence mechanism (i.e. Debian)." OFF )
    OPTION( OW_PACKAGE_NOLINK_OPENTHREADS "Enable this to avoid linking OpenThreads. On some systems, OpenSceneGraph already is linked against it and thus linking OpenThreads to OW is not needed (Debian and Ubuntu for example)." OFF )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# The walnut executable should print the revision/tag
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
GET_VERSION_STRING( dummy OW_VERSION "1.0.0 " )
IF( NOT DEFINED OW_VERSION )
    # CMake does not like unset variables for target properties.
    SET( OW_VERSION "1.0.0" )
ENDIF()

# We need a SOVERSION too. This somehow describes the API compatibility. We use the major number here.
SPLIT_VERSION_STRING( ${OW_VERSION} OW_VERSION_MAJOR OW_VERSION_MINOR OW_VERSION_PATCH )
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

# find the boost packages
FIND_PACKAGE( Boost 1.39.0 REQUIRED program_options thread filesystem date_time system signals regex )

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
FIND_PACKAGE( OpenSceneGraph ${MIN_OSG_VERSION} REQUIRED osgDB osgUtil osgGA osgViewer osgSim osgWidget osgText )

# If the user did not disable license-copying, do it
# NOTE: use this "double-negative" to use the fact that undefined variables yield FALSE.
IF( OW_PACKAGE_NOLINK_OPENTHREADS )
    # NOTE: this is a hack to remove the openthread lib from the list of OSG libs. It gets automatically added by the current
    #       find script but it is not really needed on some systems (since they provide an osg lib which already is linked against this.).
    FOREACH( osglib ${OPENSCENEGRAPH_LIBRARIES} )
        STRING( REGEX MATCH ".*OpenThreads.*" IsExcluded "${osglib}" )
        IF( IsExcluded )
            LIST( REMOVE_ITEM OPENSCENEGRAPH_LIBRARIES ${osglib} )
        ENDIF( IsExcluded )
    ENDFOREACH()
ENDIF()

# -----------------------------------------------------------------------------------------------------------------------------------------------
# NiftiLib, at least 2.0.0
# See  http://niftilib.sourceforge.net

# we provide the niftilib in ext. If it is found on the system, the system version is used
SET( OWExtNiftiIOName  "openwalnut_niftiio" )
SET( OWExtNiftiZNZName  "openwalnut_niftiznz" )

FIND_PACKAGE( niftilib QUIET )
IF( NIFTILIB_FOUND )
    MESSAGE( STATUS "Using system installed nifitlib." )
    INCLUDE_DIRECTORIES( ${NIFTILIB_INCLUDE_DIR} )
    SET( OWExtNiftiIOName  ${NIFTILIB_LIBRARY} )
ELSE()
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext/nifti )
    ADD_SUBDIRECTORY( ext/nifti )
ENDIF()

# -----------------------------------------------------------------------------------------------------------------------------------------------
# BioSig, at least 0.84
# See http://biosig.sourceforge.net

# we provide the biosig in ext. If it is found on the system, the system version is used
SET( OWExtBioSigName "openwalnut_biosig" )

FIND_PACKAGE( biosig QUIET )
IF( BIOSIG_FOUND )
    MESSAGE( STATUS "Using system installed biosig." )
    INCLUDE_DIRECTORIES( ${BIOSIG_INCLUDE_DIR} )
    SET( OWExtBioSigName  ${BIOSIG_LIBRARY} )
ELSE()
    IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
        INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext/biosig )
        ADD_SUBDIRECTORY( ext/biosig )
    ELSE ()
        INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext/biosigWin )
        ADD_SUBDIRECTORY( ext/biosigWin )
    ENDIF()
ENDIF()

# -----------------------------------------------------------------------------------------------------------------------------------------------
# Eigen3, at least 3.0.0
# See http://eigen.tuxfamily.org

FIND_PACKAGE( eigen3 QUIET )
IF( EIGEN3_FOUND )
    MESSAGE( STATUS "Using system installed eigen3." )
    INCLUDE_DIRECTORIES( ${EIGEN3_INCLUDE_DIR} )
ELSE()
    INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/ext/eigen3" )
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
#  - This simply copies the structure from ../resources to the build directory
#  - Additonaly takes care that the user doc and manual pages get placed where they belong.
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Setup everything in ../resources
SETUP_RESOURCES()
SETUP_ADDITIONAL_FILES( ${OW_DOC_DIR_RELATIVE} 
                        "SHARE"
                        "${PROJECT_SOURCE_DIR}/../README"
                        "${PROJECT_SOURCE_DIR}/../AUTHORS"
                        "${PROJECT_SOURCE_DIR}/../CONTRIBUTORS"
                      )

# If the user did not disable license-copying, do it
# NOTE: use this "double-negative" to use the fact that undefined variables yield FALSE.
IF( NOT OW_PACKAGE_NOCOPY_LICENSE )
    SETUP_ADDITIONAL_FILES( ${OW_DOC_DIR_RELATIVE} 
                            "SHARE"
                            "${PROJECT_SOURCE_DIR}/../COPYING"
                            "${PROJECT_SOURCE_DIR}/../COPYING.LESSER"
                          )
ENDIF()

# also copy all the user documentation we have
SETUP_ADDITIONAL_DIRECTORY( ${OW_DOC_DIR_RELATIVE}
                            ${PROJECT_SOURCE_DIR}/../doc/user/
                            "SHARE"
                            TRUE    # this denotes that the contents of doc/user should be copied instead of user directly
                          )  
SETUP_ADDITIONAL_DIRECTORY( ${OW_MAN_DIR_RELATIVE}
                            ${PROJECT_SOURCE_DIR}/../doc/man/
                            "SHARE"
                            TRUE    # this denotes that the contents of doc/man should be copied instead of user directly
                          )

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
    SET( NUM_CORES_SOURCE_DIR ${CMAKE_MODULE_PATH}/numCores )
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

# libcnt needs absolute inclusion somewhere
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext/libeep )
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext )

# to allow non-core code to access core and ext absolutely
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR} )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# external libs

SET( OWExtEEPName    "openwalnut_eep" )

# build external libs in ext/
ADD_SUBDIRECTORY( ext ) # The needed external libraries

# -----------------------------------------------------------------------------------------------------------------------------------------------
# openwalnut core library

SET( OWCoreName "openwalnut" )

# build core
ADD_SUBDIRECTORY( core )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# QT4 Walnut binary

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
INCLUDE( ${CMAKE_MODULE_PATH}/Packaging.cmake )

