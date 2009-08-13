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
### BOOST ###
SET( Boost_USE_MULTITHREAD ON )
# To see which boost libs we currently use, you may run the following command
# in the trunk/src directory on a linux box to make some investigations:
# grep -i include `find . -type f -and -not -name '*svn*'` | grep boost | awk '{print $2}' | sort | uniq
FIND_PACKAGE( Boost 1.36.0 REQUIRED program_options thread )
include_directories( ${Boost_INCLUDE_DIR} )

### OpenGL ###
FIND_PACKAGE( OpenGL REQUIRED )

### CxxTest ###
FIND_PACKAGE( CxxTest QUIET )
MARK_AS_ADVANCED ( CXXTEST_PERL_TESTGEN_EXECUTABLE )
FIND_PACKAGE( OpenSceneGraph REQUIRED osgUtil osgDB osgViewer)

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
SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH 
        "The executables will be put into this directory.")
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH )


# Set default build type
IF ( NOT CMAKE_BUILD_TYPE )
    SET( CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: Debug Release RelWithDebInfo Static" 
    FORCE )
ENDIF ( NOT CMAKE_BUILD_TYPE )


# Enable unit tests via CxxTest
IF( CXXTEST_FOUND )
  INCLUDE_DIRECTORIES( ${CXXTEST_INCLUDE_DIR} )
  # Package settings:
  SET( CXXTEST_USE_PYTHON 1 )
  # Activate CTest and "test" target
  ENABLE_TESTING()
ENDIF( CXXTEST_FOUND )


