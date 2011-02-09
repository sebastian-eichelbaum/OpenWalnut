#!/usr/bin/env python

import os
import string
import shutil
import sys

HELP_USAGE = "Usage: copyWinRelease.py\n"

def get_immediate_subdirectories(dir):
    return [name for name in os.listdir(dir)
            if os.path.isdir(os.path.join(dir, name))]

def copyRelevantBinarys(fromDir,toDir):
    shutil.copy2( fromDir + "/walnut.exe", toDir )
    shutil.copy2( fromDir + "/OWcommon.dll", toDir )
    shutil.copy2( fromDir + "/OWdataHandler.dll", toDir )
    shutil.copy2( fromDir + "/OWge.dll", toDir )
    shutil.copy2( fromDir + "/OWkernel.dll", toDir )
    
def main(argv):
    num_args = len(sys.argv)-1
    if num_args != 0:
        print HELP_USAGE
        return
        
    # assume we are in the output directory (build)
    # first create a folder were all the ow binary release is put
    # copy from /bin all the relevant dll's and the executable
    # copy the share folder
    # get a list of directorys from /src/modules
    # remove all the folders in build/lib/modules not in the list before
    # in the target dir create a folder lib/modules
    # iterate over the list and copy all the release dll's (OWmodule_%MODULE_NAME%.dll) and if existing the shaders
    OWBinDir = "OWBinary"
    if ( os.path.exists(OWBinDir) ):
        shutil.rmtree(OWBinDir)
    os.mkdir(OWBinDir)
    OWBinExec = OWBinDir + "/bin"
    os.mkdir(OWBinExec)
    copyRelevantBinarys("bin",OWBinExec)
    shutil.copytree("share",OWBinDir+"/share")
    moduleExistingDirs = get_immediate_subdirectories("../src/modules")
    moduleCompDirs = get_immediate_subdirectories("lib/modules")
    # delete leftover compiled modules, e.g. if renamed removed or sth like this
    for module in moduleCompDirs:
        found = 0
        for srcModule in moduleExistingDirs:
            if ( string.find(module,srcModule) != -1 ):
                found = 1
        if ( found == 0 ):
            print "lib/modules/"+module +" not found in src, deleting"
            shutil.rmtree("lib/modules"+"/"+module)
    OWBinModuleDir = OWBinDir + "/lib/modules"
    os.makedirs(OWBinModuleDir)
    for cpymodule in moduleExistingDirs:
        currentModuleDir = OWBinModuleDir+"/"+cpymodule
        currentSrcDir = "lib/modules/"+cpymodule
        if ( os.path.exists(currentSrcDir) ):
            os.mkdir(currentModuleDir)
            if ( os.path.exists(currentSrcDir+"/OWmodule_"+cpymodule+".dll") ):
                shutil.copy2(currentSrcDir+"/OWmodule_"+cpymodule+".dll",currentModuleDir)
            if ( os.path.exists(currentSrcDir+"/shaders") ):
                shutil.copytree(currentSrcDir+"/shaders",currentModuleDir+"/shaders")    
        
if __name__ == "__main__":
    main( sys.argv )
