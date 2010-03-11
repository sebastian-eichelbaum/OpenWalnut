# this is a script that puts all needed shell scripts to the right place
#-----------------------------------------------------------------------

  SET( CMAKE_BINARY_DIRWIN ${CMAKE_BINARY_DIR}/bin )
  message( ${CMAKE_BINARY_DIRWIN} )
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIRWIN})

FILE( GLOB_RECURSE ALL_SHELLSCRIPTS ${OW_SOURCE_DIR}/../scripts/* )
FOREACH( fname ${ALL_SHELLSCRIPTS} )
    #message( ${fname} )
    configure_file(  ${fname} ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
ENDFOREACH( fname )
