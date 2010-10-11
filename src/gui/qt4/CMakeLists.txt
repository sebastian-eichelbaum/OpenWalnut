# Package dependencies:
FIND_PACKAGE( Qt4 REQUIRED )

# Includes:
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )

# Libraries for linking:
SET( QT_LIBS ${QT_QTCORE_LIBRARY}
             ${QT_QTGUI_LIBRARY}
             ${QT_QTOPENGL_LIBRARY} )

# add here all classes that inherit from QObject 
SET(GUI_QT4_MOC_HDRS
    WMainWindow.h
    WQtConfigWidget.h
    WQtNavGLWidget.h
    WQtCombinerToolbar.h
    guiElements/WQtPushButton.h
    guiElements/WQtModuleOneToOneCombinerAction.h
    guiElements/WQtPropertyBoolAction.h
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
    controlPanel/WQtControlPanel.h
    controlPanel/WQtPropertyGroupWidget.h
    controlPanel/WQtTreeWidget.h
    controlPanel/WQtTreeItem.h
    controlPanel/WQtTextureSorter.h
)

QT4_WRAP_CPP(GUI_QT4_MOC_SRCS ${GUI_QT4_MOC_HDRS} )

FILE(GLOB GUI_QT4_SRC "*.cpp" "guiElements/*.cpp" "controlPanel/*.cpp" "ribbonMenu/*.cpp" "events/*.cpp" "*.h" "guiElements/*.h" "controlPanel/*.h" "ribbonMenu/*.h" "events/*.h" )

ADD_LIBRARY( OWguiqt4 ${GUI_QT4_SRC} ${GUI_QT4_MOC_SRCS} )
TARGET_LINK_LIBRARIES( OWguiqt4 OWcommon OWgui OWge OWkernel ${QT_LIBS} ${Boost_LIBRARIES} )

IF(MSVC_IDE)
	SET_TARGET_PROPERTIES( OWguiqt4 PROPERTIES PREFIX "../")    
ENDIF(MSVC_IDE)
