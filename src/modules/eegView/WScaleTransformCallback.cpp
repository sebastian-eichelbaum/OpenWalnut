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

#include <cstddef>

#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/NodeVisitor>

#include "core/common/WPropertyTypes.h"
#include "core/common/WPropertyVariable.h"
#include "WScaleTransformCallback.h"


WScaleTransformCallback::WScaleTransformCallback( std::size_t channelID,
                                                  WPropDouble ySpacing,
                                                  WPropDouble ySensitivity )
    : m_channelID( channelID ),
      m_currentYSpacing( 0.0 ),
      m_currentYSensitivity( 1.0 ),
      m_ySpacing( ySpacing ),
      m_ySensitivity( ySensitivity )
{
}

void WScaleTransformCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    const double ySpacing = m_ySpacing->get();
    const double ySensitivity = m_ySensitivity->get();

    if( ySpacing != m_currentYSpacing || ySensitivity != m_currentYSensitivity )
    {
        osg::MatrixTransform* transform = static_cast< osg::MatrixTransform* >( node );
        if( transform )
        {
            transform->setMatrix( osg::Matrix::scale( 1.0, 1.0 / ySensitivity, 1.0 )
                                  * osg::Matrix::translate( 0.0, -ySpacing * m_channelID, 0.0 ) );
        }

        m_currentYSpacing = ySpacing;
        m_currentYSensitivity = ySensitivity;
    }

    traverse( node, nv );
}
