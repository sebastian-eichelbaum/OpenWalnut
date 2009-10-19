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
    WQtNavGLWidget.h
    datasetbrowser/WQtCheckBox.h
    datasetbrowser/WQtDatasetBrowser.h
    datasetbrowser/WQtDSBWidget.h
    datasetbrowser/WQtLineEdit.h
    datasetbrowser/WQtNumberEdit.h
    datasetbrowser/WQtTreeWidget.h
)

QT4_WRAP_CPP(GUI_QT4_MOC_SRCS ${GUI_QT4_MOC_HDRS} )

FILE(GLOB GUI_QT4_SRC "*.[c,h]pp" "datasetbrowser/*cpp" )

ADD_LIBRARY( guiqt4 SHARED ${GUI_QT4_SRC} ${GUI_QT4_MOC_SRCS} )

TARGET_LINK_LIBRARIES( guiqt4 common gui ge kernel ${QT_LIBS} ${Boost_LIBRARIES} )
