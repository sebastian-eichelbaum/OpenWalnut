# this is a script that puts all shaders to the right place or 
# updates them if they are already there.
#--------------------------------------------------------------------

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/shaders)
FILE( GLOB_RECURSE ALL_GE_SHADERS ${OW_SOURCE_DIR}/graphicsEngine/shaders/* )
FOREACH( fname ${ALL_GE_SHADERS} )
  #message( ${fname} )
  configure_file(  ${fname} ${CMAKE_BINARY_DIR}/bin/shaders/ COPYONLY)
ENDFOREACH( fname )
FILE( GLOB_RECURSE ALL_MODULE_SHADERS ${OW_SOURCE_DIR}/modules/*/shaders/* )
FOREACH( fname ${ALL_MODULE_SHADERS} )
  #message( ${fname} )
  configure_file(  ${fname} ${CMAKE_BINARY_DIR}/bin/shaders/ COPYONLY)
ENDFOREACH( fname )
FILE( GLOB_RECURSE ALL_KERNELMODULE_SHADERS ${OW_SOURCE_DIR}/kernel/modules/*/shaders/* )
FOREACH( fname ${ALL_KERNELMODULE_SHADERS} )
  #message( ${fname} )
  configure_file(  ${fname} ${CMAKE_BINARY_DIR}/bin/shaders/ COPYONLY)
ENDFOREACH( fname )
