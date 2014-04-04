#!/usr/bin/env python

from optparse import OptionParser
import os
import sys

class RawCode(object):
    def __init__(self, name, attr=None, structors=None, subclass=None, includes=None):
        self._attr = [] if not attr else attr
        self._structors = False if not structors else structors
        self._name = "W" + name
        self._indent = "    "
        self._header = \
"""\
//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2014 OpenWalnut Community, Nemtics, BSV@Uni-Leipzig
// For more information see http://www.openwalnut.org/copying
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
        self._subclass = "" if not subclass else subclass
        self._includes = [] if not includes else includes

        if os.environ['USER']:
            self._author = os.environ['USER']
        elif os.environ['LOGNAME']:
            self._author = os.environ['LOGNAME']
        else:
            self._author = "Unkown"
        self._doxy = "/**\n * TODO(" + self._author + "): Document this!\n */\n"

    def renderHeader(self):
        return self._header

    def renderIncludes(self):
        result = ""
        for include in self._includes:
            result += "#include " + include + "\n"
        return result

    def renderStructors(self):
        return ""

    def renderBody(self):
        return self._body

    def renderFooter(self):
        return self._footer

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

    def structor(self, type):
        assert type == "con" or type == "de"
        result = "~" if type == "de" else ""
        result += self._name
        return result

    def __str__(self):
        result = ""
        result += self.renderHeader()
        result += self.renderIncludes()
        result += self.renderBody()
        result += self.renderFooter()
        return result

class HeaderCode(RawCode):
    def __init__(self, name, attr=None, structors=None, subclass=None, includes=None):
        RawCode.__init__(self, name, attr, structors, subclass, includes)
        self._guard = self._name.upper() + "_H"
        self._fname = self._name + ".h"

    def renderHeader(self):
        result = RawCode.renderHeader(self)
        result += "#ifndef " + self._guard + "\n"
        result += "#define " + self._guard + "\n\n"
        return result

    def renderStructors(self):
        result = self.indent(1, self._doxy + self.structor("con") + "();\n\n")
        result += self.indent(1, self._doxy + self.structor("de") + "();\n\n")
        return result

    def renderBody(self):
        modifier_level = 0
        member_level = 1
        result = self.indent(0, self._doxy)
        result += "class " + self._name
        if self._subclass != "":
            result += " : " + self._subclass
        result += "\n"
        result += "{\n"
        result += self.indent(modifier_level, "public:\n")
        if self._structors:
            result += self.renderStructors()
        result += self.indent(member_level, self.renderSignatures())
        result += self.indent(modifier_level, "protected:\n")
        result += self.indent(modifier_level, "private:\n")
        result += self.indent(member_level, self.renderMemberVariables())
        result += "};\n\n"
        return result

    def renderFooter(self):
        return "#endif  // " + self._guard + "\n"

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


class ImplementationCode(RawCode):
    def __init__(self, name, attr=None, structors=None, subclass=None, includes=None):
        RawCode.__init__(self, name, attr, structors, subclass, includes)
        self._includes.append("\"" + self._name + ".h\"\n")
        self._fname = self._name + ".cpp"

    def structor(self, type):
        return self._name + "::" + RawCode.structor(self, type)

    def renderStructors(self):
        result = self.indent(0, self.structor("con") + "()\n{\n}\n\n")
        result += self.indent(0, self.structor("de") + "()\n{\n}\n\n")
        return result

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

    def renderBody(self):
        member_level = 0
        result = ""
        if self._structors:
            result += self.renderStructors()
        result += self.renderSignatures()
        return result

class TestCode(HeaderCode):
    def __init__(self, name):
        HeaderCode.__init__(self, name, [], structors=False)
        self._guard = self._name.upper() + "_TEST_H"
        self._fname = "test/" + self._name + "_test.h"
        self._includes.append("<cxxtest/TestSuite.h>\n")
        self._includes.append("\"../" + self._name + ".h\"\n")

    def renderBody(self):
        modifier_level = 0
        member_level = 1
        result = ""
        result += self.indent(0, self._doxy)
        result += "class " + self._name + "Test : public CxxTest::TestSuite\n"
        result += "{\n"
        result += self.indent(modifier_level, "public:\n")
        result += self.indent(member_level, self._doxy + "void testSomething( void )\n{\n}\n")
        result += "};\n\n"
        return result

class ExceptionHeader(HeaderCode):
    def __init__(self, name, structors=None):
        HeaderCode.__init__(self, name,
                            structors=structors,
                            subclass="public std::logic_error",
                            includes=["<stdexcept>", "<string>\n"]
                           )

    def renderStructors(self):
        result = self.indent(1, self._doxy + self.structor("con") + "( const std::string &s = std::string() ) throw();\n\n")
        result += self.indent(1, self._doxy + self.structor("de") + "();\n\n")
        return result

class ExceptionImplementation(ImplementationCode):
    def __init__(self, name, structors=None):
        ImplementationCode.__init__(self, name,
                                    structors=structors,
                                    includes=["<stdexcept>", "<string>\n"]
                                   )

    def renderStructors(self):
        result = self.indent(0, self.structor("con") + "( const std::string &s = std::string() ) throw()\n" + self._indent + ": std::logic_error( s )\n{\n}\n\n")
        result += self.indent(0, self.structor("de") + "()\n{\n}\n\n")
        return result

class ExceptionTest(TestCode):
    def __init__(self, name):
        TestCode.__init__(self, name)


def writeToFile( fname, payload ):
    file = open(fname, 'w')
    file.write( payload )
    file.close()

def writeIfNotExists( code, nofile):
    if os.path.exists( code._fname ):
        sys.stderr.write("Error: file " + code._fname + " already exists.\n")
        sys.exit(1)
    else:
        if nofile:
            print str(code)
        else:
            if (code._fname.split("/")[0] == "test") and not os.path.exists( "test/"):
                sys.stderr.write("Error: test dir does not exists for " + code._fname + "\n")
                sys.exit(1)
            writeToFile( code._fname, str(code) )

def main():
    synopsis  = "\n\t%prog [options] Classname"
    synopsis += "\n\t%prog --help\n"
    synopsis += "Example:"
    synopsis += "\n\t%prog --class --header --member=int,plistId --member=SomeOtherClass,myMemeberVar SomeClass"
    synopsis += "\n\nWARNING %prog automatically writes the code to the corresponding files if they not exist"
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
    parser.add_option("-e","--exception", action="store_true", help="Generates class, header and test stub for an exception", dest="exception")
    parser.add_option("--nofile", action="store_true", help="Writes all to stdout", dest="nofile")
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

    if not options.cls and not options.header and not options.testsuite and not options.exception:
        parser.error("No code should be generated?, use at least one option: --class, --header or --test")

    if options.exception and (options.header or options.cls or options.testsuite):
        parser.error("To generate exception code is exceptional, hence, please don't use other generators like --class, --header or --test here")

    if options.cls:
        writeIfNotExists( ImplementationCode(classname, attributes, options.structors), options.nofile )
    if options.header:
        writeIfNotExists( HeaderCode(classname, attributes, options.structors), options.nofile )
    if options.testsuite:
        writeIfNotExists( TestCode(classname), options.nofile )
    if options.exception:
        writeIfNotExists( ExceptionHeader(classname, structors=options.structors), options.nofile )
        writeIfNotExists( ExceptionImplementation(classname, structors=options.structors), options.nofile )
        writeIfNotExists( ExceptionTest(classname), options.nofile )

if __name__ == '__main__':
    main()
