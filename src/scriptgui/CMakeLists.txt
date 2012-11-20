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

# NOTE: to ensure a uniqze script gui name, add gui to the script lib name. This is used as target name in cmake and is NOT the name of the
# binary.
SET( LibName "${OW_LIB_OPENWALNUTSCRIPT}-gui" )
# NOTE: ensure that this  name is uniqe. It must not match the scripting lib's name
SET( BinaryName "openwalnut-script" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Search additional dependencies
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Add sources as target
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Collect the compile-files for this target
COLLECT_COMPILE_FILES( "${CMAKE_CURRENT_SOURCE_DIR}" TARGET_CPP_FILES TARGET_H_FILES TARGET_TEST_FILES )

# This is needed since the mocs will be generated there
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

# Setup the target
ADD_EXECUTABLE( ${BinaryName} OpenWalnutScript.cpp ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${OW_VERSION_HEADER} )
TARGET_LINK_LIBRARIES( ${BinaryName} ${OW_LIB_OPENWALNUT} ${OW_LIB_OPENWALNUTSCRIPT} ${Boost_LIBRARIES} )

# Tell CMake that someone creates this file for us. See doc of SETUP_VERSION_HEADER for details why this is needed.
# NOTE: this needs to be placed AFTER ADD_EXECUTABLE or ADD_LIBRARY
SETUP_USE_VERSION_HEADER( ${BinaryName} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Test Setup
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Setup tests of this target
SETUP_TESTS( "${TARGET_TEST_FILES}" "${LibName}" "" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Doxygen Release documentation
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# included in standard developer doc

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Copy local resources
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# no local resources yet

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Copy Shaders
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# no shaders

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Style Checker
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# setup the stylechecker. Ignore the platform specific stuff.
SETUP_STYLECHECKER( "${LibName}"
                    "${TARGET_CPP_FILES};${TARGET_H_FILES};${TARGET_TEST_FILES};${TARGET_GLSL_FILES}"  # add all these files to the stylechecker
                    "" )                                                                               # exlude some ugly files
