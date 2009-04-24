add_subdirectory( qt4 )

list( APPEND GUI_SRC 
  BGUI.cpp
)

add_library( gui SHARED ${GUI_SRC} )