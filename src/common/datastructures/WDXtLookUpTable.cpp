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

#include <cassert>
#include <algorithm>
#include <vector>

#include "WDXtLookUpTable.h"

WDXtLookUpTable::WDXtLookUpTable( size_t dim )
    : m_data( ( dim * ( dim - 1 ) ) / 2, 0.0 ),
      m_dim( dim )
{
}

double& WDXtLookUpTable::operator()( size_t i, size_t j ) throw( WOutOfBounds )
{
    if( i == j )
    {
        std::stringstream ss;
        ss << "Invalid Element Access ( " << i << ", " << j
           << " ). Main Diagonal Elements are forbidden in this table.";
        throw WOutOfBounds( ss.str() );
    }
    if( i > j )
    {
        std::swap( i, j );
    }
    return m_data[( i * m_dim + j - ( i + 1 ) * ( i + 2 ) / 2 )];
}

size_t WDXtLookUpTable::size() const
{
    return m_data.size();
}

size_t WDXtLookUpTable::dim() const
{
    return m_dim;
}

const std::vector< double >& WDXtLookUpTable::getData() const
{
    return m_data;
}

void WDXtLookUpTable::setData( const std::vector< double > &data ) throw( WOutOfBounds )
{
    if( m_dim * ( m_dim - 1 ) / 2 != data.size() )
    {
        std::stringstream ss;
        ss << "Data vector length: " << data.size() << " doesn't fit to dimension: " << m_dim;
        throw WOutOfBounds( ss.str() );
    }
    m_data = std::vector< double >( data ); // copy content
}
