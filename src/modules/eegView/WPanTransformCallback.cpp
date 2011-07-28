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

#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/NodeVisitor>

#include "core/common/WPropertyTypes.h"
#include "core/common/WPropertyVariable.h"
#include "WPanTransformCallback.h"


WPanTransformCallback::WPanTransformCallback( WPropInt labelsWidth,
                                              WPropDouble timePos,
                                              WPropDouble timeRange,
                                              WPropInt graphWidth,
                                              WPropDouble yPos )
    : m_currentLabelsWidth( 0 ),
      m_currentTimePos( 0.0 ),
      m_currentTimeRange( 1.0 ),
      m_currentGraphWidth( 1 ),
      m_currentYPos( 0.0 ),
      m_labelsWidth( labelsWidth ),
      m_timePos( timePos ),
      m_timeRange( timeRange ),
      m_graphWidth( graphWidth ),
      m_yPos( yPos )
{
}

void WPanTransformCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    const int labelsWidth = m_labelsWidth->get();
    const double timePos = m_timePos->get();
    const double timeRange = m_timeRange->get();
    const int graphWidth = m_graphWidth->get();
    const double yPos = m_yPos->get();

    if( labelsWidth != m_currentLabelsWidth || timePos != m_currentTimePos || timeRange != m_currentTimeRange
        || graphWidth != m_currentGraphWidth || yPos != m_currentYPos )
    {
        osg::MatrixTransform* transform = static_cast< osg::MatrixTransform* >( node );
        if( transform )
        {
            transform->setMatrix( osg::Matrix::scale( graphWidth / timeRange, 1.0, 1.0 )
                                  * osg::Matrix::translate( labelsWidth - timePos * graphWidth / timeRange, -yPos, 0.0 ) );
        }

        m_currentLabelsWidth = labelsWidth;
        m_currentTimePos = timePos;
        m_currentTimeRange = timeRange;
        m_currentGraphWidth = graphWidth;
        m_currentYPos = yPos;
    }

    traverse( node, nv );
}
