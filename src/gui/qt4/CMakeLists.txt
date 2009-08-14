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
)

QT4_WRAP_CPP(GUI_QT4_MOC_SRCS ${GUI_QT4_MOC_HDRS} )

FILE(GLOB GUI_QT4_SRC "*.cpp")

ADD_LIBRARY( guiqt4 SHARED ${GUI_QT4_SRC} ${GUI_QT4_MOC_SRCS} )

TARGET_LINK_LIBRARIES( guiqt4 common gui ge kernel ${QT_LIBS} )
