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
#include "WSPSliceGeodeBuilder.h"

WSPSliceGeodeBuilder::WSPSliceGeodeBuilder( ProbTractList probTracts, boost::shared_ptr< const WDataSetFibers > detTracts, WPropGroup sliceGroup )
    : m_detTracts( detTracts ),
      m_probTracts( probTracts )
{
    if( m_probTracts.empty() || !m_detTracts )
    {
        wlog::warn( "WSPSliceGeodeBuilder" ) << "No probabilistic tractograms were given for slice geode generation!";
    }
    wlog::debug( "WSPSliceGeodeBuilder" ) << "Init geode builder...";
    m_zPos = sliceGroup->findProperty( "Axial Position" )->toPropInt();
    m_yPos = sliceGroup->findProperty( "Coronal Position" )->toPropInt();
    m_xPos = sliceGroup->findProperty( "Sagittal Position" )->toPropInt();

    m_showonZ = sliceGroup->findProperty( "Show Axial" )->toPropBool();
    m_showonY = sliceGroup->findProperty( "Show Coronal" )->toPropBool();
    m_showonX = sliceGroup->findProperty( "Show Sagittal" )->toPropBool();

    checkAndExtractGrids();

    // compute BB for each deterministic tract
    m_tractBB.clear();
    m_tractBB.reserve( detTracts->size() );
    for( size_t i = 0; i < detTracts->size(); ++i )
    {
        m_tractBB.push_back( wmath::computeBoundingBox( ( *detTracts )[i] ) );
    }

    computeSliceBB(); // just to be sure those are initialized, since they may change due to m_xPos, et al. anyway
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
    m_xSliceBB = WBoundingBox( m_grid->getOrigin() + m_xPos->get() * m_grid->getDirectionX(),
            m_grid->getOrigin() + m_xPos->get() * m_grid->getDirectionX() + m_grid->getNbCoordsY() * m_grid->getDirectionY() +
            m_grid->getNbCoordsZ() * m_grid->getDirectionZ() );
    m_ySliceBB = WBoundingBox( m_grid->getOrigin() + m_yPos->get() * m_grid->getDirectionY(),
            m_grid->getOrigin() + m_yPos->get() * m_grid->getDirectionY() + m_grid->getNbCoordsX() * m_grid->getDirectionX() +
            m_grid->getNbCoordsZ() * m_grid->getDirectionZ() );
    m_zSliceBB = WBoundingBox( m_grid->getOrigin() + m_zPos->get() * m_grid->getDirectionZ(),
            m_grid->getOrigin() + m_zPos->get() * m_grid->getDirectionZ() + m_grid->getNbCoordsY() * m_grid->getDirectionY() +
            m_grid->getNbCoordsX() * m_grid->getDirectionX() );
}

void WSPSliceGeodeBuilder::determineIntersectingDeterministicTracts()
{
    computeSliceBB();

    // tidy up old intersections
    m_xIntersections.clear();
    m_yIntersections.clear();
    m_zIntersections.clear();

    for( size_t tract = 0; tract <  m_detTracts->getLineStartIndexes()->size() ; ++tract )
    {
        if( m_showonX->get() && m_xSliceBB.intersects( m_tractBB[ tract ] ) )
        {
            m_xIntersections.push_back( tract );
        }
        if( m_showonY->get() && m_ySliceBB.intersects( m_tractBB[ tract ] ) )
        {
            m_yIntersections.push_back( tract );
        }
        if( m_showonZ->get() && m_zSliceBB.intersects( m_tractBB[ tract ] ) )
        {
            m_zIntersections.push_back( tract );
        }
    }
}
void WSPSliceGeodeBuilder::projectTractOnSlice( unsigned char component, osg::ref_ptr< osg::Vec3Array > vertices, int slicePos ) const
{
    WAssert( vertices, "Bug: a given internal array was not expected empty here!" );
    WAssert( component <= 2, "Bug: The selected component ( 0 == x, 1 == y, 2 == z ) was invalid" );

    for( osg::Vec3Array::iterator vertex = vertices->begin(); vertex != vertices->end(); ++vertex )
    {
        ( *vertex )[ component ] = slicePos;
    }
}

osg::ref_ptr< osg::Geode > WSPSliceGeodeBuilder::generateSlice( std::list< size_t > intersections, int slicePos, unsigned char component,
        double maxDistance ) const
{
    WAssert( component <= 2, "Bug: The selected component ( 0 == x, 1 == y, 2 == z ) was invalid" );
    osg::ref_ptr< osg::Geode > geode = new osg::Geode();
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    colors->push_back( osg::Vec4( 1.0, 0.0, 0.0, 1.0 ) );
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry );

    osg::ref_ptr< osg::Vec3Array > pv = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Geometry > pg = osg::ref_ptr< osg::Geometry >( new osg::Geometry );

    WDataSetFibers::VertexArray fibVerts = m_detTracts->getVertices();

    size_t currentPos = 0;
    for( std::list< size_t >::const_iterator cit = intersections.begin(); cit != intersections.end(); ++cit )
    {
        size_t sidx = m_detTracts->getLineStartIndexes()->at( *cit ) * 3;
        size_t len = m_detTracts->getLineLengths()->at( *cit );
        size_t k = 0;
        while( k < len )
        {
            size_t startIdx = vertices->size();
            // proceed to a vertex inside the deltaX environment
            while( ( k < len ) && std::abs( slicePos - fibVerts->at( ( 3 * k ) + sidx + component ) ) > maxDistance )
            {
                k++;
            }
            osg::ref_ptr< osg::Vec3Array > candidate = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
            WBoundingBox cBB;
            while( ( k < len ) &&  std::abs( slicePos - fibVerts->at( ( 3 * k ) + sidx + component ) ) <= maxDistance )
            {
                wmath::WPosition cv( fibVerts->at( ( 3 * k ) + sidx ),
                            fibVerts->at( ( 3 * k ) + sidx + 1 ),
                            fibVerts->at( ( 3 * k ) + sidx + 2 ) );
                candidate->push_back( cv );
                cBB.expandBy( cv );
                k++;
            }
            if( !candidate->empty() && cBB.intersects( m_xSliceBB ) )
            {
                vertices->insert( vertices->end(), candidate->begin(), candidate->end() );
                geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, startIdx, candidate->size() ) );
                projectTractOnSlice( component, candidate, slicePos );
                pv->insert( pv->end(), candidate->begin(), candidate->end() );
                pg->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, startIdx, candidate->size() ) );
            }
            // note: this loop will terminate since either the first while loop is true or the second or both!
        }
    }
    osg::ref_ptr< osg::Vec4Array > pc = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    pc->push_back( osg::Vec4( 0.0, 1.0, 0.0, 1.0 ) );
    pg->setVertexArray( pv );
    pg->setColorArray( pc );
    pg->setColorBinding( osg::Geometry::BIND_OVERALL );

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geode->addDrawable( geometry );
    geode->addDrawable( pg );

    return geode;
}

osg::ref_ptr< osg::Geode > WSPSliceGeodeBuilder::generateXSlice( double maxDistance ) const
{
    return generateSlice( m_xIntersections, m_xPos->get(), 0, maxDistance );
}

osg::ref_ptr< osg::Geode > WSPSliceGeodeBuilder::generateYSlice( double maxDistance ) const
{
    return generateSlice( m_yIntersections, m_yPos->get(), 1, maxDistance );
}

osg::ref_ptr< osg::Geode > WSPSliceGeodeBuilder::generateZSlice( double maxDistance ) const
{
    return generateSlice( m_zIntersections, m_zPos->get(), 2, maxDistance );
}

