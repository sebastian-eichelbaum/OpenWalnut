# Adapted flags for this directory to inhibit all warnings.
# There is a good reason for this: We do not want to debug
# third party software!

IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
    SET( CMAKE_C_FLAGS "/TC /DWIN32")
ELSE()
    SET( CMAKE_C_FLAGS "-w")
ENDIF()

FILE( GLOB LIBEEP_SRC "libavr/*.c" "libcnt/*.c" "libeep/*.c" )

ADD_LIBRARY( OWext_eep ${LIBEEP_SRC} )
IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
    TARGET_LINK_LIBRARIES( OWext_eep )
ELSE()
    TARGET_LINK_LIBRARIES( OWext_eep m )
ENDIF()

IF(MSVC_IDE)
    SET_TARGET_PROPERTIES( OWext_eep PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)
