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
            result += self.indent(2, self._doxy)
            result += self.indent(2, ''.join(sig) + ";" ) + "\n\n"
        for sig in self.signatures("set"):
            result += self.indent(2, self._doxy)
            result += self.indent(2, ''.join(sig) + ";" ) + "\n\n"
        return result

    def renderMemberVariables(self):
        result = ""
        for attribute in self._attr:
            result += self.indent(2, self._doxy)
            result += self.indent(2, attribute[0] + " m_" + attribute[1] + ";") + "\n";
        return result

    def __str__(self):
        result = self._header
        result += self.indent(0, self._doxy)
        result += "class " + self._name + " : public \n"
        result += "{\n"
        result += self.indent(1, "public:\n")
        result += self.renderSignatures()
        result += self.indent(1, "protected:\n\n")
        result += self.indent(1, "private:\n")
        result += self.renderMemberVariables()
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

#def main():
#    usage = "Usage: %prog [options] arg"
#    parser = OptionParser(usage)
#
#
#if __name__ == '__main__':
#    main()

c = HeaderCode("BGUI", [["int", "someInt"], ["char*", "message"]])
b = ImplementationCode("BGUI", [["int", "someInt"], ["char*", "message"]])
print c
print b
