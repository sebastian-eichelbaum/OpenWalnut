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

# Includes:
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )

# Libraries for linking:
SET( QT_LIBS ${QT_QTCORE_LIBRARY}
             ${QT_QTGUI_LIBRARY}
             ${QT_QTOPENGL_LIBRARY}
             ${QT_QTWEBKIT_LIBRARY} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# The walnut executable should print the revision/tag
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# We use mercurial to get current version information. This is stored in version.h, included by OpenWalnut.cpp
IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
    add_custom_command( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/version.h
                        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/../../.hg/dirstate
                        COMMAND hg parents --template '\#define W_VERSION \"{node|short} {rev} {branches} {tags}\" //' > ${CMAKE_CURRENT_SOURCE_DIR}/version.h
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        COMMENT "Creating version.h"
                     )
ELSE()
    add_custom_command( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/version.h
                        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/../../.hg/dirstate
                        COMMAND hg parents --template "\"#define W_VERSION \"\"{node|short} {rev} {branches} {tags}\"\"\"" > ${CMAKE_CURRENT_SOURCE_DIR}/version.h
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        COMMENT "Creating version.h"
                     )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Add sources as target
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Collect the compile-files for this target
COLLECT_COMPILE_FILES( "${CMAKE_CURRENT_SOURCE_DIR}" TARGET_CPP_FILES TARGET_H_FILES TARGET_TEST_FILES )

# Qt4 Requires all classes with a QWidget stuff inside to be put into the MOC mechanism. We utilize the automoc mechanism here.
SET( GUI_QT4_MOC_HDRS ${TARGET_H_FILES} )
QT4_AUTOMOC( ${TARGET_CPP_FILES} )
# This is needed since the mocs will be generated there
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

# Setup the target
ADD_EXECUTABLE( ${BinName} ${TARGET_CPP_FILES} ${TARGET_H_FILES} "version.h" )
TARGET_LINK_LIBRARIES( ${BinName} ${OWCoreName} ${QT_LIBS} ${Boost_LIBRARIES} )

# Do not forget the install targets
INSTALL( TARGETS ${BinName}
            RUNTIME
                DESTINATION ${OW_RUNTIME_DIR_RELATIVE}
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                            GROUP_READ GROUP_EXECUTE
                            WORLD_READ WORLD_EXECUTE
        )

# TODO(all): comment this stuff. I do not know what the purpose is
IF( MSVC_IDE )
    SET_TARGET_PROPERTIES( ${BinName} PROPERTIES DEBUG_POSTFIX "d")
    SET_TARGET_PROPERTIES( ${BinName} PROPERTIES PREFIX "../")
ENDIF( MSVC_IDE )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Test Setup
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Setup tests of this target
SETUP_TESTS( "${TARGET_TEST_FILES}" "${BinName}" "" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Copy local resources
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Copy the help documents. This is not in the resource directory since it is qt4gui specific, so we need to copy it.
ADD_CUSTOM_TARGET( help
    ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../doc/user/help/ ${OW_DOC_DIR}/${LibName}/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/icons/ ${OW_DOC_DIR}/${LibName}/icons
    COMMENT "Copy the GUI Documentation"
)
# Also create an install target for this
INSTALL( DIRECTORY ${PROJECT_SOURCE_DIR}/../doc/user/help/ DESTINATION ${OW_DOC_DIR_RELATIVE}/${LibName} )
INSTALL( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/icons DESTINATION ${OW_DOC_DIR_RELATIVE}/${LibName} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Style Checker
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# setup the stylechecker. Ignore the platform specific stuff.
SETUP_STYLECHECKER( "${BinName}"
                    "${TARGET_CPP_FILES};${TARGET_H_FILES};${TARGET_TEST_FILES};${TARGET_GLSL_FILES}"  # add all these files to the stylechecker
                    ".*/version\\.h" )                                                                 # exlude some ugly files
