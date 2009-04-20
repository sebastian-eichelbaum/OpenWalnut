cmake_minimum_required(VERSION 2.6)
PROJECT( BrainCognize )

find_package( Qt4 REQUIRED )
include_directories( ${QT_INCLUDE_DIR} )

set( 
  QT_LINK_LIBRARIES 
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTOPENGL_LIBRARY} 
  )

add_subdirectory( gui )

set( LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib )
mark_as_advanced( LIBRARY_OUTPUT_PATH )
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
mark_as_advanced( EXECUTABLE_OUTPUT_PATH )

set( CMAKE_CXX_FLAGS   "-Wall -pedantic " )

add_executable( bCognize bCognize.cpp )	

target_link_libraries( 
  bCognize
  guiqt4
  ${QT_LINK_LIBRARIES}
  )

#---------------------
# Special make tagets
#---------------------
add_custom_target(doc
                  COMMAND doxygen ${PROJECT_SOURCE_DIR}/../doc/developer/doxygenConfig > ${PROJECT_SOURCE_DIR}/../doc/developer/docErr.txt 2>&1
                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                  COMMENT "Build doxygen documentation"
                  VERBATIM
                  )
  