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

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>

#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "WBoundaryLines.h"

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

osg::ref_ptr< WGEGroupNode > WBoundaryLines::generateSlice( const unsigned char sliceNum ) const
{
    boost::shared_ptr< WPosition > origin( new WPosition );
    boost::shared_ptr< WVector3D > a( new WVector3D );
    boost::shared_ptr< WVector3D > b( new WVector3D );
    std::pair< unsigned char, unsigned char > activeDims = computeSliceBase( sliceNum, origin, a, b );

    boost::array< unsigned int, 3 > numCoords = getNbCoords( m_grid );

    osg::ref_ptr< osg::Vec3Array > gmvertices( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec3Array > wmvertices( new osg::Vec3Array );

    double spacing = m_resolution->get();

    const double gmIsoValue = m_grayMatter->get() * m_texture->getMax();
    const double wmIsoValue = m_whiteMatter->get() * m_texture->getMax();

    for( double x = 0.0; ( x + 2 * spacing ) < numCoords[ activeDims.first ] - 1; x += spacing )
    {
        for( double y = 0.0; ( y + 2 * spacing ) < numCoords[ activeDims.second ] - 1; y += spacing )
        {
            boost::array< WPosition, 4 > corners;
            corners[0] =  ( *origin ) + x * ( *a ) + y * ( *b );
            corners[1] =  corners[0] + spacing * ( *a );
            corners[2] =  corners[1] + spacing * ( *b );
            corners[3] =  corners[0] + spacing * ( *b );
            osg::ref_ptr< osg::Vec3Array > gm = checkQuad( corners, gmIsoValue );
            osg::ref_ptr< osg::Vec3Array > wm = checkQuad( corners, wmIsoValue );
            gmvertices->insert( gmvertices->end(), gm->begin(), gm->end() );
            wmvertices->insert( wmvertices->end(), wm->begin(), wm->end() );
        }
    }
    osg::ref_ptr< osg::Geometry > gmgeometry = new osg::Geometry();
    gmgeometry->setVertexArray( gmvertices );
    osg::ref_ptr< osg::Vec4Array > gmcolors( new osg::Vec4Array );
    gmcolors->push_back( WColor( 0.0, 0.0, 0.0, 1.0 ) );
    gmgeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    gmgeometry->setColorArray( gmcolors );
    gmgeometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, gmvertices->size() ) );
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

osg::ref_ptr< osg::Vec3Array > WBoundaryLines::checkEdge( const WPosition& p0, const double w0, const WPosition& p1, const double w1,
        const double isoValue ) const
{
    osg::ref_ptr< osg::Vec3Array > result( new osg::Vec3Array );

    if( w0 == isoValue )
    {
        result->push_back( p0 );
    }
    else if( w1 == isoValue )
    {
        result->push_back( p1 );
    }
    else if( ( ( w0 < isoValue ) && ( isoValue < w1 ) ) || ( ( w1 < isoValue ) && ( isoValue < w0 ) ) ) // this edge hits the isoValue
    if( ( ( w0 < isoValue ) && ( isoValue < w1 ) ) || ( ( w1 < isoValue ) && ( isoValue < w0 ) ) ) // this edge hits the isoValue
    {
        // interpolate:
        double nu = std::abs( w0 - isoValue ) / std::abs( w0 - w1 );
        result->push_back( p0 + ( p1 - p0 ) * nu );
    }
    return result;
}

osg::ref_ptr< osg::Vec3Array > WBoundaryLines::checkQuad( const boost::array< WPosition, 4 >& corners, const double isoValue ) const
{
    osg::ref_ptr< osg::Vec3Array > result( new osg::Vec3Array );

    // check if we can skip, otherwise the computation was necessary anyway
    boost::array< double, 4 > values;
    bool success;
    for( char i = 0; i < 4; ++i )
    {
        values[i] =  m_texture->interpolate( corners[i], &success );
    }
    if( ( values[0] > isoValue && values[1] > isoValue && values[2] > isoValue && values[3] > isoValue ) ||
        ( values[0] < isoValue && values[1] < isoValue && values[2] < isoValue && values[3] < isoValue ) )
    {
        return result;
    }

    for( char i = 0; i < 4; i++ ) // check every edge
    {
        // to always get the exact coordinates of the start and end point of the lines we ensure that the first
        // point has always lower index
        size_t r = i;
        size_t s = ( i + 1 ) % 4;
        if( r > s )
        {
            std::swap( r, s );
        }

        osg::ref_ptr< osg::Vec3Array > point = checkEdge( corners[r], values[r], corners[s], values[s], isoValue );
        result->insert( result->end(), point->begin(), point->end() );
    }
    if( result->size() == 1 )
    {
        result->clear();
    }
    else if( result->size() == 3 ) // duplicate middle element
    {
        result->push_back( result->back() );
        ( *result )[2] = ( *result )[1];
    }
    //std::stringstream ss;
    //ss << "w0123" << values[0] << " XXX " << values[1] << " XXX " << values[2] << " XXX " << values[3]
    //   << " XXX " << result->size() << " XXX " << isoValue << " XXX ";
    //for( size_t i = 0; i < result->size(); ++i )
    //{
    //    ss << (*result)[i][0] << ", " << (*result)[i][1] << ", " << (*result)[i][2] << " YYY ";
    //}
    //for( size_t i = 0; i < corners.size(); ++i )
    //{
    //    ss << corners[i][0] << ", " << corners[i][1] << ", " << corners[i][2] << " YYY ";
    //}
    //WAssert( result->size() % 2 == 0, "Bug: there should at least be two vertices returned to draw a line: " + ss.str() );
    return result;
}

std::pair< unsigned char, unsigned char > WBoundaryLines::computeSliceBase( const unsigned char sliceNum,
        boost::shared_ptr< WVector3D > origin, boost::shared_ptr< WVector3D > a, boost::shared_ptr< WVector3D > b ) const
{
    std::vector< WVector3D > dir;
    dir.push_back( m_grid->getDirectionX() * m_grid->getOffsetX() );
    dir.push_back( m_grid->getDirectionY() * m_grid->getOffsetY() );
    dir.push_back( m_grid->getDirectionZ() * m_grid->getOffsetZ() );
    *origin = m_grid->getOrigin() + dir[ sliceNum ] * m_slicePos[ sliceNum ]->get();

    std::set< unsigned char > slices;
    slices.insert( 0 );
    slices.insert( 1 );
    slices.insert( 2 );
    slices.erase( sliceNum );
    WAssert( slices.size() == 2, "Bug: mapping the selected slice to the other dimensions did not succeeded as expected." );
    *a = dir[ *( slices.begin() ) ];
    *b = dir[ *( slices.rbegin() ) ];

    return std::make_pair< unsigned char, unsigned char >( *( slices.begin() ), *( slices.rbegin() ) );
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
