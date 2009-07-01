# Package dependencies:
FIND_PACKAGE( Qt4 REQUIRED )

# Includes:
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )

# Libraries for linking:
SET( QT_LIBS optimized ${QT_QTCORE_LIBRARY} debug ${QT_QTCORE_LIBRARY_DEBUG}
                       optimized ${QT_QTGUI_LIBRARY} debug ${QT_QTGUI_LIBRARY_DEBUG}
                       optimized ${QT_QTOPENGL_LIBRARY} debug ${QT_QTOPENGL_LIBRARY_DEBUG} )

# add here all classes that inherit from QObject 
SET(GUI_QT4_MOC_HDRS
  WMainWindow.h
)

QT4_WRAP_CPP(GUI_QT4_MOC_SRCS ${GUI_QT4_MOC_HDRS} )

FILE(GLOB GUI_QT4_SRC "*.cpp")

ADD_LIBRARY( guiqt4 SHARED ${GUI_QT4_SRC} ${GUI_QT4_MOC_SRCS} )

TARGET_LINK_LIBRARIES( guiqt4 gui ge ${QT_LIBS} )
