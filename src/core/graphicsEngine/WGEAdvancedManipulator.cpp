//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community
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

#include "WGEAdvancedManipulator.h"
#include "WGraphicsEngine.h"

WGEAdvancedManipulator::WGEAdvancedManipulator():
    TrackballManipulator(),
    m_zoom( 100.0 ),
    m_allowThrow( false ),
    m_paintMode( 0 )
{
    setTrackballSize( .3 ); // changes the effect of a mouse move for rotation
}

void WGEAdvancedManipulator::setByMatrix( const osg::Matrixd& matrix )
{
    m_zoom = 1.0 / matrix.getScale()[0];

    // The zoom needs to be undone before forwarding the matrix.
    TrackballManipulator::setByMatrix( osg::Matrixd::inverse( osg::Matrixd::scale( 1.0 / m_zoom, 1.0 / m_zoom, 1.0 / m_zoom ) ) * matrix );
}

osg::Matrixd WGEAdvancedManipulator::getMatrix() const
{
    return osg::Matrixd::scale( 1.0 / m_zoom, 1.0 / m_zoom, 1.0 / m_zoom ) * TrackballManipulator::getMatrix();
}

osg::Matrixd WGEAdvancedManipulator::getMatrixWithoutZoom() const
{
    return TrackballManipulator::getMatrix();
}

osg::Matrixd WGEAdvancedManipulator::getInverseMatrix() const
{
    return TrackballManipulator::getInverseMatrix() * osg::Matrixd::scale( m_zoom, m_zoom, m_zoom );
}

void WGEAdvancedManipulator::home( double /* currentTime */ )
{
    osg::Vec3d center;
    osg::Vec3d dummy;
    TrackballManipulator::getHomePosition( dummy, center, dummy);

    m_zoom = 100.0 / center[0];

    TrackballManipulator::home( 0 /* currentTime */ );
}

bool WGEAdvancedManipulator::zoom( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    double zoomDelta = 0.0;

    if( ea.getKey() && ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN )
    {
        if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Minus )
        {
            zoomDelta = -0.05;
        }
        if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Plus )
        {
            zoomDelta = 0.05;
        }
        if( zoomDelta != 0.0 )
        {
            m_zoom *= 1.0 + zoomDelta;
            us.requestRedraw();
        }
    }
    else
    {
        if( ea.getHandled() )
        {
            return true;
        }

        switch( ea.getScrollingMotion() )
        {
            case osgGA::GUIEventAdapter::SCROLL_UP:
                zoomDelta = 0.05;
                break;
            case osgGA::GUIEventAdapter::SCROLL_DOWN:
                zoomDelta = -0.05;
                break;
            case osgGA::GUIEventAdapter::SCROLL_2D:
                zoomDelta = 0.05 / 120.0 * ea.getScrollingDeltaY();
                break;
                // case osgGA::GUIEventAdapter::SCROLL_LEFT:
                // case osgGA::GUIEventAdapter::SCROLL_RIGHT:
                // case osgGA::GUIEventAdapter::SCROLL_NONE:
            default:
                // do nothing
                zoomDelta = 0.0;
                break;
        }
    }

    if( zoomDelta != 0.0 )
    {
        m_zoom *= 1.0 + zoomDelta;
        us.requestRedraw();
    }

    us.requestContinuousUpdate( false );
    return true;
}

bool WGEAdvancedManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    _thrown &= m_allowThrow; // By default we do not want the auto-rotation thingy.

    if( WGraphicsEngine::getGraphicsEngine()->getScene()->isHomePositionRequested()
        || ( ea.getKey() == osgGA::GUIEventAdapter::KEY_Space
             && ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN )
        )
    {
        // We set the scene to the manipulator home position if the scene
        // requests to do so. See WGEScene for more details.
        home( 0 );
        us.requestRedraw();
        return true;
    }
    else if( ea.getEventType() == osgGA::GUIEventAdapter::SCROLL
             || ea.getKey() == osgGA::GUIEventAdapter::KEY_Minus
             || ea.getKey() == osgGA::GUIEventAdapter::KEY_Plus )
    {
        return zoom( ea, us );
    }
    // NOTE: we need to ignore the right mouse-button drag! This manipulates the underlying Trackball Manipulator while, at the same time, is
    // used for moving ROIS! Zooming is done using Scroll Wheel or +/- keys.
    else if( ( ea.getEventType() == osgGA::GUIEventAdapter::DRAG ) || ( ea.getEventType() == osgGA::GUIEventAdapter::PUSH ) )
    {
        if( ea.getButtonMask() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
        {
            return true;
        }
        else if(  ( ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ) && ( m_paintMode == 1 ) )
        {
            return true;
        }
        else
        {
            return TrackballManipulator::handle( ea, us );
        }
    }
    else
    {
        return TrackballManipulator::handle( ea, us );
    }
}

void WGEAdvancedManipulator::setPaintMode( int mode )
{
    m_paintMode = mode;
}

void WGEAdvancedManipulator::setThrow( bool allowThrow )
{
    m_allowThrow = allowThrow;
}

bool WGEAdvancedManipulator::getThrow() const
{
    return m_allowThrow;
}
