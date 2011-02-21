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

#include "../../common/exceptions/WTypeMismatch.h"
#include "../../common/WLogger.h"
#include "../../common/math/WPosition.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "WSPSliceGeodeBuilder.h"

WSPSliceGeodeBuilder::WSPSliceGeodeBuilder( ProbTractList probTracts, boost::shared_ptr< const WDataSetFibers > detTracts, WPropGroup sliceGroup,
        std::vector< WPropGroup > colorMap )
    : m_detTracts( detTracts ),
      m_probTracts( probTracts ),
      m_intersectionList( 3 ),
      m_sliceBB( 3 ),
      m_slicePos( 3 ),
      m_colorMap( colorMap ) // yes this is a COPY of the vector but WPropGroup is a boost::shared_ptr so updates will propagate!
{
    if( m_probTracts.empty() || !m_detTracts )
    {
        wlog::warn( "WSPSliceGeodeBuilder" ) << "No probabilistic tractograms were given for slice geode generation!";
    }
    wlog::debug( "WSPSliceGeodeBuilder" ) << "Init geode builder...";

    m_slicePos[2] = sliceGroup->findProperty( "Axial Position" )->toPropInt();
    m_slicePos[1] = sliceGroup->findProperty( "Coronal Position" )->toPropInt();
    m_slicePos[0] = sliceGroup->findProperty( "Sagittal Position" )->toPropInt();

    checkAndExtractGrids();

    // compute BB for each deterministic tract
    m_tractBB.clear();
    m_tractBB.reserve( detTracts->size() );
    for( size_t i = 0; i < detTracts->size(); ++i )
    {
        m_tractBB.push_back( wmath::computeBoundingBox( ( *detTracts )[i] ) );
    }

    computeSliceBB(); // just to be sure those are initialized, since they may change due to m_slicePos[0], et al. anyway
    wlog::debug( "WSPSliceGeodeBuilder" ) << "Init geode builder done";
}

// helper functions only to be DRY
namespace
{
    /**
     * Try a cast to WGridRegular3D, and return the cast result if it was successful, otherwise throw an exception of
     * WTypeMismatched.
     *
     * \param dataset The dataset of which the grid is taken from to check.
     *
     * \return The grid of the dataset casted to WGridRegular3D.
     */
    boost::shared_ptr< const WGridRegular3D > ensureWGridRegular3D( boost::shared_ptr< const WDataSetScalar > dataset )
    {
        boost::shared_ptr< const WGridRegular3D > result = boost::shared_dynamic_cast< WGridRegular3D >( dataset->getGrid() );
        if( !result )
        {
            wlog::error( "WSPSliceGeodeBuilder" ) << "Cast to WGridRegular3D failed.";
            throw WTypeMismatch( "WSPSliceGeodeBuilder::extractGrid(): WGridRegular3D expected, but cast failed." );
        }
        return result;
    }
}

void WSPSliceGeodeBuilder::checkAndExtractGrids()
{
    if( m_probTracts.empty() )
    {
        m_grid.reset();
    }
    else
    {
        try
        {
            m_grid = ensureWGridRegular3D( m_probTracts.front() );

            for( ProbTractList::const_iterator cit = m_probTracts.begin(); cit != m_probTracts.end(); ++cit )
            {
                boost::shared_ptr< const WGridRegular3D > grid = ensureWGridRegular3D( *cit );
                // TODO(math): ensure that each WGridRegular3D is the same once the operator== is available for WGridRegular3D
                // grid == m_grid
            }
        }
        catch( const WTypeMismatch& e )
        {
            wlog::error( "WSPSliceGeodeBuilder" ) << "At least one probabilistic tractogram has a grid which is not castable to WGridRegluar3D";
            throw e;
        }
    }
}

