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

#include <list>
#include <vector>

#include <boost/lambda/bind.hpp>

#include <osg/Geometry>
#include <osg/LineStipple>

#include "core/common/datastructures/WFiber.h"
#include "core/common/exceptions/WTypeMismatch.h"
#include "core/common/math/linearAlgebra/WPosition.h"
#include "core/common/WLogger.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEGroupNode.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "WSPSliceBuilderTracts.h"

WSPSliceBuilderTracts::WSPSliceBuilderTracts( ProbTractList probTracts, WPropGroup sliceGroup, std::vector< WPropGroup > colorMap,
        boost::shared_ptr< const WDataSetFibers > detTracts, WPropGroup tractGroup )
    : WSPSliceBuilder( probTracts, sliceGroup, colorMap ),
      m_detTracts( detTracts ),
      m_intersectionList( 3 )
{
    if( m_probTracts.empty() || !m_detTracts )
    {
        wlog::warn( "WSPSliceBuilder" ) << "No probabilistic tractograms were given for slice geode generation!";
    }

    m_maxDistance = tractGroup->findProperty( "Slices Environment" )->toPropDouble();
    m_probThreshold = tractGroup->findProperty( "Prob Threshold" )->toPropDouble();
    m_showIntersections = tractGroup->findProperty( "Show Intersections" )->toPropBool();
    m_showProjections = tractGroup->findProperty( "Show Projections" )->toPropBool();

    // compute BB for each deterministic tract
    m_tractBB.clear();
    m_tractBB.reserve( detTracts->size() );
    for( size_t i = 0; i < detTracts->size(); ++i )
    {
        m_tractBB.push_back( computeBoundingBox( ( *detTracts )[i] ) );
    }
}

void WSPSliceBuilderTracts::preprocess()
{
    computeSliceBB();

    // tidy up old intersections
    for( unsigned char sliceNum = 0; sliceNum <= 2; ++sliceNum )
    {
       m_intersectionList[ sliceNum ].clear();
    }

    for( size_t tract = 0; tract < m_detTracts->getLineStartIndexes()->size() ; ++tract )
    {
        // check each slice
        for( unsigned char sliceNum = 0; sliceNum <= 2; ++sliceNum )
        {
            if( m_sliceBB[ sliceNum ].intersects( m_tractBB[ tract ] ) )
            {
                m_intersectionList[ sliceNum ].push_back( tract );
            }
        }
    }
}

void WSPSliceBuilderTracts::projectTractOnSlice( const unsigned char sliceNum, osg::ref_ptr< osg::Vec3Array > vertices, const int slicePos ) const
{
    WAssert( vertices, "Bug: a given internal array was not expected empty here!" );
    WAssert( sliceNum <= 2, "Bug: The selected sliceNum ( 0 == x, 1 == y, 2 == z ) was invalid" );

    for( osg::Vec3Array::iterator vertex = vertices->begin(); vertex != vertices->end(); ++vertex )
    {
        ( *vertex )[ sliceNum ] = slicePos;
    }
}

osg::ref_ptr< osg::Vec4Array > WSPSliceBuilderTracts::colorVertices( osg::ref_ptr< const osg::Vec3Array > vertices ) const
{
    osg::ref_ptr< osg::Vec4Array > result( new osg::Vec4Array );
    result->reserve( vertices->size() );

    for( osg::Vec3Array::const_iterator cit = vertices->begin(); cit != vertices->end(); ++cit )
    {
        osg::ref_ptr< osg::Vec4Array > colors = computeColorsFor( *cit );

        // erase those colors where the alpha (aka probability) is below the threshold
        colors->erase( std::remove_if( colors->begin(), colors->end(), boost::bind( &WSPSliceBuilderTracts::alphaBelowThreshold, this, _1,
                        m_probThreshold->get() ) ), colors->end() );

        // compose all eligible vertices to one color!
        WColor color( 0.0, 0.0, 0.0, 0.0 );
        double share = static_cast< double >( colors->size() );
        if( share > wlimits::DBL_EPS )
        {
            for( osg::Vec4Array::const_iterator vc = colors->begin(); vc != colors->end(); ++vc )
            {
                WAssert( ( *vc )[3] >= m_probThreshold->get(), "Bug: There were colors left, below the given threshold" );
                color += *vc / share;
            }
        }

        result->push_back( color );
    }

    WAssert( result->size() == vertices->size(), "Bug: There are not as many colors as vertices computed!" );
    return result;
}

