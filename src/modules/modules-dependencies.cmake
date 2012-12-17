# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Some modules need third party libraries. We provide them if they are available
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# NOTE: for every third party lib you add here: search it -> if found: add option! Not: Add option -> if option -> search
# FIND_PACKAGE( Blah )
# IF( BLAH_FOUND )
#     OPTION( OW_USE_BLAH "Enable Blah support." )
#     IF( OW_USE_BLAH )
#           # do something
#     ENDIF( OW_USE_BLAH )
# ENDIF( BLAH_FOUND )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# Teem: http://teem.sourceforge.org
#  - NOTE: you need the SVN version

FIND_PACKAGE( Teem PATHS /usr/local/lib /opt/teem/lib QUIET )
IF( Teem_FOUND )    # we need to add the include path and lib path to allow the compiler and linker to find teem even if it was not installed to
                    # /usr/local
    INCLUDE_DIRECTORIES( ${Teem_INCLUDE_DIRS} )
    LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${Teem_LIBRARY_DIRS} )
ENDIF()


# -----------------------------------------------------------------------------------------------------------------------------------------------
# OpenCL: http://www.khronos.org/opencl

FIND_PACKAGE( OpenCL QUIET )
IF( OPENCL_FOUND )
    OPTION( OW_USE_OPENCL "Enable OpenCL support." )
    IF( OW_USE_OPENCL )
        # add the include directories
        INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIR} )
        INCLUDE_DIRECTORIES( ${OPENCL_CPP_INCLUDE_DIR} )

        # do not forget to add the libs as dependency to your module!
    ENDIF( OW_USE_OPENCL )
ENDIF( OPENCL_FOUND )

