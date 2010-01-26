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

#include "WDataSetFibers2.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetFibers2::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetFibers2::WDataSetFibers2()
    : WDataSet()
{
    // default constructor used by the prototype mechanism
}

WDataSetFibers2::WDataSetFibers2( boost::shared_ptr< std::vector< float > >vertices,
                boost::shared_ptr< std::vector< unsigned int > > lineStartIndexes,
                boost::shared_ptr< std::vector< unsigned int > > lineLengths,
                boost::shared_ptr< std::vector< unsigned int > > verticesReverse )
    : WDataSet(),
    m_vertices( vertices ),
    m_lineStartIndexes( lineStartIndexes ),
    m_lineLengths( lineLengths ),
    m_verticesReverse( verticesReverse )
{
    for ( int i = 0; i < m_vertices->size(); ++i )
    {
        m_vertices->at( i ) = 160 - m_vertices->at( i );
        ++i;
        m_vertices->at( i ) = 200 - m_vertices->at( i );
        ++i;
        //m_pointArray[i] = m_dh->frames - m_pointArray[i];
    }

}

void WDataSetFibers2::sortDescLength()
{
    //std::sort( m_fibers->begin(), m_fibers->end(), wmath::hasGreaterLengthThen );
}

bool WDataSetFibers2::isTexture() const
{
    return false;
}

size_t WDataSetFibers2::size() const
{
    return m_lineStartIndexes->size();
    //return m_fibers->size();
}

//const wmath::WFiber& WDataSetFibers2::operator[]( const size_t index ) const
//{
//    assert( index < m_fibers->size() );
//    return (*m_fibers)[index];
//}

const std::string WDataSetFibers2::getName() const
{
    return "WDataSetFibers";
}

const std::string WDataSetFibers2::getDescription() const
{
    return "Contains tracked fiber data.";
}

boost::shared_ptr< WPrototyped > WDataSetFibers2::getPrototype()
{
    if ( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetFibers2() );
    }

    return m_prototype;
}

boost::shared_ptr< std::vector< float > > WDataSetFibers2::getVertices() const
{
    return m_vertices;
}

boost::shared_ptr< std::vector< unsigned int > > WDataSetFibers2::getLineStartIndexes() const
{
    return m_lineStartIndexes;
}

boost::shared_ptr< std::vector< unsigned int > > WDataSetFibers2::getLineLengths() const
{
    return m_lineLengths;
}

boost::shared_ptr< std::vector< unsigned int > > WDataSetFibers2::getVerticesReverse() const
{
    return m_verticesReverse;
}

wmath::WPosition WDataSetFibers2::getPosition( unsigned int fiber, unsigned int vertex ) const
{
    unsigned int index = m_lineStartIndexes->at( fiber ) * 3;
    index += vertex * 3;
    return wmath::WPosition( m_vertices->at( index ), m_vertices->at( index + 1 ), m_vertices->at( index + 2 ) );
}
