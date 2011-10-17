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
#
# General CPack Setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# For a list of possible generators, see http://www.vtk.org/Wiki/CMake:CPackPackageGenerators
SET( CPACK_GENERATOR TGZ STGZ )
# NOTE: DEBs
# We do not create debian packages using CPack. It creates binary packages, which is not needed. We want a source package.

# For a list of possible settings for CPack, see  http://www.cmake.org/Wiki/CMake:CPackConfiguration

# Disallow CPack to create an top-level directory?
# SET( CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0 )
SET( CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/../README )
SET( CPACK_PACKAGE_VENDOR "OpenWalnut Community" )
SET( CPACK_PACKAGE_CONTACT "Sebastian Eichelbaum <eichelbaum@informatik.uni-leipzig.de>" )
SET( CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/../COPYING )
SET( CPACK_RESOURCE_FILE_README ${PROJECT_SOURCE_DIR}/../README )

# Version
# TODO(all): we should somehot automate this (using hg or our VERSION file for example)
SET( CPACK_PACKAGE_VERSION_MAJOR "1" )
SET( CPACK_PACKAGE_VERSION_MINOR "2" )
SET( CPACK_PACKAGE_VERSION_PATCH "0" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# TGZ and STGZ specific setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Who is the maintainer?
SET( CPACK_PACKAGE_CONTACT "Sebastian Eichelbaum <eichelbaum@informatik.uni-leipzig.de>" )
# Fortunately, TGZ and STGZ do not need any further setup

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# NSIS specific setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Who is the maintainer?
SET( CPACK_PACKAGE_CONTACT "André Reichenbach <reichenbach@informatik.uni-leipzig.de>" )

# TODO: do it, test it.

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Mac bundle specific setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Who is the maintainer?
SET( CPACK_PACKAGE_CONTACT "Mathias Goldau <math@informatik.uni-leipzig.de>" )

# TODO: do it, test it.

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# RPM specific setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Who is the maintainer?
SET( CPACK_PACKAGE_CONTACT "Mathias Goldau <math@informatik.uni-leipzig.de>" )

# TODO: do it, test it.

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Let CPack do its work
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------
    
INCLUDE( CPack )

