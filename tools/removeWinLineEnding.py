#!/usr/bin/env python

import codecs
import string
import sys
import unicodedata
import os

HELP_USAGE = "Usage: removeWinLineEnding.py fileListFile\n"

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

def removeWinLineEndingFromFile(filename):
    if filename == '-':
      lines = codecs.StreamReaderWriter(sys.stdin,
                                        codecs.getreader('utf8'),
                                        codecs.getwriter('utf8'),
                                        'replace').read().split('\n')
    else:
      lines = codecs.open(filename, 'r', 'utf8', 'replace').read().split('\n')

    carriage_return_found = False
    # Remove trailing '\r'.
    replaced = 0
    for linenum in range(len(lines)):
      if lines[linenum].endswith('\r'):
        lines[linenum] = lines[linenum].rstrip('\r')
        carriage_return_found = True
        replaced += 1
    if ( carriage_return_found == True ):
        writeFile(filename,lines)
        print "replaced " +str(replaced)+" rolbacks in "+filename
    
def main(argv):
    num_args = len(sys.argv)-1
    if num_args != 1:
        print HELP_USAGE
        return
        
    fileListFile = sys.argv[1];
    filesInList = read_file(fileListFile)
    for files in filesInList:
        removeWinLineEndingFromFile(files)
            
        
if __name__ == "__main__":
    main( sys.argv )
