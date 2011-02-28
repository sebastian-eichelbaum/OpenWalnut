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

#include <osg/Geometry>
#include <osg/LineStipple>

#include "../../common/exceptions/WTypeMismatch.h"
#include "../../common/math/WPosition.h"
#include "../../common/WLogger.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "WSPSliceBuilderVectors.h"

WSPSliceBuilderVectors::WSPSliceBuilderVectors( ProbTractList probTracts, WPropGroup sliceGroup, std::vector< WPropGroup > colorMap,
        boost::shared_ptr< const WDataSetVector > vector, WPropGroup vectorGroup, boost::filesystem::path shaderPath )
    : WSPSliceBuilder( probTracts, sliceGroup, colorMap ),
      m_vectors( vector ),
      m_shader( new WGEShader( "WSPSliceBuilderVectors", shaderPath ) )
{
    m_probThreshold = vectorGroup->findProperty( "Prob Threshold" )->toPropDouble();
    m_spacing = vectorGroup->findProperty( "Spacing" )->toPropInt();
    m_glyphSize = vectorGroup->findProperty( "Glyph size" )->toPropDouble();
}

void WSPSliceBuilderVectors::preprocess()
{
    computeSliceBB();
}

osg::ref_ptr< WGEGroupNode > WSPSliceBuilderVectors::generateSlice( const unsigned char sliceNum ) const
{
    boost::shared_ptr< WPosition > origin( new WPosition );
    boost::shared_ptr< WVector3D > a( new WVector3D );
    boost::shared_ptr< WVector3D > b( new WVector3D );
    std::pair< unsigned char, unsigned char > activeDims = computeSliceBase( sliceNum, origin, a, b );

    std::vector< size_t > numCoords;
    numCoords.push_back( m_grid->getNbCoordsX() );
    numCoords.push_back( m_grid->getNbCoordsY() );
    numCoords.push_back( m_grid->getNbCoordsZ() );

    osg::ref_ptr< osg::Vec3Array > quadVertices( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec3Array > quadSpanning( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec3Array > quadNormals( new osg::Vec3Array );
    WVector3D normal( 0.0, 0.0, 0.0 );
    normal[ sliceNum ] = -1.0;
    quadNormals->push_back( normal );
    osg::ref_ptr< osg::Vec4Array > quadColors( new osg::Vec4Array );
    quadColors->push_back( WColor( 1.0, 0.0, 0.0, 1.0 ) );

    osg::ref_ptr< osg::Vec3Array > texCoordsPerPrimitive = generateQuadSpanning( activeDims );

    for( size_t x = 0; x < numCoords[ activeDims.first ]; x += m_spacing->get() )
    {
        for( size_t y = 0; y < numCoords[ activeDims.second ]; y += m_spacing->get() )
        {
            WPosition pos =  ( *origin ) + x * ( *a ) + y * ( *b );
            quadVertices->push_back( pos );
            quadVertices->push_back( pos );
            quadVertices->push_back( pos );
            quadVertices->push_back( pos );

            // for each primitive copy the same texture coordinates, misused as the vertex transformation information to make a real
            // quad out of the four center points
            quadSpanning->insert( quadSpanning->end(), texCoordsPerPrimitive->begin(), texCoordsPerPrimitive->end() );

            osg::ref_ptr< osg::Vec4Array > colors = computeColorsFor( pos );

            // draw degenerated quads around this point if alpha value of color is greater than threshold
        }
    }

    osg::ref_ptr< osg::Geometry > geometry = new osg::Geometry();
    geometry->setVertexArray( quadVertices );
    geometry->setTexCoordArray( 0, quadSpanning );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );
    geometry->setNormalArray( quadNormals );
    geometry->setColorArray( quadColors );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, quadVertices->size() ) );

    osg::ref_ptr< WGEGroupNode > result( new WGEGroupNode );
    osg::ref_ptr< osg::Geode > geode( new osg::Geode );
    geode->addDrawable( geometry );
    result->insert( geode );
    result->insert( wge::generateBoundingBoxGeode( m_sliceBB[ sliceNum ], WColor( 0.0, 0.0, 0.0, 1.0 ) ) );

    m_shader->apply( geode );

    geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_glyphSize", m_glyphSize ) );

    bindTextures( geode );

    return result;
}

