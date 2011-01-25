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
        boost::shared_ptr< const WDataSetVector > vector, WPropGroup vectorGroup )
    : WSPSliceBuilder( probTracts, sliceGroup, colorMap ),
      m_vectors( vector )
{
    m_probThreshold = vectorGroup->findProperty( "Prob Threshold" )->toPropDouble();
    m_spacing = vectorGroup->findProperty( "Spacing" )->toPropInt();

}

void WSPSliceBuilderVectors::preprocess()
{
    // no preprocessing needed, but we have to implement this to be not abstract anymore
}

// namespace
// {
//     /**
//      * Operator compares if a color has an alpha value below a given threshold.
//      */
//     struct alphaBelowThreshold:
//     {
//         /**
//          * Compares the color's alpha value and the threshold.
//          *
//          * \param c The given color
//          * \param threshold The given Threshold
//          *
//          * \return ture if the color has an alpha value below the given threshold.
//          */
//         bool operator()( const WColor& c, const double threshold ) const
//         {
//             return c[3] < threshold;
//         }
//     };
// }

osg::ref_ptr< WGEGroupNode > WSPSliceBuilderVectors::generateSlice( const unsigned char sliceNum ) const
{
    boost::shared_ptr< wmath::WPosition > origin( new wmath::WPosition );
    boost::shared_ptr< wmath::WVector3D > a( new wmath::WVector3D );
    boost::shared_ptr< wmath::WVector3D > b( new wmath::WVector3D )
    std::pair< unsigned char > activeDims = computeSliceBase( sliceNum, origin, a, b );

    std::vector< size_t > numCoords;
    numCoords.push_back( m_grid->getNbCoordsX() );
    numCoords.push_back( m_grid->getNbCoordsY() );
    numCoords.push_back( m_grid->getNbCoordsZ() );

    osg::ref_ptr< osg::Vec3Array > quadVertices( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec3Array > quadTexCoords( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec3Array > quadNormals( new osg::Vec3Array );
    wmath::WVector3D normal( 0.0, 0.0, 0.0 );
    normal[ sliceNum ] = -1.0;
    quadNormals->push_back( normal );
    osg::ref_ptr< osg::Vec4Array > quadColors( new osg::Vec4Array );
    quadColors->push_back( WColor( 1.0, 0.0, 0.0, 1.0 ) );

    osg::ref_ptr< const osg::Vec3Array > texCoordsPerPrimitve = generateQuadTexCoords( activeDims, 1.0 );

    for( size_t x = 0; x < numCoords[ activeDims.first ]; x += m_spacing )
    {
        for( size_t y = 0; y < numCoord[ activeDims.second ]; y += m_spacing )
        {
            wmath::WPosition pos =  origin + x * a + y * b;
            quadVertices->push_back( pos );
            quadVertices->push_back( pos );
            quadVertices->push_back( pos );
            quadVertices->push_back( pos );

            // for each primitive copy the same texture coordinates, misused as the vertex transformation information to make a real quad out of the 4
            // center points
            quadTexCoords->insert( quadTexCoords->end(), texCoordsPerPrimitive->begin(), texCoordsPerPrimitive->end() );

//            osg::ref_ptr< osg::Vec4Array > colors = computeColorsFor( pos );

            // erase those colors where the alpha (aka probability) is below the threshold
            // colors->erase( std::remove_if( colors->begin(), colors->end(), std::bind2nd( alphaBelowThreshold(), m_probThreshold->get() ) ),
            //         colors->end() );

            // draw degenerated quads around this point if alpha value of color is greater than threshold
        }
    }

    osg::ref_ptr< osg::Geometry > geometry = new osg::Geometry();
    geometry->setVertexArray( quadVertices );
    geometry->setTexCoordArray( 0, quadTexCoords );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );
    geometry->setNormalArray( quadNormals );
    geometry->setColorArray( quadColors );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, quadVertices->size() ) );

    osg::ref_ptr< WGEGroupNode > result( new WGEGroupNode );
    osg::ref_ptr< osg::Geode > geode( new osg::Geode );
    geode->addDrawable( geometry );
    result->insert( geode );
    return result;
}

