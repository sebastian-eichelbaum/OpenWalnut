# this is a script that puts all needed shell scripts to the right place
#-----------------------------------------------------------------------

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR})
FILE( GLOB_RECURSE ALL_SHELLSCRIPTS ${OW_SOURCE_DIR}/../scripts/* )
FOREACH( fname ${ALL_SHELLSCRIPTS} )
    #message( ${fname} )
    configure_file(  ${fname} ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
ENDFOREACH( fname )
