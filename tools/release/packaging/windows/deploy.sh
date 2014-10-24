#!/bin/bash

###############################################################################
# Install using make
###############################################################################

installDir=$(grep -ir "CMAKE_INSTALL_PREFIX" CMakeCache.txt | sed "s/^.*=//")
if [ -z "$installDir" ]; then
    echo "ERROR: Could not find install directory. Did you run CMake to configure the build beforehand? Exit."
    exit -1
fi

# remove trailing slash (if any)
installDir=${installDir%/}
# Convert to Unix path in MSYS. Required to ensure that PATH is working. Windows paths a la c:\ will not work.
installDir=$(cygpath -u "$installDir")

echo "INFO: Installing OpenWalnut to: $installDir"
#make install
# Faster than make but does not ensure proper previous build!
cmake -P cmake_install.cmake
# Error?
if [ $? -ne 0 ]; then
    echo "ERROR: install failed. Did you call make first? Exit."
    exit -1
fi

###############################################################################
# Setup some paths
###############################################################################

# Where to copy external dependencies? Relative to current dir: bin
target="$installDir/libExt"
mkdir -p "$target"

# Extend PATH to contain "bin" to allow ldd to find libOpenWalnut and stuff
# NOTE: leave orig path at the beginning to allow multiple runs
PATH="$PATH:./lib/:./bin/:$target"
export PATH

###############################################################################
# Some files need to be copied to libExt "manually"
###############################################################################

# Although linked dynamically, these files where not found:
# Nifti
echo "Copy NiFTI"
cp /c/ow_workspace/nifticlib-2.0.0/znzlib/libznz.dll "$target"
cp /c/ow_workspace/nifticlib-2.0.0/niftilib/libniftiio.dll "$target"

# Plugins. Not dynamically linked. Loaded on startup. Copy: 
# OSG Plugins
echo "Copy OSG Plugins"
cp -r /mingw64/bin/osgPlugins-* "$target"

# Qt Plugins
echo "Copy Qt Plugins"
mkdir -p "$target/qtPlugins/"
cp -r /mingw64/share/qt5/plugins/platforms "$target/qtPlugins/"

###############################################################################
# Find all DLL to which the OW exe and DLL are linked to:
###############################################################################

# Collect a list of DLL and Exe we build.
linkables=$(find ./lib -iname "*.dll" -or -iname "*.exe" && find ./bin -iname "*.dll" -or -iname "*.exe")

# Fill all DLLs into this var:
allDeps=""

# Collect a list of files where deps could not be resolved.
warnFiles=""

# For each file which might have dependencies:
for linkable in $linkables
do
	echo "Found file with potential dependencies: $linkable"
	# Get the dependencies, filter out unknowns and Windows DLL
	# winDeps=$(ldd "$linkable" | awk '{print $3;}' | grep -i "/c/Windows")
	# echo " * INFO: Windows Deps: $winDeps"
	unresolvedDeps=$(ldd "$linkable" | awk '{print $3;}' | grep -i "\?")
	if [ -n "$unresolvedDeps" ]; then
		echo " * WARN: Unresolved Deps!"
		warnFiles="$warnFiles"$'\n'"$linkable"
	fi
	localDeps=$(ldd "$linkable" | \
				awk '{print $3;}' | \
				grep -iv "/c/Windows" | \
				grep -iv "\?" | \
				grep -iv "`pwd`")
	if [ -n "$localDeps" ]; then
		echo " * INFO: Dependencies found: "$'\n'"$localDeps"
		allDeps="$allDeps"$'\n'"$localDeps"
	fi
	echo ""
done

###############################################################################
# Copy everything
###############################################################################

echo "DONE!"
if [ -n "$warnFiles" ]; then
	echo "WARN: found files with unresolved dependencies. Here is a list:"
	echo " * $warnFiles"
	echo ""
    echo "You will need to fix this manually, or OpenWalnut might refuse to start."
fi

echo "Found the following dependencies. Copying them to $target."
# NOTE: might still be some doubles ...
allDeps=$(echo "$allDeps" | sort | uniq)
echo "$allDeps"
# CP complains about doubled entries
cp $allDeps "$target"

###############################################################################
# Zipping ? Maybe done later ...
###############################################################################
