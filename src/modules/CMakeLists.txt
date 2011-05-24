# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Some modules need third party libraries. We provide them if they are available
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

include( "modules-dependencies.cmake" )

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

