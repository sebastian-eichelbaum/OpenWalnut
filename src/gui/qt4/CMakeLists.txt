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
    datasetbrowser/WPropertyWidget.h
    datasetbrowser/WPropertyBoolWidget.h 
    datasetbrowser/WPropertyIntWidget.h
    datasetbrowser/WPropertyDoubleWidget.h
    datasetbrowser/WPropertyStringWidget.h
    datasetbrowser/WPropertyColorWidget.h
    datasetbrowser/WPropertyFilenameWidget.h
    datasetbrowser/WPropertyTriggerWidget.h
    datasetbrowser/WPropertySelectionWidget.h
    datasetbrowser/WPropertyPositionWidget.h
    datasetbrowser/WQtDatasetBrowser.h
    datasetbrowser/WQtDSBWidget.h
    datasetbrowser/WQtLineEdit.h
    datasetbrowser/WQtSliderWithEdit.h
    datasetbrowser/WQtNumberEdit.h
    datasetbrowser/WQtNumberEditDouble.h
    datasetbrowser/WQtTreeWidget.h
    datasetbrowser/WQtTreeItem.h
    datasetbrowser/WQtTextureSorter.h
)

QT4_WRAP_CPP(GUI_QT4_MOC_SRCS ${GUI_QT4_MOC_HDRS} )

FILE(GLOB GUI_QT4_SRC "*.cpp" "guiElements/*.cpp" "datasetbrowser/*.cpp" "ribbonMenu/*.cpp" "events/*.cpp" "*.h" "guiElements/*.h" "datasetbrowser/*.h" "ribbonMenu/*.h" "events/*.h" )

ADD_LIBRARY( guiqt4 ${GUI_QT4_SRC} ${GUI_QT4_MOC_SRCS} )
TARGET_LINK_LIBRARIES( guiqt4 common gui ge kernel ${QT_LIBS} ${Boost_LIBRARIES} )
