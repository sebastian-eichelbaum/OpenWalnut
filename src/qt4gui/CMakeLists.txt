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

SET( LibName ${OWQt4GuiName} )
SET( BinName ${OWBinaryName} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Setup QT4
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Package dependencies:
FIND_PACKAGE( Qt4 REQUIRED )
MARK_AS_ADVANCED( FORCE QT_QMAKE_EXECUTABLE )

# Includes:
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )

# Libraries for linking:
SET( QT_LIBS ${QT_QTCORE_LIBRARY}
             ${QT_QTGUI_LIBRARY}
             ${QT_QTOPENGL_LIBRARY}
             ${QT_QTWEBKIT_LIBRARY} )

# If Qt4 Webkit is not available:
IF( NOT QT_QTWEBKIT_FOUND )
    ADD_DEFINITIONS( "-DQT4GUI_NOWEBKIT" )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# GUI compilation options
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Add sources as target
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Collect the compile-files for this target
COLLECT_COMPILE_FILES( "${CMAKE_CURRENT_SOURCE_DIR}" TARGET_CPP_FILES TARGET_H_FILES TARGET_TEST_FILES )

# Qt4 Requires all classes with a QWidget stuff inside to be put into the MOC mechanism. We utilize the automoc mechanism here.
SET( GUI_QT4_MOC_HDRS ${TARGET_H_FILES} )
QT4_AUTOMOC( ${TARGET_CPP_FILES} )
SET( AUTOMOC_MOC_OPTIONS "${OW_CPP_FLAGS_INJECT}" )

# This is needed since the mocs will be generated there
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

# Setup the target
ADD_EXECUTABLE( ${BinName} ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${OW_VERSION_HEADER} )

# Some linux distributions need to explicitily link against X11. We add this lib here.
IF( CMAKE_HOST_SYSTEM MATCHES "Linux" )
    SET( ADDITIONAL_TARGET_LINK_LIBRARIES "X11" )
ENDIF()
TARGET_LINK_LIBRARIES( ${BinName} ${OW_LIB_OPENWALNUT} ${QT_LIBS} ${Boost_LIBRARIES} ${ADDITIONAL_TARGET_LINK_LIBRARIES} )

# Tell CMake that someone creates this file for us. See doc of SETUP_VERSION_HEADER for details why this is needed.
# NOTE: this needs to be placed AFTER ADD_EXECUTABLE or ADD_LIBRARY
SETUP_USE_VERSION_HEADER( ${BinName} )

# Do not forget the install targets
INSTALL( TARGETS ${BinName}
            RUNTIME
                DESTINATION ${OW_RUNTIME_DIR_RELATIVE}
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                            GROUP_READ GROUP_EXECUTE
                            WORLD_READ WORLD_EXECUTE
         COMPONENT "QT4GUI"
        )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Test Setup
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Setup tests of this target
SETUP_TESTS( "${TARGET_TEST_FILES}" "${BinName}" "" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Copy local resources
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# copy our own resources
SETUP_GLOBAL_RESOURCES( ${LibName} "QT4GUI" )

# where to put doc files?
SET( OW_DOC_DIR_RELATIVE "share/doc/openwalnut-qt4" )
SETUP_COMMON_DOC( ${OW_DOC_DIR_RELATIVE} "QT4GUI" )

# also copy all the user documentation we have for qt4gui
SETUP_ADDITIONAL_DIRECTORY( ${OW_DOC_DIR_RELATIVE}
                            ${PROJECT_SOURCE_DIR}/../doc/${LibName}/help/
                            "QT4GUI"
                            TRUE    # this denotes that the contents of doc/user should be copied instead of user directly
                          )  
SETUP_ADDITIONAL_DIRECTORY( ${OW_MAN_DIR_RELATIVE}
                            ${PROJECT_SOURCE_DIR}/../doc/${LibName}/man/
                            "QT4GUI"
                            TRUE    # this denotes that the contents of doc/man should be copied instead of user directly
                          )

# Our user-help needs the icons too. Copy them.
SETUP_ADDITIONAL_DIRECTORY( "${OW_DOC_DIR_RELATIVE}/icons"
                            "${CMAKE_CURRENT_SOURCE_DIR}/icons"
                            "QT4GUI"
                            FALSE   # this denoted that the whole directory icons should be copied instead of its contents
                          )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Style Checker
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# setup the stylechecker. Ignore the platform specific stuff.
SETUP_STYLECHECKER( "${BinName}"
                    "${TARGET_CPP_FILES};${TARGET_H_FILES};${TARGET_TEST_FILES};${TARGET_GLSL_FILES}"  # add all these files to the stylechecker
                    "" )                                                                               # exlude some ugly files
