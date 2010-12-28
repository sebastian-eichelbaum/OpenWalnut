#!/usr/bin/env python
#
# little helper script for OpenWalnut to copy files if they dont exist or differ from the one existing
# especialy useful with generated files that should not trigger a rebuild when they get generated the same
# By Robin Ledig - Oct 2010

import sys
import os
import string
import os.path

HELP_USAGE = "Usage: copyIfNes.py srcFile targetFile \n"

#read a complete file into a stringlist
def read_file (filename):
    usefn = filename
    pos = string.find(usefn,"\n") 
    if ( pos >= 0 ):
        usefn = filename[:pos]

    if not os.path.exists(usefn):
        print "File " + usefn + " doesn't exist."
        return ()
        
    f = file(usefn, "r")
    lines = f.readlines()
    f.close()
    for i in range(len(lines)):
        lines[i] = string.rstrip(lines[i],"\n")
    return lines
    
def writeFile(filename,lines):
    for i in range(len(lines)):
        lines[i] = string.rstrip(lines[i],"\n")
        lines[i] = lines[i] + "\n"
    f = open(filename, "w")
    f.writelines(lines)
    f.close()


def main(argv):
    num_args = len(sys.argv)-1
    if num_args != 2:
        print HELP_USAGE
        return
    orgFile = read_file(sys.argv[1])
    #if the target file doesn't exist, just write it
    if ( not os.path.exists(sys.argv[2]) ):
        writeFile(sys.argv[2],orgFile)
        return
    targetFile = read_file(sys.argv[2])
    #unequal line count -> overwrite the file
    if ( len(orgFile) != len(targetFile) ):
        writeFile(sys.argv[2],orgFile)
        return
    # check if all lines match, if not overwrite otherwise done
    identical = 1
    for i in range(len(orgFile)):
        if ( string.find(orgFile[i],targetFile[i]) == -1 ):
            identical = 0
            break
    if ( identical == 0 ):
        writeFile(sys.argv[2],orgFile)

if __name__ == "__main__":
    main( sys.argv )

