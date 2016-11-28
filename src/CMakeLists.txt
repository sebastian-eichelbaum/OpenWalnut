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
# General CMake Setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT( OpenWalnut )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Include the OpenWalnut build system.
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# where to find our utils and cmake modules?
SET( OW_TOOLS_DIR ${PROJECT_SOURCE_DIR}/../tools )

# append search path for FindModules:
LIST( APPEND CMAKE_MODULE_PATH ${OW_TOOLS_DIR}/cmake )

# use internally
SET( OW_EXTERNAL_MODULE false )

# These scripts contains all the needed tools to setup the build:
# * Compiler Setup
# * Common OpenWalnut Options
# * Third-party Dependencies Setup
# * Unit Testing Setup if found
# * Doxygen Setup if found
INCLUDE( OpenWalnut )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Resource/Doc Copy
#  - Resource copy is done by the OW parts. This just copies additional helpers
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# copy our debug utilities. This is only useful if compiled debug or RelWithDebInfo
IF( NOT cmake_build_type_tolower STREQUAL "release" )
    # NOTE: DO NOT add and install target here. Debugging tools are not useful and wished in an OpenWalnut installation.
    ADD_CUSTOM_TARGET( CopyDebugUtilities
        ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../tools/debugging" "${PROJECT_BINARY_DIR}/"
        COMMENT "Copying debug utilities"
    )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Convenience targets
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# some convenience install targets:
ADD_CUSTOM_TARGET( list_install_tarets
                   COMMAND echo The following components can be installed:
                   COMMAND echo " * install - install everything."
                   COMMAND echo " * install_runtime - install the UIs."
                   COMMAND echo " * install_lib - install only libopenwalnut."
                   COMMAND echo " * install_modules - install only the compiled modules."
                   COMMAND echo " * install_dev - install the development headers for libopenwalnut."
                   COMMAND echo " * install_devdoc - install the development documentation for libopenwalnut."
                   COMMENT "List installation targets."
                 )

# install targets for different parts
ADD_CUSTOM_TARGET( install_runtime
                    ${CMAKE_COMMAND}
                        -DCOMPONENT=QTGUI
                        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
                 )
ADD_CUSTOM_TARGET( install_lib
                    ${CMAKE_COMMAND}
                        -DCOMPONENT=CORE
                        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
                    ${CMAKE_COMMAND}
                        -DCOMPONENT=EXT
                        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
                 )
ADD_CUSTOM_TARGET( install_modules
                    ${CMAKE_COMMAND}
                        -DCOMPONENT=MODULES
                        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
                 )
ADD_CUSTOM_TARGET( install_dev
                    ${CMAKE_COMMAND}
                        -DCOMPONENT=CORE_DEV
                        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
                 )
ADD_CUSTOM_TARGET( install_devdoc
                    ${CMAKE_COMMAND}
                        -DCOMPONENT=CORE_DOC
                        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
                    DEPENDS core_devdoc
                 )
ADD_DEPENDENCIES( install_devdoc core_devdoc )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Compilation Targets
#  - The GUI + OpenWalnut.cpp ==> openwalnut binary
#  - Ext ==> libOWext_*
#  - All the others ==> libOWcore
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# -----------------------------------------------------------------------------------------------------------------------------------------------
# core library

# build core
ADD_SUBDIRECTORY( core )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# Compilation fixes:

OPTION( OW_FIX_BOOST_PO_ARG "Enable this to enable the boost::program_options linker fix." ON )
OPTION( OW_FIX_EIGENSYSTEM_GCC_PARSE_ERROR "Enable this to disable the eigenSystem module. It causes trouble when using GCC and the latest eigen3 lib." ON )
OPTION( OW_FIX_EEP_WINDOWS_ERROR "Enable this to disable the EEP Reader. It causes trouble when in the winnt.h header on Windows." OFF )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# QT GUI

OPTION( OW_GUI_QT "Enable this to build the QT-based OpenWalnut GUI." ON )
IF( OW_GUI_QT )
    # build
    ADD_SUBDIRECTORY( qtgui )
ENDIF()


# -----------------------------------------------------------------------------------------------------------------------------------------------
# Scripting UI

OPTION( OW_UI_SCRIPT "Enable this to build the script-based OpenWalnut interface for commandline-only use." ON )
IF( OW_UI_SCRIPT )
    # build
    ADD_SUBDIRECTORY( scriptUI )
ENDIF()

# -----------------------------------------------------------------------------------------------------------------------------------------------
# Modules

# build modules
OPTION( OW_MODULE_BUILD "Enable this to build the modules." ON )
IF( OW_MODULE_BUILD )
  ADD_SUBDIRECTORY( modules )
ENDIF()

