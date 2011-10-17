CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( OpenWalnut )

# guard against in-source builds
IF( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  MESSAGE( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. (you may need to remove CMakeCache.txt" )
ENDIF()

ADD_DEFINITIONS( -DBOOST_FILESYSTEM_VERSION=2 )

IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
    ADD_DEFINITIONS( -DEIGEN_DONT_VECTORIZE -DEIGEN_DONT_ALIGN -DEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT )
ENDIF()

# append search path for FindModules:
#------------------------------------
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../tools/CMakeModules )
# include custom CMake commands like assert
INCLUDE( ${PROJECT_SOURCE_DIR}/../tools/CMakeModules/Utils.cmake )
INCLUDE( ${PROJECT_SOURCE_DIR}/../tools/CMakeModules/OnlyOneBuildFile.cmake )

# mimic layout of install dir for build:
#----------------------------------------
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
SET( SHARED_FILES_RELATIVE "../share/${CMAKE_PROJECT_NAME}" CACHE INTERNAL "Relative path to bin/ denoting the destination for shaders and cfg files." )  # this path us the relative path to bin/ used for all shared files like shaders and config file

# CMAKE automatism to select static vs. shared building:
#-------------------------------------------------------
IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
  OPTION( BUILD_SHARED_LIBS "Build shared libs" ON )
  MARK_AS_ADVANCED( BUILD_SHARED_LIBS )
ENDIF()

# platform dependend stuff:
#--------------------------
IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
  INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/common/platformDependent/visualStudio/" )
ENDIF()

OPTION( OW_BUILD_AS_ONE_BIG_FILE "Build every library as one big cpp" OFF )
INCLUDE( FindPythonInterp )

# define libnames
SET( OWCommonName "OWcommon" )
SET( OWDatahandlerName "OWdataHandler" )
SET( OWgeName "OWge" )
SET( OWguiName "OWgui" )
SET( OWguiqt4Name "OWguiqt4" )
SET( OWKernelName "OWkernel" )
SET( OWmodulesPrefix "OWmodule" )

# Package dependencies:
#----------------------
### BOOST ###
IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
  SET( Boost_USE_STATIC_LIBS ON )
ENDIF()

SET( Boost_USE_MULTITHREAD ON )

# To see which boost libs we currently use, you may run the following command
# in the trunk/src directory on a linux box to make some investigations:
# grep -i include `find . -type f` | grep boost | awk '{print $2}' | sort | uniq
FIND_PACKAGE( Boost REQUIRED program_options thread filesystem date_time system signals regex )
ASSERT_GE_VERSION( "Boost" "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}" 1.39.0 )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

### OpenGL ###
FIND_PACKAGE( OpenGL REQUIRED )

### CxxTest ###
FIND_PACKAGE( CxxTest QUIET )
MARK_AS_ADVANCED( CXXTEST_PERL_TESTGEN_EXECUTABLE )
OPTION( OW_USE_TESTS "This enables compilation of tests" ON )
IF( CXXTEST_FOUND )
  INCLUDE_DIRECTORIES( ${CXXTEST_INCLUDE_DIR} )
  IF( OW_USE_TESTS )
    SET( OW_COMPILE_TESTS ON ) #We need this variable because this is tested more often.
    # Package settings:
    SET( CXXTEST_USE_PYTHON 1 )
    # Activate CTest and "test" target
    ENABLE_TESTING()
  ELSE()
    SET( OW_COMPILE_TESTS OFF )
  ENDIF()
ELSE()
  SET( OW_COMPILE_TESTS OFF )
ENDIF()

### OpenSceneGraph ###
SET( MIN_OSG_VERSION 2.8.0 )
IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
  FIND_PACKAGE( osg ${MIN_OSG_VERSION} REQUIRED )
  FIND_PACKAGE( osgUtil ${MIN_OSG_VERSION} REQUIRED )
  FIND_PACKAGE( osgDB ${MIN_OSG_VERSION} REQUIRED )
  FIND_PACKAGE( osgViewer ${MIN_OSG_VERSION} REQUIRED )
  FIND_PACKAGE( osgText ${MIN_OSG_VERSION} REQUIRED )
  FIND_PACKAGE( osgGA ${MIN_OSG_VERSION} REQUIRED )
  FIND_PACKAGE( osgSim ${MIN_OSG_VERSION} REQUIRED )
  FIND_PACKAGE( osgWidget ${MIN_OSG_VERSION} REQUIRED )
  FIND_PACKAGE( OpenThreads ${MIN_OSG_VERSION} REQUIRED )
  SET( OPENSCENEGRAPH_LIBRARIES ${OSGDB_LIBRARY} ${OSGTEXT_LIBRARY} ${OSGGA_LIBRARY} ${OSGUTIL_LIBRARY} ${OSGVIEWER_LIBRARY} ${OSG_LIBRARY} ${OSGSIM_LIBRARY} ${OSGWIDGET_LIBRARY} ${OPENTHREADS_LIBRARY} )
  SET( OPENSCENEGRAPH_INCLUDE_DIRS ${OSG_INCLUDE_DIR} ${OSGDB_INCLUDE_DIR} ${OSGTEXT_INCLUDE_DIR} ${OSGGA_INCLUDE_DIR} ${OSGVIEWER_INCLUDE_DIR} ${OSGUTIL_INCLUDE_DIR} ${OSGSIM_INCLUDE_DIR} ${OSGWIDGET_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} )
  # When new cmake version available >=2.6.4 we may use this the line below instead the stuff above
  # FIND_PACKAGE( OpenSceneGraph 2.8.0 REQUIRED osgDB osgUtil osgGA osgViewer osgSim OpenThreads )
  INCLUDE_DIRECTORIES( ${OPENSCENEGRAPH_INCLUDE_DIRS} )
ELSE()
  IF( NOT VERSION LESS 2.6.3 )
    SET ( CMAKE_PREFIX_PATH "$ENV{ProgramFiles}/OpenSceneGraph" )
  ENDIF()
  FIND_PACKAGE( OpenSceneGraph ${MIN_OSG_VERSION} REQUIRED osgUtil osgDB osgViewer osgText osgGA osgSim osgWidget )
  INCLUDE_DIRECTORIES( ${OPENSCENEGRAPH_INCLUDE_DIRS} )
ENDIF()

# some modules need the teem library (http://teem.sourceforge.net/)
find_package( Teem PATHS /usr/local/lib /opt/teem/lib QUIET )
IF( Teem_FOUND )    # we need to add the include path and lib path to allow the compiler and linker to find teem even if it was not installed to
                    # /usr/local
    INCLUDE_DIRECTORIES( ${Teem_INCLUDE_DIRS} )
    LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${Teem_LIBRARY_DIRS} )
ENDIF()

### OSSIM ###
FIND_PACKAGE( OSSIM QUIET )
IF( OSSIM_FOUND )
  MESSAGE( STATUS "Found OSSIM." )
  OPTION( OW_USE_OSSIM "Use the ossim library for singular value decomposition." OFF )
  INCLUDE_DIRECTORIES( ${OSSIM_INCLUDE_DIR} )
  LINK_LIBRARIES( ${OSSIM_LIBRARY} ) # this is necessary since you may install OSSIM not in the default library path
  MARK_AS_ADVANCED( OSSIM_INCLUDEDIR )
  MARK_AS_ADVANCED( OSSIM_LIBRARIES )
  IF( OW_USE_OSSIM )
    ADD_DEFINITIONS( -DOW_USE_OSSIM )
  ENDIF()
ENDIF()

# insight toolkit (for image segmentation and registration)
#----------------------------------------------------------
FIND_PACKAGE( ITK QUIET )
IF( ITK_FOUND )
    OPTION( OW_USE_ITK "Use the insight toolkit for image segmentation" OFF )
    INCLUDE( ${ITK_USE_FILE} )
    IF( OW_USE_ITK )
        ADD_DEFINITIONS( -DOW_USE_ITK )
    ENDIF()
ENDIF()

### OPENCL ###
OPTION( OW_USE_OPENCL "Enable OpenCL if available." )
IF( OW_USE_OPENCL )
    FIND_PACKAGE( OpenCL QUIET )
    IF( OPENCL_FOUND )
        INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIR} )
        INCLUDE_DIRECTORIES( ${OPENCL_CPP_INCLUDE_DIR} )
        LINK_LIBRARIES( ${OPENCL_LIBRARY} )
        MARK_AS_ADVANCED( OPENCL_INCLUDE_DIR )
        MARK_AS_ADVANCED( OPENCL_LIBRARY )
    ENDIF()
