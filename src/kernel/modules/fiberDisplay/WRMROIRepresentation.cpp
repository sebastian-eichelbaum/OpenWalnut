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
#include <string>
#include <vector>

#include "../../../dataHandler/WDataSetScalar.h"

#include "../../../graphicsEngine/WGraphicsEngine.h"
#include "../../../graphicsEngine/WROIBox.h"
#include "../../../graphicsEngine/WROIArbitrary.h"

#include "WRMBranch.h"
#include "WROIManagerFibers.h"

#include "WRMROIRepresentation.h"

WRMROIRepresentation::WRMROIRepresentation( osg::ref_ptr< WROI > roi, boost::shared_ptr< WRMBranch > branch  ) :
    m_dirty( true ),
    m_roi( roi ),
    m_branch( branch )
{
    m_size = m_branch->getRoiManager()->size();

    m_currentArray = m_branch->getRoiManager()->getDataSet()->getVertices();
    m_currentReverse = m_branch->getRoiManager()->getDataSet()->getVerticesReverse();
    m_kdTree = m_branch->getRoiManager()->getKdTree();

    properties();

    roi->getSignalIsModified()->connect( boost::bind( &WRMROIRepresentation::setDirty, this ) );
}

WRMROIRepresentation::~WRMROIRepresentation()
{
    removeFromGE();
}

void WRMROIRepresentation::properties()
{
    m_properties = boost::shared_ptr< WProperties >( new WProperties( "Properties", "This ROI's properties" ) );
    m_isNot = m_properties->addProperty( "NOT", "description", false, boost::bind( &WRMROIRepresentation::propertyChanged, this ) );
    m_threshold = m_properties->addProperty( "Threshold", "description", 0., boost::bind( &WRMROIRepresentation::propertyChanged, this ) );
    m_threshold->setHidden( true );
    m_isActive = m_properties->addProperty( "active", "description", true, boost::bind( &WRMROIRepresentation::propertyChanged, this ) );
    m_isActive->setHidden( true );

    if ( osg::dynamic_pointer_cast<WROIArbitrary>( m_roi ).get() )
    {
        m_threshold->set( osg::dynamic_pointer_cast<WROIArbitrary>( m_roi ).get()->getThreshold() );
        m_threshold->setHidden( false );
        m_threshold->setMax( osg::dynamic_pointer_cast<WROIArbitrary>( m_roi ).get()->getMaxThreshold() );
    }
}

void WRMROIRepresentation::propertyChanged()
{
    m_roi->setNot( m_isNot->get() );
    m_roi->setActive( m_isActive->get() );

    if ( m_isActive->get() )
    {
        m_roi->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_roi->setNodeMask( 0x0 );
    }
    if ( osg::dynamic_pointer_cast< WROIArbitrary >( m_roi ).get() )
    {
        osg::dynamic_pointer_cast< WROIArbitrary >( m_roi ).get()->setThreshold( m_threshold->get() );
        setDirty();
    }
    setDirty();
}


void WRMROIRepresentation::removeFromGE()
{
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_roi );
}

osg::ref_ptr< WROI > WRMROIRepresentation::getROI()
{
    return m_roi;
}

boost::shared_ptr< std::vector< bool > > WRMROIRepresentation::getBitField()
{
    if ( m_dirty )
    {
        recalculate();
    }
    return m_outputBitfield;
}

void WRMROIRepresentation::recalculate()
{
    m_workerBitfield = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_size, false ) );

    if ( osg::dynamic_pointer_cast<WROIBox>( m_roi ).get() )
    {
        //std::cout << "roi recalc" << std::endl;
        m_boxMin.resize( 3 );
        m_boxMax.resize( 3 );

        osg::ref_ptr<WROIBox> box = osg::dynamic_pointer_cast<WROIBox>( m_roi );

        m_boxMin[0] = box->getMinPos()[0];
        m_boxMax[0] = box->getMaxPos()[0];
        m_boxMin[1] = box->getMinPos()[1];
        m_boxMax[1] = box->getMaxPos()[1];
        m_boxMin[2] = box->getMinPos()[2];
        m_boxMax[2] = box->getMaxPos()[2];

        boxTest( 0, m_currentArray->size() / 3 - 1, 0 );
    }

    if ( osg::dynamic_pointer_cast<WROIArbitrary>( m_roi ).get() )
    {
        osg::ref_ptr<WROIArbitrary>roi = osg::dynamic_pointer_cast<WROIArbitrary>( m_roi );

        float threshold = static_cast<float>( roi->getThreshold() );

        size_t nx = roi->getCoordDimensions()[0];
        size_t ny = roi->getCoordDimensions()[1];

        double dx = roi->getCoordOffsets()[0];
        double dy = roi->getCoordOffsets()[1];
        double dz = roi->getCoordOffsets()[2];

        for ( size_t i = 0; i < m_currentArray->size()/3; ++i )
        {
            size_t x = static_cast<size_t>( ( *m_currentArray )[i * 3 ] / dx );
            size_t y = static_cast<size_t>( ( *m_currentArray )[i * 3 + 1] / dy );
            size_t z = static_cast<size_t>( ( *m_currentArray )[i * 3 + 2] / dz );
            int index = x + y * nx + z * nx * ny;

            if ( static_cast<float>( roi->getValue( index ) ) - threshold > 0.1 )
            {
                ( *m_workerBitfield )[getLineForPoint( i )] = 1;
            }
        }
    }
    m_dirty = false;
    m_outputBitfield = m_workerBitfield;
}

void WRMROIRepresentation::boxTest( int left, int right, int axis )
{
    // abort condition
    if ( left > right )
        return;

    int root = left + ( ( right - left ) / 2 );
    int axis1 = ( axis + 1 ) % 3;
    int pointIndex = m_kdTree->m_tree[root] * 3;

    if ( ( *m_currentArray )[pointIndex + axis]  < m_boxMin[axis] )
    {
        boxTest( root + 1, right, axis1 );
    }
    else if ( ( *m_currentArray )[pointIndex + axis] > m_boxMax[axis] )
    {
        boxTest( left, root - 1, axis1 );
    }
    else
    {
        int axis2 = ( axis + 2 ) % 3;
        if ( ( *m_currentArray )[pointIndex + axis1] <= m_boxMax[axis1] && ( *m_currentArray )[pointIndex + axis1]
                >= m_boxMin[axis1] && ( *m_currentArray )[pointIndex + axis2] <= m_boxMax[axis2]
                && ( *m_currentArray )[pointIndex + axis2] >= m_boxMin[axis2] )
        {
            ( *m_workerBitfield )[getLineForPoint( m_kdTree->m_tree[root] )] = 1;
        }
        boxTest( left, root - 1, axis1 );
        boxTest( root + 1, right, axis1 );
    }
}

void WRMROIRepresentation::setDirty()
{
    m_dirty = true;
    m_branch->setDirty();
}

boost::shared_ptr< WProperties > WRMROIRepresentation::getProperties()
{
    return m_properties;
}

boost::shared_ptr< WRMBranch > WRMROIRepresentation::getBranch()
{
    return m_branch;
}
