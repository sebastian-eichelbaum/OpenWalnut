# Needed because libeep has includes of form #include <cnt/cnt.h>
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ext/libeep )
ADD_SUBDIRECTORY( libeep )
ADD_SUBDIRECTORY( nifti )