ENDIF( OW_USE_OPENCL )

### CUDA #### (e.g. for the detTractClustering module )
OPTION( OW_USE_CUDA "Enable CUDA if available. Minimum version required is 2.3" )
IF( OW_USE_CUDA )
    FIND_PACKAGE( CUDA QUIET )
    IF( CUDA_FOUND )
        # BugFix: Subsequent calls don't set CUDA_VERSION_MAJOR or .._MINOR variables, so we have to extract them from CUDA_VERSION variable
        STRING(REGEX REPLACE "([0-9]+)\\.([0-9]+).*" "\\1" CUDA_VERSION_MAJOR ${CUDA_VERSION})
        STRING(REGEX REPLACE "([0-9]+)\\.([0-9]+).*" "\\2" CUDA_VERSION_MINOR ${CUDA_VERSION})

        # Enforce at least CUDA version 2.3, since the detTractClustering is know to require this at least
        SET( OW_MIN_CUDA_MAJOR_VERSION 2 )
        SET( OW_MIN_CUDA_MINOR_VERSION 3 )

        IF( CUDA_VERSION_MAJOR LESS OW_MIN_CUDA_MAJOR_VERSION )
            SET( CUDA_FOUND_BUT_LOW_VERSION 1 )
        ELSEIF( CUDA_VERSION_MAJOR EQUAL OW_MIN_CUDA_MAJOR_VERSION )
            IF( CUDA_VERSION_MINOR LESS OW_MIN_CUDA_MINOR_VERSION )
                SET( CUDA_FOUND_BUT_LOW_VERSION 1 )
            ENDIF()
        ENDIF()

        # abort incase invalid version of CUDA was found.
        IF( CUDA_FOUND_BUT_LOW_VERSION )
            MESSAGE( FATAL_ERROR "You have selected to use CUDA but an insufficent version: ${CUDA_VERSION_MAJOR}.${CUDA_VERSION_MINOR} was found, but ${OW_MIN_CUDA_MAJOR_VERSION}.${OW_MIN_CUDA_MINOR_VERSION} is required." )
        ENDIF()
        MESSAGE( STATUS "CUDA version: ${CUDA_VERSION_MAJOR}.${CUDA_VERSION_MINOR} was found." )
    ENDIF( CUDA_FOUND )
