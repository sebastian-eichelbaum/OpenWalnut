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

#include "../common/WColor.h"
#include "../graphicsEngine/WGEUtils.h"
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
    // TODO(schurade): replace this with a permanent solution
    for ( size_t i = 0; i < m_vertices->size(); ++i )
    {
        m_vertices->at( i ) = 160 - m_vertices->at( i );
        ++i;
        m_vertices->at( i ) = 200 - m_vertices->at( i );
        ++i;
        //m_pointArray[i] = m_dh->frames - m_pointArray[i];
    }

    m_tangents = boost::shared_ptr< std::vector< float > >( new std::vector<float>() );
    m_tangents->resize( m_vertices->size() );
    m_globalColors = boost::shared_ptr< std::vector< float > >( new std::vector<float>() );
    m_globalColors->resize( m_vertices->size() );
    m_localColors = boost::shared_ptr< std::vector< float > >( new std::vector<float>() );
    m_localColors->resize( m_vertices->size() );

    int pc = 0;
    float r, g, b, rr, gg, bb;
    float x1, x2, y1, y2, z1, z2;
    float lastx, lasty, lastz;
    for ( size_t i = 0; i < m_lineLengths->size(); ++i )
    {
        x1 = m_vertices->at( pc );
        y1 = m_vertices->at( pc + 1 );
        z1 = m_vertices->at( pc + 2 );
        x2 = m_vertices->at( pc + m_lineLengths->at( i ) * 3 - 3 );
        y2 = m_vertices->at( pc + m_lineLengths->at( i ) * 3 - 2 );
        z2 = m_vertices->at( pc + m_lineLengths->at( i ) * 3 - 1 );

        r = ( x1 ) - ( x2 );
        g = ( y1 ) - ( y2 );
        b = ( z1 ) - ( z2 );
        if ( r < 0.0 )
            r *= -1.0;
        if ( g < 0.0 )
            g *= -1.0;
        if ( b < 0.0 )
            b *= -1.0;

        float norm = sqrt( r * r + g * g + b * b );
        r *= 1.0 / norm;
        g *= 1.0 / norm;
        b *= 1.0 / norm;

        lastx = m_vertices->at( pc ) + ( m_vertices->at( pc ) - m_vertices->at( pc + 3 ) );
        lasty = m_vertices->at( pc + 1 ) + ( m_vertices->at( pc + 1 ) - m_vertices->at( pc + 4 ) );
        lastz = m_vertices->at( pc + 2 ) + ( m_vertices->at( pc + 2 ) - m_vertices->at( pc + 5 ) );

        for ( size_t j = 0; j < m_lineLengths->at( i ); ++j )
        {
            rr = lastx - m_vertices->at( pc );
            gg = lasty - m_vertices->at( pc + 1 );
            bb = lastz - m_vertices->at( pc + 2 );
            lastx = m_vertices->at( pc );
            lasty = m_vertices->at( pc + 1 );
            lastz = m_vertices->at( pc + 2 );

//            if ( rr < 0.0 )
//                rr *= -1.0;
//            if ( gg < 0.0 )
//                gg *= -1.0;
//            if ( bb < 0.0 )
//                bb *= -1.0;
            float norm = sqrt( rr * rr + gg * gg + bb * bb );
            rr *= 1.0 / norm;
            gg *= 1.0 / norm;
            bb *= 1.0 / norm;

            m_tangents->at( pc ) = rr;
            m_tangents->at( pc + 1 ) = gg;
            m_tangents->at( pc + 2 ) = bb;

            m_globalColors->at( pc ) = r;
            m_globalColors->at( pc + 1 ) = g;
            m_globalColors->at( pc + 2 ) = b;

            // local color fun:
            WColor localColor;
            wmath::WPosition currentPos( m_vertices->at( pc ), m_vertices->at( pc + 1 ), m_vertices->at( pc + 2 ) );
            wmath::WPosition nextPos;
            if( j < m_lineLengths->at( i ) - 1 )
            {
                nextPos[0] = m_vertices->at( pc + 3 );
                nextPos[1] = m_vertices->at( pc + 4 );
                nextPos[2] = m_vertices->at( pc + 5 );
                localColor = wge::getRGBAColorFromDirection( currentPos, nextPos );
            }
            else if( m_lineLengths->size() > 1 ) // there was a color in this line before!
            {
                localColor = WColor( m_localColors->at( pc - 3 ),
                                     m_localColors->at( pc - 2 ),
                                     m_localColors->at( pc - 1 ) );
            }
            else // there was only one point
            {
                localColor = WColor( r, g, b ); // same as global color
            }
            m_localColors->at( pc ) = localColor.getRed();
            m_localColors->at( pc + 1 ) = localColor.getGreen();
            m_localColors->at( pc + 2 ) = localColor.getBlue();
            pc += 3;
        }
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

boost::shared_ptr< std::vector< float > > WDataSetFibers::getTangents() const
{
    return m_tangents;
}

boost::shared_ptr< std::vector< float > > WDataSetFibers::getGlobalColors() const
{
    return m_globalColors;
}

boost::shared_ptr< std::vector< float > > WDataSetFibers::getLocalColors() const
{
    return m_localColors;
}

wmath::WPosition WDataSetFibers::getPosition( size_t fiber, size_t vertex ) const
{
    size_t index = m_lineStartIndexes->at( fiber ) * 3;
    index += vertex * 3;
    return wmath::WPosition( m_vertices->at( index ), m_vertices->at( index + 1 ), m_vertices->at( index + 2 ) );
}

wmath::WPosition WDataSetFibers::getTangent( size_t fiber, size_t vertex ) const
{
    wmath::WPosition point = getPosition( fiber, vertex );
    wmath::WPosition tangent;

    if ( vertex == 0 ) // first point
    {
        wmath::WPosition pointNext = getPosition( fiber, vertex + 1 );
        tangent = point - pointNext;
    }
    else if ( vertex == m_lineLengths->at( fiber ) - 1 ) // last point
    {
        wmath::WPosition pointBefore = getPosition( fiber, vertex - 1 );
        tangent = pointBefore - point;
    }
    else // somewhere in between
    {
        wmath::WPosition pointBefore = getPosition( fiber, vertex - 1 );
        wmath::WPosition pointNext = getPosition( fiber, vertex + 1 );
        tangent = pointBefore - pointNext;
    }

    tangent.normalize();
    return tangent;
}
