SET( CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -Wno-long-long")#without "-ansi"
FILE( GLOB BIOSIG_SRC "*.c" "*.cpp" "XMLParser/*.cpp" )
ADD_LIBRARY( biosig SHARED ${BIOSIG_SRC} )
