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

#include <algorithm>
#include <bitset>
#include <list>

#include <boost/unordered_map.hpp>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "WBoundaryLines.h"

WBoundaryLines::WBoundaryLines( boost::shared_ptr< const WDataSetScalar > texture, boost::shared_ptr< const WProperties > properties,
    boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > *slices, boost::filesystem::path localPath )
    : WBoundaryBuilder( texture, properties, slices )
{
    m_grid = boost::shared_dynamic_cast< WGridRegular3D >( m_texture->getGrid() );
    m_resolution = properties->findProperty( "Resolution" )->toPropDouble();

    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMBoundaryCurvesWMGM-IsoLines", localPath ) );

    WAssert( m_grid, "Bug: each DataSetScalar should have a regular grid 3d!!!" );
}

void WBoundaryLines::run( osg::ref_ptr< WGEManagedGroupNode > output, const char sliceNum )
{
    computeSliceBB();

    if( sliceNum == -1 )
    {
        for( char i = 0; i < 3; ++i )
        {
            output->remove( m_slices[i] );
            osg::ref_ptr< WGEGroupNode > newNode = generateSlice( i );
            m_slices[i]->clear();
            m_slices[i]->insert( newNode );
            m_shader->apply( m_slices[i] );
            output->insert( m_slices[i] );
        }
    }
    else
    {
        output->remove( m_slices[ sliceNum ] );
        osg::ref_ptr< WGEGroupNode > newNode = generateSlice( sliceNum );
        m_slices[ sliceNum ]->clear();
        m_slices[ sliceNum ]->insert( newNode );
        output->insert( m_slices[ sliceNum ] );
    }
}

boost::shared_ptr< WGridRegular3D > WBoundaryLines::generateSliceGrid( const unsigned char sliceNum, const double resolution ) const
{
    // compute indices of the other dimensions
    boost::array< std::pair< size_t, size_t >, 3 > otherDims = { { std::make_pair( 1, 2 ), std::make_pair( 0, 2 ), std::make_pair( 0, 1 ) } }; // NOLINT curly braces
    size_t x = otherDims[sliceNum].first;
    size_t y = otherDims[sliceNum].second;

    boost::array< WVector3d, 3 > directions = getDirections( m_grid );
    boost::array< unsigned int, 3 > numCoords = getNbCoords( m_grid );
    boost::array< double, 3 > offsets = getOffsets( m_grid );

    double delta = 1e-3; // since the positions at the boundaries of the result must lie withing m_grid in order to get the interpolation working.
    size_t nbPosX = static_cast< size_t >( ( ( numCoords[x] - 1 ) * offsets[x] - delta ) / resolution );
    size_t nbPosY = static_cast< size_t >( ( ( numCoords[y] - 1 ) * offsets[y] - delta ) / resolution );

    double slicePos = m_slicePos[ sliceNum ]->get();
    if( m_slicePos[ sliceNum ]->get() == numCoords[ sliceNum ] - 1 )
    {
        slicePos -= delta;
    }

    WPosition origin = m_grid->getOrigin() + directions[ sliceNum ] * slicePos;
    WVector3d xDir = normalize( directions[x] ) * resolution;
    WVector3d yDir = normalize( directions[y] ) * resolution;

    WMatrix< double > mat( 4, 4 );
    mat.makeIdentity();
    mat( 0, 0 ) = xDir[0];
    mat( 1, 0 ) = xDir[1];
    mat( 2, 0 ) = xDir[2];
    mat( 0, 1 ) = yDir[0];
    mat( 1, 1 ) = yDir[1];
    mat( 2, 1 ) = yDir[2];
    // set zdirection to an orthogonal vector
    WVector3d zDir = cross( xDir, yDir );
    mat( 0, 2 ) = zDir[0];
    mat( 1, 2 ) = zDir[1];
    mat( 2, 2 ) = zDir[2];
    mat( 0, 3 ) = origin[0];
    mat( 1, 3 ) = origin[1];
    mat( 2, 3 ) = origin[2];

    WGridTransformOrtho v( mat );
    boost::shared_ptr< WGridRegular3D > result( new WGridRegular3D( nbPosX, nbPosY, 1, v ) );

    return result;
}

