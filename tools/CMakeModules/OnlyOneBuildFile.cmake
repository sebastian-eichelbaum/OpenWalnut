#
# Usage: GENERATE_UNITY_BUILD_FILE( projectFilesVariableName )
#
# Takes the list of files in the project and makes an ${UNITY_BUILD_FILE_NAME} files that
# #includes all of the source files. Adds ${UNITY_BUILD_FILE_NAME} to the list of source files.
#
# This is a compilation speedup technique suggested on various web sites such as:
# http://buffered.io/2007/12/10/the-magic-of-unity-builds/
# http://stackoverflow.com/questions/543697/include-all-cpp-files-into-a-single-compilation-unit
#

#You can use it like this:
#SET( PROJECT_SOURCES file1.cpp file1.h file2.cpp, file2.h … )
#SET( UNITY_BUILD_FILE_NAME "_UnityBuildFile.cpp" )
#GENERATE_UNITY_BUILD_FILE( PROJECT_SOURCES )

macro( GENERATE_UNITY_BUILD_FILE projectFilesVariableName )

	set( PROJECT_SINGLE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${UNITY_BUILD_FILE_NAME} )

	# Generate list of project source files and set them to be excluded from the build
#	FILE_LIST_BODIES( PROJECT_SOURCES ${${projectFilesVariableName}} )
	FILE( GLOB PROJECT_SOURCES ${${projectFilesVariableName}} )

	foreach( source_file ${PROJECT_SOURCES} )
		# Don’t add qrc_* generated files to ${UNITY_BUILD_FILE_NAME} – if more than one they clash with each other!
		if( NOT source_file MATCHES ".*qrc_.*" )
			# Mark source files as "header" so it will be excluded from the build
			SET_PROPERTY( SOURCE ${source_file} PROPERTY HEADER_FILE_ONLY true )
		endif( NOT source_file MATCHES ".*qrc_.*" )
	endforeach( source_file ${ARG_FILES} )

	# Add this derived file to the project
#	LIST( APPEND ${projectFilesVariableName} ${PROJECT_SINGLE_FILE} )

	# Check to see if .parameters file matches ${PROJECT_SOURCES} – if so don’t need to regenerate ${UNITY_BUILD_FILE_NAME}
#	SET( PROJECT_SINGLE_FILE_PARAMETERS ${PROJECT_SINGLE_FILE}.parameters )
#	if( EXISTS ${PROJECT_SINGLE_FILE_PARAMETERS} )
#		FILE( READ ${PROJECT_SINGLE_FILE_PARAMETERS} PROJECT_PARAMETERS )
#	endif( EXISTS ${PROJECT_SINGLE_FILE_PARAMETERS} )

#	if ( PROJECT_PARAMETERS STREQUAL PROJECT_SOURCES )
	# Do nothing since current parameters match parameters used to generate ${UNITY_BUILD_FILE_NAME}
#	else ( PROJECT_PARAMETERS STREQUAL PROJECT_SOURCES )
	# Store the list of PROJECT_SOURCES in a .parameters file to see if need to regenerate ${UNITY_BUILD_FILE_NAME} or not
#		FILE( WRITE ${PROJECT_SINGLE_FILE_PARAMETERS} "${PROJECT_SOURCES}" )
		# Now generate the single project source file

#		foreach( source_file ${PROJECT_SOURCES} )
			# Don’t add qrc_* generated files to ${UNITY_BUILD_FILE_NAME} – if more than one they clash with each other!
#			if( NOT source_file MATCHES ".*qrc_.*" )
				# Write the inclujde line to the newly-generated project single source file
