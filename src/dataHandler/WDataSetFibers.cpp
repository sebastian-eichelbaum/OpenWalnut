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
#include <utility>
#include <vector>
#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

#include "../common/WColor.h"
#include "../common/WLogger.h"
#include "../graphicsEngine/WGEUtils.h"
#include "WDataSet.h"
#include "WDataSetFibers.h"
#include "WCreateColorArraysThread.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetFibers::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetFibers::WDataSetFibers()
    : WDataSet()
{
    // default constructor used by the prototype mechanism
}

WDataSetFibers::WDataSetFibers( WDataSetFibers::VertexArray vertices,
                WDataSetFibers::IndexArray lineStartIndexes,
                WDataSetFibers::LengthArray lineLengths,
                WDataSetFibers::IndexArray verticesReverse,
                std::pair< wmath::WPosition, wmath::WPosition > boundingBox )
    : WDataSet(),
    m_vertices( vertices ),
    m_lineStartIndexes( lineStartIndexes ),
    m_lineLengths( lineLengths ),
    m_verticesReverse( verticesReverse ),
    m_bbMin( boundingBox.first ),
    m_bbMax( boundingBox.second )

{
    m_tangents = boost::shared_ptr< std::vector< float > >( new std::vector<float>() );
    m_tangents->resize( m_vertices->size() );
    boost::shared_ptr< std::vector< float > > globalColors = boost::shared_ptr< std::vector< float > >( new std::vector<float>() );
    globalColors->resize( m_vertices->size() );
    boost::shared_ptr< std::vector< float > > localColors = boost::shared_ptr< std::vector< float > >( new std::vector<float>() );
    localColors->resize( m_vertices->size() );

    // TODO(all): use the new WThreadedJobs functionality
    WCreateColorArraysThread* t1 = new WCreateColorArraysThread( 0, m_lineLengths->size()/4, m_vertices,
            m_lineStartIndexes, m_lineLengths, globalColors, localColors, m_tangents );
    WCreateColorArraysThread* t2 = new WCreateColorArraysThread( m_lineLengths->size()/4+1, m_lineLengths->size()/2, m_vertices,
            m_lineStartIndexes, m_lineLengths, globalColors, localColors, m_tangents );
    WCreateColorArraysThread* t3 = new WCreateColorArraysThread( m_lineLengths->size()/2+1, m_lineLengths->size()/4*3, m_vertices,
            m_lineStartIndexes, m_lineLengths, globalColors, localColors, m_tangents );
    WCreateColorArraysThread* t4 = new WCreateColorArraysThread( m_lineLengths->size()/4*3+1, m_lineLengths->size()-1, m_vertices,
            m_lineStartIndexes, m_lineLengths, globalColors, localColors, m_tangents );
    t1->run();
    t2->run();
    t3->run();
    t4->run();

    t1->wait();
    t2->wait();
    t3->wait();
    t4->wait();

    delete t1;
    delete t2;
    delete t3;
    delete t4;

    // add both arrays to m_colors
    m_colors.push_back( globalColors );
    m_colors.push_back( localColors );

    // the colors can be selected by properties
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

WDataSetFibers::VertexArray WDataSetFibers::getVertices() const
{
    return m_vertices;
}

WDataSetFibers::IndexArray WDataSetFibers::getLineStartIndexes() const
{
    return m_lineStartIndexes;
}

WDataSetFibers::LengthArray WDataSetFibers::getLineLengths() const
{
    return m_lineLengths;
}

WDataSetFibers::IndexArray WDataSetFibers::getVerticesReverse() const
{
    return m_verticesReverse;
}

WDataSetFibers::TangentArray WDataSetFibers::getTangents() const
{
    return m_tangents;
}

WDataSetFibers::ColorArray WDataSetFibers::getGlobalColors() const
{
    return m_colors[0];
}

WDataSetFibers::ColorArray WDataSetFibers::getLocalColors() const
{
    return m_colors[1];
}

void WDataSetFibers::addColorScheme( WDataSetFibers::ColorArray colors, std::string name, std::string description )
{
    m_colors.push_back( colors );
}

void WDataSetFibers::removeColorScheme( WDataSetFibers::ColorArray colors )
{
    m_colors.erase( colors );
    //addColorScheme( colors, std::string name, std::string description );
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

void WDataSetFibers::saveSelected( std::string filename, boost::shared_ptr< std::vector< bool > > active ) const
{
    std::vector< float > pointsToSave;
    std::vector< int > linesToSave;
    std::vector< unsigned char > colorsToSave;

    int pointIndex = 0;
    int countLines = 0;

    for ( size_t l = 0; l < m_lineLengths->size(); ++l )
    {
        if ( ( *active )[l] )
        {
            unsigned int pc = ( *m_lineStartIndexes )[l] * 3;

            linesToSave.push_back( ( *m_lineLengths )[l] );

            boost::shared_ptr< std::vector< float > > localColors = m_colors[1];

            for ( size_t j = 0; j < ( *m_lineLengths )[l]; ++j )
            {
                // TODO(schurade): replace this with a permanent solution
                pointsToSave.push_back( 160 - ( *m_vertices )[pc] );
                colorsToSave.push_back( static_cast<unsigned char> ( ( *localColors )[pc] * 255 ) );
                ++pc;
                pointsToSave.push_back( 200 - ( *m_vertices )[pc] );
                colorsToSave.push_back( static_cast<unsigned char> ( ( *localColors )[pc] * 255 ) );
                ++pc;
                pointsToSave.push_back( ( *m_vertices )[pc] );
                colorsToSave.push_back( static_cast<unsigned char> ( ( *localColors )[pc] * 255 ) );
                ++pc;
                linesToSave.push_back( pointIndex );
                ++pointIndex;
            }
            ++countLines;
        }
    }

    converterByteINT32 c;
    converterByteFloat f;

    std::vector< char > vBuffer;

    std::string header1 = "# vtk DataFile Version 3.0\nvtk output\nBINARY\nDATASET POLYDATA\nPOINTS ";
    header1 += boost::lexical_cast<std::string>( pointsToSave.size() / 3 );
    header1 += " float\n";

    for ( unsigned int i = 0; i < header1.size(); ++i )
    {
        vBuffer.push_back( header1[i] );
    }

    for ( unsigned int i = 0; i < pointsToSave.size(); ++i )
    {
        f.f = pointsToSave[i];
        vBuffer.push_back( f.b[3] );
        vBuffer.push_back( f.b[2] );
        vBuffer.push_back( f.b[1] );
        vBuffer.push_back( f.b[0] );
    }

    vBuffer.push_back( '\n' );

    std::string header2 = "LINES " + boost::lexical_cast<std::string>( countLines ) + " " +
            boost::lexical_cast<std::string>( linesToSave.size() ) + "\n";

    for ( unsigned int i = 0; i < header2.size(); ++i )
    {
        vBuffer.push_back( header2[i] );
    }

    for ( unsigned int i = 0; i < linesToSave.size(); ++i )
    {
        c.i = linesToSave[i];
        vBuffer.push_back( c.b[3] );
        vBuffer.push_back( c.b[2] );
        vBuffer.push_back( c.b[1] );
        vBuffer.push_back( c.b[0] );
    }

    vBuffer.push_back( '\n' );

    std::string header3 = "POINT_DATA ";

    header3 += boost::lexical_cast<std::string>( pointsToSave.size() / 3 );
    header3 += " float\n";
    header3 += "COLOR_SCALARS scalars 3\n";


    for ( unsigned int i = 0; i < header3.size(); ++i )
    {
        vBuffer.push_back( header3[i] );
    }

    for ( unsigned int i = 0; i < colorsToSave.size(); ++i )
    {
        vBuffer.push_back( colorsToSave[i] );
    }

    vBuffer.push_back( '\n' );

    boost::filesystem::path p( filename );
    boost::filesystem::ofstream ofs( p );

    for ( unsigned int i = 0; i < vBuffer.size(); ++i )
    {
        ofs << vBuffer[i];
    }
}

std::pair< wmath::WPosition, wmath::WPosition > WDataSetFibers::getBoundingBox() const
{
    return std::make_pair( m_bbMin, m_bbMax );
}