std::list< size_t > WBoundaryLines::extractLineStripEdges( WBoundaryLines::EdgeNeighbourMap *l ) const
{
    std::list< size_t > result;
    result.push_back( l->begin()->first );
    int left = l->begin()->second.first;
    int right = l->begin()->second.second;
    l->erase( result.front() );

    // try first direction as long as possible
    while( l->find( right ) != l->end() )
    {
        size_t pred = result.back();
        result.push_back( right );
        int newRight = ( *l )[right].second;
        if( newRight == static_cast< int >( pred ) )
        {
            newRight = ( *l )[right].first;
        }
        l->erase( right );
        right = newRight;
    }

    // check if the curve is closed => if so => finished, otherwise check other direction
    if( right == -1 )
    {
        while( l->find( left ) != l->end() )
        {
            size_t pred = result.front();
            result.push_front( left );
            int newLeft = ( *l )[left].second;
            if( newLeft == static_cast< int >( pred ) )
            {
                newLeft = ( *l )[left].first;
            }
            l->erase( left );
            left = newLeft;
        }
    }
    else
    {
        result.push_back( result.front() );
    }
    return result;
}

osg::ref_ptr< WGEGroupNode > WBoundaryLines::generateSlice( const unsigned char sliceNum ) const
{
    // Attentions this grid3D is used as WGridRegular2DIn3D, meaning its z-dimenion is set to one
    boost::shared_ptr< WGridRegular3D > sliceGrid = generateSliceGrid( sliceNum, m_resolution->get() );

    std::size_t numCellsX = sliceGrid->getNbCoordsX() - 1;
    std::size_t numCellsY = sliceGrid->getNbCoordsY() - 1;
    std::size_t numCells = numCellsX * numCellsY;
    std::size_t numEdges = 2 * numCells + numCellsX + numCellsY;

    std::vector< double > interpolates( sliceGrid->size(), 0.0 );
    bool success = false;
    for( size_t pointID = 0; pointID < sliceGrid->size(); ++pointID )
    {
        interpolates[pointID] = m_texture->interpolate( sliceGrid->getPosition( pointID ), &success );
        // if( !success )
        // {
        //     std::stringstream ss;
        //     ss << "posid: " << pointID << ", " <<  sliceGrid->getPosition( pointID ) << sliceGrid->getTransformationMatrix();
        //     WAssert( success, "Bug: interpolation failed.\n" + ss.str() );
        // }
    }

    // relative edge numbering
    //
    //    __2__
    //   |     |
    // 3 |     | 1
    //   |_____|
    //      0
    boost::array< SegmentVector, 16 > caseLookUp;
    // caseLookUp[0] no segments, all are below threshold
    caseLookUp[1].push_back( std::make_pair( 3, 2 ) );
    caseLookUp[2].push_back( std::make_pair( 2, 1 ) );
    caseLookUp[3].push_back( std::make_pair( 3, 1 ) );
    caseLookUp[4].push_back( std::make_pair( 0, 1 ) );
    caseLookUp[5].push_back( std::make_pair( 3, 2 ) );
    caseLookUp[5].push_back( std::make_pair( 0, 1 ) );
    caseLookUp[6].push_back( std::make_pair( 0, 2 ) );
    caseLookUp[7].push_back( std::make_pair( 3, 0 ) );

    boost::array< SegmentList, 2 > segments;
    std::vector< Edge > edgeIDToPointIDs( numEdges );
    boost::array< double, 2 > isoValues = { { m_grayMatter->get() * m_texture->getMax(), m_whiteMatter->get() * m_texture->getMax() } }; // NOLINT curly braces
    boost::array< bool, 4 > b;
    boost::array< size_t, 4 > edgeIDs;
    for( size_t cellID = 0; cellID < numCells; ++cellID )
    {
        size_t cellRow = cellID / numCellsX;
        size_t cellCol = cellID % numCellsX;

        edgeIDs[0] = cellID;                                                    // absolute edge numbering:
        edgeIDs[2] = cellID + numCellsX;                                        //
        edgeIDs[3] = ( numCells + numCellsX ) + cellRow + numCellsY * cellCol;  //  +--6--+--7--+
        edgeIDs[1] = edgeIDs[3] + numCellsY;                                    //  |     |     |
                                                                                //  10    13    16
                                                                                //  |     |     |
                                                                                //  +--4--+--5--+
                                                                                //  |     |     |
                                                                                //  9     12    15
                                                                                //  |     |     |
                                                                                //  +--2--+--3--+
                                                                                //  |     |     |
                                                                                //  8     11    14
                                                                                //  |     |     |
                                                                                //  +--0--+--1--+


        std::vector< size_t > vertexIDs = sliceGrid->getCellVertexIds( cellID ); // only the first four are of interest since z-axis is set to zero

        // swap indices to make the numbering clockwise
        //
        //  y-axis
        //  |
        // 3|_____2
        //  |     |
        //  |     |
        //  |_____|____x-axis
        // 0      1
        std::swap( vertexIDs[2], vertexIDs[3] );

        edgeIDToPointIDs[ edgeIDs[0] ] = std::make_pair( vertexIDs[0], vertexIDs[1] );
        edgeIDToPointIDs[ edgeIDs[1] ] = std::make_pair( vertexIDs[1], vertexIDs[2] );
        edgeIDToPointIDs[ edgeIDs[2] ] = std::make_pair( vertexIDs[2], vertexIDs[3] );
        edgeIDToPointIDs[ edgeIDs[3] ] = std::make_pair( vertexIDs[3], vertexIDs[0] );

        std::bitset<4> c;
        for( int isoIdx = 0; isoIdx < 2; ++isoIdx )
        {
            for( int corner = 0; corner < 4; ++corner )
            {
                b[corner] = ( interpolates[ vertexIDs[corner] ] > isoValues[ isoIdx ] );
                c[3 - corner] = ( b[corner] ? 1 : 0 );
            }
            size_t caseIdx = c.to_ulong();

            // reduce all cases to eight cases
            if( caseIdx > 7 )
            {
                caseIdx = c.flip().to_ulong();
            }

            for( SegmentVector::const_iterator cit = caseLookUp[ caseIdx ].begin(); cit != caseLookUp[ caseIdx ].end(); ++cit )
            {
                // transform relative segment into absolute segment and append
                segments[isoIdx].push_back( std::make_pair( edgeIDs[ cit->first ], edgeIDs[ cit->second ] ) );
            }
        }
    }

    boost::array< boost::shared_ptr< EdgeNeighbourMap >, 2 > edgeLineStrips;
    edgeLineStrips[0] = generateEdgeNeighbourMap( segments[0] );
    edgeLineStrips[1] = generateEdgeNeighbourMap( segments[1] );

    osg::ref_ptr< WGEGroupNode > result( new WGEGroupNode );
    osg::ref_ptr< osg::Geode > geode( new osg::Geode );
    geode->addDrawable( traverseEdgeHashMap( isoValues[0], edgeLineStrips[0], edgeIDToPointIDs, interpolates, sliceGrid ) );
    geode->addDrawable( traverseEdgeHashMap( isoValues[1], edgeLineStrips[1], edgeIDToPointIDs, interpolates, sliceGrid ) );
    result->insert( geode );
    // result->insert( wge::generateBoundingBoxGeode( m_sliceBB[ sliceNum ], WColor( 0.0, 0.0, 0.0, 1.0 ) ) );

    osg::ref_ptr< osg::Uniform > u_gmColor = new WGEPropertyUniform< WPropColor >( "u_gwColor", m_gmColor );
    osg::ref_ptr< osg::Uniform > u_wmColor = new WGEPropertyUniform< WPropColor >( "u_gwColor", m_wmColor );

    osg::StateSet *ss_gm = geode->getDrawable( 0 )->getOrCreateStateSet();
    ss_gm->addUniform( u_gmColor );
    osg::StateSet *ss_wm = geode->getDrawable( 1 )->getOrCreateStateSet();
    ss_wm->addUniform( u_wmColor );

    m_shader->apply( geode );

    return result;
}

