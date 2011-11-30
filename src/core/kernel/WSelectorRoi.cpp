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

#include <vector>

//#include "../../../graphicsEngine/WGraphicsEngine.h"
#include "../graphicsEngine/WROIBox.h"
#include "../graphicsEngine/WROIArbitrary.h"
#include "WKdTree.h"

#include "WSelectorRoi.h"


WSelectorRoi::WSelectorRoi( osg::ref_ptr< WROI > roi, boost::shared_ptr< const WDataSetFibers > fibers, boost::shared_ptr< WKdTree> kdTree ) :
    m_roi( roi ),
    m_fibers( fibers ),
    m_kdTree( kdTree ),
    m_size( fibers->size() ),
    m_dirty( true )
{
    m_bitField = boost::shared_ptr< std::vector<bool> >( new std::vector<bool>( m_size, false ) );

    m_currentArray = m_fibers->getVertices();
    m_currentReverse = m_fibers->getVerticesReverse();

    m_changeRoiSignal
        = boost::shared_ptr< boost::function< void() > >( new boost::function< void() >( boost::bind( &WSelectorRoi::setDirty, this ) ) );
    m_roi->addROIChangeNotifier( m_changeRoiSignal );
}

WSelectorRoi::~WSelectorRoi()
{
    m_roi->removeROIChangeNotifier( m_changeRoiSignal );
}

void WSelectorRoi::setDirty()
{
    m_dirty = true;
}

void WSelectorRoi::recalculate()
{
    m_workerBitfield = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_size, false ) );

    if( osg::dynamic_pointer_cast<WROIBox>( m_roi ).get() )
    {
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

    if( osg::dynamic_pointer_cast<WROIArbitrary>( m_roi ).get() )
    {
        osg::ref_ptr<WROIArbitrary>roi = osg::dynamic_pointer_cast<WROIArbitrary>( m_roi );

        float threshold = static_cast<float>( roi->getThreshold() );

        size_t nx = roi->getCoordDimensions()[0];
        size_t ny = roi->getCoordDimensions()[1];

        double dx = roi->getCoordOffsets()[0];
        double dy = roi->getCoordOffsets()[1];
        double dz = roi->getCoordOffsets()[2];

        for( size_t i = 0; i < m_currentArray->size()/3; ++i )
        {
            size_t x = static_cast<size_t>( ( *m_currentArray )[i * 3 ] / dx );
            size_t y = static_cast<size_t>( ( *m_currentArray )[i * 3 + 1] / dy );
            size_t z = static_cast<size_t>( ( *m_currentArray )[i * 3 + 2] / dz );
            int index = x + y * nx + z * nx * ny;

            if( static_cast<float>( roi->getValue( index ) ) - threshold > 0.1 )
            {
                ( *m_workerBitfield )[getLineForPoint( i )] = 1;
            }
        }
    }
    m_dirty = false;
    m_bitField = m_workerBitfield;
}

void WSelectorRoi::boxTest( int left, int right, int axis )
{
    // abort condition
    if( left > right )
        return;

    int root = left + ( ( right - left ) / 2 );
    int axis1 = ( axis + 1 ) % 3;
    int pointIndex = m_kdTree->m_tree[root] * 3;

    if( ( *m_currentArray )[pointIndex + axis]  < m_boxMin[axis] )
    {
        boxTest( root + 1, right, axis1 );
    }
    else if( ( *m_currentArray )[pointIndex + axis] > m_boxMax[axis] )
    {
        boxTest( left, root - 1, axis1 );
    }
    else
    {
        int axis2 = ( axis + 2 ) % 3;
        if( ( *m_currentArray )[pointIndex + axis1] <= m_boxMax[axis1] && ( *m_currentArray )[pointIndex + axis1]
                >= m_boxMin[axis1] && ( *m_currentArray )[pointIndex + axis2] <= m_boxMax[axis2]
                && ( *m_currentArray )[pointIndex + axis2] >= m_boxMin[axis2] )
        {
            ( *m_workerBitfield )[getLineForPoint( m_kdTree->m_tree[root] )] = 1;
        }
        boxTest( left, root - 1, axis1 );
        boxTest( root + 1, right, axis1 );
    }
}
