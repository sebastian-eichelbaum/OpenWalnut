# Package dependencies:
FIND_PACKAGE( Qt4 REQUIRED )

# Includes:
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )

# Libraries for linking:
SET( QT_LINK_LIBRARIES ${QT_QTCORE_LIBRARY}
                       ${QT_QTGUI_LIBRARY}
                       ${QT_QTOPENGL_LIBRARY} )

FILE(GLOB GUI_QT4_SRC "*.cpp")

ADD_LIBRARY( guiqt4 SHARED ${GUI_QT4_SRC} )

TARGET_LINK_LIBRARIES( guiqt4 gui ge ${QT_LINK_LIBRARIES} )
