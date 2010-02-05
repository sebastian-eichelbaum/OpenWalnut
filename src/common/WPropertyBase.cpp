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

#include <list>
#include <string>

#include <boost/filesystem.hpp>

#include "WPropertyBase.h"
#include "WPropertyVariable.h"

WPropertyBase::WPropertyBase( std::string name, std::string description ):
    m_name( name ),
    m_description( description ),
    m_hidden( false )
{
    // initialize members
}

WPropertyBase::~WPropertyBase()
{
    // cleanup
}

std::string WPropertyBase::getName() const
{
    return m_name;
}

std::string WPropertyBase::getDescription() const
{
    return m_description;
}

PROPERTY_TYPE WPropertyBase::getType() const
{
    return m_type;
}

void WPropertyBase::updateType()
{
    m_type = PV_UNKNOWN;
}

bool WPropertyBase::isHidden() const
{
    return m_hidden;
}

void WPropertyBase::setHidden( bool hidden )
{
    m_hidden = hidden;
}

WPropInt WPropertyBase::toPropInt()
{
    return boost::shared_static_cast< WPropertyVariable< int32_t > >( shared_from_this() );
}

WPropDouble WPropertyBase::toPropDouble()
{
    return boost::shared_static_cast< WPropertyVariable< double > >( shared_from_this() );
}

WPropBool WPropertyBase::toPropBool()
{
    return boost::shared_static_cast< WPropertyVariable< bool > >( shared_from_this() );
}

WPropString WPropertyBase::toPropString()
{
    return boost::shared_static_cast< WPropertyVariable< std::string > >( shared_from_this() );
}

WPropFilename WPropertyBase::toPropPath()
{
    return boost::shared_static_cast< WPropertyVariable< boost::filesystem::path > >( shared_from_this() );
}

WPropList WPropertyBase::toPropList()
{
    return boost::shared_static_cast< WPropertyVariable< std::list< std::string > > >( shared_from_this() );
}