#				FILE( APPEND ${PROJECT_SINGLE_FILE} "#include \"" ${source_file} "\"\n" )
#			endif( NOT source_file MATCHES ".*qrc_.*" )
#		endforeach( source_file ${ARG_FILES} )
#	endif ( PROJECT_PARAMETERS STREQUAL PROJECT_SOURCES )
    
    SET( PROJECT_TMP_FILE ${PROJECT_SINGLE_FILE}.tmp )
    
    FILE( WRITE ${PROJECT_TMP_FILE} "" )
    foreach( source_file ${PROJECT_SOURCES} )
        # Write the inclujde line to the newly-generated project single source file
        if( NOT source_file MATCHES ".*qrc_.*" )
            # Write the inclujde line to the newly-generated project single source file
            FILE( APPEND ${PROJECT_TMP_FILE} "#include \"" ${source_file} "\"\n" )
        endif( NOT source_file MATCHES ".*qrc_.*" )
    endforeach( source_file ${ARG_FILES} )

    # check if the file changed or exists at all, dont copy if its the same
    EXECUTE_PROCESS(
        COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/../tools/copyIfNes.py ${UNITY_BUILD_FILE_NAME}.tmp ${UNITY_BUILD_FILE_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
endmacro( GENERATE_UNITY_BUILD_FILE projectFilesVariableName )

macro( GENERATE_PCH_LIST_FILE projectFilesVariableName )
	set( PROJECT_SINGLE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Filelist.txt )

	FILE( GLOB PROJECT_SOURCES ${${projectFilesVariableName}} )

#    FILE( WRITE ${PROJECT_SINGLE_FILE} "${PROJECT_SOURCES}" )
    # Now generate the single file list

    FILE( WRITE ${PROJECT_SINGLE_FILE} "" )
    foreach( source_file ${PROJECT_SOURCES} )
        # Write the inclujde line to the newly-generated project single source file
        if( NOT source_file MATCHES ".*moc_.*" )
            FILE( APPEND ${PROJECT_SINGLE_FILE} "" ${source_file} "\n" )
        endif( NOT source_file MATCHES ".*moc_.*" )
    endforeach( source_file ${ARG_FILES} )

endmacro( GENERATE_PCH_LIST_FILE projectFilesVariableName )

macro( CLEAN_PCH_IF_NECESSARY LIB_NAME LIB_SOURCE_FILES )
    IF( ( ( NOT OW_BUILD_WITH_PCH ) OR OW_BUILD_AS_ONE_BIG_FILE ) AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_NAME}_PCH.cpp )
        GENERATE_PCH_LIST_FILE( ${LIB_SOURCE_FILES} )
        EXECUTE_PROCESS(
            COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/../tools/createPCH.py ${LIB_NAME} Filelist.txt 0
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        FILE( REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/Filelist.txt )
        #message("${${LIB_NAME}_ALL_CPP}")
        #message("${LIB_NAME}_ALL_CPP")
        #message("${${${LIB_NAME}_REMOVE_PCH}}")
        #message("${${LIB_NAME}_REMOVE_PCH}")
        #message("${LIB_NAME}_REMOVE_PCH")
        LIST( REMOVE_ITEM ${LIB_NAME}_ALL_CPP ${${LIB_NAME}_REMOVE_PCH} )
        #message("${LIB_NAME}_ALL_CPP")
        #message("${${LIB_NAME}_ALL_CPP}")
        LIST( REMOVE_ITEM ${LIB_NAME}_COMBINER_SRC ${${LIB_NAME}_REMOVE_PCH} )
    ENDIF( ( ( NOT OW_BUILD_WITH_PCH ) OR OW_BUILD_AS_ONE_BIG_FILE) AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_NAME}_PCH.cpp )
endmacro( CLEAN_PCH_IF_NECESSARY LIB_NAME LIB_SOURCE_FILES )

macro( CREATE_PCH LIB_NAME LIB_SOURCE_FILES LIB_SOURCEFILES_WITHOUT_HEADER SOURCES_WITHOUT_MOC )
    GENERATE_PCH_LIST_FILE( ${LIB_SOURCE_FILES} )
    EXECUTE_PROCESS(
        COMMAND ${CMAKE_COMMAND} -E environment
        COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/../tools/createPCH.py ${LIB_NAME} Filelist.txt 1
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    ADD_LIBRARY( ${LIB_NAME} SHARED ${LIB_NAME}_PCH.cpp  ${${LIB_SOURCE_FILES}} )
    IF(MSVC)
        #make sure to not do that to moc files
        SET_SOURCE_FILES_PROPERTIES( ${${SOURCES_WITHOUT_MOC}} PROPERTIES COMPILE_FLAGS "/Yu${LIB_NAME}_PCH.h /Fp${LIB_NAME}_PCH.pch" )
        SET_SOURCE_FILES_PROPERTIES( ${LIB_NAME}_PCH.cpp PROPERTIES COMPILE_FLAGS "/Yc${LIB_NAME}_PCH.h /Fp${LIB_NAME}_PCH.pch" )
    ENDIF(MSVC)
#    IF(CMAKE_COMPILER_IS_GNUCXX)
#        ADD_PRECOMPILED_HEADER( ${LIB_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_NAME}_PCH.h )  
#    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
     FILE( REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/Filelist.txt )
endmacro( CREATE_PCH LIB_NAME LIB_SOURCE_FILES LIB_SOURCEFILES_WITHOUT_HEADER SOURCES_WITHOUT_MOC )

macro( CREATE_PCH_NOT_SHARED LIB_NAME LIB_SOURCE_FILES LIB_SOURCEFILES_WITHOUT_HEADER SOURCES_WITHOUT_MOC )
    GENERATE_PCH_LIST_FILE( ${LIB_SOURCE_FILES} )
    EXECUTE_PROCESS(
        COMMAND ${CMAKE_COMMAND} -E environment
        COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/../tools/createPCH.py ${LIB_NAME} Filelist.txt 1
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    ADD_LIBRARY( ${LIB_NAME} ${LIB_NAME}_PCH.cpp  ${${LIB_SOURCE_FILES}} )
    IF(MSVC)
        #make sure to not do that to moc files
        SET_SOURCE_FILES_PROPERTIES( ${${SOURCES_WITHOUT_MOC}} PROPERTIES COMPILE_FLAGS "/Yu${LIB_NAME}_PCH.h /Fp${LIB_NAME}_PCH.pch" )
        SET_SOURCE_FILES_PROPERTIES( ${LIB_NAME}_PCH.cpp PROPERTIES COMPILE_FLAGS "/Yc${LIB_NAME}_PCH.h /Fp${LIB_NAME}_PCH.pch" )
    ENDIF(MSVC)
#    IF(CMAKE_COMPILER_IS_GNUCXX)
#        ADD_PRECOMPILED_HEADER( ${LIB_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_NAME}_PCH.h )  
#    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
     FILE( REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/Filelist.txt )
endmacro( CREATE_PCH_NOT_SHARED LIB_NAME LIB_SOURCE_FILES LIB_SOURCEFILES_WITHOUT_HEADER SOURCES_WITHOUT_MOC )
