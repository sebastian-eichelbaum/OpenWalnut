# Adapted flags for this directory to inhibit all warnings.
# There is a good reason for this: We do not want to debug
# third party software!

IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
    SET( CMAKE_C_FLAGS "/TC /DWIN32")
ELSE()
    SET( CMAKE_C_FLAGS "-w")
ENDIF()

FILE( GLOB LIBEEP_SRC "libavr/*.c" "libcnt/*.c" "libeep/*.c" )

ADD_LIBRARY( ${OWExtEEPName} SHARED ${LIBEEP_SRC} )
IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
    TARGET_LINK_LIBRARIES( ${OWExtEEPName} )
ELSE()
    TARGET_LINK_LIBRARIES( ${OWExtEEPName} m )
ENDIF()

IF(MSVC_IDE)
    SET_TARGET_PROPERTIES( ${OWExtEEPName} PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)

# Set the version of the library.
SET_TARGET_PROPERTIES( ${OWExtEEPName} PROPERTIES
        VERSION ${OW_LIB_VERSION} 
        SOVERSION ${OW_SOVERSION}
)

# install target
INSTALL( TARGETS ${OWExtEEPName}
            ARCHIVE # NOTE: this is needed on windows
                DESTINATION ${OW_LIBRARY_DIR_RELATIVE}
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                            GROUP_READ GROUP_EXECUTE
                            WORLD_READ WORLD_EXECUTE
            RUNTIME # NOTE: this is needed on windows
                DESTINATION ${OW_LIBRARY_DIR_RELATIVE}
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                            GROUP_READ GROUP_EXECUTE
                            WORLD_READ WORLD_EXECUTE
            LIBRARY # NOTE: this is needed for all the others
                DESTINATION ${OW_LIBRARY_DIR_RELATIVE}
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                            GROUP_READ GROUP_EXECUTE
                            WORLD_READ WORLD_EXECUTE
                NAMELINK_SKIP
         COMPONENT "EXT"
)
