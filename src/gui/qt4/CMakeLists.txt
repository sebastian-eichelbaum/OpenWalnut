# Package dependencies:
FIND_PACKAGE( Qt4 REQUIRED )

# Includes:
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )

# Libraries for linking:
SET( QT_LIBS ${QT_QTCORE_LIBRARY}
             ${QT_QTGUI_LIBRARY}
             ${QT_QTOPENGL_LIBRARY}
             ${QT_QTWEBKIT_LIBRARY} )

# add here all classes that inherit from QObject
SET(GUI_QT4_MOC_HDRS
    WMainWindow.h
    WQtNavGLWidget.h
    WQtGLDockWidget.h
    WQtCombinerToolbar.h
    guiElements/WQtPushButton.h
    guiElements/WQtModuleOneToOneCombinerAction.h
    guiElements/WQtPropertyBoolAction.h
    guiElements/WScaleLabel.h
    controlPanel/WPropertyWidget.h
    controlPanel/WPropertyBoolWidget.h
    controlPanel/WPropertyIntWidget.h
    controlPanel/WPropertyDoubleWidget.h
    controlPanel/WPropertyStringWidget.h
    controlPanel/WPropertyColorWidget.h
    controlPanel/WPropertyFilenameWidget.h
    controlPanel/WPropertyTriggerWidget.h
    controlPanel/WPropertySelectionWidget.h
    controlPanel/WPropertyPositionWidget.h
    controlPanel/WPropertyMatrix4X4Widget.h
    controlPanel/WQtControlPanel.h
    controlPanel/WQtPropertyGroupWidget.h
    controlPanel/WQtTreeWidget.h
    controlPanel/WQtTreeItem.h
    controlPanel/WQtColormapper.h
    WQtGLScreenCapture.h
    networkEditor/WQtNetworkEditor.h
    networkEditor/WQtNetworkScene.h
    commandPrompt/WQtCommandPromptToolbar.h
    commandPrompt/WQtCommandPrompt.h
)
IF( CMAKE_HOST_SYSTEM MATCHES Darwin )
  LIST( APPEND GUI_QT4_MOC_HDRS platformDependent/WQtGLWidgetMac.h )

ELSE()
  IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
    LIST( APPEND GUI_QT4_MOC_HDRS platformDependent/WQtGLWidgetWin.h )
  ELSE( CMAKE_GENERATOR MATCHES "Visual Studio" )
    LIST( APPEND GUI_QT4_MOC_HDRS platformDependent/WQtGLWidgetAll.h )
  ENDIF( CMAKE_GENERATOR MATCHES "Visual Studio" )
ENDIF()

QT4_WRAP_CPP(GUI_QT4_MOC_SRCS ${GUI_QT4_MOC_HDRS} )

FILE(GLOB GUI_QT4_SRC "*.cpp" "guiElements/*.cpp" "controlPanel/*.cpp" "events/*.cpp"
    "networkEditor/*.cpp" "*.h" "guiElements/*.h" "networkEditor/layout/*.cpp"
        "controlPanel/*.h" "events/*.h" "networkEditor/*.h" "../icons/WIcons.h" "commandPrompt/*.h"
        "commandPrompt/*.cpp" "networkEditor/layout/*.h" )
IF( CMAKE_HOST_SYSTEM MATCHES Darwin )
  FILE( GLOB GUIQT4_PLATFORMDEPENDENT_SRC
    "platformDependent/WQtGLWidgetMac.cpp"
    "platformDependent/WQtGLWidgetMac.h" )
ELSE()
  IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
    FILE( GLOB GUIQT4_PLATFORMDEPENDENT_SRC
      "platformDependent/WQtGLWidgetWin.cpp"
      "platformDependent/WQtGLWidgetWin.h" )
  ELSE( CMAKE_GENERATOR MATCHES "Visual Studio" )
    FILE( GLOB GUIQT4_PLATFORMDEPENDENT_SRC
      "platformDependent/WQtGLWidgetAll.cpp"
      "platformDependent/WQtGLWidgetAll.h" )
  ENDIF( CMAKE_GENERATOR MATCHES "Visual Studio" )
ENDIF()

SET( LIB_NAME "${OWguiqt4Name}" )

SET( ${LIB_NAME}_COMBINER_SRC ${GUI_QT4_SRC} ${GUIQT4_PLATFORMDEPENDENT_SRC} ${GUI_QT4_MOC_SRCS} )
SET( ${LIB_NAME}_ALL_CPP ${GUI_QT4_SRC} ${GUIQT4_PLATFORMDEPENDENT_SRC} ${GUI_QT4_MOC_SRCS} )
SET( ${LIB_NAME}_ALL_CPP_WITHOUT_MOC ${GUI_QT4_SRC} ${GUIQT4_PLATFORMDEPENDENT_SRC} )
FILE( GLOB_RECURSE ${LIB_NAME}_REMOVE_H "*.h"  "../icons/WIcons.h" )

LIST( REMOVE_ITEM ${LIB_NAME}_ALL_CPP ${${LIB_NAME}_REMOVE_H} )
LIST( REMOVE_ITEM ${LIB_NAME}_ALL_CPP_WITHOUT_MOC ${${LIB_NAME}_REMOVE_H} )

IF( OW_BUILD_AS_ONE_BIG_FILE )
	SET( UNITY_BUILD_FILE_NAME "${LIB_NAME}UnityBuildFile.cpp" )
	GENERATE_UNITY_BUILD_FILE( ${LIB_NAME}_ALL_CPP )

	ADD_LIBRARY( ${LIB_NAME} ${UNITY_BUILD_FILE_NAME} ${${LIB_NAME}_COMBINER_SRC} )
ENDIF( OW_BUILD_AS_ONE_BIG_FILE )

IF( NOT OW_BUILD_AS_ONE_BIG_FILE )
    ADD_LIBRARY( ${LIB_NAME} ${${LIB_NAME}_COMBINER_SRC} )
ENDIF( NOT OW_BUILD_AS_ONE_BIG_FILE )

#IF( OW_BUILD_AS_ONE_BIG_FILE )
#	SET( ALL_GUI_QT4_COMBINER_SRC ${GUI_QT4_SRC} ${GUIQT4_PLATFORMDEPENDENT_SRC} ${GUI_QT4_MOC_SRCS} )
#	SET( UNITY_BUILD_FILE_NAME "GuiQt4UnityBuildFile.cpp" )
#
	#only add cpp files to the big file
#	SET( ALL_GUI_QT4_COMBINER_CPP ${ALL_GUI_QT4_COMBINER_SRC} )
 #   LIST( REMOVE_ITEM ALL_GUI_QT4_COMBINER_CPP "*.h" )

#	GENERATE_UNITY_BUILD_FILE( ALL_GUI_QT4_COMBINER_CPP )
#	ADD_LIBRARY( OWguiqt4 SHARED ${UNITY_BUILD_FILE_NAME} ${ALL_GUI_QT4_COMBINER_SRC} )
#ELSE( OW_BUILD_AS_ONE_BIG_FILE )
#	ADD_LIBRARY( OWguiqt4 ${GUI_QT4_SRC} ${GUIQT4_PLATFORMDEPENDENT_SRC} ${GUI_QT4_MOC_SRCS} )
#ENDIF( OW_BUILD_AS_ONE_BIG_FILE )

TARGET_LINK_LIBRARIES( ${LIB_NAME} ${OWCommonName} ${OWguiName} ${OWgeName} ${OWKernelName} ${QT_LIBS} ${Boost_LIBRARIES} )

IF(MSVC_IDE)
    SET_TARGET_PROPERTIES( ${LIB_NAME} PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)
