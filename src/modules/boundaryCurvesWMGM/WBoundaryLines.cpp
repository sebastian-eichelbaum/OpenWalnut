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

#include <bitset>
#include <boost/unordered_map.hpp>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>

#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "WBoundaryLines.h"
#include "../../common/WLogger.h"

WBoundaryLines::WBoundaryLines( boost::shared_ptr< const WDataSetScalar > texture, boost::shared_ptr< const WProperties > properties,
    boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > *slices )
    : WBoundaryBuilder( texture, properties, slices )
{
    m_grid = boost::shared_dynamic_cast< WGridRegular3D >( m_texture->getGrid() );
    m_resolution = properties->findProperty( "Resolution" )->toPropDouble();

    WAssert( m_grid, "Bug: each DataSetScalar should have a regular grid 3d!!!" );
}

void WBoundaryLines::run( osg::ref_ptr< WGEManagedGroupNode > output, const char sliceNum )
{
    computeSliceBB();

    if( sliceNum == -1 )
    {
        for( char i = 0; i < 3; ++i )
        {
            osg::ref_ptr< WGEGroupNode > newNode = generateSlice( i );
            output->remove( m_slices[i] );
            m_slices[i]->clear();
            m_slices[i]->insert( newNode );
            output->insert( m_slices[i] );
        }
    }
    else
    {
        osg::ref_ptr< WGEGroupNode > newNode = generateSlice( sliceNum );
        output->remove( m_slices[ sliceNum ] );
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

    boost::array< WVector3D, 3 > directions = getDirections( m_grid );
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
    WVector3D xDir = directions[x].normalized() * resolution;
    WVector3D yDir = directions[y].normalized() * resolution;

    WMatrix< double > mat( 4, 4 );
    mat.makeIdentity();
    mat( 0, 0 ) = xDir[0];
    mat( 1, 0 ) = xDir[1];
    mat( 2, 0 ) = xDir[2];
    mat( 0, 1 ) = yDir[0];
    mat( 1, 1 ) = yDir[1];
    mat( 2, 1 ) = yDir[2];
    // set zdirection to an orthogonal vector
    WVector3D zDir = xDir ^ yDir;
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

// typedef std::vector< std::pair< bool, double > > InterpolateCache;
//
// class Edge
// {
// public:
//     Edge( size_t edgeIdx )
//         : m_edgeIdx( edgeIdx )
//     {
//         WAssert( m_initialized, "Bug: Edge object needs initialization first, please use Edge::init(...) before creating objects." );
//         m_hit[0] = false;
//         m_hit[1] = false;
//         m_lineStripID[0] = -1;
//         m_lineStripID[1] = -1;
//         computePointIDs( edgeIdx );
//         interpolateValuesAtPoints();
//         checkIsoValues();
//     }
//
//     static void init( double gmIsoValue, double wmIsoValue, boost::shared_ptr< const WGridRegular3D > sliceGrid,
//             boost::shared_ptr< const WDataSetScalar > ds, boost::shared_ptr< InterpolateCache > interpolates )
//     {
//         m_isoValues[0] = gmIsoValue;
//         m_isoValues[1] = wmIsoValue;
//         m_sliceGrid = sliceGrid;
//         m_ds = ds;
//         m_interpolates = interpolates;
//         m_initialized = true;
//     }
//
//     bool hitsIsoValue( size_t i ) const
//     {
//         WAssert( i < m_hit.size(), "Bug: invalid isovalue selected for test." );
//         return m_hit[i];
//     }
//
//     bool isNotInLineStrip( size_t i ) const
//     {
//         if( m_lineStripID[i] == -1 )
//         {
//             return true;
//         }
//         return false;
//     }
//
//     WPosition getIntersectingPosition() const
//     {
//         WPosition result;
//         return result;
//     }
//
//     void setLineStripID( std::size_t isoIdx, std::size_t lineStripID )
//     {
//         m_lineStripID[ isoIdx ] = lineStripID;
//     }
//
//     size_t getLineStripID( std::size_t isoIdx )
//     {
//         return m_lineStripID[ isoIdx ];
//     }
//
// private:
//     void checkIsoValues()
//     {
//         double w0 = ( *m_interpolates )[ m_pointIDs[0] ].second;
//         double w1 = ( *m_interpolates )[ m_pointIDs[1] ].second;
//
//         for( int isoIdx = 0; isoIdx < 2; ++isoIdx )
//         {
//             bool b0 = ( w0 > m_isoValues[isoIdx] );
//             bool b1 = ( w1 > m_isoValues[isoIdx] );
//             WAssert( ( *m_interpolates )[ m_pointIDs[0] ].first && ( *m_interpolates )[ m_pointIDs[1] ].first, "Bug: the interpolation did not succeed for some edge points!" );
//             m_hit[isoIdx] = ( b0 != b1 );
//         }
//     }
//
//     void computePointIDs( size_t edgeIdx )
//     {
//         // each cell-row has 2 * numCoordsX() - 1 edges
//         size_t numEdgesPerCellRow =  2 * m_sliceGrid->getNbCoordsX() - 1;
//
//         // which cell row we are in?
//         size_t cellRow = edgeIdx / numEdgesPerCellRow;
//         size_t cellRowRemainder = edgeIdx % numEdgesPerCellRow;
//         if( cellRowRemainder > ( m_sliceGrid->getNbCoordsX() - 2 ) ) // vertical edge
//         {
//             WAssert( cellRow < m_sliceGrid->getNbCoordsY(), "Bug: The cell row is out of grid!" );
//             m_pointIDs[0] = cellRowRemainder - ( m_sliceGrid->getNbCoordsX() - 1 ) + cellRow * m_sliceGrid->getNbCoordsX();
//             m_pointIDs[1] = m_pointIDs[0] + m_sliceGrid->getNbCoordsX();
//         }
//         else // horizontal edge
//         {
//             m_pointIDs[0] = cellRowRemainder + cellRow * m_sliceGrid->getNbCoordsX();
//             m_pointIDs[1] = m_pointIDs[0] + 1;
//         }
//         WAssert( m_pointIDs[0] < m_pointIDs[1], "Bug: point IDs not sorted" );
//         WAssert( m_pointIDs[1] < m_sliceGrid->size(), "Bug: point ID not inside grid" );
//     }
//
//     void interpolateValuesAtPoints()
//     {
//         bool success = false;
//         for( size_t i = 0; i < m_pointIDs.size(); ++i )
//         {
//             if( !( ( *m_interpolates )[ m_pointIDs[i] ].first ) ) // no cache hit
//             {
//                 ( *m_interpolates )[ m_pointIDs[i] ] = std::make_pair( true, m_ds->interpolate(  m_sliceGrid->getPosition( m_pointIDs[i] ), &success ) );
//                 if( !success )
//                 {
//                     std::stringstream ss;
//                     ss << "Bug: interpolation of one point of an edge: " << m_pointIDs[i] << " failed.";
//                     WAssert( success, ss.str() );
//                 }
//             }
//             // else, for this point the interpolation has already been done
//         }
//     }
//
//     std::size_t m_edgeIdx;
//     boost::array< bool, 2 > m_hit;
//     boost::array< int, 2 > m_lineStripID;
//     boost::array< size_t, 2 > m_pointIDs;
//     static boost::shared_ptr< InterpolateCache > m_interpolates;
//     static boost::shared_ptr< const WGridRegular3D > m_sliceGrid;
//     static boost::shared_ptr< const WDataSetScalar > m_ds;
//     /**
//      * Idx 0: Gray Matter iso Value and Idx 1: White Matter iso Value.
//      */
//     static boost::array< double, 2 > m_isoValues;
//     static bool m_initialized;
// };
//
// bool Edge::m_initialized = false;
// boost::shared_ptr< InterpolateCache > Edge::m_interpolates;
// boost::shared_ptr< const WGridRegular3D > Edge::m_sliceGrid;
// boost::shared_ptr< const WDataSetScalar > Edge::m_ds;
// boost::array< double, 2 > Edge::m_isoValues;

std::list< size_t > WBoundaryLines::extractLS( boost::unordered_map< size_t, std::pair< int , int > > *l ) const
{
    std::list< size_t > result;
    WAssert( !l->empty(), "Bug: map empty." );
    result.push_back( l->begin()->first );
    int left = l->begin()->second.first;
    int right = l->begin()->second.second;
    l->erase( result.front() );

    // solange langhangeln geht => hangel dich rechts lang
    while( l->find( right ) != l->end() )
    {
        size_t pred = result.back();
        result.push_back( right );
        int newRight = ( *l )[right].second;
        if( newRight == pred )
        {
            newRight = ( *l )[right].first;
        }
        l->erase( right );
        right = newRight;
    }
    WAssert( !result.empty(), "Bug: empty linestrip" );
    // check if the curve is closed => if so => finished, otherwise check other direction
    if( right == -1 )
    {
        // solange langhangeln geht => hangel dich rechts lang
        while( l->find( left ) != l->end() )
        {
            size_t pred = result.front();
            result.push_front( left );
            int newLeft = ( *l )[left].second;
            if( newLeft == pred )
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

    osg::ref_ptr< osg::Vec3Array > gmvertices( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec3Array > wmvertices( new osg::Vec3Array );


    std::size_t numCellsX = sliceGrid->getNbCoordsX() - 1;
    std::size_t numCellsY = sliceGrid->getNbCoordsY() - 1;
    std::size_t numCells = numCellsX * numCellsY;
    std::size_t numEdges = 2 * numCells + numCellsX + numCellsY;

    std::vector< double > interpolates( sliceGrid->size(), 0.0 );
    bool success = false;
    for( size_t pointID = 0; pointID < sliceGrid->size(); ++pointID )
    {
        interpolates[pointID] = m_texture->interpolate( sliceGrid->getPosition( pointID ), &success );
        if( !success )
        {
            std::stringstream ss;
            ss << "posid: " << pointID << ", " <<  sliceGrid->getPosition( pointID ) << sliceGrid->getTransformationMatrix();
            WAssert( success, "Bug: interpolation failed.\n" + ss.str() );
        }
    }

    typedef std::pair< std::size_t, std::size_t > Edge;
    typedef std::vector< Edge > SegmentVector;
    typedef std::list< Edge > SegmentList;

    // relative edge numbering
    //
    //    __2__
    //   |     |
    // 3 |     | 1
    //   |_____|
    //      0
    boost::array< SegmentVector, 16 > caseLookUp;
    // caseLookUp[0] no segments, all are below
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
    wlog::debug( "WBoundaryLines" ) << isoValues[0] << " " << isoValues[1];
    boost::array< bool, 4 > b;
    boost::array< size_t, 4 > edgeIDs;
    for( size_t cellID = 0; cellID < numCells; ++cellID )
    {
        size_t cellRow = cellID / numCellsX;
        size_t cellCol = cellID % numCellsX;

        // absolute edge numbering:
        //
        //  +--6--+--7--+
        //  |     |     |
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
        edgeIDs[0] = cellID;
        edgeIDs[2] = cellID + numCellsX;
        edgeIDs[3] = ( numCells + numCellsX ) + cellRow + numCellsY * cellCol;
        edgeIDs[1] = edgeIDs[3] + numCellsY;

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

//                size_t p0 = edgeIDToPointIDs[ *edge ].first;
//                size_t p1 = edgeIDToPointIDs[ *edge ].second;
////                WAssert( p0 < sliceGrid->size() && p1 < sliceGrid->size(), "Bug: fucking points out of fucking range" );
//                double w0 = interpolates[ p0 ];
//                double w1 = interpolates[ p1 ];
//                double nu = std::abs( w0 - isoValues[isoIdx] ) / std::abs( w0 - w1 );
//                gmvertices->push_back( sliceGrid->getPosition( p0 ) + (  sliceGrid->getPosition( p1 ) -  sliceGrid->getPosition( p0 ) ) * nu );
            }
        }
    }

    boost::array< boost::unordered_map< size_t, std::pair< int , int > >, 2 > edgeLineStrips;
    osg::ref_ptr< osg::Geometry > gmgeometry = new osg::Geometry();

    for( int isoIdx = 0; isoIdx < 2; ++isoIdx )
    {
        for( SegmentList::const_iterator cit = segments[isoIdx].begin(); cit != segments[isoIdx].end(); ++cit )
        {
            if( edgeLineStrips[isoIdx].find( cit->first ) == edgeLineStrips[isoIdx].end() ) // first edge not found
            {
                edgeLineStrips[isoIdx][ cit->first ] = std::make_pair( cit->second, -1 );
            }
            else
            {
                edgeLineStrips[isoIdx][ cit->first ].second = cit->second;
            }
            if( edgeLineStrips[isoIdx].find( cit->second ) == edgeLineStrips[isoIdx].end() ) // second edge not found
            {
                edgeLineStrips[isoIdx][ cit->second ] = std::make_pair( cit->first, -1 );
            }
            else
            {
                edgeLineStrips[isoIdx][ cit->second ].second = cit->first;
            }
        }
    }

    boost::array< std::list< std::list< size_t > >, 2 > eLS;

    for( int isoIdx = 0; isoIdx < 2; ++isoIdx )
    {
        while( !edgeLineStrips[isoIdx].empty() )
        {
            eLS[isoIdx].push_back( extractLS( &edgeLineStrips[isoIdx] ) );
        }

        for( std::list< std::list< size_t > >::const_iterator cit = eLS[isoIdx].begin(); cit != eLS[isoIdx].end(); ++cit )
        {
            size_t start = gmvertices->size();
            for( std::list< size_t >::const_iterator edge = cit->begin(); edge != cit->end(); ++edge )
            {
                size_t p0 = edgeIDToPointIDs[ *edge ].first;
                size_t p1 = edgeIDToPointIDs[ *edge ].second;
                WAssert( p0 < sliceGrid->size() && p1 < sliceGrid->size(), "Bug: fucking points out of fucking range" );
                double w0 = interpolates[ p0 ];
                double w1 = interpolates[ p1 ];
                double nu = std::abs( w0 - isoValues[isoIdx] ) / std::abs( w0 - w1 );
                gmvertices->push_back( sliceGrid->getPosition( p0 ) + (  sliceGrid->getPosition( p1 ) -  sliceGrid->getPosition( p0 ) ) * nu );
            }
            gmgeometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, start, gmvertices->size() - start ) );
        }
    }

    // transform linestrip-edge-arrays into real osg linestrips

//    osg::ref_ptr< osg::Vec3Array > gm = checkQuad( corners, gmIsoValue, &cache );
//    osg::ref_ptr< osg::Vec3Array > wm = checkQuad( corners, wmIsoValue, &cache );
//    gmvertices->insert( gmvertices->end(), gm->begin(), gm->end() );
//    wmvertices->insert( wmvertices->end(), wm->begin(), wm->end() );

    gmgeometry->setVertexArray( gmvertices );
    osg::ref_ptr< osg::Vec4Array > gmcolors( new osg::Vec4Array );
    gmcolors->push_back( WColor( 0.0, 0.0, 0.0, 1.0 ) );
    gmgeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    gmgeometry->setColorArray( gmcolors );
//    gmgeometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, gmvertices->size() ) );
    gmgeometry->getOrCreateStateSet()->setAttributeAndModes( new osg::LineWidth( 4.0f ), osg::StateAttribute::ON );

    osg::ref_ptr< osg::Geometry > wmgeometry = new osg::Geometry();
    wmgeometry->setVertexArray( wmvertices );
    osg::ref_ptr< osg::Vec4Array > wmcolors( new osg::Vec4Array );
    wmcolors->push_back( WColor( 0.5, 0.5, 0.5, 1.0 ) );
    wmgeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    wmgeometry->setColorArray( wmcolors );
    wmgeometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, wmvertices->size() ) );
    wmgeometry->getOrCreateStateSet()->setAttributeAndModes( new osg::LineWidth( 3.0f ), osg::StateAttribute::ON );

    osg::ref_ptr< WGEGroupNode > result( new WGEGroupNode );
    osg::ref_ptr< osg::Geode > geode( new osg::Geode );
    geode->addDrawable( gmgeometry );
    geode->addDrawable( wmgeometry );
    result->insert( geode );
    result->insert( wge::generateBoundingBoxGeode( m_sliceBB[ sliceNum ], WColor( 0.0, 0.0, 0.0, 1.0 ) ) );

