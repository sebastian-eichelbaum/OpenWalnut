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
#include "WLabelsTransformCallback.h"


WLabelsTransformCallback::WLabelsTransformCallback( WPropDouble yPos, WPropDouble ySpacing )
    : m_currentYPos( 0.0 ),
      m_currentYSpacing( 1.0 ),
      m_yPos( yPos ),
      m_ySpacing( ySpacing )
{
}

void WLabelsTransformCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    const double yPos = m_yPos->get();
    const double ySpacing = m_ySpacing->get();

    if( yPos != m_currentYPos || ySpacing != m_currentYSpacing )
    {
        osg::MatrixTransform* transform = static_cast< osg::MatrixTransform* >( node );
        if( transform )
        {
            transform->setMatrix( osg::Matrix::scale( 1.0, ySpacing, 1.0 ) * osg::Matrix::translate( 0.0, -yPos, 0.0 ) );
        }

        m_currentYPos = yPos;
        m_currentYSpacing = ySpacing;
    }

    traverse( node, nv );
}