osg::ref_ptr< WGEGroupNode > WSPSliceBuilderTracts::generateSlice( const unsigned char sliceNum ) const
{
    WAssert( sliceNum <= 2, "Bug: Invalid slice number given: must be 0, 1 or 2." );
    // select slicePos, intersections and bounding box
    const std::list< size_t >& intersections = m_intersectionList[ sliceNum ];
    const WBoundingBox& bb = m_sliceBB[ sliceNum ];
    const int slicePos = m_slicePos[ sliceNum ]->get();

    osg::ref_ptr< osg::Geode > intersectionGeode = new osg::Geode;
    osg::ref_ptr< osg::Geode > projectionGeode = new osg::Geode;
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    colors->push_back( osg::Vec4( 1.0, 0.0, 0.0, 1.0 ) );
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry );

    osg::ref_ptr< osg::Vec3Array > pv = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Geometry > pg = osg::ref_ptr< osg::Geometry >( new osg::Geometry );
    osg::ref_ptr< osg::Vec4Array > pc = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );

    WDataSetFibers::VertexArray fibVerts = m_detTracts->getVertices();

    for( std::list< size_t >::const_iterator cit = intersections.begin(); cit != intersections.end(); ++cit )
    {
        size_t sidx = m_detTracts->getLineStartIndexes()->at( *cit ) * 3;
        size_t len = m_detTracts->getLineLengths()->at( *cit );
        size_t k = 0;
        while( k < len )
        {
            size_t startIdx = vertices->size();
            // proceed to a vertex inside the deltaX environment
            while( ( k < len ) && std::abs( slicePos - fibVerts->at( ( 3 * k ) + sidx + sliceNum ) ) > m_maxDistance->get() )
            {
                k++;
            }
            osg::ref_ptr< osg::Vec3Array > candidate = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
            WBoundingBox cBB;
            while( ( k < len ) &&  std::abs( slicePos - fibVerts->at( ( 3 * k ) + sidx + sliceNum ) ) <= m_maxDistance->get() )
            {
                WPosition cv( fibVerts->at( ( 3 * k ) + sidx ),
                            fibVerts->at( ( 3 * k ) + sidx + 1 ),
                            fibVerts->at( ( 3 * k ) + sidx + 2 ) );
                candidate->push_back( cv );
                cBB.expandBy( cv );
                k++;
            }
            if( !candidate->empty() && cBB.intersects( bb ) )
            {
                vertices->insert( vertices->end(), candidate->begin(), candidate->end() );
                geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, startIdx, candidate->size() ) );
                osg::ref_ptr< osg::Vec4Array > candidateColors = colorVertices( candidate );
                pc->insert( pc->end(), candidateColors->begin(), candidateColors->end() );
                projectTractOnSlice( sliceNum, candidate, slicePos );
                pv->insert( pv->end(), candidate->begin(), candidate->end() );
                pg->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, startIdx, candidate->size() ) );
            }
            // note: this loop will terminate since either the first while loop is true or the second or both!
        }
    }
    pg->setVertexArray( pv );
    pg->setColorArray( pc );
    pg->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    intersectionGeode->addDrawable( geometry );
    intersectionGeode->addDrawable( wge::generateBoundingBoxGeode( bb, WColor( 0.0, 0.0, 0.0, 1.0 ) )->getDrawable( 0 ) );
    projectionGeode->addDrawable( pg );
    projectionGeode->addDrawable( wge::generateBoundingBoxGeode( bb, WColor( 0.0, 0.0, 0.0, 1.0 ) )->getDrawable( 0 ) );

    osg::StateSet* state = projectionGeode->getOrCreateStateSet();
    // transparent colors are really transparent and not black
    state->setMode( GL_BLEND, osg::StateAttribute::ON );
    // transparent colors are rendered at last, so not use the background color but the color of the nav slices
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    osg::ref_ptr<osg::LineStipple> ls = new osg::LineStipple();
    ls->setPattern( 0x00FF );
    ls->setFactor( 2 );

    state->setAttributeAndModes( ls.get(), osg::StateAttribute::ON );
    osg::ref_ptr< WGEManagedGroupNode > intersectionGroup = new WGEManagedGroupNode( m_showIntersections );
    intersectionGroup->insert( intersectionGeode );
    osg::ref_ptr< WGEManagedGroupNode > projectionGroup = new WGEManagedGroupNode( m_showProjections );
    projectionGroup->insert( projectionGeode );
    osg::ref_ptr< WGEGroupNode > result = new WGEGroupNode();
    result->clear();
    result->insert( intersectionGroup );
    result->insert( projectionGroup );
    return result;
}
