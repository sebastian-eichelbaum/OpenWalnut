list( APPEND GUI_QT4_SRC 
  BMainWindow.cpp
  BQtGLWidget.cpp
  BQtPipelineBrowser.cpp
  )
  
add_library( guiqt4 SHARED ${GUI_QT4_SRC} )