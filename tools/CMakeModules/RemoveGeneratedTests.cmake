# this is a platform independent script for removing *_test.cc files:
#--------------------------------------------------------------------

FILE( GLOB_RECURSE GENERATED_TESTS ${CMAKE_CURRENT_BINARY_DIR}/*_test.cc )
# we need a dummy in case of multiple invocation of the "make cleantest" target
LIST( APPEND GENERATED_TESTS non_existing_dummy_file )
FILE( REMOVE ${GENERATED_TESTS} )
