#!/usr/bin/perl

$creator=qx("whoami");
chomp($creator);

$header = <<EOF
//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#include <boost/shared_ptr.hpp>

#include <osg/Geode>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"
#include "../dataHandler/WDataSetScalar.h"

/**
 * TODO(\"$creator\"): Document this.
 *
 * \\ingroup modules
 */
class WM#name# : public WModule
{
public:
    /**
     * Default constructor for constructing a module instance.
     */
    WM#name#();

    /**
     * Deconstruct this module instance.
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

private:
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input; //!< TODO(\"$creator\"): Document this!

    boost::shared_ptr< WDataSetScalar > m_dataSet; //!< TODO(\"$creator\"): Document this!
};
#endif  // WM#NAME#_H
EOF
;

$impl = <<EOF
//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#include "../../kernel/WKernel.h"

#include "WM#name#.h"

WM#name#::WM#name#()
    : WModule()
{
}

WM#name#::~WM#name#()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WM#name#::factory() const
{
    return boost::shared_ptr< WModule >( new WM#name#() );
}

const std::string WM#name#::getName() const
{
    // ToDo(\"$creator\"): Don't use CamleCase here!
    return "#name#";
}

const std::string WM#name#::getDescription() const
{
    // ToDo(\"$creator\"): Document this!
    return "";
}

void WM#name#::connectors()
{

    WModule::connectors();
}

void WM#name#::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.
}

void WM#name#::moduleMain()
{
}
EOF
;

die "Need exactly one command line argument (module name)\n" unless $#ARGV == 0;

$modulename = $ARGV[0];

$header =~ s/\#name\#/$modulename/gm;
$impl   =~ s/\#name\#/$modulename/gm;

# Set the header guards in capitals
$header =~ s/\#NAME\#/\U$modulename/gm;

open( FILE, ">WM${modulename}.h" ) or die;
print FILE $header;
close( FILE );

open( FILE, ">WM${modulename}.cpp" ) or die;
print FILE $impl;
close( FILE );
