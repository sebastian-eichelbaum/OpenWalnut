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


#include <iostream>
#include <map>
#include <string>

#include "../common/WLogger.h"

#include "WProperties.h"

WProperties::WProperties()
{
}

WProperties::~WProperties()
{
}

void WProperties::addBool( std::string name, bool value, std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, shortDesc, longDesc );
    m_propertyList[name] = prop;
}

void WProperties::addChar( std::string name, char value, std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, shortDesc, longDesc );

    m_propertyList[name] = prop;
}


void WProperties::addInt( std::string name, int value, std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, shortDesc, longDesc );
    m_propertyList[name] = prop;
}


void WProperties::addFloat( std::string name, float value, std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, shortDesc, longDesc );
    m_propertyList[name] = prop;
}


void WProperties::addDouble( std::string name, double value, std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, shortDesc, longDesc );
    m_propertyList[name] = prop;
}


void WProperties::addString( std::string name, std::string value, std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, shortDesc, longDesc );
    m_propertyList[name] = prop;
}

WProperty* WProperties::findProp( std::string name )
{
    if ( m_propertyList.count( name) != 0 )
    {
        return m_propertyList[name];
    }
    else
    {
        std::string msg( "Property doesn't exist: " + name );
        WLogger::getLogger()->addLogMessage( msg, "Properties", LL_ERROR );
        return 0;
    }
}


std::string WProperties::getValueString( const std::string prop )
{
    if ( findProp( prop ) )
    {
        return findProp( prop )->getValueString();
    }
    return "";
}

std::map < std::string, WProperty* >* WProperties::getProperties()
{
    return &m_propertyList;
}
