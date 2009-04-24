list( APPEND GE_SRC 
  BGLScenePainter.cpp
)

add_library( ge SHARED ${GE_SRC} )