osg::ref_ptr< osg::Vec3Array > generateQuadTexCoords( std::pair< unsigned char > activeDims, double size ) const
{
    osg::ref_ptr< osg::Vec3Array > result( new osg::Vec3Array( 4, wmath::WPosition( 0.0, 0.0, 0.0 ) ) );
    result->at( 0 )[ activeDims.first ] = -0.5 * size;   result->at( 0 )[ activeDims.second ] = -0.5 * size;
    result->at( 1 )[ activeDims.first ] =  0.5 * size;   result->at( 1 )[ activeDims.second ] = -0.5 * size;
    result->at( 2 )[ activeDims.first ] =  0.5 * size;   result->at( 2 )[ activeDims.second ] =  0.5 * size;
    result->at( 3 )[ activeDims.first ] = -0.5 * size;   result->at( 3 )[ activeDims.second ] =  0.5 * size;
    return result;
}

std::pair< unsigned char > WSPSliceBuilderVectors::computeSliceBase( const unsigned char sliceNum,
        boost::shared_ptr< wmath::WVector3D > origin, boost::shared_ptr< wmath::WVector3D > a, boost::shared_ptr< wmath::WVector3D > b ) const
{
    std::vector< wmath::WVector3D > dir;
    dir.push_back( m_grid->getDirectionX() * m_grid->getOffSetX() );
    dir.push_back( m_grid->getDirectionY() * m_grid->getOffSetY() );
    dir.push_back( m_grid->getDirectionZ() * m_grid->getOffSetZ() );
    *origin = m_grid->getOrigin() + dir[ sliceNum ] * m_slicePos[ sliceNum ];

    std::set< unsigned char > slices;
    slices.push_back( 0 );
    slices.push_back( 1 );
    slices.push_back( 2 );
    slices.erase( sliceNum );
    WAssert( slices.size() == 2, "Bug: mapping the selected slice to the other dimensions did not succeeded as expected." );
    *a = dir[ slices.front() ];
    *b = dir[ slices.back() ];

    return std::make_pair< unsigned char >( slices.front(), slices.back() );
}

// boost::shared_ptr< std::vector< wmath::WVector3D > > WSPSliceBuilderVectors::generateClockwiseDir( std::pair< unsigned char > activeDims,
//         double distance ) const
// {
//     boost::shared_ptr< std::vector< wmath::WVector3D > > result( new std::vector< wmath::WVector3D >( 9, wmath::WVector3D( 0.0, 0.0, 0.0 ) ) );
//     result->at( 0 )[ activeDims.first ] =  0.0;    result->at( 0 )[ activeDims.second ] =  0.0;
//     result->at( 1 )[ activeDims.first ] =  1.0;    result->at( 1 )[ activeDims.second ] =  0.0;
//     result->at( 2 )[ activeDims.first ] =  1.0;    result->at( 2 )[ activeDims.second ] =  1.0;
//     result->at( 3 )[ activeDims.first ] =  0.0;    result->at( 3 )[ activeDims.second ] =  1.0;
//     result->at( 4 )[ activeDims.first ] = -1.0;    result->at( 4 )[ activeDims.second ] =  1.0;
//     result->at( 5 )[ activeDims.first ] = -1.0;    result->at( 5 )[ activeDims.second ] =  0.0;
//     result->at( 6 )[ activeDims.first ] = -1.0;    result->at( 6 )[ activeDims.second ] = -1.0;
//     result->at( 7 )[ activeDims.first ] =  0.0;    result->at( 7 )[ activeDims.second ] = -1.0;
//     result->at( 8 )[ activeDims.first ] =  1.0;    result->at( 8 )[ activeDims.second ] = -1.0;
//     return result;
// }
//
// osg::ref_ptr< osg::Vec3Array > WSPSliceBuilderVectors::generateQuadStubs( const wmath::WPosition& pos ) const
// {
//     osg::ref_ptr< osg::Vec3Array > result( new osg::Vec3Array );
//     result->reserve( m_probTracts->size() * 4 ); // four corners for each quad stub
//
//     if( m_probTracts->size() < 10 )
//     {
//         throw WNotImplemented( "Bug: The jittering works only for at most 9 tracts." );
//     }
//
//     std::vector< wmath::WVector3D > dir;
//
//     for( size_t i = 0; i < m_probTracts->size(); ++i )
//     {
//
//     }
//     return result;
// }
