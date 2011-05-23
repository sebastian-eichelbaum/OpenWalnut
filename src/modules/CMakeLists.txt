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
# Cuda: http://www.nvidia.com

FIND_PACKAGE( CUDA QUIET )
IF( CUDA_FOUND )
    OPTION( OW_USE_CUDA "Enable Cuda support." )
    IF( OW_USE_CUDA )
        # BugFix: Subsequent calls don't set CUDA_VERSION_MAJOR or .._MINOR variables, so we have to extract them from CUDA_VERSION variable
        STRING(REGEX REPLACE "([0-9]+)\\.([0-9]+).*" "\\1" CUDA_VERSION_MAJOR ${CUDA_VERSION})
        STRING(REGEX REPLACE "([0-9]+)\\.([0-9]+).*" "\\2" CUDA_VERSION_MINOR ${CUDA_VERSION})

        # Enforce at least CUDA version 2.3, since the detTractClustering is know to require this at least
        SET( OW_MIN_CUDA_MAJOR_VERSION 2 )
        SET( OW_MIN_CUDA_MINOR_VERSION 3 )

        IF( CUDA_VERSION_MAJOR LESS OW_MIN_CUDA_MAJOR_VERSION )
            SET( CUDA_FOUND_BUT_LOW_VERSION 1 )
        ELSEIF( CUDA_VERSION_MAJOR EQUAL OW_MIN_CUDA_MAJOR_VERSION )
            IF( CUDA_VERSION_MINOR LESS OW_MIN_CUDA_MINOR_VERSION )
                SET( CUDA_FOUND_BUT_LOW_VERSION 1 )
            ENDIF()
        ENDIF()

        # abort incase invalid version of CUDA was found.
        IF( CUDA_FOUND_BUT_LOW_VERSION )
            MESSAGE( FATAL_ERROR "You have selected to use CUDA but an insufficent version: ${CUDA_VERSION_MAJOR}.${CUDA_VERSION_MINOR} was found, but ${OW_MIN_CUDA_MAJOR_VERSION}.${OW_MIN_CUDA_MINOR_VERSION} is required." )
        ENDIF()
        MESSAGE( STATUS "CUDA version: ${CUDA_VERSION_MAJOR}.${CUDA_VERSION_MINOR} was found." )

        # set some common options
        ADD_DEFINITIONS( -DCUDA_FOUND )
        set( CUDA_ATTACH_VS_BUILD_RULE_TO_CUDA_FILE ON )
        set( BUILD_SHARED_LIBS ON )
        set( CUDA_NVCC_FLAGS --compiler-options -fno-inline --ptxas-options=-v -arch=sm_12 )
        set( CUDA_PROPAGATE_HOST_FLAGS OFF )
        INCLUDE_DIRECTORIES( ${CUDA_TOOLKIT_INCLUDE} )
        INCLUDE_DIRECTORIES( "${CUDA_SDK_ROOT_DIR}/common/inc" )
    ENDIF( OW_USE_CUDA )
ENDIF( CUDA_FOUND )

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

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# 
# Toolbox Setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Toolbox mechanism. This is a nice possibility to turn on/off several modules. This can speed up build times if unneeded modules are turned off.
option( OW_MODULE_TOOLBOX_ALL             "Turn all toolboxes on." OFF )
option( OW_MODULE_TOOLBOX_BASE            "All the basic functionality modules. These modules are useful and needed for common visualizatuion tasks." ON )
option( OW_MODULE_TOOLBOX_EEG             "EEG data processing and visualization." ON )
option( OW_MODULE_TOOLBOX_IMAGEPROCESSING "Image processing tools like filters." ON )
option( OW_MODULE_TOOLBOX_IO              "Reader and Writer for several kinds of data." ON )
# this option is a simple list of all modules not in the above lists.
option( OW_MODULE_TOOLBOX_OTHERS          "Advanced processing and visualization tools currently in development which have not been categorized to one of the other toolboxes." ON )

# Include needed lists
if( OW_MODULE_TOOLBOX_BASE OR OW_MODULE_TOOLBOX_ALL )
  include( modules-base.toolbox )
endif( OW_MODULE_TOOLBOX_BASE OR OW_MODULE_TOOLBOX_ALL )

if( OW_MODULE_TOOLBOX_EEG OR OW_MODULE_TOOLBOX_ALL )
  include( modules-eeg.toolbox )
endif( OW_MODULE_TOOLBOX_EEG OR OW_MODULE_TOOLBOX_ALL )

if( OW_MODULE_TOOLBOX_IMAGEPROCESSING OR OW_MODULE_TOOLBOX_ALL )
  include( modules-imageprocessing.toolbox )
endif( OW_MODULE_TOOLBOX_IMAGEPROCESSING OR OW_MODULE_TOOLBOX_ALL )

if( OW_MODULE_TOOLBOX_IO OR OW_MODULE_TOOLBOX_ALL )
  include( modules-io.toolbox )
endif( OW_MODULE_TOOLBOX_IO OR OW_MODULE_TOOLBOX_ALL )

if( OW_MODULE_TOOLBOX_OTHERS OR OW_MODULE_TOOLBOX_ALL )
  include( modules-others.toolbox )
endif( OW_MODULE_TOOLBOX_OTHERS OR OW_MODULE_TOOLBOX_ALL )

# optional user toolbox which is not unter version control allowing all to only compile their modules needed.
include( modules-user.toolbox OPTIONAL )

