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

SET( LibName ${OW_LIB_OPENWALNUTSCRIPT} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Search additional dependencies
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Python
FIND_PACKAGE( PythonLibs )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS} )

# these lists contain the compile files if the appropriate interpreter was found
SET( INTERPRETER_CPP_FILES )
SET( INTERPRETER_H_FILES )

# the libs to link against
SET( INTERPRETER_LINK_LIBRARIES )
                        
# Python found?
IF( PYTHONLIBS_FOUND )
    ADD_DEFINITIONS( -DPYTHON_FOUND )

    # also get all compile  files 
    FILE( GLOB PYTHON_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/python/*.cpp )
    FILE( GLOB PYTHON_H_FILES   ${CMAKE_CURRENT_SOURCE_DIR}/python/*.h )

    # extend the information about available interpreters
    SET( INTERPRETER_CPP_FILES ${INTERPRETER_CPP_FILES} ${PYTHON_CPP_FILES} )
    SET( INTERPRETER_H_FILES ${INTERPRETER_H_FILES} ${PYTHON_H_FILES} )
    SET( INTERPRETER_LINK_LIBRARIES ${INTERPRETER_LINK_LIBRARIES} ${PYTHON_LIBRARIES} )
ENDIF()

# Lua
FIND_PACKAGE( Lua51 )
INCLUDE_DIRECTORIES( ${LUA_INCLUDE_DIR} )
FIND_PACKAGE( Luabind )
INCLUDE_DIRECTORIES( ${LUABIND_INCLUDE_DIR} )

# Lua found?
IF( LUABIND_FOUND AND LUA51_FOUND )
    ADD_DEFINITIONS( -DLUA_FOUND )

    # also get all compile  files 
    FILE( GLOB LUA_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/lua/*.cpp )
    FILE( GLOB LUA_H_FILES   ${CMAKE_CURRENT_SOURCE_DIR}/lua/*.h )

    # extend the information about available interpreters
    SET( INTERPRETER_CPP_FILES ${INTERPRETER_CPP_FILES} ${LUA_CPP_FILES} )
    SET( INTERPRETER_H_FILES ${INTERPRETER_H_FILES} ${LUA_H_FILES} )
    SET( INTERPRETER_LINK_LIBRARIES ${INTERPRETER_LINK_LIBRARIES} ${LUA_LIBRARIES} ${LUABIND_LIBRARIES} )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Add sources as target
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Collect the compile-files for this target

# NOTE: we do not use COLLECT_COMPILE_FILES as this searches recursively. W do it the old style
# COLLECT_COMPILE_FILES( "${CMAKE_CURRENT_SOURCE_DIR}" TARGET_CPP_FILES TARGET_H_FILES TARGET_TEST_FILES )

# Search common compile files:
FILE( GLOB CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
FILE( GLOB H_FILES   ${CMAKE_CURRENT_SOURCE_DIR}/*.h )
FILE( GLOB WRAPPER_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/wrappers/*.cpp )
FILE( GLOB WRAPPER_H_FILES   ${CMAKE_CURRENT_SOURCE_DIR}/wrappers/*.h )

# Add the interpreter sources. Please note that these variables are empty if the interpreters where not found
SET( TARGET_CPP_FILES ${CPP_FILES} ${WRAPPER_CPP_FILES} ${INTERPRETER_CPP_FILES} )
SET( TARGET_H_FILES ${H_FILES} ${WRAPPER_H_FILES} ${INTERPRETER_H_FILES} )

# Setup the target
ADD_LIBRARY( ${LibName} SHARED ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${OW_VERSION_HEADER} )
TARGET_LINK_LIBRARIES( ${LibName} ${OW_LIB_OPENWALNUT}
                       ${Boost_LIBRARIES}
                       ${INTERPRETER_LINK_LIBRARIES}                      
                     )

# Tell CMake that someone creates this file for us. See doc of SETUP_VERSION_HEADER for details why this is needed.
# NOTE: this needs to be placed AFTER ADD_EXECUTABLE or ADD_LIBRARY
SETUP_USE_VERSION_HEADER( ${LibName} )

# Set the version of the library.
SET_TARGET_PROPERTIES( ${LibName} PROPERTIES 
                       VERSION ${OW_LIB_VERSION} 
                       SOVERSION ${OW_SOVERSION}
)

# Do not forget the install targets
SETUP_LIB_INSTALL( ${LibName} ${OW_LIBRARY_DIR_RELATIVE} "SCRIPT" )

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
                    "${TARGET_CPP_FILES};${TARGET_H_FILES};${TARGET_TEST_FILES}"  # add all these files to the stylechecker
                    "" )                                        # exlude some ugly files
