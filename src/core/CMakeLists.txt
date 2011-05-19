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

# Collect the compile-files for OWCore
COLLECT_COMPILE_FILES( "${CMAKE_CURRENT_SOURCE_DIR}" CORE_CPP_FILES CORE_H_FILES CORE_TEST_FILES )

# Setup the OWCore target
ADD_LIBRARY( ${OWCoreName} SHARED ${CORE_CPP_FILES} ${CORE_H_FILES} )
TARGET_LINK_LIBRARIES( ${OWCoreName} ${OWExtBioSigName} ${OWExtEEPName} ${OWExtNiftiName}
                                     ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} ${OPENGL_gl_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES} 
                     )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Test Setup
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Setup tests of this target
SETUP_TESTS( "${CORE_TEST_FILES}" "${OWCoreName}" "" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Copy Shaders
# ---------------------------------------------------------------------------------------------------------------------------------------------------



