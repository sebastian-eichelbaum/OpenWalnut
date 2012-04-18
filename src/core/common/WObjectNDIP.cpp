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

#include "WObjectNDIP.h"

WObjectNDIP::WObjectNDIP( std::string name, std::string description, const char** icon ):
    m_name( name ),
    m_description( description ),
    m_icon( icon ),
    m_properties( new WProperties( name, description ) )
{
    // init
}

WObjectNDIP::~WObjectNDIP()
{
    // cleanup
}

std::string WObjectNDIP::getName() const
{
    return m_name;
}

std::string WObjectNDIP::getDescription() const
{
    return m_description;
}

const char** WObjectNDIP::getIcon() const
{
    return m_icon;
}

WProperties::SPtr WObjectNDIP::getProperties()
{
    return m_properties;
}