//    osg::StateSet* state = geode->getOrCreateStateSet();
//    state->setMode( GL_BLEND, osg::StateAttribute::ON );
//    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    return result;
}

//WCell WBoundaryLines::generateCell( size_t cellXCoord, size_t cellYCoord, const WPosition& origin, const WVector3D& a, const WVector3D& b, const double spacing )
//{
//    boost::array< WPosition, 3 > corners;
//    corners[0] =  ( *origin ) + cellXCoord * spacing * ( *a ) + cellYCoord * spacing * ( *b );
//    corners[1] =  corners[0] + spacing * ( *a );
//    corners[2] =  corners[1] + spacing * ( *b );
//    corners[3] =  corners[0] + spacing * ( *a );
//
//    return WCell( corners );
//}

//osg::ref_ptr< osg::Vec3Array > WBoundaryLines::checkQuad( const boost::array< WPosition, 4 >& corners, const double isoValue ) const
//{
//    osg::ref_ptr< osg::Vec3Array > result( new osg::Vec3Array );
//
//    // check if we can skip, otherwise the computation was necessary anyway
//    boost::array< double, 4 > values;
//    bool success;
//    for( char i = 0; i < 4; ++i )
//    {
//        values[i] =  m_texture->interpolate( corners[i], &success );
//    }
//    if( ( values[0] > isoValue && values[1] > isoValue && values[2] > isoValue && values[3] > isoValue ) ||
//        ( values[0] < isoValue && values[1] < isoValue && values[2] < isoValue && values[3] < isoValue ) )
//    {
//        return result;
//    }
//
//    for( char i = 0; i < 4; i++ ) // check every edge
//    {
//        // to always get the exact coordinates of the start and end point of the lines we ensure that the first
//        // point has always lower index
//        size_t r = i;
//        size_t s = ( i + 1 ) % 4;
//        //if( r > s )
//        //{
//        //    std::swap( r, s );
//        //}
//
////        osg::ref_ptr< osg::Vec3Array > point = checkEdge( corners[r], values[r], corners[s], values[s], isoValue );
//        result->insert( result->end(), point->begin(), point->end() );
//    }
//    if( result->size() == 1 )
//    {
//        result->clear();
//    }
//    else if( result->size() == 3 ) // duplicate middle element
//    {
//        result->push_back( result->back() );
//        ( *result )[2] = ( *result )[1];
//    }
//
//    return result;
//}


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
