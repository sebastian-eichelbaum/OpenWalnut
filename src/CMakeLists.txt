CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( BrainCognize )

# Package dependencies:
FIND_PACKAGE( Qt4 REQUIRED )
FIND_PACKAGE( OpenGL REQUIRED )
FIND_PACKAGE( Boost REQUIRED )

# Includes:
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OPENGL_INCLUDE_DIR} )

# Libraries for linking:
SET( QT_LINK_LIBRARIES ${QT_QTCORE_LIBRARY}
                       ${QT_QTGUI_LIBRARY}
                       ${QT_QTOPENGL_LIBRARY} )

SET( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib )
MARK_AS_ADVANCED( LIBRARY_OUTPUT_PATH )
SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH )

# Global compiler flags:
#-----------------------
# -Wno-long-long    since on Ubuntu 8.10 it won't compile without it
# -ansi             force ISO-C++98 compliance (not GNU++98)
SET( CMAKE_CXX_FLAGS   "-Wall -pedantic -ansi -Wno-long-long" )

ADD_SUBDIRECTORY( math )
ADD_SUBDIRECTORY( dataHandler )
ADD_SUBDIRECTORY( graphicsEngine )
ADD_SUBDIRECTORY( gui )
ADD_SUBDIRECTORY( plugins )
ADD_EXECUTABLE( BCognize BCognize.cpp )

TARGET_LINK_LIBRARIES( BCognize guiqt4 ${QT_LINK_LIBRARIES} )


# Special targets:
#-----------------
# Creates API via Doxygen
ADD_CUSTOM_TARGET(doc
                  COMMAND doxygen ${PROJECT_SOURCE_DIR}/../doc/developer/doxygenConfig > ${PROJECT_SOURCE_DIR}/../doc/developer/docErr.txt 2>&1
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
