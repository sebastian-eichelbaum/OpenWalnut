CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( BrainCognize )

# Package dependencies
FIND_PACKAGE( Qt4 REQUIRED )
FIND_PACKAGE( OpenGL REQUIRED )
FIND_PACKAGE( Boost REQUIRED )

# Includes
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OPENGL_INCLUDE_DIR} )

SET(
  QT_LINK_LIBRARIES
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTOPENGL_LIBRARY}
  )

ADD_SUBDIRECTORY( gui )

SET( LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib )
MARK_AS_ADVANCED( LIBRARY_OUTPUT_PATH )
SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH )

# Global compiler flags:
#-----------------------
# -Wno-long-long is used since on Ubuntu 8.10 it won't compile without it

SET( CMAKE_CXX_FLAGS   "-Wall -pedantic -Wno-long-long" )

ADD_EXECUTABLE( bCognize bCognize.cpp )

TARGET_LINK_LIBRARIES(
  bCognize
  guiqt4
  ${QT_LINK_LIBRARIES}
  )

#---------------------
# Special make tagets
#---------------------
ADD_CUSTOM_TARGET(doc
                  COMMAND doxygen ${PROJECT_SOURCE_DIR}/../doc/developer/doxygenConfig > ${PROJECT_SOURCE_DIR}/../doc/developer/docErr.txt 2>&1
                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                  COMMENT "Build doxygen documentation"
                  VERBATIM
                  )

