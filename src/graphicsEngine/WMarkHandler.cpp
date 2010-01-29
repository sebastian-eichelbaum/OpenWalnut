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

#include "WMarkHandler.h"


WMarkHandler::WMarkHandler()
    : m_positionFlag( new WCondition, -1.0 )
{
}

WFlag< double >* WMarkHandler::getPositionFlag()
{
    return &m_positionFlag;
}

WMarkHandler::~WMarkHandler()
{
}

bool WMarkHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    bool handled = false;
    switch( ea.getEventType() )
    {
        case osgGA::GUIEventAdapter::PUSH:
        case osgGA::GUIEventAdapter::DRAG:
            if( ea.getButtonMask() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
            {
                handled = calculateNewPosition( ea, aa );
            }
            break;
        default:
            // do nothing
            break;
    }
    return handled;
}

bool WMarkHandler::calculateNewPosition( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    bool handled = false;
    osg::View* view = aa.asView();
    if( view )
    {
        osg::Camera* camera = view->getCamera();
        if( camera )
        {
            osg::Matrixd matrix = camera->getViewMatrix();
            matrix.postMult( camera->getProjectionMatrix() );
            if( camera->getViewport() )
            {
                matrix.postMult( camera->getViewport()->computeWindowMatrix() );
            }

            osg::Vec3d selectedPoint = osg::Vec3d( ea.getX(), ea.getY(), 0.0 ) * osg::Matrixd::inverse( matrix );

            m_positionFlag.set( selectedPoint.x() );

            handled = true;
        }
    }
    return handled;
}
