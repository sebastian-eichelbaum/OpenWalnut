#!/usr/bin/env python

from optparse import OptionParser
import os

class RawCode(object):
    def __init__(self, name, attr):
        self._name = name
        self._attr = attr
        self._indent = "    "
        self._header = \
"""
//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

"""
        self._body = ""
        self._footer = ""

        if os.environ['USER']:
            self._author = os.environ['USER']
        elif os.environ['LOGNAME']:
            self._author = os.environ['USER']
        else:
            self._author = "Unkown"
        self._doxy = "/**\n * TODO(" + self._author + "): Document this!\n */\n"

    def indent(self, level, str):
        result = ""
        for line in str.splitlines( True ):
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
    def __init__(self, name, attr):
        RawCode.__init__(self, name, attr)
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

    def __str__(self):
        modifier_level = 0
        member_level = 1
        result = self._header
        result += self.indent(0, self._doxy)
        result += "class " + self._name + " : public \n"
        result += "{\n"
        result += self.indent(modifier_level, "public:\n")
        result += self.indent(member_level, self.renderSignatures())
        result += self.indent(modifier_level, "protected:\n\n")
        result += self.indent(modifier_level, "private:\n")
        result += self.indent(member_level, self.renderMemberVariables())
        result += "};\n\n"
        result += self._footer
        return result

class ImplementationCode(RawCode):
    def __init__(self, name, attr):
        RawCode.__init__(self, name, attr)

    def renderSignatures(self):
        result = ""
        for sig in self.signatures("get"):
            result += "\n" + sig[0] + self._name + "::" + sig[1] + "\n"
            result += "{\n"
            result += self.indent(1, "return m_" + sig[1][3].lower() + sig[1][4:-8]) + ";\n"
            result += "}\n"
        for sig in self.signatures("set"):
            result += "\n" + sig[0] + self._name + "::" + sig[1] + "\n"
            result += "{\n"
            result += self.indent(1, "m_" + sig[1].split()[-2] + " = " + sig[1].split()[-2] + ";\n")
            result += "}\n"
        return result

    def __str__(self):
        result = self._header
        result += "#include \"" + self._name + ".h\"\n"
        result += self.renderSignatures()
        result += self._footer
        return result

class TestCode(RawCode):
    def __init__(self, name):
        RawCode.__init__(self, name, [])

    def __str__(self):
        result = "TestCode"
        return result

def main():
    synopsis  = "\n\t%prog [options] Classname\n"
    synopsis += "Example:"
    synopsis += "\n\t%prog --class --header --member=int,plistId --member=BSomeOtherClass,myMemeberVar BSomeClass"
    parser = OptionParser(usage=synopsis, version="%prog 0.0_pre_alpha (USE AT YOUR OWN RISK)")
    parser.add_option("--test", action="store_true", help="Generates test stub", dest="testsuite", default=False)
    parser.add_option("--class", action="store_true", help="Generates class stub", dest="cls", default=False)
    parser.add_option("--header", action="store_true", help="Generates header stub", dest="header", default=False)
    parser.add_option("--member", action="append", help="Adds member to class with public getter and setter methods", dest="members", type="string")
    (options, args) = parser.parse_args()

    attributes = []
    classname = ""
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
        print ImplementationCode(classname, attributes)
    if options.header:
        print HeaderCode(classname, attributes)
    if options.testsuite:
        print TestCode(classname)

if __name__ == '__main__':
    main()
