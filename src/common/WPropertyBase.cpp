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

#include "WPropertyBase.h"

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

bool WPropertyBase::isHidden() const
{
    return m_hidden;
}

void WPropertyBase::setHidden( bool hidden )
{
    m_hidden = hidden;
}