ENDIF( OW_USE_CUDA )


# other options:
#---------------
# Set default build type
IF( NOT CMAKE_BUILD_TYPE )
    SET( CMAKE_BUILD_TYPE Release
         CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo"
         FORCE
       )
ENDIF( NOT CMAKE_BUILD_TYPE )

# guard against typos in build-type strings
STRING( TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_tolower)
IF( NOT cmake_build_type_tolower STREQUAL "debug" AND
    NOT cmake_build_type_tolower STREQUAL "release" AND
    NOT cmake_build_type_tolower STREQUAL "relwithdebinfo" AND
    NOT cmake_build_type_tolower STREQUAL "")
  MESSAGE( SEND_ERROR "Unknown build type \"${CMAKE_BUILD_TYPE}\". Allowed values are Debug, Release, RelWithDebInfo  and \"\" (case-insensitive).")
ENDIF()

SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "The executables will be put into this directory." )
MARK_AS_ADVANCED( LIBRARY_OUTPUT_PATH )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH )

# Global compiler flags:
#-----------------------
# -Wno-long-long    since on Ubuntu 8.10 it won't compile without it
# -ansi             force ISO-C++98 compliance (not GNU++98)

IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
    FILE( GLOB EXCLUDE_LIBRARIES RELATIVE ${Boost_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS}/libboost* )
    SET( CMAKE_SHARED_LINKER_FLAGS " /STACK:10000000 /machine:I386 /NODEFAULTLIB:${EXCLUDE_LIBRARIES}"  CACHE STRING "" FORCE )
    SET( CMAKE_EXE_LINKER_FLAGS " /STACK:10000000 /machine:I386 /NODEFAULTLIB:${EXCLUDE_LIBRARIES}"  CACHE STRING "" FORCE )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D \"NOMINMAX\"" )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /wd4101" )
    SET( CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows" )
ELSE()
    # Unfortunately libstdc++'s header files don't work with mingw in ansi mode (basically libstdc++'s fault)
    IF( CMAKE_HOST_SYSTEM MATCHES "Windows" )
        SET( CMAKE_CXX_FLAGS "-frtti -pedantic -Wall -Wno-long-long -Wextra -Wold-style-cast" CACHE STRING "" FORCE )
    ELSE()
        # SET( CMAKE_CXX_FLAGS "-frtti -pedantic -ansi -Wall -Wno-long-long -Wextra -Wold-style-cast" CACHE STRING "" FORCE )
        SET( CMAKE_CXX_FLAGS "-frtti -pedantic -ansi -Wall -Wno-long-long -Wextra " CACHE STRING "" FORCE )
    ENDIF()

    # Darwin's ld isn't GNU and doesn't like the following
    IF( NOT CMAKE_SYSTEM_NAME MATCHES "Darwin" )
        ## The following allows us to prevent cyclic dependencies even on linux
        SET( CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined -Wl,--allow-shlib-undefined" CACHE STRING "" FORCE )
    ENDIF()
    SET( CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "" FORCE )
    SET( CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -O0" CACHE STRING "" FORCE )
ENDIF()

SET( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -DDEBUG -O2" CACHE STRING "" FORCE )

ADD_SUBDIRECTORY( dataHandler )
ADD_SUBDIRECTORY( graphicsEngine )
ADD_SUBDIRECTORY( gui )
ADD_SUBDIRECTORY( kernel )
ADD_SUBDIRECTORY( common )
ADD_SUBDIRECTORY( modules )
ADD_SUBDIRECTORY( ext ) # The needed external libraries

#-------------------------------------------------------------------------------------------------------------
# Writes information about the revision of the current code to the file version.h which then gets included by OpenWalnut.cpp
IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
    add_custom_command( OUTPUT ${PROJECT_SOURCE_DIR}/version.h
                        MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/../.hg/dirstate
                        COMMAND hg parents --template '\#define W_VERSION \"{node|short} {rev} {branches} {tags}\" //' > ${PROJECT_SOURCE_DIR}/version.h
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        COMMENT "Creating version.h"
                     )
ELSE()
    add_custom_command( OUTPUT ${PROJECT_SOURCE_DIR}/version.h
                        MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/../.hg/dirstate
                        COMMAND hg parents --template "\"#define W_VERSION \"\"{node|short} {rev} {branches} {tags}\"\"\"" > ${PROJECT_SOURCE_DIR}/version.h
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        COMMENT "Creating version.h"
                     )
ENDIF()

# finally, build walnut
ADD_EXECUTABLE( walnut OpenWalnut.cpp version.h )
TARGET_LINK_LIBRARIES( walnut ${OWKernelName} ${OWDatahandlerName} ${OWguiName} ${OWguiqt4Name} ${OWCommonName} ${OWgeName} ${Boost_LIBRARIES} )

IF(MSVC_IDE)
    ADD_DEFINITIONS( -DEIGEN_DONT_VECTORIZE -DEIGEN_DONT_ALIGN -DEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT )
    SET_TARGET_PROPERTIES( walnut PROPERTIES DEBUG_POSTFIX "d")
    SET_TARGET_PROPERTIES( walnut PROPERTIES PREFIX "../")
ENDIF(MSVC_IDE)

#---------- Shaders -------------
option( OW_COPY_SHADERS "This enables copying shaders with every make call." ON )
if( OW_COPY_SHADERS )
  add_custom_target( shaderDirectoryConfiguration
    ALL
    COMMAND ${CMAKE_COMMAND} -D OW_SOURCE_DIR:STRING=${PROJECT_SOURCE_DIR} -D SHARED_FILES_RELATIVE:PATH=${SHARED_FILES_RELATIVE} -P ${PROJECT_SOURCE_DIR}/../tools/CMakeModules/UpdateShaders.cmake
    COMMENT "Put all global shaders to the right place or update them if they are already there."
    )
endif( OW_COPY_SHADERS )

#---------- fonts -------------
add_custom_target( fontsDirectoryConfiguration
  ALL
  COMMAND ${CMAKE_COMMAND} -D OW_SOURCE_DIR:STRING=${PROJECT_SOURCE_DIR} -D SHARED_FILES_RELATIVE:PATH=${SHARED_FILES_RELATIVE} -P ${PROJECT_SOURCE_DIR}/../tools/CMakeModules/CopyFonts.cmake
  COMMENT "Put all fonts to the right place."
  )
  
  
#---------- OpenWalnut help -------------
ADD_CUSTOM_TARGET( onlineHelp
    ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../doc/user/help/OpenWalnutHelp.html ${PROJECT_BINARY_DIR}/share/OpenWalnut
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../doc/user/help/images/ ${PROJECT_BINARY_DIR}/share/OpenWalnut/images
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/gui/icons/ ${PROJECT_BINARY_DIR}/share/OpenWalnut/icons
    COMMENT "Copy online help"
)

#---------- About OpenWalnut -------------
ADD_CUSTOM_TARGET( aboutOpenWalnut
    ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/gui/qt4/OpenWalnutAbout.html ${PROJECT_BINARY_DIR}/share/OpenWalnut
    COMMENT "Copy about page"
)

#---------- shell scripts -------------
IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
  add_custom_target( binDirectoryConfiguration
    ALL
    COMMAND ${CMAKE_COMMAND} -D OW_SOURCE_DIR:STRING=${PROJECT_SOURCE_DIR} -P ${PROJECT_SOURCE_DIR}/../tools/CMakeModules/CopyShellScriptsWin.cmake
    COMMENT "Put all shell scripts to the right place."
  )
ELSE()
  add_custom_target( binDirectoryConfiguration
    ALL
    COMMAND ${CMAKE_COMMAND} -D OW_SOURCE_DIR:STRING=${PROJECT_SOURCE_DIR} -P ${PROJECT_SOURCE_DIR}/../tools/CMakeModules/CopyShellScripts.cmake
    COMMENT "Put all shell scripts to the right place."
  )
ENDIF()

#---------- walnut.cfg file -------------
IF( NOT EXISTS ${PROJECT_BINARY_DIR}/bin/${SHARED_FILES_RELATIVE}/walnut.cfg )
  CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/../doc/user/walnut.cfg.official ${PROJECT_BINARY_DIR}/bin/${SHARED_FILES_RELATIVE}/walnut.cfg COPYONLY )
  MESSAGE( STATUS "Copied ${PROJECT_SOURCE_DIR}/../doc/user/walnut.cfg.official to ${PROJECT_BINARY_DIR}/bin/${SHARED_FILES_RELATIVE}/walnut.cfg" )
ENDIF()

# Special targets:
#-------------------------------------------------------------------------------------------------------------
# Creates API via Doxygen
ADD_CUSTOM_TARGET( doc
                   COMMAND doxygen ${PROJECT_SOURCE_DIR}/../doc/developer/doxygenConfig
                   COMMAND chmod -R g+r ${PROJECT_SOURCE_DIR}/../doc/developer/html
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                   COMMENT "Build doxygen documentation"
                   VERBATIM
                 )

# NOTE: this full documentation also includes call/caller graphs, colaboration graphs and nicer inheritance graphs
# but needs "dot" from graphviz to render them.
ADD_CUSTOM_TARGET( fulldoc
                   COMMAND doxygen ${PROJECT_SOURCE_DIR}/../doc/developer/doxygenConfigFull
                   COMMAND chmod -R g+r ${PROJECT_SOURCE_DIR}/../doc/developer/htmlFull
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                   COMMENT "Build verbose doxygen documentation"
                   VERBATIM
                 )

#-------------------------------------------------------------------------------------------------------------
# Uses ctags to create source tags, usable in many editors like VIM or Emacs
ADD_CUSTOM_COMMAND( OUTPUT ctags
                    COMMAND ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/
                    COMMENT "Build ctags tagfile"
                  )

#-------------------------------------------------------------------------------------------------------------
# Uses cscope to create several source graphs (dependency, call, caller, ...), usable in many editors like VIM or Emacs
ADD_CUSTOM_COMMAND( OUTPUT cscope
                    COMMAND cscope -R -b
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/
                    COMMENT "Build cscope tagfile"
                  )

#-------------------------------------------------------------------------------------------------------------
# The command "make tags" creates ctags and cscope tag files
ADD_CUSTOM_TARGET( tags DEPENDS ctags DEPENDS cscope )

#-------------------------------------------------------------------------------------------------------------
# Determines the number of cores available on this machine
IF( NOT CMAKE_GENERATOR MATCHES "Visual Studio" )
    SET( NUM_CORES_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../tools/numCores )
    SET( NUM_CORES_BINARY_DIR ${PROJECT_BINARY_DIR}/numCores )
    SET( NUM_CORES_BINARY ${NUM_CORES_BINARY_DIR}/numCores )
    ADD_CUSTOM_TARGET( numCores
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${NUM_CORES_BINARY_DIR}
                   COMMAND cd ${NUM_CORES_BINARY_DIR} && ${CMAKE_COMMAND} ${NUM_CORES_SOURCE_DIR} > /dev/null
                   COMMAND $(MAKE) -C ${NUM_CORES_BINARY_DIR} > /dev/null
                   COMMENT "Determines the number of cores available on this machine"
                 )
ENDIF()

#-------------------------------------------------------------------------------------------------------------
# Checks style guide lines via the BrainLint tool against all source code files
#    The output is colored when variable CMAKE_COLOR_MAKEFILE is set to ON,
#    noncolored if it is set to OFF
FILE( GLOB_RECURSE STYLE_CPP ${PROJECT_SOURCE_DIR}/*.cpp )
FILE( GLOB_RECURSE STYLE_H ${PROJECT_SOURCE_DIR}/*.h )
FILE( GLOB_RECURSE STYLE_GLSL ${PROJECT_SOURCE_DIR}/*.glsl )
FILE( GLOB_RECURSE EXT_SOURCES ${PROJECT_SOURCE_DIR}/ext/* )

FOREACH( file ${EXT_SOURCES} )
    LIST( REMOVE_ITEM STYLE_H ${file} )
    LIST( REMOVE_ITEM STYLE_CPP ${file} )
ENDFOREACH( file )

# Set dirs to exclude from stylecheck
LIST( APPEND EXCLUDE_DIRS "/dataHandler/io/biosig" )
LIST( APPEND EXCLUDE_DIRS "/dataHandler/io/biosigWin" )
LIST( APPEND EXCLUDE_DIRS "/dataHandler/io/biosig/XMLParser" )
LIST( APPEND EXCLUDE_DIRS "/dataHandler/io/biosigWin/XMLParser" )
LIST( APPEND EXCLUDE_DIRS "/common/platformDependent/visualStudio" )
LIST( APPEND EXCLUDE_DIRS "/modules/lic/fibernavigator" )
LIST( APPEND EXCLUDE_DIRS "/modules/lic/fantom" )
LIST( APPEND EXCLUDE_DIRS "/dataHandler/io/biosigWin" )

FOREACH( dirname ${EXCLUDE_DIRS} )
    FILE( GLOB EXCLUDE_H   ${PROJECT_SOURCE_DIR}${dirname}/*.h )
    FILE( GLOB EXCLUDE_CPP ${PROJECT_SOURCE_DIR}${dirname}/*.cpp )

    FOREACH( fname ${EXCLUDE_H} )
      LIST( REMOVE_ITEM STYLE_H ${fname} )
    ENDFOREACH( fname )

    FOREACH( fname ${EXCLUDE_CPP} )
        LIST( REMOVE_ITEM STYLE_CPP ${fname} )
    ENDFOREACH( fname )
ENDFOREACH( dirname )

LIST( REMOVE_ITEM STYLE_H ${PROJECT_SOURCE_DIR}/version.h )

IF( CMAKE_COLOR_MAKEFILE )
    SET( STYLECHECK_OPTIONS "--color" )
ELSE()
    SET( STYLECHECK_OPTIONS "" )
ENDIF()

IF( CMAKE_HOST_WIN32 )
    SET( XARGS_OPTIONS "-n 128" )
ELSEIF( CMAKE_HOST_UNIX )
    SET( XARGS_OPTIONS -P \$\$\(${NUM_CORES_BINARY}\) -n 64 )
ELSE()
    SET( XARGS_OPTIONS "" )
ENDIF()

FILE( WRITE ${PROJECT_BINARY_DIR}/brainlintlist "" )
FOREACH( fname ${STYLE_CPP} ${STYLE_H} ${STYLE_GLSL} )
    FILE( APPEND ${PROJECT_BINARY_DIR}/brainlintlist "${fname}\n" )
ENDFOREACH()

ADD_CUSTOM_TARGET( stylecheck
                   COMMAND  cat ${PROJECT_BINARY_DIR}/brainlintlist | xargs ${XARGS_OPTIONS} ${PROJECT_SOURCE_DIR}/../tools/brainlint.py ${STYLECHECK_OPTIONS} 2>&1 | grep -iv 'Total errors found: 0$$' | cat
                   DEPENDS numCores
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                   COMMENT "Check if code complies to CodingStandard"
                 )


#-------------------------------------------------------------------------------------------------------------
# Removes nasty windows line endings
ADD_CUSTOM_TARGET( removewinlineendings
                   COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/../tools/removeWinLineEnding.py ${PROJECT_BINARY_DIR}/brainlintlist
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                   COMMENT "Removes Windows Line endings"
                 )

#-------------------------------------------------------------------------------------------------------------
# windows helper script to copy all files needed to build a release to one folder
IF( CMAKE_GENERATOR MATCHES "Visual Studio" )
    ADD_CUSTOM_TARGET( copyWinRelease
                       COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/../tools/copyWinRelease.py
                       WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                       COMMENT "Copys all the necessary Windows dll's and executables so binaries can easyly be distributed"
                     )
ENDIF( CMAKE_GENERATOR MATCHES "Visual Studio" )
                 #-------------------------------------------------------------------------------------------------------------
# Prints the verbose output of the test runners, so you may identify the source of error early, if any
ADD_CUSTOM_TARGET( vtest
                   COMMAND $(MAKE) test ARGS="-V"
                   COMMENT "Runs the test in verboseness to see what actually went wrong"
                 )

#-------------------------------------------------------------------------------------------------------------
# Checks if all is well to commit (aka commitCheck, cicheck)
ADD_CUSTOM_TARGET( cicheck
                   COMMAND $(MAKE) stylecheck && $(MAKE) test && $(MAKE) doc
                   COMMENT "Checks if all is well to commit"
                 )

#-------------------------------------------------------------------------------------------------------------
# Makes many special make targets in combination. Default number of cores is 1.
ADD_CUSTOM_TARGET( many
                   COMMAND echo Using \$\$\(${NUM_CORES_BINARY}\) cores.\; $(MAKE) all -j\$\$\(${NUM_CORES_BINARY}\)
                   COMMAND $(MAKE) test
                   COMMAND $(MAKE) stylecheck
                   COMMAND $(MAKE) doc
                   DEPENDS numCores
                   COMMENT "Make many special make targets in combination."
                 )

#-------------------------------------------------------------------------------------------------------------
# Make target for MacOS. The difference to the normal one is that we need to fix some paths in libraries.
IF( CMAKE_HOST_SYSTEM MATCHES Darwin )
ADD_CUSTOM_TARGET( macos
                   $(MAKE) all && ${PROJECT_SOURCE_DIR}/../tools/MacOS/macOS_install_tool
                   COMMENT "Make on MacOS. This incudes fixing paths in libraries."
                 )
ENDIF()
