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

# Automatically add a module. Do not use this function if your module nees additional dependencies or similar. For more flexibility, use your own
# CMakeLists in combination with the SETUP_MODULE function. The Code for the module is searched in ${CMAKE_CURRENT_SOURCE_DIR}/${_MODULE_NAME}.
# It loads the CMakeLists in the module dir if there is any.
# _MODULE_NAME the name of the module
# Optional second Parameter: list of additional dependencies
# Optional third Parameter: list of style-excludes as regexp.
FUNCTION( ADD_MODULE _MODULE_NAME )
    SET( MODULE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${_MODULE_NAME} )

    # is there a CMakeLists.txt? If yes, use it.
    IF( EXISTS ${MODULE_SOURCE_DIR}/CMakeLists.txt )
        ADD_SUBDIRECTORY( ${_MODULE_NAME} )
        RETURN()
    ENDIF()

    # Optional second parameter: style exclusion list
    SET( _DEPENDENCIES ${ARGV1} )
    SET( _EXCLUDES ${ARGV2} )

    # -----------------------------------------------------------------------------------------------------------------------------------------------
    # Setup for compilation
    # -----------------------------------------------------------------------------------------------------------------------------------------------

    # Let this function do the job. It sets up tests and copies shaders automatically. It additionally configures the stylecheck mechanism for this
    # module.
    SETUP_MODULE( ${_MODULE_NAME}                # name of the module
                 "${MODULE_SOURCE_DIR}"
                 "${_DEPENDENCIES}"
                 "${_EXCLUDES}"
    )
ENDFUNCTION( ADD_MODULE )

# Comfortably setup a module for compilation. This automatically handles the target creation, stylecheck and tests (with fixtures).
# _MODULE_NAME the name of the module
# _MODULE_SOURCE_DIR where to finx the code for the module
# _MODULE_DEPENDENCIES additional dependencies can be added here. This is a list. Use ";" to add multiple additional dependencies
# _MODULE_STYLE_EXCLUDES exclude files from stylecheck matching these regular expressions (list)
FUNCTION( SETUP_MODULE _MODULE_NAME _MODULE_SOURCE_DIR _MODULE_DEPENDENCIES _MODULE_STYLE_EXCLUDES )
    # -----------------------------------------------------------------------------------------------------------------------------------------------
    # Some common setup
    # -----------------------------------------------------------------------------------------------------------------------------------------------

    # setup the target directories and names
    SET( MODULE_NAME ${_MODULE_NAME} )
    SET( MODULE_TARGET_DIR_RELATIVE ${OW_MODULE_DIR_RELATIVE}/${MODULE_NAME} )
    SET( MODULE_TARGET_RESOURCE_DIR_RELATIVE ${OW_SHARE_DIR_RELATIVE}/modules/${MODULE_NAME} )
    SET( MODULE_TARGET_DIR ${PROJECT_BINARY_DIR}/${MODULE_TARGET_DIR_RELATIVE} )
    SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MODULE_TARGET_DIR} )
    SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MODULE_TARGET_DIR} )
    SET( MODULE_SOURCE_DIR ${_MODULE_SOURCE_DIR} )

    # -----------------------------------------------------------------------------------------------------------------------------------------------
    # Add sources as target
    # -----------------------------------------------------------------------------------------------------------------------------------------------

    # Collect the compile-files for this target
    COLLECT_COMPILE_FILES( "${MODULE_SOURCE_DIR}" TARGET_CPP_FILES TARGET_H_FILES TARGET_TEST_FILES )

    # Setup the target
    ADD_LIBRARY( ${MODULE_NAME} SHARED ${TARGET_CPP_FILES} ${TARGET_H_FILES} )
    TARGET_LINK_LIBRARIES( ${MODULE_NAME} ${OWCoreName} ${Boost_LIBRARIES} ${OPENGL_gl_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES} ${_MODULE_DEPENDENCIES} )

    # Set the version of the library.
    SET_TARGET_PROPERTIES( ${MODULE_NAME} PROPERTIES
        VERSION ${OW_LIB_VERSION}
        SOVERSION ${OW_SOVERSION}
    )

    # Do not forget the install targets
    SETUP_LIB_INSTALL( ${MODULE_NAME} ${MODULE_TARGET_DIR_RELATIVE} "MODULES" )

    # -----------------------------------------------------------------------------------------------------------------------------------------------
    # Test Setup
    # -----------------------------------------------------------------------------------------------------------------------------------------------

    # Setup tests of this target
    SETUP_TESTS( "${TARGET_TEST_FILES}" "${MODULE_NAME}" "${_MODULE_DEPENDENCIES}" )

    # -----------------------------------------------------------------------------------------------------------------------------------------------
    # Copy Shaders
    # -----------------------------------------------------------------------------------------------------------------------------------------------

    COLLECT_SHADER_FILES( ${MODULE_SOURCE_DIR} TARGET_GLSL_FILES )
    SETUP_SHADERS( "${TARGET_GLSL_FILES}" "${MODULE_TARGET_RESOURCE_DIR_RELATIVE}/shaders" "MODULES" )

    # -----------------------------------------------------------------------------------------------------------------------------------------------
    # Copy Additional Resources
    # -----------------------------------------------------------------------------------------------------------------------------------------------
    SETUP_RESOURCES_GENERIC( "${MODULE_SOURCE_DIR}/resources" ${MODULE_TARGET_RESOURCE_DIR_RELATIVE} "${_MODULE_NAME}" "MODULES" )

    # -----------------------------------------------------------------------------------------------------------------------------------------------
    # Style Checker
    # -----------------------------------------------------------------------------------------------------------------------------------------------

    # setup the stylechecker. Ignore the platform specific stuff.
    SETUP_STYLECHECKER( "${MODULE_NAME}"
                        "${TARGET_CPP_FILES};${TARGET_H_FILES};${TARGET_TEST_FILES};${TARGET_GLSL_FILES}"  # add all these files to the stylechecker
                        "${_MODULE_STYLE_EXCLUDES}" )                                                      # exlude some ugly files

ENDFUNCTION( SETUP_MODULE )

