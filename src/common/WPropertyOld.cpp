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

#include "WPropertyOld.h"

WPropertyOld::WPropertyOld( std::string name, std::string value, bool hidden, std::string shortDesc, std::string longDesc )
    : m_type( P_STRING ),
    m_value( value )
{
    initMembers( name, shortDesc, longDesc, hidden );
}

WPropertyOld::WPropertyOld( std::string name, bool value, bool hidden, std::string shortDesc, std::string longDesc )
    : m_type( P_BOOL )
{
    setValue( value );
    initMembers( name, shortDesc, longDesc, hidden );
}

WPropertyOld::WPropertyOld( std::string name, char value, bool hidden, std::string shortDesc, std::string longDesc )
    : m_type( P_CHAR )
{
    setMin( -128 );
    setMax( 127 );
    setValue( value );
    initMembers( name, shortDesc, longDesc, hidden );
}

WPropertyOld::WPropertyOld( std::string name, int value, bool hidden, std::string shortDesc, std::string longDesc )
    : m_type( P_INT )
{
    setMin( 0 );
    setMax( 255 );
    setValue( value );
    initMembers( name, shortDesc, longDesc, hidden );
}

WPropertyOld::WPropertyOld( std::string name, float value, bool hidden, std::string shortDesc, std::string longDesc )
    : m_type( P_FLOAT )
{
    setMin( 0.0 );
    setMax( 100.0 );
    setValue( value );
    initMembers( name, shortDesc, longDesc, hidden );
}

WPropertyOld::WPropertyOld( std::string name, double value, bool hidden, std::string shortDesc, std::string longDesc )
    : m_type( P_DOUBLE )
{
    setMin( 0.0 );
    setMax( 100.0 );
    setValue( value );
    initMembers( name, shortDesc, longDesc, hidden );
}

WPropertyOld::WPropertyOld( std::string name, WColor value, bool hidden, std::string shortDesc, std::string longDesc )
    : m_type( P_DOUBLE )
{
    setValue( value );
    initMembers( name, shortDesc, longDesc, hidden );
}

WPropertyOld::~WPropertyOld()
{
}


PropertyType WPropertyOld::getType()
{
    return m_type;
}

std::string WPropertyOld::getName()
{
    return m_name;
}

void WPropertyOld::setShortDesc( const std::string desc )
{
    m_shortDesc = desc;
}


void WPropertyOld::setLongDesc( const std::string desc )
{
    m_longDesc = desc;
}


std::string WPropertyOld::getShortDesc()
{
    return m_shortDesc;
}


std::string WPropertyOld::getLongDesc()
{
    return m_longDesc;
}


std::string WPropertyOld::getValueString()
{
    return m_value;
}

void WPropertyOld::hide()
{
    m_isHidden = true;
}

void WPropertyOld::unhide()
{
    m_isHidden = false;
}

bool WPropertyOld::isHidden()
{
    return m_isHidden;
}

bool WPropertyOld::isDirty() const
{
    return m_isDirty;
}

void WPropertyOld::dirty( bool isDirty )
{
    m_isDirty = isDirty;
}

void WPropertyOld::initMembers( const std::string& name, const std::string& shortDesc, const std::string& longDesc, const bool hidden )
{
    m_name = name;
    m_shortDesc = shortDesc;
    m_longDesc = longDesc;
    m_isHidden = hidden;
    m_isDirty = false;
}

void WPropertyOld::signalValueChanged()
{
    m_signalValueChanged( m_name );
}

boost::signals2::signal1< void, std::string >* WPropertyOld::getSignalValueChanged()
{
    return &m_signalValueChanged;
}