# Global compiler flags:
#-------------------------------------------------------------------------------------------------------------
# -Wno-long-long    since on Ubuntu 8.10 it won't compile without it
# -ansi             force ISO-C++98 compliance (not GNU++98)
SET( CMAKE_CXX_FLAGS "-Wall -pedantic -ansi -Wno-long-long" CACHE STRING "" FORCE )
SET( CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -O0" CACHE STRING "" FORCE )
SET( CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "" FORCE )
SET( CMAKE_CXX_FLAGS_STATIC "-O3" CACHE STRING "" FORCE )
MARK_AS_ADVANCED( CMAKE_CXX_FLAGS_STATIC )
SET( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -DDEBUG -O2" CACHE STRING "" FORCE )

INCLUDE_DIRECTORIES( ${OPENSCENEGRAPH_INCLUDE_DIRS} )

IF ( NOT CMAKE_BUILD_TYPE STREQUAL "Static" )
    ADD_SUBDIRECTORY( math )
    ADD_SUBDIRECTORY( dataHandler )
    ADD_SUBDIRECTORY( graphicsEngine )
    ADD_SUBDIRECTORY( gui )
    ADD_SUBDIRECTORY( plugins )
    ADD_SUBDIRECTORY( utils )
    ADD_SUBDIRECTORY( kernel )
    ADD_SUBDIRECTORY( common )
    ADD_EXECUTABLE( walnut OpenWalnut.cpp utils/WOptionHandler.cpp )
    TARGET_LINK_LIBRARIES( walnut kernel guiqt4 ${Boost_LIBRARIES} )
    
ELSE ( NOT CMAKE_BUILD_TYPE STREQUAL "Static" )
    FILE( GLOB_RECURSE ALL_SRC ${PROJECT_SOURCE_DIR}/*.cpp )
    FILE( GLOB NIFTI_SRC "dataHandler/io/nifti/*.c" )
    ADD_DEFINITIONS(-DHAVE_ZLIB)

    # Package dependencies:
    FIND_PACKAGE( Qt4 REQUIRED )
    FIND_PACKAGE( GLEW REQUIRED )

    # Includes:
    INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )

    # Libraries for linking:
    SET( QT_LINK_LIBRARIES ${QT_QTCORE_LIBRARY}
                       ${QT_QTGUI_LIBRARY}
                       ${QT_QTOPENGL_LIBRARY} )

    # add here all classes that inherit from QObject 
    SET(GUI_QT4_MOC_HDRS
        gui/qt4/WMainWindow.h
    )

    QT4_WRAP_CPP(GUI_QT4_MOC_SRCS ${GUI_QT4_MOC_HDRS} )
    
    ADD_EXECUTABLE( walnut ${ALL_SRC}  ${NIFTI_SRC} ${GUI_QT4_MOC_SRCS} )
    TARGET_LINK_LIBRARIES( walnut ${Boost_LIBRARIES} ${QT_LINK_LIBRARIES} ${GLEW_LIBRARY} )
ENDIF ( NOT CMAKE_BUILD_TYPE STREQUAL "Static" )




# Special targets:
#-------------------------------------------------------------------------------------------------------------
# Creates API via Doxygen
ADD_CUSTOM_TARGET(doc
                  COMMAND doxygen ${PROJECT_SOURCE_DIR}/../doc/developer/doxygenConfig
                  COMMAND chmod -R g+r ${PROJECT_SOURCE_DIR}/../doc/developer/html
                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                  COMMENT "Build doxygen documentation"
                  VERBATIM )

# Uses ctags to create source tags, usable in many editors like VIM or Emacs
ADD_CUSTOM_COMMAND(OUTPUT ctags
                   COMMAND ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/
                   COMMENT "Build ctags tagfile"
                  )

# Uses cscope to create several source graphs (dependency, call, caller, ...), usable in many editors like VIM or Emacs
ADD_CUSTOM_COMMAND(OUTPUT cscope
                   COMMAND cscope -R -b
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/
                   COMMENT "Build cscope tagfile"
                  )

# The command "make tags" creates ctags and cscope tag files
ADD_CUSTOM_TARGET(tags
                  DEPENDS ctags
                  DEPENDS cscope
                  )

# Checks style guide lines via the BrainLint tool against all source code files
#    The output is colored when variable CMAKE_COLOR_MAKEFILE is set to ON,
#    noncolored if it is set to OFF
FILE(GLOB_RECURSE STYLE_CPP ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB_RECURSE STYLE_H ${PROJECT_SOURCE_DIR}/*.h)

#-------------------------------------------------------------------------------------------------------------
#Set dirs to exclude from stylecheck
SET ( EXCLUDE_DIRS /dataHandler/io/nifti/ )

FOREACH( dirname ${EXCLUDE_DIRS} )
    FILE( GLOB EXCLUDE_H   ${PROJECT_SOURCE_DIR}${dirname}*.h )
    FILE( GLOB EXCLUDE_CPP ${PROJECT_SOURCE_DIR}${dirname}*.cpp )
    
    FOREACH( fname ${EXCLUDE_H} )
        LIST ( REMOVE_ITEM STYLE_H ${fname} )
    ENDFOREACH( fname )

    FOREACH( fname ${EXCLUDE_CPP} )
        LIST ( REMOVE_ITEM STYLE_CPP ${fname} )
    ENDFOREACH( fname )
ENDFOREACH( dirname )


IF( CMAKE_COLOR_MAKEFILE )
    ADD_CUSTOM_TARGET(stylecheck
        COMMAND  ${PROJECT_SOURCE_DIR}/../tools/brainlint.py --color ${STYLE_CPP} ${STYLE_H}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Check if code complies to CodingStandard" )
ELSE( CMAKE_COLOR_MAKEFILE )
    ADD_CUSTOM_TARGET(stylecheck
        COMMAND  ${PROJECT_SOURCE_DIR}/../tools/brainlint.py ${STYLE_CPP} ${STYLE_H}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Check if code complies to CodingStandard and use noncolored output for violation messages" )
ENDIF( CMAKE_COLOR_MAKEFILE )


#-------------------------------------------------------------------------------------------------------------
# Prints the verbose output of the test runners, so you may identify the source of error early, if any
ADD_CUSTOM_TARGET(vtest
                  COMMAND make test ARGS="-V"
                  COMMENT "Runs the test in verboseness to see what actually went wrong" )

#-------------------------------------------------------------------------------------------------------------
# Removes all tests thus forcing them to be recompiled during the next "make"
ADD_CUSTOM_TARGET(cleantest
                  COMMAND rm -f ${PROJECT_BINARY_DIR}/*/*_test.cc
                  COMMENT "Remove all tests to force them to be recompiled" )

#-------------------------------------------------------------------------------------------------------------
# Checks if all is well to commit (aka commitCheck, cicheck)
ADD_CUSTOM_TARGET(cicheck
                  COMMAND make test && make stylecheck
                  COMMENT "Checks if all is well to commit" )


#-------------------------------------------------------------------------------------------------------------
# Copies fixtures used for tests to the binary directory (where they are expected).
ADD_CUSTOM_TARGET( fixtures
                   ALL
                   COMMAND cp -rf ${PROJECT_SOURCE_DIR}/dataHandler/test/fixtures 
                        ${PROJECT_BINARY_DIR}/dataHandler/
                   COMMENT "Copy fixtures to the right place." )
