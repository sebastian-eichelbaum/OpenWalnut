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
#include <algorithm>
#include <vector>

#include "WDataSet.h"

#include "WDataSetFibers.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetFibers::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetFibers::WDataSetFibers()
    : WDataSet()
{
    // default constructor used by the prototype mechanism
}

WDataSetFibers::WDataSetFibers( boost::shared_ptr< std::vector< float > >vertices,
                boost::shared_ptr< std::vector< size_t > > lineStartIndexes,
                boost::shared_ptr< std::vector< size_t > > lineLengths,
                boost::shared_ptr< std::vector< size_t > > verticesReverse )
    : WDataSet(),
    m_vertices( vertices ),
    m_lineStartIndexes( lineStartIndexes ),
    m_lineLengths( lineLengths ),
    m_verticesReverse( verticesReverse )
{
    for ( size_t i = 0; i < m_vertices->size(); ++i )
    {
        m_vertices->at( i ) = 160 - m_vertices->at( i );
        ++i;
        m_vertices->at( i ) = 200 - m_vertices->at( i );
        ++i;
        //m_pointArray[i] = m_dh->frames - m_pointArray[i];
    }
}

void WDataSetFibers::sortDescLength()
{
    //std::sort( m_fibers->begin(), m_fibers->end(), wmath::hasGreaterLengthThen );
}

bool WDataSetFibers::isTexture() const
{
    return false;
}

size_t WDataSetFibers::size() const
{
    return m_lineStartIndexes->size();
}

const std::string WDataSetFibers::getName() const
{
    return "WDataSetFibers";
}

const std::string WDataSetFibers::getDescription() const
{
    return "Contains tracked fiber data.";
}

boost::shared_ptr< WPrototyped > WDataSetFibers::getPrototype()
{
    if ( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetFibers() );
    }

    return m_prototype;
}

boost::shared_ptr< std::vector< float > > WDataSetFibers::getVertices() const
{
    return m_vertices;
}

boost::shared_ptr< std::vector< size_t > > WDataSetFibers::getLineStartIndexes() const
{
    return m_lineStartIndexes;
}

boost::shared_ptr< std::vector< size_t > > WDataSetFibers::getLineLengths() const
{
    return m_lineLengths;
}

boost::shared_ptr< std::vector< size_t > > WDataSetFibers::getVerticesReverse() const
{
    return m_verticesReverse;
}

wmath::WPosition WDataSetFibers::getPosition( size_t fiber, size_t vertex ) const
{
    size_t index = m_lineStartIndexes->at( fiber ) * 3;
    index += vertex * 3;
    return wmath::WPosition( m_vertices->at( index ), m_vertices->at( index + 1 ), m_vertices->at( index + 2 ) );
}
