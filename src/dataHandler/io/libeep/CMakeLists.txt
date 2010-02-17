# Adapted flags for this directory to inhibit all warnings.
# There is a good reason for this: We do not want to debug 
# third party software!

IF( CMAKE_HOST_SYSTEM MATCHES Windows )
	SET( CMAKE_CXX_FLAGS "/EHsc /DWIN32")
	SET( CMAKE_C_FLAGS "/TC /DWIN32")
ELSE()
	SET( CMAKE_CXX_FLAGS "-pedantic -Wno-write-strings -Wno-deprecated -Wformat=0")
	SET( CMAKE_C_FLAGS "-w")
ENDIF()

INCLUDE_DIRECTORIES( . )

FILE( GLOB LIBEEP_SRC "avr/*.h" "cnt/*.h" "eep/*.h" "libavr/*.c" "libcnt/*.c" "libeep/*.c" )
ADD_LIBRARY( libeep SHARED ${LIBEEP_SRC} )
