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

#include <cstdlib>

#include <osg/Geometry>
#include <osg/LineStipple>

#include "../../../common/exceptions/WTypeMismatch.h"
#include "../../../common/math/WPosition.h"
#include "../../../common/WLogger.h"
#include "../../../dataHandler/WDataSetScalar.h"
#include "../../../dataHandler/WDataSetVector.h"
#include "../../../dataHandler/WGridRegular3D.h"
#include "../../../graphicsEngine/WGEGeodeUtils.h"
#include "../../../graphicsEngine/WGEGroupNode.h"
#include "../../../graphicsEngine/WGEManagedGroupNode.h"
#include "WSPSliceBuilderVectors.h"

WSPSliceBuilderVectors::WSPSliceBuilderVectors( ProbTractList probTracts, WPropGroup sliceGroup, std::vector< WPropGroup > colorMap,
        boost::shared_ptr< const WDataSetVector > vector, WPropGroup vectorGroup, boost::filesystem::path shaderPath )
    : WSPSliceBuilder( probTracts, sliceGroup, colorMap ),
      m_vectors( vector ),
      m_shader( new WGEShader( "WSPSliceBuilderVectors", shaderPath ) )
{
    m_probThreshold = vectorGroup->findProperty( "Prob Threshold" )->toPropDouble();
    m_spacing = vectorGroup->findProperty( "Spacing" )->toPropDouble();
    m_glyphSize = vectorGroup->findProperty( "Glyph size" )->toPropDouble();
    m_glyphSpacing = vectorGroup->findProperty( "Glyph Spacing" )->toPropDouble();
    m_glyphThickness = vectorGroup->findProperty( "Glyph Thickness" )->toPropDouble();
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
    osg::ref_ptr< osg::Vec3Array > firstFocalPoint( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec3Array > secondFocalPoint( new osg::Vec3Array );

    WVector3D normal( 0.0, 0.0, 0.0 );
    normal[ sliceNum ] = -1.0;
    quadNormals->push_back( normal );
    osg::ref_ptr< osg::Vec4Array > quadColors( new osg::Vec4Array );

    osg::ref_ptr< osg::Vec3Array > texCoordsPerPrimitive = generateQuadSpanning( activeDims );
    boost::shared_ptr< std::vector< WVector3D > > glyphDirections = generateClockwiseDir( activeDims, m_glyphSpacing->get() );

    std::srand( 0 ); // we just really need only pseudo random numbers

    for( double x = 0.0; x < numCoords[ activeDims.first ]; x += m_spacing->get() )
    {
        for( double y = 0.0; y < numCoords[ activeDims.second ]; y += m_spacing->get() )
        {
            WPosition pos = ( *origin ) + x * ( *a ) + y * ( *b );

            for( size_t i = 0; i < m_probTracts.size(); ++i )
            {
                WPosition realPos = pos + glyphDirections->at( i );
                WColor tractColor = lookUpColor( realPos, i );
                if( tractColor[3] > m_probThreshold->get() )
                {
                    // determine density of line stipples 1 => 10 samples. 0 => 0 samples
                    size_t numSamples = static_cast< size_t >( tractColor[3] * 10 );
                    for( size_t sample = 0; sample < numSamples; ++sample )
                    {
                        double s = 0.9 * m_spacing->get(); // jitter scaling
                        WVector3D jitter( s * ( std::rand() % 1000 ) / 1000.0, s * ( std::rand() % 1000 ) / 1000.0, s * ( std::rand() % 1000 ) / 1000.0 ); // NOLINT line length

                        std::pair< WPosition, WPosition > focalPoints = computeFocalPoints( realPos + jitter, sliceNum );
                        for( int j = 0; j < 4; ++j )
                        {
                            quadVertices->push_back( realPos + jitter );
                            quadColors->push_back( tractColor );
                            firstFocalPoint->push_back( focalPoints.first );
                            secondFocalPoint->push_back( focalPoints.second );
                        }

                        // for each primitive copy the same texture coordinates, misused as the vertex transformation information to make a real
                        // quad out of the four center points
                        quadSpanning->insert( quadSpanning->end(), texCoordsPerPrimitive->begin(), texCoordsPerPrimitive->end() );
                    }
                }
            }

            // draw degenerated quads around this point if alpha value of color is greater than threshold
        }
    }

    osg::ref_ptr< osg::Geometry > geometry = new osg::Geometry();
    geometry->setVertexArray( quadVertices );
    geometry->setTexCoordArray( 0, quadSpanning );
    geometry->setTexCoordArray( 1, firstFocalPoint );
    geometry->setTexCoordArray( 2, secondFocalPoint );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
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
    geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_glyphThickness", m_glyphThickness ) );
    osg::StateSet* state = geode->getOrCreateStateSet();
    state->setMode( GL_BLEND, osg::StateAttribute::ON );
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // bind vector field as texture
    wge::bindTexture( geode, m_vectors->getTexture2() );

    return result;
}

std::pair< WPosition, WPosition > WSPSliceBuilderVectors::computeFocalPoints( const WPosition& pos, size_t sliceNum ) const
{
    std::pair< WPosition, WPosition > result;

    bool success = false;
    WVector3D vec = m_vectors->eigenVectorInterpolate( pos, &success );
    vec.normalize();

    // project into plane
    vec[ sliceNum ] = 0.0;

    result.first = -0.4 * vec;
    result.second = 0.4 * vec;

    return result;
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

boost::shared_ptr< std::vector< WVector3D > > WSPSliceBuilderVectors::generateClockwiseDir( std::pair< unsigned char, unsigned char > activeDims,
        double distance ) const
{
    boost::shared_ptr< std::vector< WVector3D > > result( new std::vector< WVector3D >( 9, WVector3D( 0.0, 0.0, 0.0 ) ) );
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
