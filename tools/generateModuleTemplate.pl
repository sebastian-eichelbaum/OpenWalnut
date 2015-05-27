#!/usr/bin/perl

$creator=qx("whoami");
chomp($creator);

$header = <<EOF
//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015 OpenWalnut Community, Nemtics, BSV\@Uni-Leipzig
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

#ifndef WM#NAME#_H
#define WM#NAME#_H

#include <string>

#include <osg/Geode>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/** 
 * Someone should add some documentation here.
 * Probably the best person would be the module's
 * creator, i.e. \"$creator\".
 *
 * This is only an empty template for a new module. For
 * an example module containing many interesting concepts
 * and extensive documentation have a look at "src/modules/template"
 *
 * \\ingroup modules
 */
class WM#name#: public WModule
{
public:
    /**
     *
     */
    WM#name#();

    /**
     *
     */
    virtual ~WM#name#();

    /**
     * Gives back the name of this module.
     * \\return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \\return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \\return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Initialize requirements for this module.
     */
    virtual void requirements();


private:
};

#endif  // WM#NAME#_H
EOF
;

$impl = <<EOF
//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015 OpenWalnut Community, Nemtics, BSV\@Uni-Leipzig
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

#include <string>

#include "core/kernel/WKernel.h"

#include "WM#name#.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WM#name# )

WM#name#::WM#name#():
    WModule()
{
}

WM#name#::~WM#name#()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WM#name#::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WM#name#() );
}

const std::string WM#name#::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "#name#";
}
 
const std::string WM#name#::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "No documentation yet. "
    "The best person to ask for documenation is probably the modules's creator, i.e. the user with the name \\\"$creator\\\".";
}

void WM#name#::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.

    WModule::connectors();
}

void WM#name#::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.

    WModule::properties();
}

void WM#name#::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WM#name#::moduleMain()
{
    // Put the code for your module's main functionality here.
    // See "src/modules/template/" for an extensively documented example.
}
EOF
;


$meta = <<EOF
//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015 OpenWalnut Community, Nemtics, BSV\@Uni-Leipzig
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

// Provide additional material and descriptions for your module. You can
// provide this for multiple modules if you like. 
// The purpose of this file is to allow the developers to provide additional
// material for their module, like documentation links, video tutorials and 
// similar. You should always write such a META file for your module, even
// if you do not provide additional material (for now at least).

// An extensively commented example of a META file can be 
// found in src/modules/template/resources

"#name#"
{
  // Provide an icon with this name in the same directory as this file.
  icon="WM#name#Icon.png";

  website = "http://www.openwalnut.org";
  author = "OpenWalnut Project";
  "OpenWalnut Project"
  {
    url="http://www.openwalnut.org";
    email="contact\@openwalnut.org";
    what="Design, Development and Bug fixing";
  };
};
EOF
;

die "Need exactly one command line argument (module name).\nPlease execute this script in the src/modules directory with the desired module name.\n" unless $#ARGV == 0;

$dirname = lcfirst($ARGV[0]); # directory has to have lower case first letter
use Cwd;
$parentdir = getcwd."/";
$modulename = ucfirst($ARGV[0]); # name has to have upper case first letter

print "Generating for ".$modulename." in ".$parentdir.$dirname.".\n";

mkdir $dirname;
$resourcesDirName = "${dirname}/resources";
print $resourcesDirName;
mkdir $resourcesDirName;


$header =~ s/\#name\#/$modulename/gm;
$impl   =~ s/\#name\#/$modulename/gm;
$meta   =~ s/\#name\#/$modulename/gm;

# Set the header guards in capitals
$header =~ s/\#NAME\#/\U$modulename/gm;



open( FILE, ">${dirname}/WM${modulename}.h" ) or die;
print FILE $header;
close( FILE );

open( FILE, ">${dirname}/WM${modulename}.cpp" ) or die;
print FILE $impl;
close( FILE );

open( FILE, ">${dirname}/resources/META" ) or die;
print FILE $meta;
close( FILE );

print "Generating completed.\n";
print "\n";
print "Look at the template module for an introduction to module programming.\n";
print "\n";
print "Do not forget to add the new module to a module-*.toolbox file in order to introduce it to the build system.\n";
