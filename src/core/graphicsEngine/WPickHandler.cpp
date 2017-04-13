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

#include <string>

#include "../common/WLogger.h"

#include "WPickHandler.h"

WPickHandler::WPickHandler()
    : m_hitResult( WPickInfo() ),
      m_startPick( WPickInfo() ),
      m_shift( false ),
      m_ctrl( false ),
      m_viewerName( "" ),
      m_paintMode( 0 ),
      m_mouseButton( WPickInfo::NOMOUSE ),
      m_inPickMode( false ),
      m_scrollWheel( 0 )
{
}

WPickHandler::WPickHandler( std::string viewerName )
    : m_hitResult( WPickInfo() ),
      m_startPick( WPickInfo() ),
      m_shift( false ),
      m_ctrl( false ),
      m_viewerName( viewerName ),
      m_paintMode( 0 ),
      m_mouseButton( WPickInfo::NOMOUSE ),
      m_inPickMode( false ),
      m_scrollWheel( 0 )
{
}

WPickHandler::~WPickHandler()
{
}

WPickInfo WPickHandler::getHitResult()
{
    return m_hitResult;
}

boost::signals2::signal< void( WPickInfo ) >* WPickHandler::getPickSignal()
{
    return &m_pickSignal;
}

bool WPickHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    switch( ea.getEventType() )
    {
        case osgGA::GUIEventAdapter::DRAG : // Mouse pushed an dragged
        case osgGA::GUIEventAdapter::PUSH : // Mousebutton pushed
        {
            unsigned int buttonMask = ea.getButtonMask();
            if( buttonMask == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
            {
                m_mouseButton = WPickInfo::MOUSE_RIGHT;
                osgViewer::View* view = static_cast< osgViewer::View* >( &aa );
                if( view )
                {
                    pick( view, ea );
                }
            }
            if( ( buttonMask == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ) && ( m_paintMode == 1 ) )
            {
                m_mouseButton = WPickInfo::MOUSE_LEFT;
                osgViewer::View* view = static_cast< osgViewer::View* >( &aa );
                if( view )
                {
                    pick( view, ea );
                }
            }
            return false;
        }
        case osgGA::GUIEventAdapter::RELEASE : // Mousebutton released
        {
            m_mouseButton = WPickInfo::NOMOUSE;
            osgViewer::View* view = static_cast< osgViewer::View* >( &aa );
            if( view )
            {
                unpick();
            }
            return false;
        }
        case osgGA::GUIEventAdapter::SCROLL : // Wheel
        {
            if( m_inPickMode )
            {
                switch( ea.getScrollingMotion() )
                {
                    case osgGA::GUIEventAdapter::SCROLL_UP:
                        m_scrollWheel++;
                        break;
                    case osgGA::GUIEventAdapter::SCROLL_DOWN:
                        m_scrollWheel--;
                    case osgGA::GUIEventAdapter::SCROLL_2D:
                        // FIXME: the osg doc tells us nothing about this value, but is seems to be always 120 or -120
                        if( ea.getScrollingDeltaY() > 0 )
                        {
                            m_scrollWheel++;
                        }
                        else
                        {
                            m_scrollWheel--;
                        }
                        break;
                    default:
                        break;
                }

                // handle as pick event
                osgViewer::View* view = static_cast< osgViewer::View* >( &aa );
                if( view )
                {
                    pick( view, ea );
                }
                ea.setHandled( true );
                return true;
            }
            return false;
        }
        case osgGA::GUIEventAdapter::KEYUP : // Key on keyboard released.
        {
            m_shift = false;
            m_ctrl = false;
            return false;
        }
        case osgGA::GUIEventAdapter::KEYDOWN : // Key on keyboard pushed.
        {
            if( ea.getKey() == 'c' )
            {
                osgViewer::View* view = static_cast< osgViewer::View* >( &aa );
                osg::ref_ptr< osgGA::GUIEventAdapter > event = new osgGA::GUIEventAdapter( ea );
                event->setX( ( ea.getXmin() + ea.getXmax() ) * 0.5 );
                event->setY( ( ea.getYmin() + ea.getYmax() ) * 0.5 );
                if( view )
                {
                    pick( view, *event );
                }
            }
            if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Shift_L )
            {
                m_shift = true;
            }
            if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Control_L ||  ea.getKey() == osgGA::GUIEventAdapter::KEY_Control_R )
            {
                m_ctrl = true;
            }
            return false;
        }
        default:
            return false;
    }
}

void WPickHandler::unpick( )
{
    m_inPickMode = false;
    if( m_hitResult != WPickInfo() )
    {
        m_hitResult = WPickInfo( "unpick", m_viewerName, WPosition(), std::make_pair( 0, 0 ), WPickInfo::NONE );
        m_startPick = WPickInfo();
        m_scrollWheel = 0;
    }
    m_pickSignal( getHitResult() );
}

std::string extractSuitableName( osgUtil::LineSegmentIntersector::Intersections::iterator hitr )
{
    if( !hitr->nodePath.empty() && !( hitr->nodePath.back()->getName().empty() ) )
    {
        return hitr->nodePath.back()->getName();
    }
    else if( hitr->drawable.valid() )
    {
        return  hitr->drawable->className();
    }
    assert( 0 && "This should not happen. Tell \"wiebel\" if it does." );
    return ""; // This line will not be reached.
}

void WPickHandler::updatePickInfoModifierKeys( WPickInfo* pickInfo )
{
    if( m_shift )
    {
        pickInfo->setModifierKey( WPickInfo::SHIFT );
    }

    if( m_ctrl )
    {
        pickInfo->setModifierKey( WPickInfo::STRG );
    }
}

