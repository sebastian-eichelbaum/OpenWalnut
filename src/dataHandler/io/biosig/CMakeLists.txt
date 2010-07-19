# Adapted flags for this directory to inhibit all warnings.
# There is a good reason for this: We do not want to debug 
# third party software!

IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
    SET( CMAKE_CXX_FLAGS "/EHsc /DWIN32")
    SET( CMAKE_C_FLAGS "/TC /DWIN32")
ELSE()
    SET( CMAKE_CXX_FLAGS "-pedantic -Wno-write-strings -Wno-deprecated -Wformat=0")
    SET( CMAKE_C_FLAGS "-w")
ENDIF()

FILE( GLOB BIOSIG_SRC "*.c" "*.cpp" "*.h" "XMLParser/*.cpp" "XMLParser/*.h" )

ADD_LIBRARY( OWext_biosig ${BIOSIG_SRC} )

IF( CMAKE_HOST_SYSTEM MATCHES Windows )
    TARGET_LINK_LIBRARIES( OWext_biosig ws2_32 )
ENDIF()
