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

SET( LibName ${OW_LIB_OPENWALNUT} )

# Set the prefix and suffix of dynamically loadable modules to match
# CMAKEs naming of them. The quotes are necessary so the shell doesn't mess
# with these defines. WSharedLib::getSystemPrefix and
# getSystemSuffix hard codes the right strings for VC.
ADD_DEFINITIONS( '-DW_LIB_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"' )
ADD_DEFINITIONS( '-DW_LIB_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"' )

OPTION( OW_STATIC_BUILD "Enable this to build the core library as static library." OFF )

# the files to link against if we found the necessary libs for a script interpreter
SET( INTERPRETER_LINK_LIBRARIES )

IF( BUILD_PYTHON_INTERPRETER )
    # Python
    FIND_PACKAGE( PythonLibs REQUIRED )
    
    # Python found?
    IF( PYTHONLIBS_FOUND AND Boost_FOUND )
        INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS} )
        ADD_DEFINITIONS( -DPYTHON_FOUND )
        SET( INTERPRETER_LINK_LIBRARIES ${INTERPRETER_LINK_LIBRARIES} ${PYTHON_LIBRARIES} )
    ENDIF()
ENDIF() # BUILD_SCRIPTENGINE

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Add sources as target
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Collect the compile-files for this target
COLLECT_COMPILE_FILES( "${CMAKE_CURRENT_SOURCE_DIR}" TARGET_CPP_FILES TARGET_H_FILES TARGET_TEST_FILES )

# Setup the target
IF( OW_STATIC_BUILD )
    ADD_LIBRARY( ${LibName} STATIC ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${OW_VERSION_HEADER} )
ELSE()
    ADD_LIBRARY( ${LibName} SHARED ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${OW_VERSION_HEADER} )
ENDIF()

TARGET_LINK_LIBRARIES( ${LibName} ${Boost_LIBRARIES} ${CMAKE_STANDARD_LIBRARIES} ${CMAKE_DL_LIBS} ${OPENGL_gl_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES} ${INTERPRETER_LINK_LIBRARIES}
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
SETUP_LIB_INSTALL( ${LibName} ${OW_LIBRARY_DIR_RELATIVE} "CORE" )

# Core contains the headers which are needed for any developer release. Install them.
# NOTE: do not use ${TARGET_H_FILES}. The command requires a list
SETUP_DEV_INSTALL( ${LibName} ${OW_LIBRARY_DIR_RELATIVE} TARGET_H_FILES "include/openwalnut/core" "CORE_DEV" )
# Install the header too
INSTALL( FILES ${OW_VERSION_HEADER} 
         DESTINATION "include/openwalnut/core"
         COMPONENT "CORE_DEV"
)

# The core does also provide some nice pkg-config file. Set this up and configure it to contain the correct install prefix
SETUP_CONFIGURED_FILE( "core-dev" "lib/pkgconfig/openwalnut.pc" "CORE_DEV" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Test Setup
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Setup tests of this target
SETUP_TESTS( "${TARGET_TEST_FILES}" "${LibName}" "" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Doxygen Release documentation
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# NOTE: this full documentation also includes call/caller graphs, colaboration graphs and nicer inheritance graphs
# but needs "dot" from graphviz to render them.
SET( DEVDOC_DIR "${PROJECT_SOURCE_DIR}/../doc/core/html" )
ADD_CUSTOM_TARGET( core_devdoc
                   COMMAND doxygen ${PROJECT_SOURCE_DIR}/../doc/core/doxygenConfig
                   COMMAND chmod -R g+r ${DEVDOC_DIR}
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                   COMMENT "Build verbose doxygen documentation for core"
                   VERBATIM
                 )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Copy local resources
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# copy our own resources
IF( NOT OW_PACKAGE_NOCOPY_COREFONTS )
    SETUP_GLOBAL_RESOURCES( "core" "CORE" )
ELSE()
    # This is a hack which is quite debian specific. We do this here to avoid font duplicates since the ttf-liberation fonts are already
    # available on Debian. If you need to modify this for another packaging thingy, please let me know (ebaum@informatik.uni-leipzig.d). We then
    # should find a generic solution.
    IF( OW_PACKAGE_PACKAGER STREQUAL "deb" )
        INSTALL( CODE "MESSAGE( \"INSTALL: Symlinking fonts to \${CMAKE_INSTALL_PREFIX}/share/openwalnut/fonts.\" )"
                COMPONENT "CORE" )
        INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E make_directory \${CMAKE_INSTALL_PREFIX}/share/openwalnut/fonts )"
                COMPONENT "CORE" ) 
        INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E create_symlink /usr/share/fonts/truetype/ttf-liberation/LiberationMono-Bold.ttf \${CMAKE_INSTALL_PREFIX}/share/openwalnut/fonts/Bold.ttf )"
                COMPONENT "CORE" )
        INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E create_symlink /usr/share/fonts/truetype/ttf-liberation/LiberationMono-Italic.ttf \${CMAKE_INSTALL_PREFIX}/share/openwalnut/fonts/Italic.ttf )"
                COMPONENT "CORE" )
        INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E create_symlink /usr/share/fonts/truetype/ttf-liberation/LiberationMono-Regular.ttf \${CMAKE_INSTALL_PREFIX}/share/openwalnut/fonts/Regular.ttf )"
                COMPONENT "CORE" )
    ELSE()
        MESSAGE( STATUS "Warning: You enabled \"OW_PACKAGE_NOCOPY_COREFONTS\" but packager unknown." ) 
    ENDIF()
ENDIF()

# where to put doc files?
# NOTE: the doc path should match the SONAME of lib. Use this command to query a proper name:
# objdump -p libopenwalnut.so | sed -n -e's/^[[:space:]]*SONAME[[:space:]]*//p' | sed -e's/\([0-9]\)\.so\./\1-/; s/\.so\.//'
SET( OW_DOC_DIR_RELATIVE "share/doc/libopenwalnut1" )
SETUP_COMMON_DOC( ${OW_DOC_DIR_RELATIVE} "CORE" )

# Also add an install target for this
INSTALL( DIRECTORY ${DEVDOC_DIR} 
         DESTINATION ${OW_DOC_DIR_RELATIVE}
         COMPONENT "CORE_DOC"
         OPTIONAL
       )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Copy Shaders
# ---------------------------------------------------------------------------------------------------------------------------------------------------

COLLECT_SHADER_FILES( ${CMAKE_CURRENT_SOURCE_DIR} TARGET_GLSL_FILES )
SETUP_SHADERS( "${TARGET_GLSL_FILES}" "${OW_SHARE_DIR_RELATIVE}/shaders" "CORE" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Style Checker
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# setup the stylechecker. Ignore the platform specific stuff.
SETUP_STYLECHECKER( "${LibName}"
                    "${TARGET_CPP_FILES};${TARGET_H_FILES};${TARGET_TEST_FILES};${TARGET_GLSL_FILES}"  # add all these files to the stylechecker
                    "" )                                        # exlude some ugly files
