# this is a script that puts all fonts to the right place
#--------------------------------------------------------------------

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/${SHARED_FILES_RELATIVE}/fonts)
FILE( GLOB_RECURSE ALL_GE_FONTS ${OW_SOURCE_DIR}/graphicsEngine/fonts/* )
FOREACH( fname ${ALL_GE_FONTS} )
  #message( ${fname} )
  configure_file(  ${fname} ${CMAKE_BINARY_DIR}/bin/${SHARED_FILES_RELATIVE}/fonts/ COPYONLY)
ENDFOREACH( fname )

