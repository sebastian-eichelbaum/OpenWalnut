# Needed because libeep has includes of form #include <cnt/cnt.h>
# See http://libeep.sf.net
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext/libeep )
ADD_SUBDIRECTORY( libeep )

