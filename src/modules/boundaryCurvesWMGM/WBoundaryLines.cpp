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

#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "WBoundaryLines.h"

WBoundaryLines::WBoundaryLines( boost::shared_ptr< const WDataSetScalar > texture, boost::shared_ptr< const WProperties > properties,
    boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > *slices )
    : WBoundaryBuilder( texture, properties, slices )
{
    m_grid = boost::shared_dynamic_cast< WGridRegular3D >( m_texture->getGrid() );

    WAssert( m_grid, "Bug: each DataSetScalar should have a regular grid 3d!!!" );
}

void WBoundaryLines::run( osg::ref_ptr< WGEManagedGroupNode > output )
{
    computeSliceBB();

    for( char i = 0; i < 3; ++i )
    {
        m_slices[i]->addChild( generateSlice( i ) );
        output->insert( m_slices[i] );
    }
}

osg::ref_ptr< WGEGroupNode > WBoundaryLines::generateSlice( const unsigned char sliceNum ) const
{
    boost::shared_ptr< WPosition > origin( new WPosition );
    boost::shared_ptr< WVector3D > a( new WVector3D );
    boost::shared_ptr< WVector3D > b( new WVector3D );
    std::pair< unsigned char, unsigned char > activeDims = computeSliceBase( sliceNum, origin, a, b );

    boost::array< unsigned int, 3 > numCoords = getNbCoords( m_grid );

    osg::ref_ptr< osg::Vec3Array > vertices( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > colors( new osg::Vec4Array );

    double spacing = 1.0;

    for( double x = 0.0; x < numCoords[ activeDims.first ]; x += spacing )
    {
        for( double y = 0.0; y < numCoords[ activeDims.second ]; y += spacing )
        {
            WPosition pos = ( *origin ) + x * ( *a ) + y * ( *b );
        }
    }

    osg::ref_ptr< osg::Geometry > geometry = new osg::Geometry();
    geometry->setVertexArray( vertices );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    geometry->setColorArray( colors );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, vertices->size() ) );

    osg::ref_ptr< WGEGroupNode > result( new WGEGroupNode );
    osg::ref_ptr< osg::Geode > geode( new osg::Geode );
    geode->addDrawable( geometry );
    result->insert( geode );
//    result->insert( wge::generateBoundingBoxGeode( m_sliceBB[ sliceNum ], WColor( 0.0, 0.0, 0.0, 1.0 ) ) );

//    osg::StateSet* state = geode->getOrCreateStateSet();
//    state->setMode( GL_BLEND, osg::StateAttribute::ON );
//    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

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
