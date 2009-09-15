# Adapted flags for this directory to inhibit all warnings.
# There is a good reason for this: We do not want to debug 
# third party software!
SET( CMAKE_CXX_FLAGS "-pedantic -Wno-write-strings -Wno-deprecated ")
SET( CMAKE_C_FLAGS "-w")

FILE( GLOB BIOSIG_SRC "*.c" "*.cpp" "XMLParser/*.cpp" )
ADD_LIBRARY( biosig SHARED ${BIOSIG_SRC} )
