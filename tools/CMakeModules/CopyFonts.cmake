# this is a script that puts all fonts to the right place
#--------------------------------------------------------------------

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/${SHARED_FILES_RELATIVE}/fonts)
FILE( GLOB_RECURSE ALL_GE_FONTS ${OW_SOURCE_DIR}/graphicsEngine/fonts/* )
FOREACH( fname ${ALL_GE_FONTS} )
  #message( ${fname} )
  configure_file(  ${fname} ${CMAKE_BINARY_DIR}/bin/${SHARED_FILES_RELATIVE}/fonts/ COPYONLY)
ENDFOREACH( fname )

# this is kind of a hack as the osgViewer::StatsHandler does not allow us to change the font path -> copy the file he needs to the destination he
# needs
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/fonts)
configure_file( ${OW_SOURCE_DIR}/graphicsEngine/fonts/arial.ttf ${CMAKE_BINARY_DIR}/bin/fonts/ COPYONLY)
