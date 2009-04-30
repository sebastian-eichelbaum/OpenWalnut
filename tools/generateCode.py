#!/usr/bin/env python

from optparse import OptionParser
import os
import sys

class RawCode(object):
    def __init__(self, name, attr, structors):
        self._structors = structors
        self._name = "W" + name
        self._attr = attr
        self._indent = "    "
        self._header = \
"""\
//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

"""
        self._body = ""
        self._footer = ""

        if os.environ['USER']:
            self._author = os.environ['USER']
        elif os.environ['LOGNAME']:
            self._author = os.environ['LOGNAME']
        else:
            self._author = "Unkown"
        self._doxy = "/**\n * TODO(" + self._author + "): Document this!\n */\n"

    def indent(self, level, str):
        result = ""
        for line in str.splitlines( True ):
            if line.strip() == "":
                # don't indent empty lines
                result += line
            else:
                result += (level*self._indent) + line
        return result

    def signatures(self, accessor):
        result = []
        for attribute in self._attr:
            sig = []
            type = attribute[0]
            name = attribute[1]
            if( accessor == "get" ):
                sig.append(type + " ")
                sig.append("get" + name[0].upper() + name[1:] + "() const")
            elif( accessor == "set" ):
                sig.append("void ")
                sig.append("set" + name[0].upper() + name[1:] + "( " + type + " " + name + " )")
            else:
                assert 1==0 and "No getter nor setter methode requested"
            result.append(sig)
        return result

class HeaderCode(RawCode):
    def __init__(self, name, attr, structors):
        RawCode.__init__(self, name, attr, structors)
        guard = self._name.upper() + "_H"
        self._header += "#ifndef " + guard + "\n"
        self._header += "#define " + guard + "\n\n"
        self._footer += "#endif  // " + self._name.upper() + "_H\n"

    def renderSignatures(self):
        result = ""
        for sig in self.signatures("get"):
            result += self._doxy
            result += ''.join(sig) + ";" + "\n\n"
        for sig in self.signatures("set"):
            result += self._doxy
            result += ''.join(sig) + ";" + "\n\n"
        return result

    def renderMemberVariables(self):
        result = ""
        for attribute in self._attr:
            result += self._doxy
            result += attribute[0] + " m_" + attribute[1] + ";" + "\n\n";
        return result

    def structor(self, type):
        result = ""
        result += self._doxy
        if type == "con":
            result += ""
        elif type == "de":
            result += "~"
        else:
            assert 1 == 0 and "Neither Constructor nor Destructor are requested but: " + type
        result += self._name + "();\n\n"
        return result

    def __str__(self):
        modifier_level = 0
        member_level = 1
        result = self._header
        result += self.indent(0, self._doxy)
        result += "class " + self._name + "\n"
        result += "{\n"
        result += self.indent(modifier_level, "public:\n")
        if self._structors:
            result += self.indent(member_level, self.structor("con"))
            result += self.indent(member_level, self.structor("de"))
        result += self.indent(member_level, self.renderSignatures())
        result += self.indent(modifier_level, "protected:\n")
        result += self.indent(modifier_level, "private:\n")
        result += self.indent(member_level, self.renderMemberVariables())
        result += "};\n\n"
        result += self._footer
        return result

class ImplementationCode(RawCode):
    def __init__(self, name, attr, structors):
        RawCode.__init__(self, name, attr, structors)
        self._header += "#include \"" + self._name + ".h\"\n\n"

    def structor(self, type):
        result = self._name + "::"
        if type == "con":
            result += ""
        elif type == "de":
            result += "~"
        else:
            assert 1 == 0 and "Neither Constructor nor Destructor are requested but: " + type
        result += self._name + "()\n{\n}\n"
        return result + "\n"

    def renderSignatures(self):
        result = ""
        for sig in self.signatures("get"):
            result += sig[0] + self._name + "::" + sig[1] + "\n"
            result += "{\n"
            result += self.indent(1, "return m_" + sig[1][3].lower() + sig[1][4:-8]) + ";\n"
            result += "}\n\n"
        for sig in self.signatures("set"):
            result += sig[0] + self._name + "::" + sig[1] + "\n"
            result += "{\n"
            result += self.indent(1, "m_" + sig[1].split()[-2] + " = " + sig[1].split()[-2] + ";\n")
            result += "}\n\n"
        return result

    def __str__(self):
        member_level = 0
        result = self._header
        result += self.renderSignatures()
        if self._structors:
            result += self.indent(member_level, self.structor("con"))
            result += self.indent(member_level, self.structor("de"))
        return result

class TestCode(RawCode):
    def __init__(self, name):
        RawCode.__init__(self, name, [], False)

    def __str__(self):
        result = "TestCode"
        return result

def exists( fname ):
    return os.path.exists( fname )

def writeToFile( fname, payload ):
    file = open(fname, 'w')
    file.write( payload )
    file.close()

def writeIfNotExists( code, extension ):
    fname  = code._name + extension
    if exists( fname ):
        sys.stderr.write("Error: file " + fname + " already exists.\n")
        sys.exit(1)
    else:
        writeToFile( fname, str(code) )

def main():
    synopsis  = "\n\t%prog [options] Classname"
    synopsis += "\n\t%prog --help\n"
    synopsis += "Example:"
    synopsis += "\n\t%prog --class --header --member=int,plistId --member=WSomeOtherClass,myMemeberVar WSomeClass"
    synopsis += "\n\n%prog automatically writes the code to the corresponding files"
    synopsis += "\n(cpp, h) and exists if it already exists."
    parser = OptionParser(usage=synopsis, version="%prog 0.0_pre_alpha (USE AT YOUR OWN RISK)")
    parser.set_defaults(testsuite=False)
    parser.set_defaults(cls=False)
    parser.set_defaults(header=False)
    parser.set_defaults(structors=False)
    parser.add_option("-T","--test", action="store_true", help="Generates test stub", dest="testsuite")
    parser.add_option("-C","--class", action="store_true", help="Generates class stub", dest="cls")
    parser.add_option("-H","--header", action="store_true", help="Generates header stub", dest="header")
    parser.add_option("-m","--member", action="append", help="Adds member to class with public getter and setter methods", dest="members", type="string", metavar="TYPE,NAME")
    parser.add_option("-s","--structors", action="store_true", help="Adds default construtor and destructor.", dest="structors")
    (options, args) = parser.parse_args()
    attributes = []
    classname = ""
    if options.members:
        for m in options.members:
            if len(m.split(',')) != 2:
                parser.error("parsing option: '" + m + "'\nStrings defining type and name of the member are required and separated by a comma, e.g.: 'int,counter'")
            elif (len(m.split(',')[0]) < 1) or (len(m.split(',')[1]) < 1):
                parser.error("parsing option: '" + m + "'\nEmpty type or name string. Member definitions must have the form: type_string,name_string, e.g. 'int,counter'")
            else:
                attributes.append(m.split(','))
    if len(args) != 1:
        parser.error("Invalid number of arguments, Classname required")
    else:
        classname = args[0]

    if not options.cls and not options.header and not options.testsuite:
        parser.error("No code should be generated?, use at least one option: --class, --header or --test")

    if options.cls:
        writeIfNotExists( ImplementationCode(classname, attributes, options.structors), ".cpp" )
    if options.header:
        writeIfNotExists( HeaderCode(classname, attributes, options.structors), ".h" )
    if options.testsuite:
        print TestCode(classname)

if __name__ == '__main__':
    main()
