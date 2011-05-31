#---------------------------------------------------------------------------
#
# Project: OpenWalnut ( http://www.openwalnut.org )
#
# Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
# For more information see http:#www.openwalnut.org/copying
#
# This file is part of OpenWalnut.
#
# OpenWalnut is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# OpenWalnut is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with OpenWalnut. If not, see <http:#www.gnu.org/licenses/>.
#
#---------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Some common setup
# ---------------------------------------------------------------------------------------------------------------------------------------------------

SET( LibName ${OWCoreName} )

# Set the prefix and suffix of dynamically loadable modules to match
# CMAKEs naming of them. The quotes are necessary so the shell doesn't mess
# with these defines. Unfortunately, the Visual Studio has problems handling
# the quotes on the command line. WSharedLib::getSystemPrefix and
# getSystemSuffix hard codes the right strings for VC.
# Maybe you have to spend another 500 bucks to have your VC support quotes.
IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
    ADD_DEFINITIONS( '-DW_LIB_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"' )
    ADD_DEFINITIONS( '-DW_LIB_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"' )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Add sources as target
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Collect the compile-files for this target
COLLECT_COMPILE_FILES( "${CMAKE_CURRENT_SOURCE_DIR}" TARGET_CPP_FILES TARGET_H_FILES TARGET_TEST_FILES )

# Setup the target
ADD_LIBRARY( ${LibName} SHARED ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${OW_VERSION_HEADER} )
TARGET_LINK_LIBRARIES( ${LibName} ${OWExtBioSigName} ${OWExtEEPName} ${OWExtNiftiName}
                                  ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} ${OPENGL_gl_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES}
                     )

# Tell CMake that someone creates this file for us. See doc of SETUP_VERSION_HEADER in BuildUtils.cmake for details why this is needed.
# NOTE: this needs to be placed AFTER ADD_EXECUTABLE or ADD_LIBRARY
SETUP_USE_VERSION_HEADER( ${LibName} )

# Do not forget the install targets
INSTALL( TARGETS ${LibName}
			ARCHIVE # NOTE: this is needed on windows
				DESTINATION ${OW_LIBRARY_DIR_RELATIVE} 
				PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
							GROUP_READ GROUP_EXECUTE  
							WORLD_READ WORLD_EXECUTE
			LIBRARY # NOTE: this is needed for all the others
                DESTINATION ${OW_LIBRARY_DIR_RELATIVE}
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                            GROUP_READ GROUP_EXECUTE
                            WORLD_READ WORLD_EXECUTE
)

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Test Setup
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Setup tests of this target
SETUP_TESTS( "${TARGET_TEST_FILES}" "${LibName}" "" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Copy Shaders
# ---------------------------------------------------------------------------------------------------------------------------------------------------

COLLECT_SHADER_FILES( ${CMAKE_CURRENT_SOURCE_DIR} TARGET_GLSL_FILES )
SETUP_SHADERS( "${TARGET_GLSL_FILES}" "${OW_SHARE_DIR_RELATIVE}/shaders" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Style Checker
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# setup the stylechecker. Ignore the platform specific stuff.
SETUP_STYLECHECKER( "${LibName}"
                    "${TARGET_CPP_FILES};${TARGET_H_FILES};${TARGET_TEST_FILES};${TARGET_GLSL_FILES}"  # add all these files to the stylechecker
                    ".*platformDependent\\/visualStudio\\/.*" )                                        # exlude some ugly files

