ADD_SUBDIRECTORY( qt4 )

# check if version.h exists, which does not when checking out for the first time
IF( NOT EXISTS ${PROJECT_SOURCE_DIR}/version.h )
    IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
        EXECUTE_PROCESS( COMMAND hg parents --template "\#define W_VERSION \"{node|short} {rev} {branches} {tags}\" //"
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
            OUTPUT_FILE ${PROJECT_SOURCE_DIR}/version.h )
    ELSE()
        EXECUTE_PROCESS( COMMAND hg parents --template "\"#define W_VERSION \"\"{node|short} {rev} {branches} {tags}\"\"\""
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
            OUTPUT_FILE ${PROJECT_SOURCE_DIR}/version.h )
    ENDIF()
ENDIF()

# finally, build walnut
ADD_EXECUTABLE( walnut OpenWalnut.cpp ../version.h )
TARGET_LINK_LIBRARIES( walnut ${OWKernelName} ${OWDatahandlerName} ${OWguiName} ${OWguiqt4Name} ${OWCommonName} ${OWgeName} ${Boost_LIBRARIES} )

IF(MSVC_IDE)
    ADD_DEFINITIONS( -DEIGEN_DONT_VECTORIZE -DEIGEN_DONT_ALIGN -DEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT )
    SET_TARGET_PROPERTIES( walnut PROPERTIES DEBUG_POSTFIX "d")
    SET_TARGET_PROPERTIES( walnut PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)

