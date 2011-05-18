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
# We additionally want to place common shared files to this path:
SET( SHARED_FILES_RELATIVE "../share/${CMAKE_PROJECT_NAME}" CACHE INTERNAL "Relative path to bin/ denoting the destination for shaders and cfg files." )

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
        SET( CMAKE_CXX_FLAGS "-frtti -pedantic -Wall -Wno-long-long -Wextra -Wold-style-cast" CACHE STRING "" FORCE )
    ELSE()
        # SET( CMAKE_CXX_FLAGS "-frtti -pedantic -ansi -Wall -Wno-long-long -Wextra -Wold-style-cast" CACHE STRING "" FORCE )
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

# -----------------------------------------------------------------------------------------------------------------------------------------------
# CXX Test
# See http://cxxtest.tigris.org

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

# -----------------------------------------------------------------------------------------------------------------------------------------------
# Eigen3
# See http://eigen.tuxfamily.org

# NOTE: this is included in ext. But we need to set several definitions to make (who else?) Visual Studio work
IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
    ADD_DEFINITIONS( -DEIGEN_DONT_VECTORIZE -DEIGEN_DONT_ALIGN -DEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# Compilation Targets
#  - The GUI + OpenWalnut.cpp ==> walnut binary
#  - Ext ==> libOWext_*
#  - All the others ==> libOWcore
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# -----------------------------------------------------------------------------------------------------------------------------------------------
# OWext

SET( OWExtNiftiName "OWext_niftiio" )
SET( OWExtEEPName "OWext_eep" )
SET( OWExtBioSigName "OWext_biosig" )

# Needed because libeep has includes of form #include <cnt/cnt.h>
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext/libeep )
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext )

# build external libs in ext/
ADD_SUBDIRECTORY( ext ) # The needed external libraries

# -----------------------------------------------------------------------------------------------------------------------------------------------
# OWcore

SET( OWCoreName "OWcore" )

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
# Kernel
COLLECT_COMPILE_FILES( "${PROJECT_SOURCE_DIR}/kernel" CORE_KERNEL_CPP_FILES CORE_KERNEL_H_FILES CORE_KERNEL_TEST_FILES )
# Common
COLLECT_COMPILE_FILES( "${PROJECT_SOURCE_DIR}/common" CORE_COMMON_CPP_FILES CORE_COMMON_H_FILES CORE_COMMON_TEST_FILES )
# DataHandler
COLLECT_COMPILE_FILES( "${PROJECT_SOURCE_DIR}/dataHandler" CORE_DATAHANDLER_CPP_FILES CORE_DATAHANDLER_H_FILES CORE_DATAHANDLER_TEST_FILES )
# GraphicsEngine
COLLECT_COMPILE_FILES( "${PROJECT_SOURCE_DIR}/graphicsEngine" CORE_GRAPHICSENGINE_CPP_FILES CORE_GRAPHICSENGINE_H_FILES CORE_GRAPHICSENGINE_TEST_FILES )

# Merge them into one big list
SET( CORE_CPP_FILES ${CORE_KERNEL_CPP_FILES} ${CORE_COMMON_CPP_FILES} ${CORE_DATAHANDLER_CPP_FILES} ${CORE_GRAPHICSENGINE_CPP_FILES} )
SET( CORE_H_FILES ${CORE_KERNEL_H_FILES} ${CORE_COMMON_H_FILES} ${CORE_DATAHANDLER_H_FILES} ${CORE_GRAPHICSENGINE_H_FILES} )
SET( CORE_TEST_FILES ${CORE_KERNEL_TEST_FILES} ${CORE_COMMON_TEST_FILES} ${CORE_DATAHANDLER_TEST_FILES} ${CORE_GRAPHICSENGINE_TEST_FILES} )

# Setup the OWCore target
ADD_LIBRARY( ${OWCoreName} SHARED ${CORE_CPP_FILES} ${CORE_H_FILES} )
TARGET_LINK_LIBRARIES( ${OWCoreName} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} ${OPENGL_gl_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES} ${OWExtBioSigName} ${OWExtEEPName} ${OWExtNiftiName} )

