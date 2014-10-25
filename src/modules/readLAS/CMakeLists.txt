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

# we use the directory name as module name
GET_FILENAME_COMPONENT( MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Search own dependencies
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# 1: FindPackage( LibYouNeed )
# 2: INCLUDE_DIRECTORIES( ${LIBYOUNEED_INCLUDE_DIR} )
# 3: LINK_DIRECTORIES( ${LIBYOUNEED_LIBRARY_DIRS} )
# 4: Add ${LIBYOUNEED_LIBRARY} as _MODULE_DEPENDENCIES parameter to SETUP_MODULE

# -----------------------------------------------------------------------------------------------------------------------------------------------
# LibLAS, at least 1.7
# See http://liblas.org
FIND_PACKAGE( liblas QUIET )
IF( LIBLAS_FOUND )
   MESSAGE( STATUS "Using system installed liblas to build readLAS." )
   INCLUDE_DIRECTORIES( ${LIBLAS_INCLUDE_DIR} )

    # ------------------------------------------------------------------------------------------------------------------------------------------------
    # Setup for compilation
    # ------------------------------------------------------------------------------------------------------------------------------------------------

    # Let this function do the job. It sets up tests and copies shaders automatically. It additionally configures the stylecheck mechanism for this
    # module.
    SETUP_MODULE( ${MODULE_NAME}                # name of the module
                 "${CMAKE_CURRENT_SOURCE_DIR}"  # where to find the source ${CMAKE_CURRENT_SOURCE_DIR} is a good idea here mostly
                 "${LIBLAS_LIBRARY};${LIBLAS_C_LIBRARY}"   # does your module need additional libs to compile?
                 ""                             # do you want to exclude files from stylechecking? (externals for example)
    )
ENDIF()

