# this is a script that puts all shaders to the right place or 
# updates them if they are already there.
#--------------------------------------------------------------------

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/${SHARED_FILES_RELATIVE}/shaders)
FILE( GLOB_RECURSE ALL_GE_SHADERS ${OW_SOURCE_DIR}/core/graphicsEngine/shaders/shaders/* )
FOREACH( fname ${ALL_GE_SHADERS} )
  configure_file(  ${fname} ${CMAKE_BINARY_DIR}/bin/${SHARED_FILES_RELATIVE}/shaders/ COPYONLY)
ENDFOREACH( fname )