void WSPSliceGeodeBuilder::computeSliceBB()
{
    if( !m_grid )
    {
        wlog::warn( "WSPSliceGeodeBuilder" ) << "Invalid grid while BB computation!";
        return;
    }
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

void WSPSliceGeodeBuilder::determineIntersectingDeterministicTracts()
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

void WSPSliceGeodeBuilder::projectTractOnSlice( const unsigned char sliceNum, osg::ref_ptr< osg::Vec3Array > vertices, const int slicePos ) const
{
    WAssert( vertices, "Bug: a given internal array was not expected empty here!" );
    WAssert( sliceNum <= 2, "Bug: The selected sliceNum ( 0 == x, 1 == y, 2 == z ) was invalid" );

    for( osg::Vec3Array::iterator vertex = vertices->begin(); vertex != vertices->end(); ++vertex )
    {
        ( *vertex )[ sliceNum ] = slicePos;
    }
}

WColor WSPSliceGeodeBuilder::colorMap( size_t probTractNum ) const
{
    return m_colorMap.at( probTractNum )->findProperty( "Color" )->toPropColor()->get();
}

osg::ref_ptr< osg::Vec4Array > WSPSliceGeodeBuilder::colorVertices( osg::ref_ptr< const osg::Vec3Array > vertices, const double probThreshold ) const
{
    osg::ref_ptr< osg::Vec4Array > result( new osg::Vec4Array );
    result->reserve( vertices->size() );

    // for each vertex its accumulated colors
    std::vector< WColor > vertexColorPerTract;
    vertexColorPerTract.reserve( m_probTracts.size() );

    size_t interpolationFailures = 0;

    for( osg::Vec3Array::const_iterator cit = vertices->begin(); cit != vertices->end(); ++cit )
    {
        const wmath::WPosition vertex( *cit );
        vertexColorPerTract.clear();

        size_t probTractNum = 0;
        // for each probabilisitc tractogram look up if its probability at this vertex is below a certain threshold or not
        for( ProbTractList::const_iterator probTract = m_probTracts.begin(); probTract != m_probTracts.end(); ++probTract, ++probTractNum )
        {
            bool success = false;
            double probability = ( *probTract )->interpolate( vertex, &success );
            if( ( *probTract )->getMax() > 10 )
            {
                // Todo(math): Solve this hack in a better way!
                // the probability is clearly not 0..1 distributed, we assume 0..255 instead.
                // We compute probability:
                probability /= 255.0;
            }
            if( success && ( probability > probThreshold ) )
            {
                WColor c = colorMap( probTractNum );
                c[3] = static_cast< float >( probability );
                vertexColorPerTract.push_back( c );
            }
            if( !success )
            {
                ++interpolationFailures;
            }
        }

        // compose all eligible vertices to one color!
        WColor color( 0.0, 0.0, 0.0, 0.0 );
        double share = static_cast< double >( vertexColorPerTract.size() );
        if( share > wlimits::DBL_EPS )
        {
            for( std::vector< WColor >::const_iterator vc = vertexColorPerTract.begin(); vc != vertexColorPerTract.end(); ++vc )
            {
                color += *vc / share;
            }
        }

        result->push_back( color );
    }

    if( interpolationFailures > 0 )
    {
        wlog::warn( "WSPSliceGeodeBuilder" ) << "While coloring vertices, the interpolation of probabilistic tractograms failed: "
            << interpolationFailures << " many times.";
    }
    WAssert( result->size() == vertices->size(), "Bug: There are not as many colors as vertices computed!" );
    return result;
}

WSPSliceGeodeBuilder::GeodePair WSPSliceGeodeBuilder::generateSlices( const unsigned char sliceNum, const double maxDistance,
        const double probThreshold ) const
{
    WAssert( sliceNum <= 2, "Bug: Invalid slice number given: must be 0, 1 or 2." );
    // select slicePos, intersections and bounding box
    const std::list< size_t >& intersections = m_intersectionList[ sliceNum ];
    const WBoundingBox& bb = m_sliceBB[ sliceNum ];
    const int slicePos = m_slicePos[ sliceNum ]->get();

    osg::ref_ptr< osg::Geode > intersectionGeode = new osg::Geode();
    osg::ref_ptr< osg::Geode > projectionGeode = new osg::Geode();
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
            while( ( k < len ) && std::abs( slicePos - fibVerts->at( ( 3 * k ) + sidx + sliceNum ) ) > maxDistance )
            {
                k++;
            }
            osg::ref_ptr< osg::Vec3Array > candidate = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
            WBoundingBox cBB;
            while( ( k < len ) &&  std::abs( slicePos - fibVerts->at( ( 3 * k ) + sidx + sliceNum ) ) <= maxDistance )
            {
                wmath::WPosition cv( fibVerts->at( ( 3 * k ) + sidx ),
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
                osg::ref_ptr< osg::Vec4Array > candidateColors = colorVertices( candidate, probThreshold );
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

    return std::make_pair( intersectionGeode, projectionGeode );
}
