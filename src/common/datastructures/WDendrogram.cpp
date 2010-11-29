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

#include <sstream>

#include "WDendrogram.h"

WDendrogram::WDendrogram( size_t n )
    : m_heights( n - 1, 0.0 )
{
    m_parents.reserve( 2 * n - 1 );
    m_parents.resize( n, 0 );
}

size_t WDendrogram::merge( size_t i, size_t j, double height )
{
#ifdef DEBUG
    std::stringstream ss;
    ss << "Bug: n=" << m_heights.size() << " many leafs can lead maximal to 2n-1 many nodes in a tree but this was violated now!" << std::endl;
    WAssert( m_parents.size() < 2 * m_heights.size() + 1, ss.str() );
#endif

    m_parents.push_back( m_parents.size() ); // the root s always self referencing

#ifdef DEBUG
    m_heights->at( m_parents.size() - 2 - m_heights.size() ) = height;
    m_parents->at( i ) = m_parents.size() - 1;
    m_parents->at( j ) = m_parents.size() - 1;
#else
    m_heights[ m_parents.size() - 2 - m_heights.size() ] = height;
    m_parents[ i ] = m_parents.back();
    m_parents[ j ] = m_parents.back();
#endif

    return m_parents.size() - 1;
}