void WPickHandler::pick( osgViewer::View* view, const osgGA::GUIEventAdapter& ea )
{
    osgUtil::LineSegmentIntersector::Intersections intersections;
    m_hitResult = WPickInfo();
    float x = ea.getX(); // pixel position in x direction
    float y = ea.getY(); // pixel position in x direction

    WPickInfo pickInfo;

    updatePickInfoModifierKeys( &pickInfo );

    // if we are in another viewer than the main view we just need the pixel position
    if( m_viewerName != "" && m_viewerName != "Main View" )
    {
        pickInfo = WPickInfo( "", m_viewerName, m_startPick.getPickPosition(), std::make_pair( x, y ),
                              m_startPick.getModifierKey(), m_mouseButton, m_startPick.getPickNormal(), m_scrollWheel );
        m_hitResult = pickInfo;

        // if nothing was picked before remember the currently picked.
        m_startPick = pickInfo;

        m_pickSignal( getHitResult() );

        return;
    }

    bool intersetionsExist = view->computeIntersections( x, y, intersections, 0xFFFFFFF0 );

    // if something is picked, get the right thing from the list, because it might be hidden.
    bool startPickIsStillInList = false;
    osgUtil::LineSegmentIntersector::Intersections::iterator hitr;
    if( intersetionsExist )
    {
        assert( intersections.size() );
        hitr = intersections.begin();

        bool ignoreFirst = m_ctrl;

        while( hitr != intersections.end() )
        {
            std::string nodeName = extractSuitableName( hitr );
            // now we skip everything that starts with an underscore if not in paint mode
            if(  nodeName[0] == '_' && ( m_paintMode == 0  ) )
            {
                ++hitr;
            }
            // // now we skip stuff with non-expressive names often used
            // else if( nodeName == "Geometry" )
            // {
            //     ++hitr;
            // }
            // if ctrl is pressed we skip the first thing that gets hit by the pick
            else if( ignoreFirst )
            {
                ++hitr;
                ignoreFirst = false;
            }
            else
            {
                break;
            }
        }

        if( hitr == intersections.end() )
        {
            // after everything was ignored nothing pickable remained and we have nothing picked before
            // we just stop further processing.
            if(  m_startPick.getName() == "" )
            {
                return;
            }
        }

        // if we have a previous pick we search for it in the list
        if( m_startPick.getName() != ""  && m_startPick.getName() != "unpick" )
        {
            while( ( hitr != intersections.end() ) && !startPickIsStillInList )
            {
                WPickInfo pickInfoTmp( extractSuitableName( hitr ), m_viewerName, WPosition(), std::make_pair( 0, 0 ), WPickInfo::NONE );
                startPickIsStillInList |= ( pickInfoTmp.getName() == m_startPick.getName() );

                if( !startPickIsStillInList ) // if iteration not finished yet go on in list
                {
                    ++hitr;
                }
            }
        }
    } // end of if( intersetionsExist )
    else
    {
        // if we found no intersection and we have noting picked before
        // we want to return "nothing" in order to provide the pixel coordinates
        // even though we did not hit anything.
        if(  m_startPick.getName() == "" )
        {
            pickInfo = WPickInfo( "nothing", m_viewerName, WPosition( 0.0, 0.0, 0.0 ), std::make_pair( x, y ),
                                  m_startPick.getModifierKey(), m_mouseButton, WVector3d( 0.0, 0.0, 0.0 ), m_scrollWheel );

            m_hitResult = pickInfo;
            m_pickSignal( getHitResult() );
            return;
        }
    }

    // Set the new pickInfo if the previously picked is still in list or we have a pick in conjunction with previously no pick
    if( startPickIsStillInList || ( intersetionsExist && ( m_startPick.getName() == "unpick" || m_startPick.getName() == "" ) ) )
    {
        // if nothing was picked before, or the previously picked was found: set new pickInfo
        WPosition pickPos;
        pickPos[0] = hitr->getWorldIntersectPoint()[0];
        pickPos[1] = hitr->getWorldIntersectPoint()[1];
        pickPos[2] = hitr->getWorldIntersectPoint()[2];

        WVector3d pickNormal;
        pickNormal[0] = hitr->getWorldIntersectNormal()[0];
        pickNormal[1] = hitr->getWorldIntersectNormal()[1];
        pickNormal[2] = hitr->getWorldIntersectNormal()[2];
        pickInfo = WPickInfo( extractSuitableName( hitr ), m_viewerName, pickPos, std::make_pair( x, y ),
                              pickInfo.getModifierKey(), m_mouseButton, pickNormal, m_scrollWheel );
    }

    // Use the old PickInfo with updated pixel info if we have previously picked something but the old is not in list anymore
    if( !startPickIsStillInList && m_startPick.getName() != ""  && m_startPick.getName() != "unpick" )
    {
        pickInfo = WPickInfo( m_startPick.getName(), m_viewerName, m_startPick.getPickPosition(), std::make_pair( x, y ),
                              m_startPick.getModifierKey(), m_mouseButton, m_startPick.getPickNormal(), m_scrollWheel );
    }

    m_hitResult = pickInfo;

    // if nothing was picked before remember the currently picked.
    m_startPick = pickInfo;
    m_inPickMode = true;

    m_pickSignal( getHitResult() );
}

void WPickHandler::setPaintMode( int mode )
{
    m_paintMode = mode;
}
