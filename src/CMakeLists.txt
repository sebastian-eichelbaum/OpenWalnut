CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( OpenWalnut )


# Include local CMake modules
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../tools/CMakeModules )


# First of all find out which platform we are on and set our own platform variable
SET( OPENWALNUT_PLATFORM_LINUX "Linux" )
SET( OPENWALNUT_PLATFORM_WINDOWS "Windows" )
IF( CMAKE_HOST_SYSTEM MATCHES Linux )
  SET( OPENWALNUT_PLATFORM
       ${OPENWALNUT_PLATFORM_LINUX}
       CACHE STRING "The platform we are building on." )
ENDIF( CMAKE_HOST_SYSTEM MATCHES Linux )
IF( CMAKE_HOST_SYSTEM MATCHES Windows )
  SET( OPENWALNUT_PLATFORM
    ${OPENWALNUT_PLATFORM_WINDOWS}
    CACHE STRING "The platform we are building on." )
ENDIF( CMAKE_HOST_SYSTEM MATCHES Windows )


# Package dependencies:
FIND_PACKAGE( Boost REQUIRED COMPONENTS program_options )
FIND_PACKAGE( CxxTest QUIET )


# Platform dependencies
IF( OPENWALNUT_PLATFORM MATCHES ${OPENWALNUT_PLATFORM_WINDOWS} )
  SET( LIBRARY_OUTPUT_PATH
       ${PROJECT_BINARY_DIR}/bin
       CACHE PATH "The libraries will be put into this directory.")
ELSE( OPENWALNUT_PLATFORM MATCHES ${OPENWALNUT_PLATFORM_WINDOWS} )
  SET( LIBRARY_OUTPUT_PATH
       ${PROJECT_BINARY_DIR}/lib
       CACHE PATH "The libraries will be put into this directory.")
ENDIF( OPENWALNUT_PLATFORM MATCHES ${OPENWALNUT_PLATFORM_WINDOWS} )
MARK_AS_ADVANCED( LIBRARY_OUTPUT_PATH )
SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "The executables will be put into this directory.")
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH )


# Enable unit tests via CxxTest
IF( CXXTEST_FOUND )
  INCLUDE_DIRECTORIES( ${CXXTEST_INCLUDE_DIR} )
  # Package settings:
  SET( CXXTEST_USE_PYTHON 1 )
  # Activate CTest and "test" target
  ENABLE_TESTING()
ENDIF( CXXTEST_FOUND )


# Global compiler flags:
#-----------------------
# -Wno-long-long    since on Ubuntu 8.10 it won't compile without it
# -ansi             force ISO-C++98 compliance (not GNU++98)
SET( CMAKE_CXX_FLAGS "-Wall -pedantic -ansi -Wno-long-long" )

ADD_SUBDIRECTORY( math )
ADD_SUBDIRECTORY( dataHandler )
ADD_SUBDIRECTORY( graphicsEngine )
ADD_SUBDIRECTORY( gui )
ADD_SUBDIRECTORY( plugins )
ADD_SUBDIRECTORY( utils )
ADD_EXECUTABLE( walnut OpenWalnut.cpp utils/WOptionHandler.cpp )

TARGET_LINK_LIBRARIES( walnut guiqt4 ${Boost_LIBRARIES} )


# Special targets:
#-----------------
# Creates API via Doxygen
ADD_CUSTOM_TARGET(doc
                  COMMAND doxygen ${PROJECT_SOURCE_DIR}/../doc/developer/doxygenConfig
                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                  COMMENT "Build doxygen documentation"
                  VERBATIM )

# Checks style guide lines via the BrainLint tool against all source code files
FILE(GLOB_RECURSE STYLE_CPP ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB_RECURSE STYLE_H ${PROJECT_SOURCE_DIR}/*.h)
ADD_CUSTOM_TARGET(stylecheck
                  COMMAND  ${PROJECT_SOURCE_DIR}/../tools/brainlint.py ${STYLE_CPP} ${STYLE_H}
                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                  COMMENT "Check if code complies to CodingStandard" )
