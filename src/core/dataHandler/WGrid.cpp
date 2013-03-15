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

#include <cstddef>

#include <boost/shared_ptr.hpp>

#include "../common/WProperties.h"

#include "WGrid.h"

WGrid::WGrid( size_t size )
    : m_infoProperties( boost::shared_ptr< WProperties >( new WProperties( "Grid Properties", "Grid's information properties" ) ) ),
      m_size( size )
{
    m_infoProperties->setPurpose( PV_PURPOSE_INFORMATION );
    WPropInt sizeGrid = m_infoProperties->addProperty( "Grid size: ", "The number of position in the grid.", static_cast<int>( m_size ) );
}

WGrid::~WGrid()
{
}

size_t WGrid::size() const
{
    return m_size;
}

boost::shared_ptr< WProperties > WGrid::getInformationProperties() const
{
    return m_infoProperties;
}
