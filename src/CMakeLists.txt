cmake_minimum_required(VERSION 2.4)
PROJECT( BrainCognize )

find_package( Qt4 REQUIRED )
include_directories( ${QT_INCLUDE_DIR} )

  set( 
    QT_LINK_LIBRARIES 
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTOPENGL_LIBRARY} 
    ${QT_QT3SUPPORT_LIBRARY}
    )

add_executable( bCognize bCognize.cpp )	

target_link_libraries( 
  bCognize
  ${QT_LINK_LIBRARIES}
  )
  