void WSPSliceBuilderVectors::bindTextures( osg::ref_ptr< osg::Node > node ) const
{
    if( m_probTracts.size() > 8 )
    {
        wlog::warn( "WSPSliceBuilderVectors" ) << "Trying to bind more than 8 textures to a single geode, this might not be possible due to "
            "graphics card, or driver limitations";
    }
    for( ProbTractList::const_iterator cit = m_probTracts.begin(); cit != m_probTracts.end(); ++cit )
    {
        wge::bindTexture( node, ( *cit )->getTexture2() );
    }
}

osg::ref_ptr< osg::Vec3Array > WSPSliceBuilderVectors::generateQuadSpanning( std::pair< unsigned char, unsigned char > activeDims ) const
{
    osg::ref_ptr< osg::Vec3Array > result( new osg::Vec3Array );
    result->reserve( 4 );
    result->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
    result->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
    result->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
    result->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
    result->at( 0 )[ activeDims.first ]  = -0.5;
    result->at( 1 )[ activeDims.first ]  =  0.5;
    result->at( 2 )[ activeDims.first ]  =  0.5;
    result->at( 3 )[ activeDims.first ]  = -0.5;
    result->at( 0 )[ activeDims.second ] = -0.5;
    result->at( 1 )[ activeDims.second ] = -0.5;
    result->at( 2 )[ activeDims.second ] =  0.5;
    result->at( 3 )[ activeDims.second ] =  0.5;
    return result;
}

std::pair< unsigned char, unsigned char > WSPSliceBuilderVectors::computeSliceBase( const unsigned char sliceNum,
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

osg::ref_ptr< osg::Vec3Array > WSPSliceBuilderVectors::generateClockwiseDir( std::pair< unsigned char, unsigned char > activeDims,
        double distance ) const
{
    osg::ref_ptr< osg::Vec3Array > result( new osg::Vec3Array( 9 ) );
    std::fill( result->begin(), result->begin() + 9, osg::Vec3( 0.0, 0.0, 0.0 ) );
    result->at( 0 )[ activeDims.first ]  =  0.0 * distance;
    result->at( 1 )[ activeDims.first ]  =  1.0 * distance;
    result->at( 2 )[ activeDims.first ]  =  1.0 * distance;
    result->at( 3 )[ activeDims.first ]  =  0.0 * distance;
    result->at( 4 )[ activeDims.first ]  = -1.0 * distance;
    result->at( 5 )[ activeDims.first ]  = -1.0 * distance;
    result->at( 6 )[ activeDims.first ]  = -1.0 * distance;
    result->at( 7 )[ activeDims.first ]  =  0.0 * distance;
    result->at( 8 )[ activeDims.first ]  =  1.0 * distance;
    result->at( 0 )[ activeDims.second ] =  0.0 * distance;
    result->at( 1 )[ activeDims.second ] =  0.0 * distance;
    result->at( 2 )[ activeDims.second ] =  1.0 * distance;
    result->at( 3 )[ activeDims.second ] =  1.0 * distance;
    result->at( 4 )[ activeDims.second ] =  1.0 * distance;
    result->at( 5 )[ activeDims.second ] =  0.0 * distance;
    result->at( 6 )[ activeDims.second ] = -1.0 * distance;
    result->at( 7 )[ activeDims.second ] = -1.0 * distance;
    result->at( 8 )[ activeDims.second ] = -1.0 * distance;
    return result;
}

// osg::ref_ptr< osg::Vec3Array > WSPSliceBuilderVectors::generateQuadStubs( const WPosition& pos ) const
// {
//     osg::ref_ptr< osg::Vec3Array > result( new osg::Vec3Array );
//     result->reserve( m_probTracts->size() * 4 ); // four corners for each quad stub
//
//     if( m_probTracts->size() < 10 )
//     {
//         throw WNotImplemented( "Bug: The jittering works only for at most 9 tracts." );
//     }
//
//     std::vector< WVector3D > dir;
//
//     for( size_t i = 0; i < m_probTracts->size(); ++i )
//     {
//
//     }
//     return result;
// }