boost::shared_ptr< WBoundaryLines::EdgeNeighbourMap > WBoundaryLines::generateEdgeNeighbourMap( const SegmentList& segments ) const
{
    boost::shared_ptr< WBoundaryLines::EdgeNeighbourMap > map( new WBoundaryLines::EdgeNeighbourMap );

    for( SegmentList::const_iterator cit = segments.begin(); cit != segments.end(); ++cit )
    {
        if( map->find( cit->first ) == map->end() ) // first edge not found
        {
            ( *map )[ cit->first ] = std::make_pair( cit->second, -1 );
        }
        else
        {
            ( *map )[ cit->first ].second = cit->second;
        }
        if( map->find( cit->second ) == map->end() ) // second edge not found
        {
            ( *map )[ cit->second ] = std::make_pair( cit->first, -1 );
        }
        else
        {
            ( *map )[ cit->second ].second = cit->first;
        }
    }

    return map;
}

osg::ref_ptr< osg::Geometry > WBoundaryLines::traverseEdgeHashMap( double isoValue, boost::shared_ptr< WBoundaryLines::EdgeNeighbourMap > map,
        const std::vector< Edge >& edgeIDToPointIDs, const std::vector< double > &interpolates, boost::shared_ptr< const WGridRegular3D > sliceGrid ) const // NOLINT line length
{
    std::list< std::list< size_t > > eLS;
    osg::ref_ptr< osg::Vec3Array > vertices( new osg::Vec3Array );
    osg::ref_ptr< osg::Geometry > geometry( new osg::Geometry() );
    osg::ref_ptr< osg::Vec4Array > colors( new osg::Vec4Array );
    colors->push_back( WColor( 0.0, 0.0, 0.0, 1.0 ) ); // Note: coloring takes place in the shader!

    while( !map->empty() )
    {
        eLS.push_back( extractLineStripEdges( map.get() ) );
    }

    for( std::list< std::list< size_t > >::const_iterator cit = eLS.begin(); cit != eLS.end(); ++cit )
    {
        size_t start = vertices->size();
        for( std::list< size_t >::const_iterator edge = cit->begin(); edge != cit->end(); ++edge )
        {
            size_t p0 = edgeIDToPointIDs[ *edge ].first;
            size_t p1 = edgeIDToPointIDs[ *edge ].second;
            // WAssert( p0 < sliceGrid->size() && p1 < sliceGrid->size(), "Bug: fucking points out of fucking range" );
            double w0 = interpolates[ p0 ];
            double w1 = interpolates[ p1 ];
            double nu = std::abs( w0 - isoValue ) / std::abs( w0 - w1 );
            vertices->push_back( sliceGrid->getPosition( p0 ) + (  sliceGrid->getPosition( p1 ) -  sliceGrid->getPosition( p0 ) ) * nu );
        }
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, start, vertices->size() - start ) );
    }

    geometry->setVertexArray( vertices );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    geometry->getOrCreateStateSet()->setAttributeAndModes( new osg::LineWidth( 2.5f ), osg::StateAttribute::ON );
    geometry->getOrCreateStateSet()->setMode( GL_LINE_SMOOTH, osg::StateAttribute::ON );
    geometry->setColorArray( colors );

    return geometry;
}

void WBoundaryLines::computeSliceBB()
{
    m_sliceBB[0] = WBoundingBox( m_grid->getOrigin() + m_slicePos[0]->get() * m_grid->getDirectionX(),
            m_grid->getOrigin() + m_slicePos[0]->get() * m_grid->getDirectionX() + m_grid->getNbCoordsY() * m_grid->getDirectionY() +
            m_grid->getNbCoordsZ() * m_grid->getDirectionZ() );
    m_sliceBB[1] = WBoundingBox( m_grid->getOrigin() + m_slicePos[1]->get() * m_grid->getDirectionY(),
            m_grid->getOrigin() + m_slicePos[1]->get() * m_grid->getDirectionY() + m_grid->getNbCoordsX() * m_grid->getDirectionX() +
            m_grid->getNbCoordsZ() * m_grid->getDirectionZ() );
    m_sliceBB[2] = WBoundingBox( m_grid->getOrigin() + m_slicePos[2]->get() * m_grid->getDirectionZ(),
            m_grid->getOrigin() + m_slicePos[2]->get() * m_grid->getDirectionZ() + m_grid->getNbCoordsY() * m_grid->getDirectionY() +
            m_grid->getNbCoordsX() * m_grid->getDirectionX() );
}
